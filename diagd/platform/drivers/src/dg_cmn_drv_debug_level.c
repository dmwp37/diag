/*==================================================================================================

    Module Name:  dg_cmn_drv_debug_level.c

    General Description: Implements the DEBUG_LEVEL common driver for Android

====================================================================================================

Revision History:
                            Modification     Tracking
Author                          Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
Xudong Huang    - xudongh    2013/12/11     xxxxx-0000   Creation

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <dg_cmn_drv_debug_level.h>
#include "dg_handler_inc.h"
#include <dg_util_drv.h>

/** @addtogroup dg_common_drivers_android
@{
*/

/** @addtogroup DEBUG_LEVEL_driver_android
@{
Android implementation of the DEBUG_LEVEL_driver
*/

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                            LOCAL MACROS
==================================================================================================*/
#define DG_CMN_DRV_DEBUG_LEVEL_SET_ERR_STRING(x ...) \
    DG_UTIL_DRV_err_string_set(&dg_cmn_drv_debug_level_err_string, x);

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
static char* dg_cmn_drv_debug_level_err_string = NULL;

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Set debug level of the component

@param[in]   component
@param[in]   debug_level
@param[out]  status         - Status of operation

*//*==============================================================================================*/
DG_CMN_DRV_ERR_T DG_CMN_DRV_DEBUG_LEVEL_set(DG_CMN_DRV_DEBUG_LEVEL_COMPONENT_T component,
                                            UINT16                             debug_level,
                                            DG_CMN_DRV_DEBUG_LEVEL_STATUS_T*   status)
{
    DG_CMN_DRV_ERR_T err = DG_CMN_DRV_ERR_NONE;
    *status = DG_CMN_DRV_DEBUG_LEVEL_FAIL;

    if (component == DG_CMN_DRV_DEBUG_LEVEL_DIAG)
    {
        if (debug_level > DG_DBG_LVL_DISABLE)
        {
            DG_CMN_DRV_DEBUG_LEVEL_SET_ERR_STRING("Invalid diag debug level, %d", debug_level);
        }
        else if (DG_PAL_DBG_save_dbg_lvl(debug_level) == FALSE)
        {
            DG_CMN_DRV_DEBUG_LEVEL_SET_ERR_STRING("Failed to save debug level gate");
        }
        else
        {
            dg_dbg_level = debug_level;
            *status      = DG_CMN_DRV_DEBUG_LEVEL_SUCCESS;
            DG_DBG_TRACE("successfully set debug level: %d", dg_dbg_level);
        }
    }
    else if (component == DG_CMN_DRV_DEBUG_LEVEL_AUTOLOG)
    {
        if (debug_level > DG_DBG_LVL_DISABLE)
        {
            DG_CMN_DRV_DEBUG_LEVEL_SET_ERR_STRING("Invalid autolog level, %d", debug_level);
        }
        else if (DG_PAL_DBG_save_autolog_lvl(debug_level) == FALSE)
        {
            DG_CMN_DRV_DEBUG_LEVEL_SET_ERR_STRING("Failed to save autolog level gate");
        }
        else
        {
            dg_dbg_autolog_level = debug_level;
            *status              = DG_CMN_DRV_DEBUG_LEVEL_SUCCESS;
            DG_DBG_TRACE("successfully set autolog level: %d", dg_dbg_autolog_level);
        }
    }
    else
    {
        DG_CMN_DRV_DEBUG_LEVEL_SET_ERR_STRING("Invalid debug level component, %d", component);
        err = DG_CMN_DRV_ERR_NOT_SUPPORTED;
    }

    return err;
}

/*=============================================================================================*//**
@brief Get debug level of the component

@param[in]   component
@param[out]  debug_level
@param[out]  status         - Status of operation

@Note debug_level is valid only when function return and status value indicate success
*//*==============================================================================================*/
DG_CMN_DRV_ERR_T DG_CMN_DRV_DEBUG_LEVEL_get(DG_CMN_DRV_DEBUG_LEVEL_COMPONENT_T component,
                                            UINT16*                            debug_level,
                                            DG_CMN_DRV_DEBUG_LEVEL_STATUS_T*   status)
{
    DG_CMN_DRV_ERR_T err = DG_CMN_DRV_ERR_NONE;
    *status = DG_CMN_DRV_DEBUG_LEVEL_FAIL;

    if (component == DG_CMN_DRV_DEBUG_LEVEL_DIAG)
    {
        *debug_level = dg_dbg_level;
        *status      = DG_CMN_DRV_DEBUG_LEVEL_SUCCESS;
        DG_DBG_TRACE("successfully get debug level: %d", dg_dbg_level);
    }
    else if (component == DG_CMN_DRV_DEBUG_LEVEL_AUTOLOG)
    {
        *debug_level = dg_dbg_autolog_level;
        *status      = DG_CMN_DRV_DEBUG_LEVEL_SUCCESS;
        DG_DBG_TRACE("successfully get autolog level: %d", dg_dbg_level);
    }
    else
    {
        DG_CMN_DRV_DEBUG_LEVEL_SET_ERR_STRING("Invalid debug level component, %d", component);
        err = DG_CMN_DRV_ERR_NOT_SUPPORTED;
    }

    return err;
}

/*=============================================================================================*//**
@brief Gets an ASCII string from the driver with detailed information of the last error which
       occurred.

@param[in]     max_length - Maximum length of buffer (including NULL)
@param[in,out] err_str    - ASCII, NULL terminated error string populated by the driver.  Buffer
                            will be of size 'max_length'

@note
- Driver shall clear its own local copy of the error string when its retrieved via this function
- If error strings are not supported, #DG_CMN_DRV_ERR_NOT_SUPPORTED shall be returned. 'err_str'
  is only considered valid if #DG_CMN_DRV_ERR_NONE returned.
- If there is no error string to return, the driver should return #DG_CMN_DRV_ERR_NONE with err_str
  being blank (err_str[0] = NULL)
- If 'err_str' is too small to contain the full driver error string, it shall be truncated.
*//*==============================================================================================*/
DG_CMN_DRV_ERR_T DG_CMN_DRV_DEBUG_LEVEL_get_err_string(UINT32 max_length, char* err_str)
{
    return DG_UTIL_DRV_err_string_get(max_length, err_str, &dg_cmn_drv_debug_level_err_string);
}

/*==================================================================================================
                                     LOCAL FUNCTION
==================================================================================================*/


/** @} */
/** @} */
