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

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                             ENUMS
==================================================================================================*/
enum
{
    DG_CMN_DRV_DEBUG_LEVEL_DIAG    = 0x00,
    DG_CMN_DRV_DEBUG_LEVEL_AUTOLOG = 0x01,
};
typedef UINT8 DG_CMN_DRV_DEBUG_LEVEL_COMPONENT_T;


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

*//*==============================================================================================*/
BOOL DG_CMN_DRV_DEBUG_LEVEL_set(DG_CMN_DRV_DEBUG_LEVEL_COMPONENT_T component, UINT16 debug_level);

/*=============================================================================================*//**
@brief Get debug level of the component

@param[in]   component
@param[out]  debug_level

*//*==============================================================================================*/
BOOL DG_CMN_DRV_DEBUG_LEVEL_get(DG_CMN_DRV_DEBUG_LEVEL_COMPONENT_T component, UINT16* debug_level);

/** @} */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _DG_CMN_DRV_DBG_LVL_H_  */

