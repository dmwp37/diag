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

/** @} */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _DG_CMN_DRV_DBG_LVL_H_  */

