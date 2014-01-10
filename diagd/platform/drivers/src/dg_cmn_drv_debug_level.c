/*==================================================================================================

    Module Name:  dg_cmn_drv_debug_level.c

    General Description: Implements the DEBUG_LEVEL common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_debug_level.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup DEBUG_LEVEL_driver
@{
implementation of the DEBUG_LEVEL_driver
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
@brief Set debug level of the component

@param[in]   component
@param[in]   debug_level

*//*==============================================================================================*/
BOOL DG_CMN_DRV_DEBUG_LEVEL_set(DG_CMN_DRV_DEBUG_LEVEL_COMPONENT_T component, UINT16 debug_level)
{
    BOOL ret = FALSE;

    if (component == DG_CMN_DRV_DEBUG_LEVEL_DIAG)
    {
        if (debug_level > DG_DBG_LVL_DISABLE)
        {
            DG_DRV_UTIL_set_error_string("Invalid diag debug level, %d", debug_level);
        }
        else if (DG_PAL_DBG_save_dbg_lvl(debug_level) == FALSE)
        {
            DG_DRV_UTIL_set_error_string("Failed to save debug level gate");
        }
        else
        {
            dg_dbg_level = debug_level;
            ret          = TRUE;
            DG_DBG_TRACE("successfully set debug level: %d", dg_dbg_level);
        }
    }
    else if (component == DG_CMN_DRV_DEBUG_LEVEL_AUTOLOG)
    {
        if (debug_level > DG_DBG_LVL_DISABLE)
        {
            DG_DRV_UTIL_set_error_string("Invalid autolog level, %d", debug_level);
        }
        else if (DG_PAL_DBG_save_autolog_lvl(debug_level) == FALSE)
        {
            DG_DRV_UTIL_set_error_string("Failed to save autolog level gate");
        }
        else
        {
            dg_dbg_autolog_level = debug_level;
            ret                  = TRUE;
            DG_DBG_TRACE("successfully set autolog level: %d", dg_dbg_autolog_level);
        }
    }
    else
    {
        DG_DRV_UTIL_set_error_string("Invalid debug level component, %d", component);
    }

    return ret;
}

/*=============================================================================================*//**
@brief Get debug level of the component

@param[in]   component
@param[out]  debug_level

*//*==============================================================================================*/
BOOL DG_CMN_DRV_DEBUG_LEVEL_get(DG_CMN_DRV_DEBUG_LEVEL_COMPONENT_T component, UINT16* debug_level)
{
    BOOL ret = FALSE;

    if (component == DG_CMN_DRV_DEBUG_LEVEL_DIAG)
    {
        *debug_level = dg_dbg_level;
        ret          = TRUE;
        DG_DBG_TRACE("successfully get debug level: %d", dg_dbg_level);
    }
    else if (component == DG_CMN_DRV_DEBUG_LEVEL_AUTOLOG)
    {
        *debug_level = dg_dbg_autolog_level;
        ret          = TRUE;
        DG_DBG_TRACE("successfully get autolog level: %d", dg_dbg_level);
    }
    else
    {
        DG_DRV_UTIL_set_error_string("Invalid debug level component, %d", component);
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

