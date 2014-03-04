/*==================================================================================================

    Module Name:  dg_reset.c

    General Description: Implements the RESET test handler

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_cmn_drv_reset.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup RESET
@{

@par
<b>RESET - 0x0101</b>

@par
Handles Power related Reset commands.
*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** Actions for RESET command */
enum
{
    DG_RESET_ACTION_SYS_RESET    = 0x00,
    DG_RESET_ACTION_CHIP_RESET   = 0x01,
    DG_RESET_ACTION_CHIP_RECOVER = 0x02
};
typedef UINT8 DG_RESET_ACTION_T;

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
@brief Handler function for the RESET command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_RESET_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_RESET_ACTION_T           action;
    DG_CMN_DRV_RESET_COMP_T     comp;
    DG_DEFS_DIAG_RSP_BUILDER_T* rsp = DG_ENGINE_UTIL_rsp_init();

    const UINT32 req_len = sizeof(action);

    /* Verify action parameter was given */
    DG_DBG_TRACE("In DG_RESET_handler_main begin to parse Request");
    if (DG_ENGINE_UTIL_req_len_check_at_least(req, req_len, rsp))
    {
        /* Parse and switch on action */
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, action);

        DG_DBG_TRACE("action=0x%02x", action);

        switch (action)
        {
        case DG_RESET_ACTION_SYS_RESET:
            if (!DG_CMN_DRV_RESET_reset(DG_CMN_DRV_RESET_SYS, TRUE))
            {
                DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                        "Failed to Reset System");
            }
            else
            {
                DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
            }
            break;

        case DG_RESET_ACTION_CHIP_RESET:
        case DG_RESET_ACTION_CHIP_RECOVER:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(comp), rsp))
            {
                BOOL is_reset = (action == DG_RESET_ACTION_CHIP_RESET);
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, comp);
                if (!DG_CMN_DRV_RESET_reset(comp, is_reset))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to Reset System, reset=%d",
                                                            is_reset);
                }
                else
                {
                    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
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

