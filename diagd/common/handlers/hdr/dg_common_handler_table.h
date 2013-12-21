#ifndef _DG_COMMON_HANDLER_TABLE_H
#define _DG_COMMON_HANDLER_TABLE_H
/*==================================================================================================

    Module Name:  dg_common_handler_table.h

    General Description:  Header file for all common handler function prototypes

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
/* Important Note: Please add all handler functions to this list in alphabetical order.  All
   functions must have the name format/prototype of
   DG_<DIAG NAME>_handler_main(DG_DEFS_DIAG_REQ_T *req) */

void DG_DEBUG_LEVEL_handler_main(DG_DEFS_DIAG_REQ_T *req);
void DG_TEST_ENGINE_handler_main(DG_DEFS_DIAG_REQ_T *req);

#endif
