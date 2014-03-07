/*==================================================================================================

    Module Name:  dg_sfp.c

    General Description: Implements the SFP test handler

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_cmn_drv_sfp.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup SFP
@{

@par
<b>SFP - 0x0006</b>

@par
SFP test handler.
*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** Actions for SFP command */
enum
{
    DG_SFP_ACTION_GET_STATUS = 0x00,
    DG_SFP_ACTION_TX_CONFIG  = 0x01,
    DG_SFP_ACTION_DUMP_REG   = 0x02,
};
typedef UINT8 DG_SFP_ACTION_T;

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
@brief Handler function for the SFP command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_SFP_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_SFP_ACTION_T         action;
    DG_CMN_DRV_SFP_ID_T     sfp;
    DG_CMN_DRV_SFP_TX_CFG_T tx_cfg;
    DG_CMN_DRV_SFP_REG_T    reg;

    DG_DEFS_DIAG_RSP_BUILDER_T* rsp = DG_ENGINE_UTIL_rsp_init();

    const UINT32 min_len = sizeof(action) + sizeof(sfp);

    DG_DBG_TRACE("In DG_SFP_handler_main begin to parse Request");
    if (DG_ENGINE_UTIL_req_len_check_at_least(req, min_len, rsp))
    {
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, action);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, sfp);

        DG_DBG_TRACE("action=0x%02x, sfp=0x%02x", action, sfp);

        switch (action)
        {
        case DG_SFP_ACTION_GET_STATUS:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, 0, rsp))
            {
                DG_CMN_DRV_SFP_STATUS_T status;

                if (!DG_CMN_DRV_SFP_get_status(sfp, &status))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to get SFP status");
                }
                else
                {
                    if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, sizeof(status)))
                    {
                        DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                        DG_ENGINE_UTIL_rsp_append_data_hton(rsp, status);
                    }
                }
            }
            break;

        case DG_SFP_ACTION_TX_CONFIG:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(tx_cfg), rsp))
            {
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, tx_cfg);

                if (!DG_CMN_DRV_SFP_tx_config(sfp, tx_cfg))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to config SFP TX");
                }
                else
                {
                    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                }
            }
            break;

        case DG_SFP_ACTION_DUMP_REG:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(reg), rsp))
            {
                DG_CMN_DRV_SFP_REG_DATA_T data;
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, reg);
                if (!DG_CMN_DRV_SFP_dump_register(sfp, reg, &data))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to dump SFP register");
                }
                else
                {

                    if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, sizeof(data.reg)))
                    {
                        DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                        DG_ENGINE_UTIL_rsp_append_buf(rsp, data.reg, sizeof(data.reg));
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

