/*==================================================================================================

    Module Name:  dg_pal_dbg.c

    General Description: Implements the PAL layer debug logging utilities

====================================================================================================

Revision History:
                            Modification     Tracking
Author                          Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
Xudong Huang    - xudongh    2013/12/11     xxxxx-0000   Creation

====================================================================================================
                                            INCLUDE FILES
==================================================================================================*/
#include <stdio.h>
#include <stdarg.h>
#include "dg_defs.h"
#include "dg_dbg.h"
#include "dg_pal_dbg.h"

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
@brief Log out the debug message according to the debug level

@param[in]   debug_level
@param[in]   format, .... the formated string
*//*==============================================================================================*/
void DG_PAL_DBG_print(int debug_level, const char* format, ...)
{
    va_list args;

    va_start(args, format);
    if (debug_level >= dg_dbg_level)
    {
        printf("DIAG: ");
        vprintf(format, args);
        printf("\n");
        fflush(stdout);
    }
    va_end(args);
}

/*=============================================================================================*//**
@brief Load debug level gate from system

@return the current saved debug level
*//*==============================================================================================*/
int DG_PAL_DBG_load_dbg_lvl()
{
    /* always return DG_DBG_LVL_TRACE for the test platform */
    return DG_DBG_LVL_TRACE;
}

/*=============================================================================================*//**
@brief Save debug level gate to system

@param[in] gate - The gate that want to be saved.

@return TRUE if no issue, otherwise returns FALSE.
*//*==============================================================================================*/
BOOL DG_PAL_DBG_save_dbg_lvl(int gate)
{
    DG_DBG_TRACE("Save debug level: %d", gate);
    return TRUE;
}

/*=============================================================================================*//**
@brief Load autolog level gate from system

@return the current saved debug level
*//*==============================================================================================*/
int DG_PAL_DBG_load_autolog_lvl()
{
    /* always return DG_DBG_LVL_DISABLE for the test platform */
    return DG_DBG_LVL_DISABLE;
}

/*=============================================================================================*//**
@brief Save autolog level gate to system

@param[in] gate - The gate that want to be saved.

@return TRUE if no issue, otherwise returns FALSE.
*//*==============================================================================================*/
BOOL DG_PAL_DBG_save_autolog_lvl(int gate)
{
    DG_DBG_TRACE("Save autolog level: %d", gate);
    return TRUE;
}

/*=============================================================================================*//**
@brief Dump the history logs to the file system

@param[in] id - a Number that distinguish the output file name.

@note This is especially useful when the command fails that could dump out the history logs to
      investigate. And whether a really dump should happen can be controlled by debug level command
*//*==============================================================================================*/
void DG_PAL_DBG_dump_fs_log(UINT16 id)
{
    if (dg_dbg_autolog_level < DG_DBG_LVL_DISABLE)
    {
        /* do nothing for the test platform */
        DG_DBG_TRACE("DG_PAL_DBG_dump_fs_log(%04x)", id);
    }
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

