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
                                           LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** Actions for I2C command */
enum
{
    DG_I2C_ACTION_READ  = 0x00,
    DG_I2C_ACTION_WRITE = 0x01,
};
typedef UINT8 DG_I2C_ACTION_T;

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/
static const UINT32 DG_I2C_REQ_LEN_MIN = sizeof(DG_I2C_ACTION_T) +
                                         sizeof(DG_CMN_DRV_I2C_BUS_T) +
                                         sizeof(DG_CMN_DRV_I2C_ADDR_T) +
                                         sizeof(DG_CMN_DRV_I2C_OFFSET_T);

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static void dg_i2c_read_bus(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp,
                            DG_CMN_DRV_I2C_BUS_T bus, DG_CMN_DRV_I2C_ADDR_T address,
                            DG_CMN_DRV_I2C_OFFSET_T offset);

static void dg_i2c_write_bus(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp,
                             DG_CMN_DRV_I2C_BUS_T bus, DG_CMN_DRV_I2C_ADDR_T address,
                             DG_CMN_DRV_I2C_OFFSET_T offset);

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
    DG_CMN_DRV_I2C_OFFSET_T     offset;
    DG_DEFS_DIAG_RSP_BUILDER_T* rsp = DG_ENGINE_UTIL_rsp_init();

    /* Verify action parameter was given */
    DG_DBG_TRACE("In DG_I2C_handler_main begin to parse Request");
    if (DG_ENGINE_UTIL_req_len_check_at_least(req, DG_I2C_REQ_LEN_MIN, rsp))
    {
        /* Parse and switch on action */
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, action);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, bus);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, address);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, offset);

        DG_DBG_TRACE("action=0x%02x, bus=0x%02x, address=0x%02x, offset=0x%02x",
                     action, bus, address, offset);

        switch (action)
        {
        case DG_I2C_ACTION_READ:
            dg_i2c_read_bus(req, rsp, bus, address, offset);
            break;

        case DG_I2C_ACTION_WRITE:
            dg_i2c_write_bus(req, rsp, bus, address, offset);
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
void dg_i2c_read_bus(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp,
                     DG_CMN_DRV_I2C_BUS_T bus, DG_CMN_DRV_I2C_ADDR_T address,
                     DG_CMN_DRV_I2C_OFFSET_T offset)
{
    DG_CMN_DRV_I2C_SIZE_T read_length = 0;
    UINT8*                read_data   = NULL;

    if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(DG_CMN_DRV_I2C_SIZE_T), rsp))
    {
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, read_length);

        read_data = (UINT8*)DG_ENGINE_UTIL_alloc_mem(read_length, rsp);

        if (read_data != NULL)
        {
            if (!DG_CMN_DRV_I2C_read_bus(bus, address, offset, read_length, read_data))
            {
                DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                        "Failed to Read I2C");
            }
            else
            {
                /* Allocate memory for read data */
                if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, read_length))
                {
                    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
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
void dg_i2c_write_bus(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp,
                      DG_CMN_DRV_I2C_BUS_T bus, DG_CMN_DRV_I2C_ADDR_T address,
                      DG_CMN_DRV_I2C_OFFSET_T offset)
{
    DG_CMN_DRV_I2C_SIZE_T write_length = 0;
    UINT8*                write_data   = NULL;

    if (DG_ENGINE_UTIL_req_remain_len_check_at_least(req, sizeof(DG_CMN_DRV_I2C_SIZE_T), rsp))
    {
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, write_length);

        if (DG_ENGINE_UTIL_req_remain_len_check_at_least(req, write_length, rsp))
        {
            write_data = DG_ENGINE_UTIL_req_get_remain_data_ptr(req);

            if (!DG_CMN_DRV_I2C_write_bus(bus, address, offset, write_length, write_data))
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

/** @} */
/** @} */

