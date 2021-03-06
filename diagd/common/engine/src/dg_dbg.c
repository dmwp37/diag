/*==================================================================================================

    Module Name:  dg_dbg.c

    General Description:  Provides debug utility functions

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

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/
#define DG_DBG_MAX_DUMP_COLS 16

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
int dg_dbg_level         = DG_DBG_LVL_TRACE;
int dg_dbg_autolog_level = DG_DBG_LVL_DISABLE;

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Dumps the contents of a buffer to the debug port

@param[in] data_buf - The buffer to dump
@param[in] len      - The length of data_buf in bytes

@note
  - Only available when verbose debugging is enabled
*//*==============================================================================================*/
void DG_DBG_data_dump(void* data_buf, int len)
{
    int   max_col      = DG_DBG_MAX_DUMP_COLS;
    int   row          = 0;
    int   col          = 0;
    int   num_col      = 0;
    int   buffer_index = 0;
    UINT8 value;

    /* Each column takes up 3 characters, plus NULL */
    char string_buffer[(DG_DBG_MAX_DUMP_COLS * 3) + 1];

    if (DG_DBG_LVL_VERBOSE == dg_dbg_level)
    {
        for (row = 0; row <= (len / max_col); row++)
        {
            /* Reset string buffer for each new row */
            memset(string_buffer, 0, sizeof(string_buffer));

            /* For all rows, the number of columns is the max number, except for the last row */
            num_col = (row == (len / max_col)) ? (len % max_col) : max_col;
            if (num_col != 0)
            {
                for (col = 0; col < num_col; col++)
                {
                    value = ((UINT8*)data_buf)[buffer_index++];
                    snprintf(string_buffer, sizeof(string_buffer),
                             "%s%02x ", string_buffer, value);
                }
                DG_DBG_VERBOSE("%s", string_buffer);
            }
        }
    }
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

