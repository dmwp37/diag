/*==================================================================================================

    Module Name:  dg_pal_security.c

    General Description: Implements the PAL layer security check

====================================================================================================

Revision History:
                            Modification     Tracking
Author                          Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
Xudong Huang    - xudongh    2013/12/11     xxxxx-0000   Creation

====================================================================================================
                                            INCLUDE FILES
==================================================================================================*/
#include "dg_defs.h"
#include "dg_dbg.h"
#include "dg_pal_util.h"

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
@brief Get the ic type in this target

@return The IC type defined in DG_DEFS_SEC_IC_STATE_T
*//*==============================================================================================*/
DG_DEFS_SEC_IC_STATE_T DG_PAL_SECURITY_get_sec_ic_state(void)
{
    return DG_DEFS_SEC_IC_STATE_ENGINEERING;
}

/*=============================================================================================*//**
@brief Determine whether to send DIAG error strings

@return TRUE if error strings to be sent, FALSE if error strings NOT to be sent
*//*==============================================================================================*/
BOOL DG_PAL_SECURITY_is_diag_error_strings_allowed(void)
{
    return TRUE;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */
