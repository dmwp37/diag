/*==================================================================================================

    Module Name:  dg_cmn_drv_led.c

    General Description: Implements the LED common driver

====================================================================================================

Revision History:
                            Modification     Tracking
Author                          Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
Xudong Huang    - xudongh    2013/12/30     xxxxx-0000   Creation

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_led.h"

/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup LED_driver
@{
implementation of the LED driver
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
@brief Turn on the LED with specified color

@param[in] led_id    - which LED to control
@param[in] led_color - the color needs to be set
*//*==============================================================================================*/
DG_CMN_DRV_ERR_T DG_CMN_DRV_LED_enable(DG_CMN_DRV_LED_ID_T led_id, DG_CMN_DRV_LED_COLOR_T led_color)
{
    DG_CMN_DRV_ERR_T err = DG_CMN_DRV_ERR_FAIL;

    switch (led_id)
    {
    case DG_CMN_DRV_LED_POWER:
    case DG_CMN_DRV_LED_STATUS:
    case DG_CMN_DRV_LED_ALARM:
    case DG_CMN_DRV_LED_HA:
    case DG_CMN_DRV_LED_STORAGE:
    case DG_CMN_DRV_LED_RPS:
    case DG_CMN_DRV_LED_SFP0:
    case DG_CMN_DRV_LED_SFP1:
    case DG_CMN_DRV_LED_SFP2:
    case DG_CMN_DRV_LED_SFP3:
    case DG_CMN_DRV_LED_SFP4:
    case DG_CMN_DRV_LED_SFP5:
    case DG_CMN_DRV_LED_SFP6:
    case DG_CMN_DRV_LED_SFP7:
        switch (led_color)
        {
        case DG_CMN_DRV_LED_COLOR_DEFAULT:
        case DG_CMN_DRV_LED_COLOR_RED:
        case DG_CMN_DRV_LED_COLOR_GREEN:
        case DG_CMN_DRV_LED_COLOR_YELLOW:
            err = DG_CMN_DRV_ERR_NONE;
            DG_DBG_TRACE("Enable LED driver: led_id=0x%02x, led_color=0x%02x",
                         led_id, led_color);
            break;

        default:
            break;
        }
        break;

    default:
        break;
    }

    if (err != DG_CMN_DRV_ERR_NONE)
    {
        DG_DRV_UTIL_set_error_string("Enable LED failed: led_id=0x%02x, led_color=0x%02x",
                                     led_id, led_color);
    }

    return err;
}

/*=============================================================================================*//**
@brief Turn off the LED

@param[in] led_id - which LED to control
*//*==============================================================================================*/
DG_CMN_DRV_ERR_T DG_CMN_DRV_LED_disable(DG_CMN_DRV_LED_ID_T led_id)
{
    DG_CMN_DRV_ERR_T err = DG_CMN_DRV_ERR_FAIL;

    switch (led_id)
    {
    case DG_CMN_DRV_LED_POWER:
    case DG_CMN_DRV_LED_STATUS:
    case DG_CMN_DRV_LED_ALARM:
    case DG_CMN_DRV_LED_HA:
    case DG_CMN_DRV_LED_STORAGE:
    case DG_CMN_DRV_LED_RPS:
    case DG_CMN_DRV_LED_SFP0:
    case DG_CMN_DRV_LED_SFP1:
    case DG_CMN_DRV_LED_SFP2:
    case DG_CMN_DRV_LED_SFP3:
    case DG_CMN_DRV_LED_SFP4:
    case DG_CMN_DRV_LED_SFP5:
    case DG_CMN_DRV_LED_SFP6:
    case DG_CMN_DRV_LED_SFP7:
        err = DG_CMN_DRV_ERR_NONE;
        DG_DBG_TRACE("Disable LED driver: led_id=0x%02x", led_id);
        break;

    default:
        break;
    }


    if (err != DG_CMN_DRV_ERR_NONE)
    {
        DG_DRV_UTIL_set_error_string("Disable LED failed: led_id=0x%02x", led_id);
    }

    return err;
}

/*==================================================================================================
                                     LOCAL FUNCTION
==================================================================================================*/


/** @} */
/** @} */

