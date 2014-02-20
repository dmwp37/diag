/*==================================================================================================

    Module Name:  dg_handler_table.c

    General Description: Table for DIAG handlers

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_defs.h"
#include "dg_common_handler_table.h"
#include "dg_handler_table.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup Handler_Table
@{

@par
<b>Handler_Table</b>

@par
Static Handler Table for the diag opcode dispatching
engine
*/

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
    { 0x0000, DG_DEFS_MODE_ALL,  DG_VERSION_handler_main,     DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0001, DG_DEFS_MODE_ALL,  DG_LED_handler_main,         DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x000D, DG_DEFS_MODE_TEST, DG_FPGA_handler_main,        DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x000F, DG_DEFS_MODE_TEST, DG_BUTTON_handler_main,      DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0010, DG_DEFS_MODE_TEST, DG_I2C_handler_main,         DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0013, DG_DEFS_MODE_ALL,  DG_PCI_handler_main,         DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0016, DG_DEFS_MODE_ALL,  DG_RTC_handler_main,         DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0100, DG_DEFS_MODE_ALL,  DG_SUSPEND_handler_main,     DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0101, DG_DEFS_MODE_TEST, DG_RESET_handler_main,       DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0FFD, DG_DEFS_MODE_ALL,  DG_DEBUG_LEVEL_handler_main, DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0FFE, DG_DEFS_MODE_ALL,  DG_TEST_ENGINE_handler_main, DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0FFF, DG_DEFS_MODE_ALL,  DG_PING_handler_main,        DG_HANDLER_TABLE_DEFAULT_TIMEOUT },

    /* IMPORTANT: This must be the last line! */
    { DG_DEFS_HANDLER_TABLE_OPCODE_END, DG_DEFS_MODE_ALL, DG_AUX_CMD_handler_main, 60000 }

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

/** @} */
/** @} */

