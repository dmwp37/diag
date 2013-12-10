#ifndef _DG_CMN_DRV_DBG_LVL_H_
#define _DG_CMN_DRV_DBG_LVL_H_
/*==================================================================================================

    Module Name:  dg_cmn_drv_debug_level.h

    General Description: This file provides interface for DEBUG_LEVEL Driver

====================================================================================================

Revision History:
                            Modification     Tracking
Author                          Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
Xudong Huang    - xudongh    2013/12/11     xxxxx-0000   Creation

====================================================================================================
                                         INCLUDE FILES
==================================================================================================*/
/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup DEBUG_LEVEL_driver
@{

@par
Provide APIs for DEBUG_LEVEL
*/

#include <dg_cmn_drv_defs.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                             ENUMS
==================================================================================================*/
typedef enum
{
    DG_CMN_DRV_DEBUG_LEVEL_DIAG    = 0x00,
    DG_CMN_DRV_DEBUG_LEVEL_MSL     = 0x01,
    DG_CMN_DRV_DEBUG_LEVEL_RATC    = 0x02,
    DG_CMN_DRV_DEBUG_LEVEL_CAT     = 0x03,
    DG_CMN_DRV_DEBUG_LEVEL_SCIM    = 0x04,
    DG_CMN_DRV_DEBUG_LEVEL_AUTOLOG = 0x05,
} DG_CMN_DRV_DEBUG_LEVEL_COMPONENT_T;

typedef enum
{
    DG_CMN_DRV_DEBUG_LEVEL_SUCCESS = 0x00,
    DG_CMN_DRV_DEBUG_LEVEL_FAIL    = 0x01
} DG_CMN_DRV_DEBUG_LEVEL_STATUS_T;

/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/
/*=============================================================================================*//**
@brief Set debug level of the component

@param[in]   component
@param[in]   debug_level
@param[out]  status         - Status of operation

*//*==============================================================================================*/
DG_CMN_DRV_ERR_T DG_CMN_DRV_DEBUG_LEVEL_set(DG_CMN_DRV_DEBUG_LEVEL_COMPONENT_T component,
                                            UINT16                             debug_level,
                                            DG_CMN_DRV_DEBUG_LEVEL_STATUS_T*   status);

/*=============================================================================================*//**
@brief Get debug level of the component

@param[in]   component
@param[out]  debug_level
@param[out]  status         - Status of operation

@Note debug_level is valid only when function return and status value indicate success
*//*==============================================================================================*/
DG_CMN_DRV_ERR_T DG_CMN_DRV_DEBUG_LEVEL_get(DG_CMN_DRV_DEBUG_LEVEL_COMPONENT_T component,
                                            UINT16*                            debug_level,
                                            DG_CMN_DRV_DEBUG_LEVEL_STATUS_T*   status);

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
DG_CMN_DRV_ERR_T DG_CMN_DRV_DEBUG_LEVEL_get_err_string(UINT32 max_length, char* err_str);

/** @} */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _DG_CMN_DRV_DBG_LVL_H_  */


