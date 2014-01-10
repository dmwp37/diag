/*==================================================================================================

    Module Name:  dg_ping.c

    General Description: Pings the device

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_aux_engine.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup PING
@{

@par
<b>PING - 0x0FFF</b>

@par
PING is PING...  Echo back the data, test for each AUX engine if configured
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
@brief Handler function for the PING requests (support multi aux)

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_PING_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_DEFS_DIAG_RSP_BUILDER_T* rsp = DG_ENGINE_UTIL_rsp_init();

#if DG_CFG_AUX_NUM > 0
    int aux_id;
    int first_enabled_aux;
    int count = DG_AUX_ENGINE_get_total_enabled_aux(&first_enabled_aux);

    if (count > 0)
    {
        for (aux_id = DG_CFG_AUX_NUM - 1; aux_id >= 0; aux_id--)
        {
            if (!DG_AUX_ENGINE_is_aux_enabled(aux_id))
            {
                continue;
            }

            DG_AUX_ENGINE_handle_aux_cmd(aux_id, req, rsp);

            if (DG_ENGINE_UTIL_rsp_is_failure(rsp))
            {
                DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                    "can't ping aux engine: aux_id=%d", aux_id);
                break;
            }
            else if (aux_id == first_enabled_aux)
            {
                /* leave the last rsp */
                break;
            }
            else
            {
                /* free the internal rsp */
                DG_ENGINE_UTIL_rsp_free(rsp);
                rsp = DG_ENGINE_UTIL_rsp_init();
            }
        }
    }
    else
    {
        DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                            "No aux engine connected!");
    }
#else
    /* Echo data from request back in DIAG response */
    if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, DG_ENGINE_UTIL_req_get_remain_len(req)))
    {
        DG_ENGINE_UTIL_rsp_append_buf(rsp,
                                      DG_ENGINE_UTIL_req_get_remain_data_ptr(req),
                                      DG_ENGINE_UTIL_req_get_remain_len(req));
        DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
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

