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
#include "dg_cmn_drv_i2c.h"


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
#define DG_CMN_DRV_FAN_I2C_BUS        DG_CMN_DRV_I2C_MUX_CPU
#define DG_CMN_DRV_FAN_I2C_ADDR       0x2c /** ADT7470 I2C address */

/* ADT7470 registers, just use the ADT7470 naming conversion */
#define ADT7470_REG_FAN_NO_PRESENT    0x36
#define ADT7470_REG_FAN_STATUS        0x42

#define ADT7470_REG_FAN_BASE_ADDR     0x2A
#define ADT7470_REG_FAN_MIN_BASE_ADDR 0x58
#define ADT7470_REG_FAN_MAX_BASE_ADDR 0x60

#define ADT7470_REG_FAN(x)            (ADT7470_REG_FAN_BASE_ADDR + ((x) * 2))
#define ADT7470_REG_FAN_MIN(x)        (ADT7470_REG_FAN_MIN_BASE_ADDR + ((x) * 2))
#define ADT7470_REG_FAN_MAX(x)        (ADT7470_REG_FAN_MAX_BASE_ADDR + ((x) * 2))

#define ADT7470_REG_PWM_BASE_ADDR     0x32
#define ADT7470_REG_PWM_MAX_BASE_ADDR 0x38
#define ADT7470_REG_PWM_MIN_BASE_ADDR 0x6A

#define ADT7470_REG_PWM(x)            (ADT7470_REG_PWM_BASE_ADDR + (x))
#define ADT7470_REG_PWM_MAX(x)        (ADT7470_REG_PWM_MAX_BASE_ADDR + (x))
#define ADT7470_REG_PWM_MIN(x)        (ADT7470_REG_PWM_MIN_BASE_ADDR + (x))

/* Fan Speed (RPM) = (90,000 * 60 )/Fan Tach Reading*/
#define ADT7470_PERIOD_TO_RPM(x)      ((UINT16)((90000 * 60) / (x)))
#define ADT7470_RPM_TO_PERIOD         ADT7470_PERIOD_TO_RPM
/* Fan PWM = percentage * 255 / 100 */
#define ADT7470_PERCENTAGE_TO_PWM(x)  ((UINT8)(2.55 * (x)))
#define ADT7470_PWM_TO_PERCENTAGE(x)  ((UINT8)(0.392 * (x)))

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
/** just use the ADT7470 naming conversion for the low level driver interface */
static BOOL adt7470_read_byte(UINT8 reg, UINT8* p_data);
static BOOL adt7470_write_byte(UINT8 reg, UINT8 value);
static BOOL adt7470_read_short(UINT8 reg, UINT16* p_data);
static BOOL adt7470_write_short(UINT8 reg, UINT16 value);

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
        UINT16 tach;
        if (adt7470_read_short(ADT7470_REG_FAN(fan), &tach))
        {
            if ((tach != 0))
            {
                *rpm = ADT7470_PERIOD_TO_RPM(tach);
                DG_DBG_TRACE("FAN %d got RPM: %d", fan, *rpm);

                ret = TRUE;
            }
            else
            {
                DG_DRV_UTIL_set_error_string("Invalid tach value, tach=0");
            }
        }
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
        UINT16 tach_min;
        UINT16 tach_max;
        if (adt7470_read_short(ADT7470_REG_FAN_MIN(fan), &tach_min) &&
            adt7470_read_short(ADT7470_REG_FAN_MAX(fan), &tach_max))
        {
            if ((tach_min != 0) && (tach_max != 0))
            {
                *min = ADT7470_PERIOD_TO_RPM(tach_min);
                *min = ADT7470_PERIOD_TO_RPM(tach_max);

                DG_DBG_TRACE("FAN %d got RPM limit: min=%d, max=%d", fan, *min, *max);

                ret = TRUE;
            }
            else
            {
                DG_DRV_UTIL_set_error_string("Invalid tach limit values, tach_min=%d, tach_max=%d",
                                             tach_min, tach_max);
            }
        }
    }

    return ret;
}

/*=============================================================================================*//**
@brief Set RPM limitation of the fan

@param[in] fan - the selected fan
@param[in] min - The minimum RPM of the fan
@param[in] max - The maximum RPM of the fan

*//*==============================================================================================*/
BOOL DG_CMN_DRV_FAN_set_rpm_limit(DG_CMN_DRV_FAN_ID_T  fan,
                                  DG_CMN_DRV_FAN_RPM_T min,
                                  DG_CMN_DRV_FAN_RPM_T max)
{
    BOOL ret = FALSE;

    if (fan > DG_CMN_DRV_FAN_ID_MAX)
    {
        DG_DRV_UTIL_set_error_string("Invalid fan=%d", fan);
    }
    else if ((min == 0) || (max == 0) || (min > max))
    {
        DG_DRV_UTIL_set_error_string("Invalid RPM limit: min=%d, max=%d", min, max);
    }
    else
    {
        UINT16 tach_min = ADT7470_RPM_TO_PERIOD(min);
        UINT16 tach_max = ADT7470_RPM_TO_PERIOD(max);

        if (adt7470_write_short(ADT7470_REG_FAN_MIN(fan), tach_min) &&
            adt7470_write_short(ADT7470_REG_FAN_MAX(fan), tach_max))
        {
            DG_DBG_TRACE("FAN %d set RPM limit: min=%d, max=%d", fan, min, max);
            ret = TRUE;
        }
    }

    return ret;
}

