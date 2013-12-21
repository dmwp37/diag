#ifndef _DG_UTIL_DRV_H
#define _DG_UTIL_DRV_H
/*==================================================================================================

    Module Name:  dg_util_drv.h

    General Description: Utilities for DIAG drivers

====================================================================================================

Revision History:
                            Modification     Tracking
Author                          Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
Xudong Huang    - xudongh    2013/12/11     xxxxx-0000   Creation

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <dg_cmn_drv_defs.h>

#ifdef __cplusplus  
extern "C" {
#endif

/*==================================================================================================
                                               MACROS
==================================================================================================*/

/*==================================================================================================
                                               ENUMS
==================================================================================================*/

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
                                      GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/
void DG_UTIL_DRV_err_string_set(char **err_string, const char *format, ...);
DG_CMN_DRV_ERR_T DG_UTIL_DRV_err_string_get(UINT32 max_length, char *dest_string, char** src_string);

#ifdef __cplusplus  
}       
#endif

#endif
