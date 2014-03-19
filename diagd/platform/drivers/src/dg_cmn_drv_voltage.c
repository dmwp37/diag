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

    float voltage = 3.73;

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
        voltage += chip;
        voltage += channel / 10.0;

        DG_DBG_TRACE("VOLTAGE chip %d channel %d got voltage: %+.2fV", chip, channel, voltage);
        *data = *(UINT32*)&voltage;

        ret = TRUE;
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

