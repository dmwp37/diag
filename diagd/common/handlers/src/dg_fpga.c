/*==================================================================================================

    Module Name:  dg_fpga.c

    General Description: Implements the FPGA test handler

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include "dg_handler_inc.h"
#include "dg_cmn_drv_fpga.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup FPGA
@{

@par
<b>FPGA - 0x000D</b>

@par
FPGA related test.
*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** Actions for FPGA command */
enum
{
    DG_FPGA_ACTION_READ_REG  = 0x00,
    DG_FPGA_ACTION_WRITE_REG = 0x01,
    DG_FPGA_ACTION_READ_MEM  = 0x02,
    DG_FPGA_ACTION_WRITE_MEM = 0x03,
    DG_FPGA_ACTION_DMA_RECV  = 0x04,
    DG_FPGA_ACTION_DMA_SEND  = 0x05,
    DG_FPGA_ACTION_SELF_TEST = 0x06,
    DG_FPGA_ACTION_SET_MODE  = 0x07,
    DG_FPGA_ACTION_DOWNLOAD  = 0x08
};
typedef UINT8 DG_FPGA_ACTION_T;

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/
static const UINT32 DG_FPGA_REQ_LEN_MIN = sizeof(DG_FPGA_ACTION_T);

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static void dg_fpga_read_reg(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp);
static void dg_fpga_write_reg(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp);
static void dg_fpga_read_mem(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp);
static void dg_fpga_write_mem(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp);
static void dg_fpga_dma_recv(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp);
static void dg_fpga_dma_send(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp);
static void dg_fpga_self_test(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp);
static void dg_fpga_set_mode(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp);
static void dg_fpga_download(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp);


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
@brief Handler function for the FPGA command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_FPGA_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_FPGA_ACTION_T            action;
    DG_DEFS_DIAG_RSP_BUILDER_T* rsp = DG_ENGINE_UTIL_rsp_init();

    /* Verify action parameter was given */
    DG_DBG_TRACE("In DG_FPGA_handler_main begin to parse Request");
    if (DG_ENGINE_UTIL_req_len_check_at_least(req, sizeof(action), rsp))
    {
        /* Parse and switch on action */
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, action);

        DG_DBG_TRACE("action=0x%02x", action);

        switch (action)
        {
        case DG_FPGA_ACTION_READ_REG:
            dg_fpga_read_reg(req, rsp);
            break;

        case DG_FPGA_ACTION_WRITE_REG:
            dg_fpga_write_reg(req, rsp);
            break;

        case DG_FPGA_ACTION_READ_MEM:
            dg_fpga_read_mem(req, rsp);
            break;

        case DG_FPGA_ACTION_WRITE_MEM:
            dg_fpga_write_mem(req, rsp);
            break;

        case DG_FPGA_ACTION_DMA_RECV:
            dg_fpga_dma_recv(req, rsp);
            break;

        case DG_FPGA_ACTION_DMA_SEND:
            dg_fpga_dma_send(req, rsp);
            break;

        case DG_FPGA_ACTION_SELF_TEST:
            dg_fpga_self_test(req, rsp);
            break;

        case DG_FPGA_ACTION_SET_MODE:
            dg_fpga_set_mode(req, rsp);
            break;

        case DG_FPGA_ACTION_DOWNLOAD:
            dg_fpga_download(req, rsp);
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
*//*==============================================================================================*/
void dg_fpga_read_reg(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    DG_CMN_DRV_FPGA_REG_ADDR_T addr;
    DG_CMN_DRV_FPGA_REG_T      reg_data;

    if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(addr), rsp))
    {
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, addr);

        if (!DG_CMN_DRV_FPGA_read_reg(addr, &reg_data))
        {
            DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                    "Failed to read FPGA register");
        }
        else
        {
            /* Allocate memory for read register response */
            if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, sizeof(addr)))
            {
                DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                DG_ENGINE_UTIL_rsp_append_data_hton(rsp, reg_data);
            }
        }
    }
}

/*=============================================================================================*//**
@brief Write bus

@param[in]     req     - DIAG request
@param[in,out] rsp     - DIAG rsp builder
*//*==============================================================================================*/
void dg_fpga_write_reg(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    DG_CMN_DRV_FPGA_REG_ADDR_T addr;
    DG_CMN_DRV_FPGA_REG_T      reg_data;

    const UINT32 min_len = sizeof(addr) + sizeof(reg_data);

    if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, min_len, rsp))
    {
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, addr);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, reg_data);

        if (!DG_CMN_DRV_FPGA_write_reg(addr, reg_data))
        {
            DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                    "Failed to write FPGA register");
        }
        else
        {
            DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
        }
    }
}

