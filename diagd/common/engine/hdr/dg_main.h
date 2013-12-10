#ifndef _DG_MAIN_H
#define _DG_MAIN_H
/*==================================================================================================

    Module Name:  dg_main.h

    General Description: This file provides an interface for the DIAG engine

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
                                             CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                               MACROS
==================================================================================================*/

/*==================================================================================================
                                TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/
DG_DEFS_STATUS_T DG_MAIN_start_engine(const DG_DEFS_OPCODE_ENTRY_T* handler_tbl, int argc, 
                                      char * argv[]);

/*==================================================================================================
                                          GLOBAL VARIABLES
==================================================================================================*/
extern BOOL DG_MAIN_engine_exit_flag;

#ifdef __cplusplus  
}       
#endif
#endif
