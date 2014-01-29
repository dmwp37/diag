/*==================================================================================================

    Module Name:  dg_debug_level.c

    General Description: Implements the DEBUG_LEVEL DIAG

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_cmn_drv_debug_level.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup DEBUG_LEVEL
@{

@par
<b>DEBUG_LEVEL - 0x0FFD</b>

@par
This command is responsible for control debug level of different component
*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** Actions for DEBUG_LEVEL command */
enum
{
    DG_DEBUG_LEVEL_GET = 0x00,
    DG_DEBUG_LEVEL_SET = 0x01,

};
typedef UINT8 DG_DEBUG_LEVEL_ACTION_T;

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
@brief Handler function for the DEBUG_LEVEL command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_DEBUG_LEVEL_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_CMN_DRV_DEBUG_LEVEL_COMPONENT_T comp;
    DG_DEBUG_LEVEL_ACTION_T            action;
    DG_CMN_DRV_DEBUG_LEVEL_LEVEL_T     dbg_lvl;
    DG_DEFS_DIAG_RSP_BUILDER_T*        rsp = DG_ENGINE_UTIL_rsp_init();

    if (DG_ENGINE_UTIL_req_len_check_at_least(req, sizeof(action), rsp))
    {
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, action);

        switch (action)
        {
        case DG_DEBUG_LEVEL_SET:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(comp) + sizeof(dbg_lvl), rsp))
            {
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, comp);
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, dbg_lvl);

                if (!DG_CMN_DRV_DEBUG_LEVEL_set(comp, dbg_lvl))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to set debug level");
                }
                else
                {
                    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                }

            }
            break;

        case DG_DEBUG_LEVEL_GET:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(comp), rsp))
            {
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, comp);

                if (!DG_CMN_DRV_DEBUG_LEVEL_get(comp, &dbg_lvl))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to get debug level");
                }
                else
                {
                    if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, sizeof(dbg_lvl)))
                    {
                        DG_ENGINE_UTIL_rsp_append_data_hton(rsp, dbg_lvl);
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

