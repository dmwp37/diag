#ifndef _DG_PAL_DBG_H
#define _DG_PAL_DBG_H
/*==================================================================================================

    Module Name:  dg_pal_dbg.h

    General Description: DIAG PAL - Debug trace Utilities

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

/** @addtogroup Engine_PAL
@{
*/

/** @addtogroup debug
@{

@par
Debug trace platform dependent interface

*/

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

/*=============================================================================================*//**
@brief Log out the debug message according to the debug level

@param[in]   debug_level
@param[in]   format, .... the formated string
*//*==============================================================================================*/
void DG_PAL_DBG_print(int debug_level, const char* format, ...);

/*=============================================================================================*//**
@brief Load debug level gate from system

@return the current saved debug level
*//*==============================================================================================*/
int DG_PAL_DBG_load_dbg_lvl();

/*=============================================================================================*//**
@brief Save debug level gate to system

@param[in] gate - The gate that want to be saved.

@return TRUE if no issue, otherwise returns FALSE.
*//*==============================================================================================*/
BOOL DG_PAL_DBG_save_dbg_lvl(int gate);

/*=============================================================================================*//**
@brief Load autolog level gate from system

@return the current saved debug level
*//*==============================================================================================*/
int DG_PAL_DBG_load_autolog_lvl();

/*=============================================================================================*//**
@brief Save autolog level gate to system

@param[in] gate - The gate that want to be saved.

@return TRUE if no issue, otherwise returns FALSE.
*//*==============================================================================================*/
BOOL DG_PAL_DBG_save_autolog_lvl(int gate);

/*=============================================================================================*//**
@brief Dump the history logs to the file system

@param[in] id - a Number that distinguish the output file name.

@note This is especially useful when the command fails that could dump out the history logs to
      investigate. And whether a really dump should happen can be controlled by debug level command
*//*==============================================================================================*/
void DG_PAL_DBG_dump_fs_log(UINT16 id);

/** @} */
/** @} */

#ifdef __cplusplus
}
#endif

#endif