/*=============================================================================================*//**
@brief Write bus

@param[in]     req     - DIAG request
@param[in,out] rsp     - DIAG rsp builder
*//*==============================================================================================*/
void dg_fpga_read_mem(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    DG_CMN_DRV_FPGA_MEM_ADDR_T addr;
    DG_CMN_DRV_FPGA_MEM_SIZE_T read_len;
    UINT8*                     read_data = NULL;

    const UINT32 min_len = sizeof(addr) + sizeof(read_len);

    if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, min_len, rsp))
    {
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, addr);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, read_len);

        read_data = (UINT8*)DG_ENGINE_UTIL_alloc_mem(read_len, rsp);
        if (read_data != NULL)
        {
            if (!DG_CMN_DRV_FPGA_read_mem(addr, read_len, read_data))
            {
                DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                        "Failed to read FPGA memory");
            }
            else
            {
                /* Allocate memory for read memory response */
                if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, read_len))
                {
                    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                    DG_ENGINE_UTIL_rsp_append_buf(rsp, read_data, read_len);
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
*//*==============================================================================================*/
void dg_fpga_write_mem(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    DG_CMN_DRV_FPGA_MEM_ADDR_T addr;
    DG_CMN_DRV_FPGA_MEM_SIZE_T write_len;
    UINT8*                     write_data = NULL;

    const UINT32 min_len = sizeof(addr) + sizeof(write_len);

    if (DG_ENGINE_UTIL_req_remain_len_check_at_least(req, min_len, rsp))
    {
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, addr);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, write_len);

        if (DG_ENGINE_UTIL_req_remain_len_check_at_least(req, write_len, rsp))
        {
            write_data = DG_ENGINE_UTIL_req_get_remain_data_ptr(req);

            DG_DBG_TRACE("FPGA write memory data");
            DG_DBG_DUMP(write_data, write_len);

            if (!DG_CMN_DRV_FPGA_write_mem(addr, write_len, write_data))
            {
                DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                        "Failed to write FPGA memory");
            }
            else
            {
                DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
            }
        }
    }
}

/*=============================================================================================*//**
@brief Write bus

@param[in]     req     - DIAG request
@param[in,out] rsp     - DIAG rsp builder
*//*==============================================================================================*/
void dg_fpga_dma_recv(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    DG_CMN_DRV_FPGA_MEM_ADDR_T addr;
    DG_CMN_DRV_FPGA_MEM_SIZE_T read_len;
    UINT8*                     read_data = NULL;

    const UINT32 min_len = sizeof(addr) + sizeof(read_len);

    if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, min_len, rsp))
    {
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, addr);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, read_len);

        read_data = (UINT8*)DG_ENGINE_UTIL_alloc_mem(read_len, rsp);
        if (read_data != NULL)
        {
            if (!DG_CMN_DRV_FPGA_dma_recv(addr, read_len, read_data))
            {
                DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                        "Failed to DMA recv FPGA memory");
            }
            else
            {
                /* Allocate memory for DMA receive response */
                if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, read_len))
                {
                    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                    DG_ENGINE_UTIL_rsp_append_buf(rsp, read_data, read_len);
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
*//*==============================================================================================*/
void dg_fpga_dma_send(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    DG_CMN_DRV_FPGA_MEM_ADDR_T addr;
    DG_CMN_DRV_FPGA_MEM_SIZE_T write_len;
    UINT8*                     write_data = NULL;

    const UINT32 min_len = sizeof(addr) + sizeof(write_len);

    if (DG_ENGINE_UTIL_req_remain_len_check_at_least(req, min_len, rsp))
    {
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, addr);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, write_len);

        if (DG_ENGINE_UTIL_req_remain_len_check_at_least(req, write_len, rsp))
        {
            write_data = DG_ENGINE_UTIL_req_get_remain_data_ptr(req);

            DG_DBG_TRACE("FPGA write memory data");
            DG_DBG_DUMP(write_data, write_len);

            if (!DG_CMN_DRV_FPGA_dma_send(addr, write_len, write_data))
            {
                DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                        "Failed to DMA send FPGA memory");
            }
            else
            {
                DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
            }
        }
    }
}

/*=============================================================================================*//**
@brief Write bus

@param[in]     req     - DIAG request
@param[in,out] rsp     - DIAG rsp builder
*//*==============================================================================================*/
void dg_fpga_self_test(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    DG_CMN_DRV_FPGA_SELF_TEST_T sub_test;

    if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(sub_test), rsp))
    {
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, sub_test);

        if (!DG_CMN_DRV_FPGA_self_test(sub_test))
        {
            DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                    "Failed to run FPGA self-test");
        }
        else
        {
            DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
        }
    }
}

/*=============================================================================================*//**
@brief Write bus

@param[in]     req     - DIAG request
@param[in,out] rsp     - DIAG rsp builder
*//*==============================================================================================*/
void dg_fpga_set_mode(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    DG_CMN_DRV_FPGA_MODE_T mode;

    if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(mode), rsp))
    {
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, mode);

        if (!DG_CMN_DRV_FPGA_set_mode(mode))
        {
            DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                    "Failed to set FPGA mode");
        }
        else
        {
            DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
        }
    }
}

/*=============================================================================================*//**
@brief Write bus

@param[in]     req     - DIAG request
@param[in,out] rsp     - DIAG rsp builder
*//*==============================================================================================*/
void dg_fpga_download(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    DG_CMN_DRV_FPGA_IMAGE_T image;

    if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(image), rsp))
    {
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, image);

        if (!DG_CMN_DRV_FPGA_download_image(image))
        {
            DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                    "Failed to download FPGA image");
        }
        else
        {
            DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
        }
    }
}

/** @} */
/** @} */

