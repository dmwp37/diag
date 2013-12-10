#ifndef _DG_DBG_H
#define _DG_DBG_H
/*==================================================================================================

    Module Name:  dg_dbg.h

    General Description: This file provides an interface for debug logging

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
#include <string.h>
#include "dg_defs.h"
#include "dg_pal_dbg.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                             CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                               MACROS
==================================================================================================*/
/** @addtogroup engine_interface
 @{
*/

/** @addtogroup debug
 @{

@par
Macros used for logging messages to the console windows
*/
#define DG_DBG_VERBOSE(x ...) DG_PAL_DBG_print(DG_DBG_LVL_VERBOSE, x)
#define DG_DBG_TRACE(x ...)   DG_PAL_DBG_print(DG_DBG_LVL_TRACE, x)
#define DG_DBG_WARN(x ...)    DG_PAL_DBG_print(DG_DBG_LVL_WARNING, x)
#define DG_DBG_ERROR(x ...)   DG_PAL_DBG_print(DG_DBG_LVL_ERROR, "ERROR: "x)

/** Dumps contents of buffer to debug print */
#define DG_DBG_DUMP(buf, len) DG_DBG_data_dump(buf, len)

/** @} */
/** @} */
/*==================================================================================================
                                TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
enum
{
    DG_DBG_LVL_VERBOSE = 0,
    DG_DBG_LVL_TRACE   = 1,
    DG_DBG_LVL_WARNING = 2,
    DG_DBG_LVL_ERROR   = 3,
    DG_DBG_LVL_DISABLE = 4,
};

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/
/*=============================================================================================*//**
@brief Dumps the contents of a buffer to the debug port

@param[in] data_buf - The buffer to dump
@param[in] len      - The length of data_buf in bytes

@note
  - Only available when verbose debugging is enabled
*//*==============================================================================================*/
void DG_DBG_data_dump(void* data_buf, int len);

/*==================================================================================================
                                          GLOBAL VARIABLES
==================================================================================================*/
extern int dg_dbg_level;
extern int dg_dbg_autolog_level;

#ifdef __cplusplus
}
#endif
#endif
