/*==================================================================================================

    Module Name:  dg_cmn_drv_ext_loop.c

    General Description: Implements the EXT_LOOP common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_ext_loop.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup EXT_LOOP_driver
@{
implementation of the EXT_LOOP driver
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
@brief external loopback operate

@param[in]  action   - set/clear
@param[out] node     - node need to be operate

@note
- result is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_EXT_LOOP_operate(DG_CMN_DRV_EXT_LOOP_ACTION_T action,
                                 DG_CMN_DRV_EXT_LOOP_NODE_T   node)
{
    BOOL ret = FALSE;

    switch (node)
    {
    case DG_CMN_DRV_EXT_LOOP_NODE_PCH:
    case DG_CMN_DRV_EXT_LOOP_NODE_FPGA:
    case DG_CMN_DRV_EXT_LOOP_NODE_SWITCH:
    case DG_CMN_DRV_EXT_LOOP_NODE_PHY_MGT:
    case DG_CMN_DRV_EXT_LOOP_NODE_PHY0_P1:
    case DG_CMN_DRV_EXT_LOOP_NODE_PHY0_P2:
    case DG_CMN_DRV_EXT_LOOP_NODE_PHY0_P3:
    case DG_CMN_DRV_EXT_LOOP_NODE_PHY0_P4:
    case DG_CMN_DRV_EXT_LOOP_NODE_PHY0_P5:
    case DG_CMN_DRV_EXT_LOOP_NODE_PHY0_P6:
    case DG_CMN_DRV_EXT_LOOP_NODE_PHY0_P7:
    case DG_CMN_DRV_EXT_LOOP_NODE_PHY0_P8:
    case DG_CMN_DRV_EXT_LOOP_NODE_PHY1_P1:
    case DG_CMN_DRV_EXT_LOOP_NODE_PHY1_P2:
    case DG_CMN_DRV_EXT_LOOP_NODE_PHY1_P3:
    case DG_CMN_DRV_EXT_LOOP_NODE_PHY1_P4:
        switch (action)
        {
        case DG_CMN_DRV_EXT_LOOP_ACTION_CLEAR:
        case DG_CMN_DRV_EXT_LOOP_ACTION_SET:
            /* The judge need change to call driver to set/clear external loopback */
            if (node != DG_CMN_DRV_EXT_LOOP_NODE_FPGA)
            {
                DG_DBG_TRACE("External loopback operate action:0x%02x, node:0x%02x", action, node);
                ret = TRUE;
            }
            else
            {
                DG_DRV_UTIL_set_error_string("External loopback operate failed, "
                                             "action:0x%02x, node:0x%02x", action, node);
            }
            break;

        default:
            DG_DRV_UTIL_set_error_string("Invalid action 0x%02x", action);
            break;
        }
        break;

    default:
        DG_DRV_UTIL_set_error_string("Invalid node 0x%02x", node);
        break;
    }

    return ret;
}


/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

