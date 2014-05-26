/*==================================================================================================

    Module Name:  dg_cmn_drv_voltage.c

    General Description: Implements the VOLTAGE common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_voltage.h"
#include "dg_cmn_drv_cpld.h"
#include "dg_cmn_drv_i2c.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup VOLTAGE_driver
@{
implementation of the VOLTAGE driver
*/

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/
const float DG_CMN_DRV_VOLTAGE_BASE = 2.5 / 255;   /** the base value of the voltage */

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/
#define CPLD_PWR_CTL_EN         0x0D
#define CPLD_PWR_MARGIN_EN_MASK 0x10
#define CPLD_PWR_MARGIN_CTL     0x12

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
/** coefficient information */
static float dg_cmn_drv_voltage_co[][DG_CMN_DRV_VOLTAGE_CHANNEL_MAX + 1] =
{
    [DG_CMN_DRV_VOLTAGE_CB_0]  = { 6.0, 2.0, 2.0, 1.0, 1.0, 1.0, 1.0, 3.0 },
    [DG_CMN_DRV_VOLTAGE_CB_1]  = { 3.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 },
    [DG_CMN_DRV_VOLTAGE_FEB_0] = { 6.0, 2.0, 2.0, 1.0, 1.0, 1.0, 1.0, 3.0 },
    [DG_CMN_DRV_VOLTAGE_FEB_1] = { 1.1, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 },
    [DG_CMN_DRV_VOLTAGE_WTB_0] = { 6.0, 2.0, 2.0, 1.0, 1.0, 2.0, 1.0, 3.0 },
    [DG_CMN_DRV_VOLTAGE_WTB_1] = { 6.0, 2.0, 2.0, 1.0, 1.0, 2.0, 1.0, 3.0 }
};
/** chip bus information */
static DG_CMN_DRV_I2C_BUS_T dg_cmn_drv_voltage_chip_bus[] =
{
    [DG_CMN_DRV_VOLTAGE_CB_0]  = DG_CMN_DRV_I2C_MUX_CPU,
    [DG_CMN_DRV_VOLTAGE_CB_1]  = DG_CMN_DRV_I2C_MUX_CPU,
    [DG_CMN_DRV_VOLTAGE_FEB_0] = DG_CMN_DRV_I2C_MUX_FEB,
    [DG_CMN_DRV_VOLTAGE_FEB_1] = DG_CMN_DRV_I2C_MUX_FEB,
    [DG_CMN_DRV_VOLTAGE_WTB_0] = DG_CMN_DRV_I2C_MUX_WTB1,
    [DG_CMN_DRV_VOLTAGE_WTB_1] = DG_CMN_DRV_I2C_MUX_WTB2
};
/** chip address information */
static DG_CMN_DRV_I2C_ADDR_T dg_cmn_drv_voltage_chip_addr[] =
{
    [DG_CMN_DRV_VOLTAGE_CB_0]  = 0x4A,
    [DG_CMN_DRV_VOLTAGE_CB_1]  = 0x4B,
    [DG_CMN_DRV_VOLTAGE_FEB_0] = 0x4A,
    [DG_CMN_DRV_VOLTAGE_FEB_1] = 0x4B,
    [DG_CMN_DRV_VOLTAGE_WTB_0] = 0x48,
    [DG_CMN_DRV_VOLTAGE_WTB_1] = 0x48
};
/** channel address information */
static DG_CMN_DRV_I2C_OFFSET_T dg_cmn_drv_voltage_channel_addr[] =
{
    [DG_CMN_DRV_VOLTAGE_CHANNEL_0] = 0b10001100,
    [DG_CMN_DRV_VOLTAGE_CHANNEL_1] = 0b11001100,
    [DG_CMN_DRV_VOLTAGE_CHANNEL_2] = 0b10011100,
    [DG_CMN_DRV_VOLTAGE_CHANNEL_3] = 0b11011100,
    [DG_CMN_DRV_VOLTAGE_CHANNEL_4] = 0b10101100,
    [DG_CMN_DRV_VOLTAGE_CHANNEL_5] = 0b11101100,
    [DG_CMN_DRV_VOLTAGE_CHANNEL_6] = 0b10111100,
    [DG_CMN_DRV_VOLTAGE_CHANNEL_7] = 0b11111100
};

/*==================================================================================================
                                         GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Read voltage from selected chip and channel

@param[in]  chip    - voltage chip selection
@param[in]  channel - voltage channel selection
@param[out] data    - The voltage value in float format

@note
- the data is a UINT32 pointer, but it must contains the exact data as a float
- *data = *(UINT32*)&(float_voltage)
*//*==============================================================================================*/
BOOL DG_CMN_DRV_VOLTAGE_get(DG_CMN_DRV_VOLTAGE_CHIP_T    chip,
                            DG_CMN_DRV_VOLTAGE_CHANNEL_T channel,
                            DG_CMN_DRV_VOLTAGE_DATA_T*   data)
{
    BOOL ret = FALSE;

    if (chip > DG_CMN_DRV_VOLTAGE_CHIP_MAX)
    {
        DG_DRV_UTIL_set_error_string("Invalid chip=%d", chip);
    }
    else if (channel > DG_CMN_DRV_VOLTAGE_CHANNEL_MAX)
    {
        DG_DRV_UTIL_set_error_string("Invalid channel=%d", channel);
    }
    else
    {
        float voltage;
        UINT8 raw_data;

        if (!DG_CMN_DRV_I2C_read_bus(dg_cmn_drv_voltage_chip_bus[chip],
                                     dg_cmn_drv_voltage_chip_addr[chip],
                                     dg_cmn_drv_voltage_channel_addr[channel],
                                     1, &raw_data))
        {
            DG_DRV_UTIL_set_error_string("Failed to get voltage. chip=%d, channel=%d",
                                         chip, channel);
        }
        else
        {
            voltage = raw_data * DG_CMN_DRV_VOLTAGE_BASE * dg_cmn_drv_voltage_co[chip][channel];

            DG_DBG_TRACE("VOLTAGE chip %d channel %d got voltage: %+.2fV", chip, channel, voltage);
            *data = *(UINT32*)&voltage;

            ret = TRUE;
        }
    }

    return ret;
}

/*=============================================================================================*//**
@brief Set system power output level

@param[in] level - the System power output level

*//*==============================================================================================*/
BOOL DG_CMN_DRV_VOLTAGE_set(DG_CMN_DRV_VOLTAGE_LEVEL_T level)
{
    BOOL                    ret = FALSE;
    DG_CMN_DRV_CPLD_VALUE_T value;

    if (level > DG_CMN_DRV_VOLTAGE_LEVEL_MAX)
    {
        DG_DRV_UTIL_set_error_string("VOLTAGE level is not valid. level=%d", level);
    }
    else if (!DG_CMN_DRV_CPLD_get(DG_CMN_DRV_CPLD_CB, CPLD_PWR_CTL_EN, &value))
    {
        DG_DBG_ERROR("can't get PWR enable register");
    }
    else if (!DG_CMN_DRV_CPLD_set(DG_CMN_DRV_CPLD_CB,
                                  CPLD_PWR_CTL_EN,
                                  value | CPLD_PWR_MARGIN_EN_MASK))
    {
        DG_DBG_ERROR("can't set PWR enable register");
    }
    else if (!DG_CMN_DRV_CPLD_set(DG_CMN_DRV_CPLD_CB, CPLD_PWR_MARGIN_CTL, level))
    {
        DG_DBG_ERROR("can't set power margin register");
    }
    else
    {
        DG_DBG_TRACE("set system VOLTAGE level: %d", level);
        ret = TRUE;
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

