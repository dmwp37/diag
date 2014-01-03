#ifndef _DG_CMN_DRV_LED_H_
#define _DG_CMN_DRV_LED_H_
/*==================================================================================================

    Module Name:  dg_cmn_drv_led.h

    General Description: This file provides driver interface for LED test

====================================================================================================

Revision History:
                            Modification     Tracking
Author                          Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
Xudong Huang    - xudongh    2013/12/30     xxxxx-0000   Creation

====================================================================================================
                                         INCLUDE FILES
==================================================================================================*/
/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup LED_driver
@{

@par
Provide APIs for LED
*/

#include "dg_cmn_drv_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                             ENUMS
==================================================================================================*/
enum
{
    DG_CMN_DRV_LED_POWER   = 0x00,
    DG_CMN_DRV_LED_STATUS  = 0x01,
    DG_CMN_DRV_LED_ALARM   = 0x02,
    DG_CMN_DRV_LED_HA      = 0x03,
    DG_CMN_DRV_LED_STORAGE = 0x04,
    DG_CMN_DRV_LED_RPS     = 0x05,
    DG_CMN_DRV_LED_SFP0    = 0x10,
    DG_CMN_DRV_LED_SFP1    = 0x11,
    DG_CMN_DRV_LED_SFP2    = 0x12,
    DG_CMN_DRV_LED_SFP3    = 0x13,
    DG_CMN_DRV_LED_SFP4    = 0x14,
    DG_CMN_DRV_LED_SFP5    = 0x15,
    DG_CMN_DRV_LED_SFP6    = 0x16,
    DG_CMN_DRV_LED_SFP7    = 0x17,
};
typedef UINT8 DG_CMN_DRV_LED_ID_T;


enum
{
    DG_CMN_DRV_LED_COLOR_DEFAULT = 0x00,
    DG_CMN_DRV_LED_COLOR_RED     = 0x01,
    DG_CMN_DRV_LED_COLOR_GREEN   = 0x02,
    DG_CMN_DRV_LED_COLOR_YELLOW  = 0x03,
};
typedef UINT8 DG_CMN_DRV_LED_COLOR_T;


/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
/*=============================================================================================*//**
@brief Turn on the LED with specified color

@param[in] led_id    - which LED to control
@param[in] led_color - the color needs to be set
*//*==============================================================================================*/
DG_CMN_DRV_ERR_T DG_CMN_DRV_LED_enable(DG_CMN_DRV_LED_ID_T led_id, DG_CMN_DRV_LED_COLOR_T led_color);

/*=============================================================================================*//**
@brief Turn off the LED

@param[in] led_id - which LED to control
*//*==============================================================================================*/
DG_CMN_DRV_ERR_T DG_CMN_DRV_LED_disable(DG_CMN_DRV_LED_ID_T led_id);

/** @} */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _DG_CMN_DRV_LED_H_  */

