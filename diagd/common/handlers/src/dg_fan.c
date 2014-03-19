/*==================================================================================================

    Module Name:  dg_fan.c

    General Description: Implements the FAN test handler

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_cmn_drv_fan.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup FAN
@{

@par
<b>FAN - 0x0002</b>

@par
FAN test handler.
*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** Actions for FAN command */
enum
{
    DG_FAN_ACTION_GET_RPM       = 0x00,
    DG_FAN_ACTION_GET_RPM_LIMIT = 0x01,
    DG_FAN_ACTION_SET_PWM       = 0x02,
    DG_FAN_ACTION_SET_PWM_MAX   = 0x03,
    DG_FAN_ACTION_GET_STATUS    = 0x04,
};
typedef UINT8 DG_FAN_ACTION_T;

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
@brief Handler function for the FAN command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_FAN_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_FAN_ACTION_T             action;
    DG_CMN_DRV_FAN_ID_T         fan;
    DG_DEFS_DIAG_RSP_BUILDER_T* rsp = DG_ENGINE_UTIL_rsp_init();

    const UINT32 min_len = sizeof(action) + sizeof(fan);

    DG_DBG_TRACE("In DG_FAN_handler_main begin to parse Request");
    if (DG_ENGINE_UTIL_req_len_check_at_least(req, min_len, rsp))
    {
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, action);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, fan);

        DG_DBG_TRACE("action=0x%02x, fan=%d", action, fan);

        switch (action)
        {
        case DG_FAN_ACTION_GET_RPM:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, 0, rsp))
            {
                DG_CMN_DRV_FAN_RPM_T rpm;

                if (!DG_CMN_DRV_FAN_get_rpm(fan, &rpm))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to get FAN rpm");
                }
                else
                {
                    if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, sizeof(rpm)))
                    {
                        DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                        DG_ENGINE_UTIL_rsp_append_data_hton(rsp, rpm);
                    }
                }
            }
            break;

        case DG_FAN_ACTION_GET_RPM_LIMIT:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, 0, rsp))
            {
                DG_CMN_DRV_FAN_RPM_T min;
                DG_CMN_DRV_FAN_RPM_T max;

                if (!DG_CMN_DRV_FAN_get_rpm_limit(fan, &min, &max))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to get FAN rpm limit");
                }
                else
                {
                    UINT32 rsp_len = sizeof(min) + sizeof(max);

                    if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, rsp_len))
                    {
                        DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                        DG_ENGINE_UTIL_rsp_append_data_hton(rsp, min);
                        DG_ENGINE_UTIL_rsp_append_data_hton(rsp, max);
                    }
                }
            }
            break;

        case DG_FAN_ACTION_SET_PWM:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(DG_CMN_DRV_FAN_PWM_T), rsp))
            {
                DG_CMN_DRV_FAN_PWM_T pwm;

                DG_ENGINE_UTIL_req_parse_data_ntoh(req, pwm);

                if (!DG_CMN_DRV_FAN_set_pwm(fan, pwm))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to set FAN PWM");
                }
                else
                {
                    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                }
            }
            break;

        case DG_FAN_ACTION_SET_PWM_MAX:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(DG_CMN_DRV_FAN_PWM_T), rsp))
            {
                DG_CMN_DRV_FAN_PWM_T max;

                DG_ENGINE_UTIL_req_parse_data_ntoh(req, max);

                if (!DG_CMN_DRV_FAN_set_pwm_max(fan, max))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to set FAN PWM MAX");
                }
                else
                {
                    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                }
            }
            break;

        case DG_FAN_ACTION_GET_STATUS:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, 0, rsp))
            {
                DG_CMN_DRV_FAN_STATUS_T status;

                if (!DG_CMN_DRV_FAN_get_status(fan, &status))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to get FAN status");
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

