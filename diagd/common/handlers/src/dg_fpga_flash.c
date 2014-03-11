/*==================================================================================================

    Module Name:  dg_fpga_flash.c

    General Description: Implements the FPGA_FLASH test handler

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include "dg_handler_inc.h"
#include "dg_cmn_drv_fpga_flash.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup FPGA_FLASH
@{

@par
<b>FPGA_FLASH - 0x0018</b>

@par
Allows generic read and write access to the FPGA FLASH information, CFI and memory.
*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** Actions for FPGA_FLASH command */
enum
{
    DG_FPGA_FLASH_ACTION_GET_INFO = 0x00,
    DG_FPGA_FLASH_ACTION_GET_CFI  = 0x01,
    DG_FPGA_FLASH_ACTION_GET_MEM  = 0x02,
    DG_FPGA_FLASH_ACTION_SET_MEM  = 0x03,
};
typedef UINT8 DG_FPGA_FLASH_ACTION_T;

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

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
@brief Handler function for the FPGA_FLASH command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_FPGA_FLASH_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_FPGA_FLASH_ACTION_T      action;
    DG_CMN_DRV_FLASH_INFO_T     info;
    DG_CMN_DRV_FLASH_CFI_T      cfi;
    DG_CMN_DRV_FLASH_ADDR_T     addr;
    DG_CMN_DRV_FLASH_LEN_T      len;
    UINT8*                      buf = NULL;
    DG_DEFS_DIAG_RSP_BUILDER_T* rsp = DG_ENGINE_UTIL_rsp_init();

    DG_DBG_TRACE("In DG_FPGA_FLASH_handler_main begin to parse Request");
    if (DG_ENGINE_UTIL_req_len_check_at_least(req, sizeof(action), rsp))
    {
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, action);

        DG_DBG_TRACE("action=0x%02x", action);

        switch (action)
        {
        case DG_FPGA_FLASH_ACTION_GET_INFO:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, 0, rsp))
            {
                if (!DG_CMN_DRV_FPGA_FLASH_get_info(&info))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to Get FPGA flash information");
                }
                else
                {
                    if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, sizeof(info)))
                    {
                        DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                        DG_ENGINE_UTIL_rsp_append_data_hton(rsp, info.mfg_id);
                        DG_ENGINE_UTIL_rsp_append_data_hton(rsp, info.dev_id1);
                        DG_ENGINE_UTIL_rsp_append_data_hton(rsp, info.dev_id2);
                        DG_ENGINE_UTIL_rsp_append_data_hton(rsp, info.dev_id3);
                        DG_ENGINE_UTIL_rsp_append_data_hton(rsp, info.dev_verify);
                        DG_ENGINE_UTIL_rsp_append_data_hton(rsp, info.protect_verify);
                    }
                }
            }
            break;

        case DG_FPGA_FLASH_ACTION_GET_CFI:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, 0, rsp))
            {
                if (!DG_CMN_DRV_FPGA_FLASH_get_cfi(&cfi))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to Get FPGA flash CFI");
                }
                else
                {
                    if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, sizeof(cfi)))
                    {
                        DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                        DG_ENGINE_UTIL_rsp_append_buf(rsp, cfi.val, sizeof(cfi.val));
                    }
                }
            }
            break;

        case DG_FPGA_FLASH_ACTION_GET_MEM:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(addr) + sizeof(len), rsp))
            {
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, addr);
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, len);
                buf = (UINT8*)DG_ENGINE_UTIL_alloc_mem(len, rsp);
                if (buf != NULL)
                {
                    if (!DG_CMN_DRV_FPGA_FLASH_get_mem(addr, len, buf))
                    {
                        DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                                "Failed to Get FPGA flash memory");
                    }
                    else
                    {
                        if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, len))
                        {
                            DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                            DG_ENGINE_UTIL_rsp_append_buf(rsp, buf, len);
                        }
                    }
                    free(buf);
                }
            }
            break;

        case DG_FPGA_FLASH_ACTION_SET_MEM:
            if (DG_ENGINE_UTIL_req_remain_len_check_at_least(req, sizeof(addr) + sizeof(len), rsp))
            {
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, addr);
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, len);
                if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, len, rsp))
                {
                    buf = DG_ENGINE_UTIL_req_get_remain_data_ptr(req);
                    if (!DG_CMN_DRV_FPGA_FLASH_set_mem(addr, len, buf))
                    {
                        DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                                "Failed to Set FPGA flash memory");
                    }
                    else
                    {
                        DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                    }
                }
            }
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

/** @} */
/** @} */

