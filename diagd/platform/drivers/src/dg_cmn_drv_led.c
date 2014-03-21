/*==================================================================================================

    Module Name:  dg_cmn_drv_led.c

    General Description: Implements the LED common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_led.h"
#include "dg_cmn_drv_cpld.h"


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
/** the address of the led register */
static UINT16 dg_cmn_drv_led_reg_addr[] =
{
    0x0002, /* power  */
    0x0002, /* status */
    0x0002, /* alarm  */
    0x0002, /* HA     */
    0x0003, /* SSD    */
    0x0003  /* RPS    */
};

/** the 2 bits shift of the led mask */
/** mask = 0x03 << offset */
static UINT8 dg_cmn_drv_led_mask_shift[] =
{
    0x06, /* power  */
    0x02, /* status */
    0x04, /* alarm  */
    0x00, /* HA     */
    0x02, /* SSD    */
    0x00  /* RPS    */
};

/** the color value of the led register */
static UINT8 dg_cmn_drv_led_color[] =
{
    0x01, /* default */
    0x01, /* green   */
    0x02, /* red     */
    0x03  /* orange  */
};

/*==================================================================================================
                                         GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Turn on the LED with specified color

@param[in] led_id    - which LED to control
@param[in] led_color - the color needs to be set
*//*==============================================================================================*/
BOOL DG_CMN_DRV_LED_enable(DG_CMN_DRV_LED_ID_T led_id, DG_CMN_DRV_LED_COLOR_T led_color)
{
    BOOL  ret = FALSE;
    UINT8 value;

    if (led_id >= DG_ARRAY_SIZE(dg_cmn_drv_led_reg_addr))
    {
        DG_DRV_UTIL_set_error_string("Invalid led: led_id=0x%02x", led_id);
    }
    else if (led_color >= DG_ARRAY_SIZE(dg_cmn_drv_led_color))
    {
        DG_DRV_UTIL_set_error_string("Invalid led color: led_color=0x%02x", led_color);
    }
    else if (!DG_CMN_DRV_CPLD_get(DG_CMN_DRV_CPLD_FEB,
                                  dg_cmn_drv_led_reg_addr[led_id], &value))
    {
        DG_DBG_ERROR("can't get CPLD led register");
    }
    else
    {
        /* clear the bits and then set the bit */
        value &= (0x03 << dg_cmn_drv_led_mask_shift[led_id]);
        value |= (dg_cmn_drv_led_color[led_color] << dg_cmn_drv_led_mask_shift[led_id]);

        if (!DG_CMN_DRV_CPLD_set(DG_CMN_DRV_CPLD_FEB,
                                 dg_cmn_drv_led_reg_addr[led_id],
                                 value))
        {
            DG_DBG_ERROR("can't set CPLD led register");
        }
        else
        {
            DG_DBG_TRACE("Enable LED: led_id=0x%02x, led_color=0x%02x",
                         led_id, led_color);
            ret = TRUE;
        }
    }

    return ret;
}

/*=============================================================================================*//**
@brief Turn off the LED

@param[in] led_id - which LED to control
*//*==============================================================================================*/
BOOL DG_CMN_DRV_LED_disable(DG_CMN_DRV_LED_ID_T led_id)
{
    BOOL  ret = FALSE;
    UINT8 value;

    if (led_id >= DG_ARRAY_SIZE(dg_cmn_drv_led_reg_addr))
    {
        DG_DRV_UTIL_set_error_string("Invalid led: led_id=0x%02x", led_id);
    }
    else if (!DG_CMN_DRV_CPLD_get(DG_CMN_DRV_CPLD_FEB,
                                  dg_cmn_drv_led_reg_addr[led_id], &value))
    {
        DG_DBG_ERROR("can't get CPLD led register");
    }
    else
    {
        /* clear the bit of the led */
        value &= (0x03 << dg_cmn_drv_led_mask_shift[led_id]);

        if (!DG_CMN_DRV_CPLD_set(DG_CMN_DRV_CPLD_FEB,
                                 dg_cmn_drv_led_reg_addr[led_id],
                                 value))
        {
            DG_DBG_ERROR("can't set CPLD led register");
        }
        else
        {
            DG_DBG_TRACE("Disable LED: led_id=0x%02x", led_id);
            ret = TRUE;
        }
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

