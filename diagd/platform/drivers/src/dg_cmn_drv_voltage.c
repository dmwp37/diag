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
    { 6.0, 2.0, 2.0, 1.0, 1.0, 1.0, 1.0, 3.0 },
    { 3.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 },
    { 6.0, 2.0, 2.0, 1.0, 1.0, 1.0, 1.0, 3.0 },
    { 1.1, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0 },
    { 6.0, 2.0, 2.0, 1.0, 1.0, 3.0, 1.0, 3.0 },
    { 6.0, 2.0, 2.0, 1.0, 1.0, 3.0, 1.0, 3.0 }
};
/** chip bus information */
static UINT8 dg_cmn_drv_voltage_chip_bus[] =
{
    DG_CMN_DRV_I2C_PCH_SMB,
    DG_CMN_DRV_I2C_PCH_SMB,
    DG_CMN_DRV_I2C_MUX_FEB,
    DG_CMN_DRV_I2C_MUX_FEB,
    DG_CMN_DRV_I2C_MUX_WTB1,
    DG_CMN_DRV_I2C_MUX_WTB2
};
/** chip address information */
static UINT8 dg_cmn_drv_voltage_chip_addr[] =
{
    0x4A, 0x4B, 0x4A, 0x4B, 0x48, 0x48
};
/** channel address information */
static UINT8 dg_cmn_drv_voltage_channel_addr[] =
{
    0b10001100, 0b11001100, 0b10011100, 0b11011100,
    0b10101100, 0b11101100, 0b10111100, 0b11111100
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
    BOOL ret = FALSE;

    if (level > DG_CMN_DRV_VOLTAGE_LEVEL_MAX)
    {
        DG_DRV_UTIL_set_error_string("VOLTAGE level is not valid. level=%d", level);
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