/*=============================================================================================*//**
@brief Get PWM Duty Cycle of the fan

@param[in]  fan - the selected fan
@param[out] pwm - PWM Duty Cycle Percentage

*//*==============================================================================================*/
BOOL DG_CMN_DRV_FAN_get_pwm(DG_CMN_DRV_FAN_ID_T fan, DG_CMN_DRV_FAN_PWM_T* pwm)
{
    BOOL ret = FALSE;

    if (fan > DG_CMN_DRV_FAN_ID_MAX)
    {
        DG_DRV_UTIL_set_error_string("Invalid fan=%d", fan);
    }
    else
    {
        UINT8 pwm_data;

        if (adt7470_read_byte(ADT7470_REG_PWM(fan), &pwm_data))
        {
            *pwm = ADT7470_PWM_TO_PERCENTAGE(pwm_data);

            DG_DBG_TRACE("FAN %d get PWM: PWM=%d%%", fan, *pwm);

            ret = TRUE;
        }
    }

    return ret;
}

/*=============================================================================================*//**
@brief Set PWM Duty Cycle of the fan

@param[in] fan - the selected fan
@param[in] pwm - PWM Duty Cycle Percentage. Valid rang (0~ PWM MAX Duty Cycle)%

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
    else
    {
        UINT8 pwm_data = ADT7470_PERCENTAGE_TO_PWM(pwm);

        if (adt7470_write_byte(ADT7470_REG_PWM(fan), pwm_data))
        {
            DG_DBG_TRACE("FAN %d set PWM: PWM=%d%%", fan, pwm);

            ret = TRUE;
        }
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
        UINT8 pwm_data = ADT7470_PERCENTAGE_TO_PWM(max);

        if (adt7470_write_byte(ADT7470_REG_PWM_MAX(fan), pwm_data))
        {
            DG_DBG_TRACE("FAN %d set MAX PWM: MAX_PWM=%d%%", fan, max);

            ret = TRUE;
        }
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
        UINT8 no_present;
        UINT8 error;
        if (adt7470_read_byte(ADT7470_REG_FAN_NO_PRESENT, &no_present) &&
            adt7470_read_byte(ADT7470_REG_FAN_STATUS, &error))
        {
            if (no_present & (1 << fan))
            {
                *status = DG_CMN_DRV_FAN_STATUS_NO_PRESENT;
                DG_DBG_TRACE("FAN %d no present", fan);
            }
            else if (error & (0x10 << fan))
            {
                *status = DG_CMN_DRV_FAN_STATUS_ERROR;

                DG_DBG_TRACE("FAN %d error detected", fan);
            }
            else
            {
                *status = DG_CMN_DRV_FAN_STATUS_NO_ERROR;

                DG_DBG_TRACE("FAN %d no error", fan);
            }

            ret = TRUE;
        }
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/
/*=============================================================================================*//**
@brief Read byte from ADT7470

@param[in]  reg    - the register address
@param[out] p_data - The read out byte

*//*==============================================================================================*/
BOOL adt7470_read_byte(UINT8 reg, UINT8* p_data)
{
    return DG_CMN_DRV_I2C_read_bus(DG_CMN_DRV_FAN_I2C_BUS,
                                   DG_CMN_DRV_FAN_I2C_ADDR,
                                   reg, 1, p_data);
}

/*=============================================================================================*//**
@brief Write byte to ADT7470

@param[in] reg   - the register address
@param[in] value - The byte value to write

*//*==============================================================================================*/
BOOL adt7470_write_byte(UINT8 reg, UINT8 value)
{
    return DG_CMN_DRV_I2C_write_bus(DG_CMN_DRV_FAN_I2C_BUS,
                                    DG_CMN_DRV_FAN_I2C_ADDR,
                                    reg, 1, &value);
}

/*=============================================================================================*//**
@brief Read word from ADT7470

@param[in]  reg    - the register address
@param[out] p_data - The read out word

*//*==============================================================================================*/
BOOL adt7470_read_short(UINT8 reg, UINT16* p_data)
{
    UINT8 data[2];

    BOOL ret = DG_CMN_DRV_I2C_read_bus(DG_CMN_DRV_FAN_I2C_BUS,
                                       DG_CMN_DRV_FAN_I2C_ADDR,
                                       reg, 2, data);
    if (ret)
    {
        *p_data  = data[0];
        *p_data |= ((UINT16)data[1] << 8);
    }

    return ret;
}

/*=============================================================================================*//**
@brief Write word to ADT7470

@param[in] reg   - the register address
@param[in] value - The word value to write

*//*==============================================================================================*/
BOOL adt7470_write_short(UINT8 reg, UINT16 value)
{
    UINT8 data[2];

    data[0] = (UINT8)(value & 0xff);
    data[1] = (UINT8)(value >> 8);

    return DG_CMN_DRV_I2C_write_bus(DG_CMN_DRV_FAN_I2C_BUS,
                                    DG_CMN_DRV_FAN_I2C_ADDR,
                                    reg, 2, data);
}

/** @} */
/** @} */

