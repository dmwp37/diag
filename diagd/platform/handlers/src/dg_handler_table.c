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
    { 0x0002, DG_DEFS_MODE_ALL,  DG_FAN_handler_main,         DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0003, DG_DEFS_MODE_ALL,  DG_TEMP_handler_main,        DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0004, DG_DEFS_MODE_TEST, DG_EEPROM_handler_main,      DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0005, DG_DEFS_MODE_TEST, DG_VOLTAGE_handler_main,     DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0006, DG_DEFS_MODE_TEST, DG_SFP_handler_main,         DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0007, DG_DEFS_MODE_TEST, DG_CLOCK_handler_main,       DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0008, DG_DEFS_MODE_ALL,  DG_DIMM_handler_main,        DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0009, DG_DEFS_MODE_TEST, DG_PTTM_handler_main,        DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x000A, DG_DEFS_MODE_TEST, DG_PSU_handler_main,         DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x000B, DG_DEFS_MODE_TEST, DG_PHY_handler_main,         DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x000D, DG_DEFS_MODE_TEST, DG_FPGA_handler_main,        DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x000E, DG_DEFS_MODE_TEST, DG_CPLD_handler_main,        DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x000F, DG_DEFS_MODE_TEST, DG_BUTTON_handler_main,      DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0010, DG_DEFS_MODE_TEST, DG_I2C_handler_main,         DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0011, DG_DEFS_MODE_ALL,  DG_USB_handler_main,         DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0012, DG_DEFS_MODE_ALL,  DG_SSD_handler_main,         DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0013, DG_DEFS_MODE_ALL,  DG_PCI_handler_main,         DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0014, DG_DEFS_MODE_ALL,  DG_TPM_handler_main,         DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0015, DG_DEFS_MODE_TEST, DG_BIOS_handler_main,        DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0016, DG_DEFS_MODE_ALL,  DG_RTC_handler_main,         DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0017, DG_DEFS_MODE_TEST, DG_GPIO_handler_main,        DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0018, DG_DEFS_MODE_TEST, DG_FPGA_FLASH_handler_main,  DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0019, DG_DEFS_MODE_TEST, DG_PORT_LED_handler_main,    DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0020, DG_DEFS_MODE_TEST, DG_PRBS_handler_main,        DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0021, DG_DEFS_MODE_TEST, DG_INT_LOOP_handler_main,    DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0022, DG_DEFS_MODE_TEST, DG_EXT_LOOP_handler_main,    DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
    { 0x0023, DG_DEFS_MODE_TEST, DG_LOOP_handler_main,        DG_HANDLER_TABLE_DEFAULT_TIMEOUT },
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

