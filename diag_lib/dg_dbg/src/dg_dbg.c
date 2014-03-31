/*==================================================================================================

    Module Name:  dg_dbg.c

    General Description: Implements the dg_dbg library

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdio.h>
#include <stdarg.h>
#include "dg_dbg.h"

/** @addtogroup libdg_dbg
@{
*/
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
/*default dbg level is just print error */
static int dg_dbg_level = DG_DBG_LVL_ERROR;

/*==================================================================================================
                                         GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Set debug level

@param[in] debug_level
*//*==============================================================================================*/
void DG_DBG_set_dbg_level(int debug_level)
{
    if (debug_level < DG_DBG_LVL_VERBOSE)
    {
        dg_dbg_level = DG_DBG_LVL_VERBOSE;
    }
    else if (debug_level > DG_DBG_LVL_DISABLE)
    {
        dg_dbg_level = DG_DBG_LVL_DISABLE;
    }
    else
    {
        dg_dbg_level = debug_level;
    }
}

/*=============================================================================================*//**
@brief Log out the debug message according to the debug level

@param[in]   debug_level
@param[in]   format, .... the formated string
*//*==============================================================================================*/
void DG_DBG_print(int debug_level, const char* format, ...)
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


/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/


/** @} */

