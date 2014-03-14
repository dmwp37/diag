/*==================================================================================================

    Module Name:  dg_cmn_drv_fan.c

    General Description: Implements the FAN common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_fan.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup FAN_driver
@{
implementation of the FAN driver
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
static DG_CMN_DRV_FAN_PWM_T dg_cmn_drv_fan_pwm_max[4] = { 90 }; /* 4 fans max pwm simulation */

/*==================================================================================================
                                         GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Get current RPM of the fan

@param[in]  fan - the selected fan
@param[out] rpm - The RPM of the fan

*//*==============================================================================================*/
BOOL DG_CMN_DRV_FAN_get_rpm(DG_CMN_DRV_FAN_ID_T fan, DG_CMN_DRV_FAN_RPM_T* rpm)
{
    BOOL ret = FALSE;

    if (fan > DG_CMN_DRV_FAN_ID_MAX)
    {
        DG_DRV_UTIL_set_error_string("Invalid fan=%d", fan);
    }
    else
    {
        *rpm = 1000;
        DG_DBG_TRACE("FAN %d got RPM: %d", fan, *rpm);
        ret = TRUE;
    }

    return ret;
}

/*=============================================================================================*//**
@brief Get RPM limitation of the fan

@param[in]  fan - the selected fan
@param[out] min - The minimum RPM of the fan
@param[out] max - The maximum RPM of the fan

*//*==============================================================================================*/
BOOL DG_CMN_DRV_FAN_get_rpm_limit(DG_CMN_DRV_FAN_ID_T   fan,
                                  DG_CMN_DRV_FAN_RPM_T* min,
                                  DG_CMN_DRV_FAN_RPM_T* max)
{
    BOOL ret = FALSE;

    if (fan > DG_CMN_DRV_FAN_ID_MAX)
    {
        DG_DRV_UTIL_set_error_string("Invalid fan=%d", fan);
    }
    else
    {
        *min = 500;
        *max = 3000;
        DG_DBG_TRACE("FAN %d got RPM limit: min=%d, max=%d", fan, *min, *max);
        ret = TRUE;
    }

    return ret;
}

/*=============================================================================================*//**
@brief Set PWM Duty Cycle of the fan

@param[in]  fan - the selected fan
@param[out] pwm - PWM Duty Cycle Percentage. Valid rang (0~ PWM MAX Duty Cycle)%

*//*==============================================================================================*/
BOOL DG_CMN_DRV_FAN_set_pwm(DG_CMN_DRV_FAN_ID_T fan, DG_CMN_DRV_FAN_PWM_T pwm)
{
    BOOL ret = FALSE;

    if (fan > DG_CMN_DRV_FAN_ID_MAX)
    {
        DG_DRV_UTIL_set_error_string("Invalid fan=%d", fan);
    }
    else if (pwm > DG_CMN_DRV_FAN_PWM_MAX)
    {
        DG_DRV_UTIL_set_error_string("FAN PWM Percentage invalid. PWM=%d%%", pwm);
    }
    else if (pwm > dg_cmn_drv_fan_pwm_max[fan])
    {
        DG_DRV_UTIL_set_error_string("FAN PWM Percentage exceeds. PWM=%d%%, MAX_PWM=%d%%",
                                     pwm, dg_cmn_drv_fan_pwm_max[fan]);
    }
    else
    {
        DG_DBG_TRACE("FAN %d set PWM: PWM=%d%%", fan, pwm);

        ret = TRUE;
    }

    return ret;
}

/*=============================================================================================*//**
@brief Set PWM MAX Duty Cycle of the fan

@param[in]  fan - the selected fan
@param[out] max - max PWM Duty Cycle Percentage. Valid range(0~100)%


*//*==============================================================================================*/
BOOL DG_CMN_DRV_FAN_set_pwm_max(DG_CMN_DRV_FAN_ID_T fan, DG_CMN_DRV_FAN_PWM_T max)
{
    BOOL ret = FALSE;

    if (fan > DG_CMN_DRV_FAN_ID_MAX)
    {
        DG_DRV_UTIL_set_error_string("Invalid fan=%d", fan);
    }
    else if (max > DG_CMN_DRV_FAN_PWM_MAX)
    {
        DG_DRV_UTIL_set_error_string("FAN MAX PWM Percentage invalid. MAX_PWM=%d%%", max);
    }
    else
    {
        dg_cmn_drv_fan_pwm_max[fan] = max;
        DG_DBG_TRACE("FAN %d set MAX PWM: MAX_PWM=%d%%", fan, max);

        ret = TRUE;
    }

    return ret;
}

/*=============================================================================================*//**
@brief Get status of the fan

@param[in]  fan    - the selected fan
@param[out] status - The status of the fan

*//*==============================================================================================*/
BOOL DG_CMN_DRV_FAN_get_status(DG_CMN_DRV_FAN_ID_T fan, DG_CMN_DRV_FAN_STATUS_T* status)
{
    BOOL ret = FALSE;

    if (fan > DG_CMN_DRV_FAN_ID_MAX)
    {
        DG_DRV_UTIL_set_error_string("Invalid fan=%d", fan);
    }
    else
    {
        *status = DG_CMN_DRV_FAN_STATUS_NO_ERROR;
        DG_DBG_TRACE("FAN %d got status: status=%d", fan, *status);
        ret = TRUE;
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

