/*==================================================================================================

    Module Name:  dg_i2c.c

    General Description: Implements the I2C test handler

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include "dg_handler_inc.h"
#include "dg_cmn_drv_i2c.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup I2C
@{

@par
<b>I2C - 0x0010</b>

@par
Allows generic read/write access to the I2C bus.
*/

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/
#define DG_I2C_REQ_LEN_MIN   3
#define DG_I2C_DATA_LEN_SIZE 2

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** Actions for I2C command */
enum
{
    DG_I2C_ACTION_READ       = 0x00,
    DG_I2C_ACTION_WRITE      = 0x01,
    DG_I2C_ACTION_WRITE_READ = 0x02,
};
typedef UINT8 DG_I2C_ACTION_T;

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static void diag_i2c_read_bus(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp,
                              DG_CMN_DRV_I2C_BUS_T bus, DG_CMN_DRV_I2C_ADDR_T address);

static void diag_i2c_write_bus(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp,
                               DG_CMN_DRV_I2C_BUS_T bus, DG_CMN_DRV_I2C_ADDR_T address);

static void diag_i2c_write_read_bus(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp,
                                    DG_CMN_DRV_I2C_BUS_T bus, DG_CMN_DRV_I2C_ADDR_T address);

/*==================================================================================================
                                         GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                         GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Handler function for the I2C command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_I2C_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_I2C_ACTION_T             action;
    DG_CMN_DRV_I2C_BUS_T        bus;
    DG_CMN_DRV_I2C_ADDR_T       address;
    DG_DEFS_DIAG_RSP_BUILDER_T* rsp = DG_ENGINE_UTIL_rsp_init();

    /* Verify action parameter was given */
    DG_DBG_TRACE("In DG_I2C_handler_main begin to parse Request");
    if (DG_ENGINE_UTIL_req_len_check_at_least(req, DG_I2C_REQ_LEN_MIN, rsp))
    {
        /* Parse and switch on action */
        action  = DG_ENGINE_UTIL_req_parse_1_byte_ntoh(req);
        bus     = DG_ENGINE_UTIL_req_parse_1_byte_ntoh(req);
        address = DG_ENGINE_UTIL_req_parse_1_byte_ntoh(req);

        DG_DBG_TRACE("action = 0x%02x, bus = 0x%02x, address = 0x%02x", action, bus, address);

        switch (action)
        {
        case DG_I2C_ACTION_READ:
            diag_i2c_read_bus(req, rsp, bus, address);
            break;

        case DG_I2C_ACTION_WRITE:
            diag_i2c_write_bus(req, rsp, bus, address);
            break;

        case DG_I2C_ACTION_WRITE_READ:
            diag_i2c_write_read_bus(req, rsp, bus, address);
            break;

        default:
            DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_ERR_PARM,
                                                "Invalid action 0x%02x", action);
            break;
        }
    }

    DG_ENGINE_UTIL_rsp_send(rsp, req);
    DG_ENGINE_UTIL_rsp_free(rsp);
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Read bus

@param[in]     req     - DIAG request
@param[in,out] rsp     - DIAG rsp builder
@param[in]     bus     - I2C bus number in the system
@param[in]     address - slave device address
*//*==============================================================================================*/
void diag_i2c_read_bus(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp,
                       DG_CMN_DRV_I2C_BUS_T bus, DG_CMN_DRV_I2C_ADDR_T address)
{
    UINT16 read_length = 0;
    UINT8* read_data   = NULL;

    if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, DG_I2C_DATA_LEN_SIZE, rsp))
    {
        read_length = DG_ENGINE_UTIL_req_parse_2_bytes_ntoh(req);

        read_data = (UINT8*)DG_ENGINE_UTIL_alloc_mem(read_length, rsp);

        if (read_data != NULL)
        {
            if (!DG_CMN_DRV_I2C_read_bus(bus, address, read_length, read_data))
            {
                DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                        "Failed to Read I2C");
            }
            else
            {
                /*  Allocate memory for read data

                    Field Name          Field Location    Field Size    Field Definition
                    Read Data Length    Byte0-1           2 bytes       Length of read data response
                    Read Data Byte2-    Variable
                */
                if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, DG_I2C_DATA_LEN_SIZE + read_length))
                {
                    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                    DG_ENGINE_UTIL_rsp_append_2_bytes_hton(rsp, read_length);
                    DG_ENGINE_UTIL_rsp_append_buf(rsp, read_data, read_length);
                }
            }

            free(read_data);
        }
    }
}

