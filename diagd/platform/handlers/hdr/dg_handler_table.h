#ifndef _DG_HANDLER_TABLE_H
#define _DG_HANDLER_TABLE_H
/*==================================================================================================

    Module Name:  dg_handler_table.h

    General Description: handler table & platform-specific handler function prototypes

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
extern const DG_DEFS_OPCODE_ENTRY_T DG_HANDLER_TABLE_data[];

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/
/*
 * Important Note: Please add all handler functions to this list in alphabetical order.  All
 * functions must have the name format/prototype of
 * DG_<DIAG NAME>_handler_main(DG_DEFS_DIAG_REQ_T *req)
 * you can declare the handler here even it's not implemented
 * we just keep the special handlers declaration here to remove compilation warnings
 */
void DG_AUX_CMD_handler_main(DG_DEFS_DIAG_REQ_T* req);
#ifdef __cplusplus
}
#endif

#endif

