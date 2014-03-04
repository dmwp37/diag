/*==================================================================================================

    Module Name:  dg_ext_loop.c

    General Description: Implements the EXT_LOOP test handler

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include "dg_handler_inc.h"
#include "dg_cmn_drv_ext_loop.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup EXT_LOOP
@{

@par
<b>EXT_LOOP - 0x0022</b>

@par
Allows Clear and set external loopback mode on specified traffic node.
*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

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
@brief Handler function for the EXT_LOOP command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_EXT_LOOP_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_CMN_DRV_EXT_LOOP_ACTION_T action;
    DG_CMN_DRV_EXT_LOOP_NODE_T   node;
    DG_DEFS_DIAG_RSP_BUILDER_T*  rsp = DG_ENGINE_UTIL_rsp_init();

    const UINT32 req_len = sizeof(action) + sizeof(node);

    /* Verify action parameter was given */
    DG_DBG_TRACE("In DG_EXT_LOOP_handler_main begin to parse Request");
    if (DG_ENGINE_UTIL_req_len_check_equal(req, req_len, rsp))
    {
        /* Parse and switch on action */
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, action);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, node);
        DG_DBG_TRACE("action=0x%02x, node=0x%02x", action, node);

        if (!DG_CMN_DRV_EXT_LOOP_operate(action, node))
        {
            DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                    "Failed to set/clear external loopback");
        }
        else
        {
            DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
        }

        DG_ENGINE_UTIL_rsp_send(rsp, req);
        DG_ENGINE_UTIL_rsp_free(rsp);
    }
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

