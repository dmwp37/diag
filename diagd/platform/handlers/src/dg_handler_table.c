/*==================================================================================================

    Module Name:  dg_handler_table.c

    General Description: Table for DIAG handlers

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
#include "dg_common_handler_table.h"
#include "dg_handler_table.h"

/*==================================================================================================
                                           LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                            LOCAL MACROS
==================================================================================================*/
#define DG_HANDLER_TABLE_DEFAULT_TIMEOUT 10000 /**< Default timeout used for DIAGs */

/*==================================================================================================
                             LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                      LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
                                          GLOBAL VARIABLES
==================================================================================================*/
/** Table for storing all opcodes/commands we are able to process with this engine
    Important: Table must be in order of ascending opcodes! The last line must have the opcode of
    DG_DEFS_HANDLER_TABLE_OPCODE_END */
const DG_DEFS_OPCODE_ENTRY_T DG_HANDLER_TABLE_data[] =
{
    {0x0FFD, DG_DEFS_MODE_ALL,  DG_DEBUG_LEVEL_handler_main,NULL, DG_HANDLER_TABLE_DEFAULT_TIMEOUT},
    {0x0FFE, DG_DEFS_MODE_ALL,  DG_TEST_ENGINE_handler_main,NULL, DG_HANDLER_TABLE_DEFAULT_TIMEOUT},
    {0x0FFF, DG_DEFS_MODE_ALL,  DG_PING_handler_main,       NULL, DG_HANDLER_TABLE_DEFAULT_TIMEOUT},

    /* IMPORTANT: This must be the last line! */
    {DG_DEFS_HANDLER_TABLE_OPCODE_END, DG_DEFS_MODE_ALL, NULL, NULL, 60000}
//    {DG_DEFS_HANDLER_TABLE_OPCODE_END, DG_DEFS_MODE_ALL, DG_AUX_CMD_handler_main, NULL, 60000}

};

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

