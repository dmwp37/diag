#ifndef _DG_CMN_DRV_PORT_LED_H_
#define _DG_CMN_DRV_PORT_LED_H_
/*==================================================================================================

    Module Name:  dg_cmn_drv_port_led.h

    General Description: This file provides driver interface for port LED test

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup PORT_LED_driver
@{

@par
Provide APIs for PORT_LED
*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                              MACROS
==================================================================================================*/

/*==================================================================================================
                                               ENUMS
==================================================================================================*/
enum
{
    DG_CMN_DRV_PORT_LED_ACTION_NORMAL   = 0x00,
    DG_CMN_DRV_PORT_LED_ACTION_TURN_ON  = 0x01,
    DG_CMN_DRV_PORT_LED_ACTION_TURN_OFF = 0x02,
    DG_CMN_DRV_PORT_LED_ACTION_BLINK    = 0x03,
};
typedef UINT8 DG_CMN_DRV_PORT_LED_ACTION_T;

enum
{
    DG_CMN_DRV_PORT_LED_ID_LINK   = 0x00,
    DG_CMN_DRV_PORT_LED_ID_ACTIVE = 0x01,
};
typedef UINT8 DG_CMN_DRV_PORT_LED_ID_T;

enum
{
    DG_CMN_DRV_PORT_LED_PORT_MGT        = 0x00,
    DG_CMN_DRV_PORT_LED_PORT_HA         = 0x01,
    DG_CMN_DRV_PORT_LED_PORT_GE_0_0     = 0x10,
    DG_CMN_DRV_PORT_LED_PORT_GE_0_1     = 0x11,
    DG_CMN_DRV_PORT_LED_PORT_GE_0_2     = 0x12,
    DG_CMN_DRV_PORT_LED_PORT_GE_0_3     = 0x13,
    DG_CMN_DRV_PORT_LED_PORT_GE_0_4     = 0x14,
    DG_CMN_DRV_PORT_LED_PORT_GE_0_5     = 0x15,
    DG_CMN_DRV_PORT_LED_PORT_GE_0_6     = 0x16,
    DG_CMN_DRV_PORT_LED_PORT_GE_0_7     = 0x17,
    DG_CMN_DRV_PORT_LED_PORT_GE_0_A     = 0x18,
    DG_CMN_DRV_PORT_LED_PORT_GE_0_9     = 0x19,
    DG_CMN_DRV_PORT_LED_PORT_GE_0_10    = 0x1A,
    DG_CMN_DRV_PORT_LED_PORT_GE_0_11    = 0x1B,
    DG_CMN_DRV_PORT_LED_PORT_SFP_0_12   = 0x1C,
    DG_CMN_DRV_PORT_LED_PORT_SFP_0_13   = 0x1D,
    DG_CMN_DRV_PORT_LED_PORT_SFP_0_14   = 0x1E,
    DG_CMN_DRV_PORT_LED_PORT_SFP_0_15   = 0x1F,
    DG_CMN_DRV_PORT_LED_PORT_SFP_0_16   = 0x20,
    DG_CMN_DRV_PORT_LED_PORT_SFP_0_17   = 0x21,
    DG_CMN_DRV_PORT_LED_PORT_SFP_0_18   = 0x22,
    DG_CMN_DRV_PORT_LED_PORT_SFP_0_19   = 0x23,
    DG_CMN_DRV_PORT_LED_PORT_WANPIM_1_0 = 0x24,
    DG_CMN_DRV_PORT_LED_PORT_WANPIM_1_1 = 0x25,
    DG_CMN_DRV_PORT_LED_PORT_WANPIM_2_0 = 0x26,
    DG_CMN_DRV_PORT_LED_PORT_WANPIM_2_1 = 0x27,
};
typedef UINT8 DG_CMN_DRV_PORT_LED_PORT_T;

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/


/*==================================================================================================
                                   GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*=============================================================================================*//**
@brief operate the leds of the interface ports

@param[in] action - operations: Normal, Turn On, Turn Off, Blink
@param[in] id     - LED selection: Link/Active
@param[in] port   - interface select
*//*==============================================================================================*/
BOOL DG_CMN_DRV_PORT_LED_operate(DG_CMN_DRV_PORT_LED_ACTION_T action,
                                 DG_CMN_DRV_PORT_LED_ID_T     led,
                                 DG_CMN_DRV_PORT_LED_PORT_T   port);

#ifdef __cplusplus
}
#endif

/** @} */
/** @} */

#endif /* _DG_CMN_DRV_PORT_LED_H_  */

