#ifndef _DG_CMN_DRV_DEFS_H
#define _DG_CMN_DRV_DEFS_H
/*==================================================================================================

    Module Name:  dg_cmn_drv_defs.h

    General Description: Common defines for Diag Daemon Common Drivers

====================================================================================================

Revision History:
                            Modification     Tracking
Author                          Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
Xudong Huang    - xudongh    2008/03/11     xxxxx-0000   Creation

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <dg_pal_client_platform_inc.h>

#ifdef __cplusplus  
extern "C" {
#endif

/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup common_defines
@{

@par 
Common defines for Diag Daemon Common Drivers
*/

/*==================================================================================================
                                               MACROS
==================================================================================================*/

/*==================================================================================================
                                               ENUMS
==================================================================================================*/
/** API function common error types, used as return value for all DIAG common driver APIs. These
    values are used to report errors that blocked the API function from completing. In cases where
    an API function returns a detailed status parameter, DG_CMN_DRV_ERR_NONE does not imply a
    requested action was completed successfully.  To indicate success, the API function must both 
    return DG_CMN_DRV_ERR_NONE and set the status parameter to success.  In these cases, it is 
    possible for the function to return DG_CMN_DRV_ERR_NONE, but the status parameter still 
    indicate failure.
*/
typedef enum
{
    DG_CMN_DRV_ERR_NONE            = 0, /**< The API function was able to complete */
    DG_CMN_DRV_ERR_INVALID_PARAM   = 1, /**< An API parameter was invalid */
    DG_CMN_DRV_ERR_ALLOC_MEM       = 2, /**< API function was unable to allocate memory that was 
                                             needed */
    DG_CMN_DRV_ERR_NOT_SUPPORTED   = 3, /**< Requested operation/action not supported on 
                                            product/platform */
    DG_CMN_DRV_ERR_INCORRECT_STATE = 4, /**< API function was incorrect for requested operation */
    DG_CMN_DRV_ERR_TIMEOUT         = 5  /**< API function timed out waiting for completion. */
} DG_CMN_DRV_ERR_T;

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
                                      GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/** @} */
/** @} */

#ifdef __cplusplus  
}       
#endif

#endif

