/*==================================================================================================

    Module Name:  dg_cmn_drv_button.c

    General Description: Implements the BUTTON common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <unistd.h>
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_button.h"
#include "dg_cmn_drv_cpld.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup BUTTON_driver
@{
implementation of the BUTTON driver
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
static BOOL dg_cmn_drv_button_get_power();
static BOOL dg_cmn_drv_button_get_reset();

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
@brief Get the last pressed button key code

@param[out] code - The last pressed button key code

*//*==============================================================================================*/
BOOL DG_CMN_DRV_BUTTON_get(DG_CMN_DRV_BUTTON_CODE_T* code)
{
    BOOL ret     = FALSE;
    BOOL b_press = FALSE;

    *code = DG_CMN_DRV_BUTTON_NONE;

    if (dg_cmn_drv_button_get_power(&b_press))
    {
        if (b_press)
        {
            *code = DG_CMN_DRV_BUTTON_POWER;
            DG_DBG_TRACE("power key was pressed!");
            return TRUE;
        }
    }

    if (dg_cmn_drv_button_get_reset(&b_press))
    {
        if (b_press)
        {
            *code = DG_CMN_DRV_BUTTON_RESET;
            DG_DBG_TRACE("reset key was pressed!");
        }

        ret = TRUE;
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Get if the power key was pressed

@param[out] pressed - TRUE if the button was pressed

@return TRUE if no error
*//*==============================================================================================*/
BOOL dg_cmn_drv_button_get_power(BOOL* pressed)
{
    BOOL                    ret = FALSE;
    DG_CMN_DRV_CPLD_VALUE_T value;

    if (!DG_CMN_DRV_CPLD_get(DG_CMN_DRV_CPLD_CB, 0x26, &value))
    {
        DG_DBG_ERROR("can't get CB CPLD power key INT");
    }
    else if (!DG_CMN_DRV_CPLD_set(DG_CMN_DRV_CPLD_CB, 0x2a, (1 << 4)))
    {
        DG_DBG_ERROR("can't clear CB CPLD power key INT");
    }
    else
    {
        *pressed = (value & (1 << 4)) > 0;
        ret      = TRUE;
    }

    return ret;
}

/*=============================================================================================*//**
@brief Get if the reset key was pressed

@param[out] pressed - TRUE if the button was pressed

@return TRUE if no error
*//*==============================================================================================*/
BOOL dg_cmn_drv_button_get_reset(BOOL* pressed)
{
    BOOL                    ret = FALSE;
    DG_CMN_DRV_CPLD_VALUE_T value;

    if (!DG_CMN_DRV_CPLD_get(DG_CMN_DRV_CPLD_FEB, 0x0d, &value))
    {
        DG_DBG_ERROR("can't get CB CPLD power key INT");
    }
    else if (!DG_CMN_DRV_CPLD_set(DG_CMN_DRV_CPLD_FEB, 0x1b, (1 << 1)))
    {
        DG_DBG_ERROR("can't clear CB CPLD power key INT");
    }
    else
    {
        *pressed = (value & (1 << 2)) > 0;
        ret      = TRUE;
    }

    return ret;
}

/** @} */
/** @} */

