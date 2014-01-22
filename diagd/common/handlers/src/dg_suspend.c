/*==================================================================================================

    Module Name:  dg_suspend.c

    General Description: Implements the SUSPEND test handler

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include "dg_handler_inc.h"
#include "dg_engine_util.h"
#include "dg_cmn_drv_suspend.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup SUSPEND
@{

@par
<b>SUSPEND - 0x0100</b>

@par
Diag SUSPEND action.
*/

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/
#define DG_SUSPEND_REQ_LEN_MIN 1
#define DG_SUSPEND_MODE_SIZE   1

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** Actions for SUSPEND command */
enum
{
    DG_SUSPEND_ACTION_GET = 0x00,
    DG_SUSPEND_ACTION_SET = 0x01,
};
typedef UINT8 DG_SUSPEND_ACTION_T;

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
@brief Handler function for the SUSPEND command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_SUSPEND_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_SUSPEND_ACTION_T         action;
    DG_DEFS_MODE_T              mode = DG_ENGINE_UTIL_get_engine_mode();
    DG_DEFS_DIAG_RSP_BUILDER_T* rsp  = DG_ENGINE_UTIL_rsp_init();

    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);

    if (DG_ENGINE_UTIL_req_len_check_at_least(req, DG_SUSPEND_REQ_LEN_MIN, rsp))
    {
        action = DG_ENGINE_UTIL_req_parse_1_byte_ntoh(req);

        switch (action)
        {
        case DG_SUSPEND_ACTION_GET:
        {
            if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, DG_SUSPEND_MODE_SIZE))
            {
                DG_ENGINE_UTIL_rsp_append_1_byte_hton(rsp, (UINT8)mode);
            }
        }
        break;

        case DG_SUSPEND_ACTION_SET:
        {
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, DG_SUSPEND_MODE_SIZE, rsp))
            {
                UINT8 new_mode = DG_ENGINE_UTIL_req_parse_1_byte_ntoh(req);

                if ((mode == DG_DEFS_MODE_NORMAL) &&
                    (new_mode == DG_DEFS_MODE_NORMAL))
                {
                    DG_DBG_TRACE("Current mode is already normal mode, do nothing");
                }
                else if ((mode == DG_DEFS_MODE_TEST) &&
                         (new_mode == DG_DEFS_MODE_TEST))
                {
                    DG_DBG_TRACE("Current mode is already test mode, do nothing");
                }
                else if ((mode == DG_DEFS_MODE_TEST) &&
                         (new_mode == DG_DEFS_MODE_NORMAL))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string(
                        rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                        "You must restart the system to recover to normal mode!");
                }
                else if ((mode == DG_DEFS_MODE_NORMAL) &&
                         (new_mode == DG_DEFS_MODE_TEST))
                {

                    if (!DG_CMN_DRV_SUSPEND_suspend())
                    {
                        DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                                "Failed to Set suspend mode");
                    }
                    else
                    {
                        DG_ENGINE_UTIL_set_engine_mode(DG_DEFS_MODE_TEST);
                    }
                }
                else
                {
                    DG_ENGINE_UTIL_rsp_set_error_string(
                        rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                        "Invalid mode detected! old_mode=%d, new mode=%d",
                        mode, new_mode);
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

