#ifndef _DG_CMN_DRV_FAN_H_
#define _DG_CMN_DRV_FAN_H_
/*==================================================================================================

    Module Name:  dg_cmn_drv_fan.h

    General Description: This file provides driver interface for FAN test

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup FAN_driver
@{

@par
Provide APIs for FAN control
*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                              MACROS
==================================================================================================*/
#define DG_CMN_DRV_FAN_PWM_MAX 100 /** the max percentage is 100% */

/*==================================================================================================
                                               ENUMS
==================================================================================================*/

/** fan selection */
enum
{
    DG_CMN_DRV_FAN_ID_0   = 0x00,
    DG_CMN_DRV_FAN_ID_1   = 0x01,
    DG_CMN_DRV_FAN_ID_2   = 0x02,
    DG_CMN_DRV_FAN_ID_3   = 0x03,
    DG_CMN_DRV_FAN_ID_MAX = 0x03
};
typedef UINT8 DG_CMN_DRV_FAN_ID_T;

/** fan status */
enum
{
    DG_CMN_DRV_FAN_STATUS_NO_ERROR   = 0x00,
    DG_CMN_DRV_FAN_STATUS_NO_PRESENT = 0x01,
    DG_CMN_DRV_FAN_STATUS_ERROR      = 0x02,
};
typedef UINT8 DG_CMN_DRV_FAN_STATUS_T;

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef UINT16 DG_CMN_DRV_FAN_RPM_T;
typedef UINT8  DG_CMN_DRV_FAN_PWM_T;

/*==================================================================================================
                                   GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*=============================================================================================*//**
@brief Get current RPM of the fan

@param[in]  fan - the selected fan
@param[out] rpm - The RPM of the fan

*//*==============================================================================================*/
BOOL DG_CMN_DRV_FAN_get_rpm(DG_CMN_DRV_FAN_ID_T fan, DG_CMN_DRV_FAN_RPM_T* rpm);

/*=============================================================================================*//**
@brief Get RPM limitation of the fan

@param[in]  fan - the selected fan
@param[out] min - The minimum RPM of the fan
@param[out] max - The maximum RPM of the fan

*//*==============================================================================================*/
BOOL DG_CMN_DRV_FAN_get_rpm_limit(DG_CMN_DRV_FAN_ID_T   fan,
                                  DG_CMN_DRV_FAN_RPM_T* min,
                                  DG_CMN_DRV_FAN_RPM_T* max);

/*=============================================================================================*//**
@brief Set RPM limitation of the fan

@param[in] fan - the selected fan
@param[in] min - The minimum RPM of the fan
@param[in] max - The maximum RPM of the fan

*//*==============================================================================================*/
BOOL DG_CMN_DRV_FAN_set_rpm_limit(DG_CMN_DRV_FAN_ID_T  fan,
                                  DG_CMN_DRV_FAN_RPM_T min,
                                  DG_CMN_DRV_FAN_RPM_T max);

/*=============================================================================================*//**
@brief Get PWM Duty Cycle of the fan

@param[in]  fan - the selected fan
@param[out] pwm - PWM Duty Cycle Percentage

*//*==============================================================================================*/
BOOL DG_CMN_DRV_FAN_get_pwm(DG_CMN_DRV_FAN_ID_T fan, DG_CMN_DRV_FAN_PWM_T* pwm);

/*=============================================================================================*//**
@brief Set PWM Duty Cycle of the fan

@param[in] fan - the selected fan
@param[in] pwm - PWM Duty Cycle Percentage. Valid rang (0~ PWM MAX Duty Cycle)%

*//*==============================================================================================*/
BOOL DG_CMN_DRV_FAN_set_pwm(DG_CMN_DRV_FAN_ID_T fan, DG_CMN_DRV_FAN_PWM_T pwm);

/*=============================================================================================*//**
@brief Set PWM MAX Duty Cycle of the fan

@param[in]  fan - the selected fan
@param[out] max - max PWM Duty Cycle Percentage. Valid range(0~100)%


*//*==============================================================================================*/
BOOL DG_CMN_DRV_FAN_set_pwm_max(DG_CMN_DRV_FAN_ID_T fan, DG_CMN_DRV_FAN_PWM_T max);

/*=============================================================================================*//**
@brief Get status of the fan

@param[in]  fan    - the selected fan
@param[out] status - The status of the fan

*//*==============================================================================================*/
BOOL DG_CMN_DRV_FAN_get_status(DG_CMN_DRV_FAN_ID_T fan, DG_CMN_DRV_FAN_STATUS_T* status);

#ifdef __cplusplus
}
#endif

/** @} */
/** @} */

#endif /* _DG_CMN_DRV_FAN_H_  */

