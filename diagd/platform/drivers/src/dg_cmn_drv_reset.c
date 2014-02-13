/*==================================================================================================

    Module Name:  dg_cmn_drv_reset.c

    General Description: Implements the RESET common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_reset.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup RESET_driver
@{
implementation of the RESET driver
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
@brief Reads data from a given address on the given RESET bus

@param[in]  comp       - Which component to reset
@param[in]  is_reset   - TRUE to reset and FALSE to recover

@note
- Read data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_RESET_reset(DG_CMN_DRV_RESET_COMP_T comp, BOOL is_reset)
{
    BOOL ret = FALSE;

    DG_DBG_TRACE("Reset Comp=%d, Reset=%d", comp, is_reset);

    switch (comp)
    {
    case DG_CMN_DRV_RESET_PCH:
    case DG_CMN_DRV_RESET_FPGA:
    case DG_CMN_DRV_RESET_SWITCH:
    case DG_CMN_DRV_RESET_PHY:
        DG_DBG_TRACE("Reset Comp=%d, Reset=%d", comp, is_reset);
        ret = TRUE;
        break;

    case DG_CMN_DRV_RESET_SYS:
        DG_DRV_UTIL_set_error_string("Can't reset system on test platform!");
        break;

    default:
        DG_DRV_UTIL_set_error_string("Unsupported reset chip component! comp=%d", comp);
        break;



    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

