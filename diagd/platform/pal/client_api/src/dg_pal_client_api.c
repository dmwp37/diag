/*==================================================================================================

    Module Name:  dg_pal_client_api.c

    General Description: Implements the PAL for Client API for Android

====================================================================================================

Revision History:
                            Modification     Tracking
Author                          Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
Xudong Huang    - xudongh    2013/12/11     xxxxx-0000   Creation

====================================================================================================
                                            INCLUDE FILES
==================================================================================================*/
#include "dg_pal_client_platform_inc.h"
#include "dg_pal_client_api.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>

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
@brief Launches the DIAG application

@return TRUE = successfully started the DIAG application, FALSE = failed to start application
*//*==============================================================================================*/
BOOL DG_PAL_CLIENT_API_start_diag_app(void)
{
    /* Always return success, users must start DIAG engine manually on test platform */
    return TRUE;
}

/*=============================================================================================*//**
@brief Creates socket for use by internal DIAG clients

@param[out] server - Socket address
@param[out] len    - Length of the socket address

@return File descriptor for the socket, -1 on fail
*//*==============================================================================================*/
int DG_PAL_CLIENT_API_create_int_diag_socket(struct sockaddr_storage *server, socklen_t *len)
{
    struct sockaddr_un *unix_srv = (struct sockaddr_un *)server;
    int sock = -1;
    
    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock != -1)
    {
        unix_srv->sun_family = AF_UNIX;
        strcpy(unix_srv->sun_path, DG_CFG_INT_SOCKET);
        *len = (sizeof(unix_srv->sun_family) + strlen(unix_srv->sun_path));
    }

    return (sock);
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/