/*=============================================================================================*//**
@brief Write bus

@param[in]     req     - DIAG request
@param[in,out] rsp     - DIAG rsp builder
@param[in]     bus     - I2C bus number in the system
@param[in]     address - slave device address
*//*==============================================================================================*/
void diag_i2c_write_bus(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp,
                        DG_CMN_DRV_I2C_BUS_T bus, DG_CMN_DRV_I2C_ADDR_T address)
{
    UINT16 write_length = 0;
    UINT8* write_data   = NULL;

    if (DG_ENGINE_UTIL_req_remain_len_check_at_least(req, DG_I2C_DATA_LEN_SIZE, rsp))
    {
        write_length = DG_ENGINE_UTIL_req_parse_2_bytes_ntoh(req);

        if (DG_ENGINE_UTIL_req_remain_len_check_at_least(req, write_length, rsp))
        {
            write_data = DG_ENGINE_UTIL_req_get_remain_data_ptr(req);

            if (!DG_CMN_DRV_I2C_write_bus(bus, address, write_length, write_data))
            {
                DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                        "Failed to Write I2C");
            }
            else
            {
                DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
            }
        }
    }
}

/*=============================================================================================*//**
@brief Write/Read bus

@param[in]     req     - DIAG request
@param[in,out] rsp     - DIAG rsp builder
@param[in]     bus     - I2C bus number in the system
@param[in]     address - slave device address
*//*==============================================================================================*/
void diag_i2c_write_read_bus(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp,
                             DG_CMN_DRV_I2C_BUS_T bus, DG_CMN_DRV_I2C_ADDR_T address)
{
    UINT16 write_length = 0;
    UINT8* write_data   = NULL;
    UINT16 read_length  = 0;
    UINT8* read_data    = NULL;

    if (DG_ENGINE_UTIL_req_remain_len_check_at_least(req, DG_I2C_DATA_LEN_SIZE, rsp))
    {
        write_length = DG_ENGINE_UTIL_req_parse_2_bytes_ntoh(req);
        DG_DBG_TRACE("write_length = 0x%02x", write_length);

        if (DG_ENGINE_UTIL_req_remain_len_check_at_least(req, write_length, rsp))
        {
            write_data = DG_ENGINE_UTIL_alloc_mem(write_length, rsp);

            if (write_data != NULL)
            {
                DG_ENGINE_UTIL_req_parse_buf(req, write_data, write_length);

                if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, DG_I2C_DATA_LEN_SIZE, rsp))
                {
                    read_length = DG_ENGINE_UTIL_req_parse_2_bytes_ntoh(req);

                    read_data = DG_ENGINE_UTIL_alloc_mem(read_length, rsp);

                    if (read_data != NULL)
                    {

                        if (!DG_CMN_DRV_I2C_write_read_bus(bus, address,
                                                           write_length, write_data,
                                                           read_length, read_data))
                        {
                            DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                                    "Failed to Write&Read I2C");
                        }
                        else
                        {
                            if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, DG_I2C_DATA_LEN_SIZE + read_length))
                            {
                                DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                                DG_ENGINE_UTIL_rsp_append_2_bytes_hton(rsp, read_length);
                                DG_ENGINE_UTIL_rsp_append_buf(rsp, read_data, read_length);
                            }
                        }
                        free(read_data);
                    }
                }
                free(write_data);
            }
        }
    }
}

/** @} */
/** @} */

