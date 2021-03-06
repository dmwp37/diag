/*==================================================================================================

    Module Name:  dg_aux_cmd.c

    General Description: Handles DIAG requests destined for the aux DIAG engine

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_aux_engine.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup AUX
@{

@par
<b>AUX - 0xDEAD</b>

@par
Handles AUX engine diag command
engine
*/

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
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
@brief Handler function for the DIAG requests being sent to aux diag engine

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_AUX_CMD_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    /* Create response builder */
    DG_DEFS_DIAG_RSP_BUILDER_T* rsp;
    rsp = DG_ENGINE_UTIL_rsp_init();

#if DG_CFG_AUX_NUM == 0
    /* If no modem existing, return error opcode */
    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_PAR_ERR_OPCODE);
    DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                        "Unsupported opcode detected! opcode=0x%04x",
                                        req->header.opcode);
#elif DG_CFG_AUX_NUM == 1
    DG_AUX_ENGINE_handle_aux_cmd(DG_AUX_ENGINE_AUX_ID_AUX1, req, rsp);
#else
    /* Use an arbitrary opcode value to determine which aux engine to forward the request to */
    if (req->header.opcode < 0xc000)
    {
        DG_AUX_ENGINE_handle_aux_cmd(DG_AUX_ENGINE_AUX_ID_AUX1, req, rsp);
    }
    else
    {
        DG_AUX_ENGINE_handle_aux_cmd(DG_AUX_ENGINE_AUX_ID_AUX2, req, rsp);
    }
#endif

    DG_ENGINE_UTIL_rsp_send(rsp, req);

    /* Free response builder */
    DG_ENGINE_UTIL_rsp_free(rsp);
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

