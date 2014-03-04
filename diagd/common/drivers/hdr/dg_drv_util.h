#ifndef _DG_DRV_UTIL_H
#define _DG_DRV_UTIL_H
/*==================================================================================================

    Module Name:  dg_drv_util.h

    General Description: Utilities for DIAG drivers

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup driver_Utils
@{

@par
Provide common driver utility
*/

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

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*=============================================================================================*//**
@brief Initializes error string for the current driver thread

@note
- the init would only happen once for threads, please see the pthread_once manual
*//*==============================================================================================*/
void DG_DRV_UTIL_init_error_string();

/*=============================================================================================*//**
@brief Sets an error string for the current driver thread

@param[in]     format     - printf style format string for error message
@param[in]     ...        - Variable argument, used to popluated format string

@note
- If err_string already has a string, the old one will be freed
- Calling function responsible for freeing
*//*==============================================================================================*/
void DG_DRV_UTIL_set_error_string(const char* format, ...);

/*=============================================================================================*//**
@brief Gets an error string for the current driver thread

@return - the current driver thread error string

@note
- reteive error string from thread specific data
- If no error string was set, will return NULL
*//*==============================================================================================*/
char* DG_DRV_UTIL_get_error_string();

/*=============================================================================================*//**
@brief Execute a system command and put the output to the

@param[in]  cmd   - the command string we need to execute
@param[out] p_out - point to the output string buffer

@return - TRUE if the command run successfully

@note
- the p_out point to heap buffer for containing the string output of the command
- when this function returns FALSE, the p_out points to the command failure out put if any
- If there is no output p_out will be set to NULL
- Calling function responsible for freeing p_out buffer
*//*==============================================================================================*/
BOOL DG_DRV_UTIL_system(const char* cmd, char** p_out);

#ifdef __cplusplus
}
#endif

/** @} */
/** @} */

#endif /* _DG_DRV_UTIL_H  */

