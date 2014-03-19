/*==================================================================================================

    Module Name:  dg_cmn_drv_port_led.c

    General Description: Implements the PORT_LED common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_port_led.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup PORT_LED_driver
@{
implementation of the PORT_LED driver
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
@brief operate the leds of the interface ports

@param[in] action - operations: Normal, Turn On, Turn Off, Blink
@param[in] id     - LED selection: Link/Active
@param[in] port   - interface select
*//*==============================================================================================*/
BOOL DG_CMN_DRV_PORT_LED_operate(DG_CMN_DRV_PORT_LED_ACTION_T action,
                                 DG_CMN_DRV_PORT_LED_ID_T     id,
                                 DG_CMN_DRV_PORT_LED_PORT_T   port)
{
    BOOL ret = FALSE;

    switch (port)
    {
    case DG_CMN_DRV_PORT_LED_PORT_MGT:
    case DG_CMN_DRV_PORT_LED_PORT_HA:
    case DG_CMN_DRV_PORT_LED_PORT_GE_0_0:
    case DG_CMN_DRV_PORT_LED_PORT_GE_0_1:
    case DG_CMN_DRV_PORT_LED_PORT_GE_0_2:
    case DG_CMN_DRV_PORT_LED_PORT_GE_0_3:
    case DG_CMN_DRV_PORT_LED_PORT_GE_0_4:
    case DG_CMN_DRV_PORT_LED_PORT_GE_0_5:
    case DG_CMN_DRV_PORT_LED_PORT_GE_0_6:
    case DG_CMN_DRV_PORT_LED_PORT_GE_0_7:
    case DG_CMN_DRV_PORT_LED_PORT_GE_0_A:
    case DG_CMN_DRV_PORT_LED_PORT_GE_0_9:
    case DG_CMN_DRV_PORT_LED_PORT_GE_0_10:
    case DG_CMN_DRV_PORT_LED_PORT_GE_0_11:
    case DG_CMN_DRV_PORT_LED_PORT_SFP_0_12:
    case DG_CMN_DRV_PORT_LED_PORT_SFP_0_13:
    case DG_CMN_DRV_PORT_LED_PORT_SFP_0_14:
    case DG_CMN_DRV_PORT_LED_PORT_SFP_0_15:
    case DG_CMN_DRV_PORT_LED_PORT_SFP_0_16:
    case DG_CMN_DRV_PORT_LED_PORT_SFP_0_17:
    case DG_CMN_DRV_PORT_LED_PORT_SFP_0_18:
    case DG_CMN_DRV_PORT_LED_PORT_SFP_0_19:
    case DG_CMN_DRV_PORT_LED_PORT_WANPIM_1_0:
    case DG_CMN_DRV_PORT_LED_PORT_WANPIM_1_1:
    case DG_CMN_DRV_PORT_LED_PORT_WANPIM_2_0:
    case DG_CMN_DRV_PORT_LED_PORT_WANPIM_2_1:
        switch (action)
        {
        case DG_CMN_DRV_PORT_LED_ACTION_NORMAL:
        case DG_CMN_DRV_PORT_LED_ACTION_TURN_ON:
        case DG_CMN_DRV_PORT_LED_ACTION_TURN_OFF:
        case DG_CMN_DRV_PORT_LED_ACTION_BLINK:
            switch (id)
            {
            case DG_CMN_DRV_PORT_LED_ID_LINK:
            case DG_CMN_DRV_PORT_LED_ID_ACTIVE:
                DG_DBG_TRACE("Port LED operate id:%d action:0x%02x, port:0x%02x", id, action, port);
                ret = TRUE;
                break;

            default:
                DG_DRV_UTIL_set_error_string("Invalid ID 0x%02x", id);
            }
            break;

        default:
            DG_DRV_UTIL_set_error_string("Invalid action 0x%02x", action);
        }
        break;

    default:
        DG_DRV_UTIL_set_error_string("Invalid port 0x%02x", port);
    }

    return ret;
}


/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

