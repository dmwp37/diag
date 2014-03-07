/*==================================================================================================

    Module Name:  dg_psu.c

    General Description: Implements the PSU test handler

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_cmn_drv_psu.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup PSU
@{

@par
<b>PSU - 0x000A</b>

@par
PSU test handler.
*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** Actions for PSU command */
enum
{
    DG_PSU_ACTION_DUMP_INFO  = 0x00,
    DG_PSU_ACTION_SET_INFO   = 0x01,
    DG_PSU_ACTION_GET_STATUS = 0x02,
    DG_PSU_ACTION_SET_CFG    = 0x03,
};
typedef UINT8 DG_PSU_ACTION_T;

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
@brief Handler function for the PSU command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_PSU_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_PSU_ACTION_T             action;
    DG_CMN_DRV_PSU_SLOT_T       slot;
    DG_CMN_DRV_PSU_CHANNEL_T    channel;
    DG_CMN_DRV_PSU_ADDR_T       addr;
    DG_CMN_DRV_PSU_DATA_T       data;
    DG_CMN_DRV_PSU_CFG_T        cfg;
    DG_DEFS_DIAG_RSP_BUILDER_T* rsp = DG_ENGINE_UTIL_rsp_init();

    UINT32 req_len = sizeof(action) + sizeof(slot);

    DG_DBG_TRACE("In DG_PSU_handler_main begin to parse Request");
    if (DG_ENGINE_UTIL_req_len_check_at_least(req, req_len, rsp))
    {
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, action);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, slot);

        DG_DBG_TRACE("action=0x%02x, slot=0x%02x", action, slot);

        switch (action)
        {
        case DG_PSU_ACTION_DUMP_INFO:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(channel), rsp))
            {
                DG_CMN_DRV_PSU_INFO_T info;

                DG_ENGINE_UTIL_req_parse_data_ntoh(req, channel);

                if (!DG_CMN_DRV_PSU_dump_info(slot, channel, &info))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to dump PSU information");
                }
                else
                {
                    if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, sizeof(info)))
                    {
                        DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                        DG_ENGINE_UTIL_rsp_append_buf(rsp, info.data, sizeof(info.data));
                    }
                }
            }
            break;

        case DG_PSU_ACTION_SET_INFO:
            req_len = sizeof(channel) + sizeof(addr) + sizeof(data);
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, req_len, rsp))
            {
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, channel);
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, addr);
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, data);

                if (!DG_CMN_DRV_PSU_write(slot, channel, addr, data))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to write PSU data");
                }
                else
                {
                    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                }
            }
            break;

        case DG_PSU_ACTION_GET_STATUS:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, 0, rsp))
            {
                DG_CMN_DRV_PSU_STATUS_T status;

                if (!DG_CMN_DRV_PSU_get_status(slot, &status))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to get PSU status");
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

        case DG_PSU_ACTION_SET_CFG:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(cfg), rsp))
            {
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, cfg);

                if (!DG_CMN_DRV_PSU_set_cfg(slot, cfg))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to set PSU config");
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

