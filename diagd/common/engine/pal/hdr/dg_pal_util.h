#ifndef _DG_PAL_UTIL_H
#define _DG_PAL_UTIL_H
/*==================================================================================================

    Module Name:  dg_pal_util.h

    General Description: DIAG PAL - Miscellaneous Engine Utilities

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

/** @addtogroup utility
@{

@par
Miscellaneous Engine Utilities

*/

/*==================================================================================================
                                               MACROS
==================================================================================================*/
#define DG_PAL_UTIL_DEFAULT_DIR_MAX_SIZE 256 /**< Max possible byte size of a platform's default
                                                directory (including NULL) */
/*==================================================================================================
                                               ENUMS
==================================================================================================*/
typedef enum
{
    DG_PAL_UTIL_ABSOLUTE_PATH_YES   = 0,
    DG_PAL_UTIL_ABSOLUTE_PATH_NO    = 1,
    DG_PAL_UTIL_ABSOLUTE_PATH_ERROR = 2
} DG_PAL_UTIL_ABSOLUTE_PATH_T;

/** Network update events */
typedef enum
{
    DG_PAL_UTIL_SOCKET_UPDATE_EXT_ADD,    /**< External DIAG network interface added */
    DG_PAL_UTIL_SOCKET_UPDATE_EXT_REMOVE, /**< External DIAG network interface removed */
    DG_PAL_UTIL_SOCKET_UPDATE_EXT_UPDATE, /**< External DIAG network interface ip updated */
    DG_PAL_UTIL_SOCKET_UPDATE_NOOP        /**< No op */
} DG_PAL_UTIL_SOCKET_UPDATE_T;

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
@brief Determine if the supplied path + filename uses an absolute path

@param[in] file_name - input file name to be handled

@return to indicate whether the file name is absolute or not

@note
 - 'file_name' must be NULL terminated
*//*==============================================================================================*/
DG_PAL_UTIL_ABSOLUTE_PATH_T DG_PAL_UTIL_check_absolute_path(W_CHAR* file_name);

/*=============================================================================================*//**
@brief Gets the default DIAG 12M directory path for the platform

@param[out] default_path - pointer to contain the platform default file path

@return TRUE success, FALSE error

@note
 - The calling function must allocate memory the size of #DG_PAL_UTIL_DEFAULT_DIR_MAX_SIZE for
   'default_path'.  The default path for each platform must not exceed that size
 - 'default_path' must be NULL terminated and the last non-NULL character must be the platform's
   directory delimiter, ie. '/' on Linux, '\' on Windows
 - 'default_path' is only valid if the return value indicates success
*//*==============================================================================================*/
BOOL DG_PAL_UTIL_get_default_path(W_CHAR* default_path);

/*=============================================================================================*//**
@brief Determines if another DIAG process (besides the process invoking this function) exists

@return TRUE = another process exists, FALSE = no other process exists

@note
 - On error, FALSE should be returned
*//*==============================================================================================*/
BOOL DG_PAL_UTIL_is_diag_process_exist(void);

/*=============================================================================================*//**
@brief Panics the DIAG process with the given reason

@param[in] reason - Printf-style panic reason format string
@param[in] ...    - Variable argument list

@note
 - This function is expected to not return
*//*==============================================================================================*/
void DG_PAL_UTIL_panic(const char* reason, ...);

/*=============================================================================================*//**
@brief Determines if the specified socket is allowed to connect to DIAGs

@param[in] socket - The socket to verify

@return TRUE = socket is allowed, FALSE = socket is not allowed/failure

@note
 - This function shall return FALSE on any failures
*//*==============================================================================================*/
BOOL DG_PAL_UTIL_is_socket_allowed(int socket);

/*=============================================================================================*//**
@brief Create a socket for listening for internal diag clients

@param[out] sock - The created socket, set to -1 on failure

@return TRUE = if an internal DIAG socket is to be used, FALSE = feature not used
*//*==============================================================================================*/
BOOL DG_PAL_UTIL_create_int_diag_listen_sock(int* sock);

/*=============================================================================================*//**
@brief Create a socket for listening for external diag clients

@param[out] sock - The created socket, set to -1 on failure

@return TRUE = if an external DIAG socket is to be used, FALSE = feature not used

@note
- In most cases, this should create a TCP/IP socket on port 11000 on the USBLan network interface
*//*==============================================================================================*/
BOOL DG_PAL_UTIL_create_ext_diag_listen_sock(int* sock);

/*=============================================================================================*//**
@brief Create a socket listening for network interface updates

@param[out] sock - The created socket, set to -1 on failure

@return TRUE = if an update socket is to be used, FALSE = feature not used
*//*==============================================================================================*/
BOOL DG_PAL_UTIL_create_update_sock(int* sock);

/*=============================================================================================*//**
@brief Handles network interface update events

@param[in] fd - Socket which is listening for the events

@return The network event which occurred, if the feature is not supported or there was an error,
#DG_PAL_UTIL_SOCKET_UPDATE_NOOP should be returned.
*//*==============================================================================================*/
DG_PAL_UTIL_SOCKET_UPDATE_T DG_PAL_UTIL_handle_update_sock_event(int fd);

/*=============================================================================================*//**
@brief Notifies glue layer of a client being added/removed

@param[in] is_add  - If a client is being added, TRUE = added, FALSE = removed
@param[in] num_int - New number of internal clients
@param[in] num_ext - New number of external clients
*//*==============================================================================================*/
void DG_PAL_UTIL_notify_client_update(BOOL is_add, int num_int, int num_ext);

/** @} */
/** @} */

#ifdef __cplusplus
}
#endif

#endif

