/*==================================================================================================

    Module Name:  dg_socket.c

    General Description: This file contains code and supporting code for socket communications for
                         DIAGs

====================================================================================================

Revision History:
                            Modification     Tracking
Author                          Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
Xudong Huang    - xudongh    2013/12/11     xxxxx-0000   Creation

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <net/if.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include "dg_defs.h"
#include "dg_dbg.h"
#include "dg_client_comm.h"
#include "dg_main.h"
#include "dg_pal_util.h"
#include "dg_socket.h"

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/
#define DG_SOCKET_MAX_PENDING_CONNECTION_QUEUE 10 /**< Max number of pending connections allowed */
/*==================================================================================================
                                            LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                             LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static void dg_socket_listen_sock(int sock, int* max_fd, fd_set* socket_set);
static void dg_socket_accept_client_connect(int client_sock, DG_CLIENT_COMM_CLIENT_TYPE_T type);
static BOOL dg_socket_set_non_blocking_mode(int fd, BOOL on_off);
static void dg_socket_exit_handler(int sig);

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Listens and handles all incoming DIAG socket connection requests
*//*==============================================================================================*/
void DG_SOCKET_connection_listener(void)
{
    int    max_fd   = -1;
    int    ext_sock = -1;
    int    int_sock = -1;
    int    up_sock  = -1;
    fd_set socket_set;
    fd_set working_set;

    FD_ZERO(&socket_set);

    /* Create the internal DIAG socket */
    if ((DG_PAL_UTIL_create_int_diag_listen_sock(&int_sock) == TRUE) && (int_sock != -1))
    {
        dg_socket_listen_sock(int_sock, &max_fd, &socket_set);
    }

    /* Create the external DIAG socket */
    if ((DG_PAL_UTIL_create_ext_diag_listen_sock(&ext_sock) == TRUE) &&
        (ext_sock != -1))
    {
        dg_socket_listen_sock(ext_sock, &max_fd, &socket_set);
    }

    /* Create network update socket */
    if ((DG_PAL_UTIL_create_update_sock(&up_sock) == TRUE) && (up_sock != -1))
    {
        max_fd = (up_sock > max_fd) ? up_sock : max_fd;
        FD_SET(up_sock, &socket_set);
        DG_DBG_TRACE("Added net update socket %d", up_sock);
    }

    /* If max_fd is non-zero, there is a valid socket listening */
    if (max_fd != -1)
    {
        struct sigaction actions;

        memset(&actions, 0, sizeof(actions));
        sigemptyset(&actions.sa_mask);
        actions.sa_flags   = 0;
        actions.sa_handler = dg_socket_exit_handler;
        sigaction(SIGUSR1, &actions, NULL);

        /* Wait incoming connection requests until the diag engine is shutting down */
        while (DG_MAIN_engine_exit_flag == FALSE)
        {
            /* init variables tracking sockets */
            memcpy(&working_set, &socket_set, sizeof(working_set));

            /* Wait for a connection request */
            DG_DBG_TRACE("Listening for incoming client connection request");
            if (select(max_fd + 1, &working_set, NULL, NULL, NULL) < 0)
            {
                DG_DBG_ERROR("Select failed, errno = %d (%s)", errno, strerror(errno));
            }
            else
            {
                /* Accept client connections for external socket */
                if ((ext_sock != -1) && FD_ISSET(ext_sock, &working_set))
                {
                    dg_socket_accept_client_connect(ext_sock, DG_CLIENT_COMM_CLIENT_TYPE_EXT);
                }

                /* Accept client connections for internal socket */
                if ((int_sock != -1) && FD_ISSET(int_sock, &working_set))
                {
                    dg_socket_accept_client_connect(int_sock, DG_CLIENT_COMM_CLIENT_TYPE_INT);
                }

                /* Handle an update message */
                if ((up_sock != -1) && FD_ISSET(up_sock, &working_set))
                {
                    DG_PAL_UTIL_SOCKET_UPDATE_T status;

                    status = DG_PAL_UTIL_handle_update_sock_event(up_sock);
                    /* If the external interface was added and we aren't open already, create
                       the external socket */
                    if ((status == DG_PAL_UTIL_SOCKET_UPDATE_EXT_ADD) &&
                        (ext_sock == -1))
                    {
                        if ((DG_PAL_UTIL_create_ext_diag_listen_sock(&ext_sock) == TRUE) &&
                            (ext_sock != -1))
                        {
                            dg_socket_listen_sock(ext_sock, &max_fd, &socket_set);
                        }
                    }
                    /* If the external interface was removed and we have the external socket open,
                       close it */
                    else if ((status == DG_PAL_UTIL_SOCKET_UPDATE_EXT_REMOVE) &&
                             (ext_sock != -1))
                    {
                        /* Stop the listening socket */
                        DG_DBG_TRACE("Closing external listen socket");
                        max_fd = (int_sock > up_sock) ? int_sock : up_sock;
                        FD_CLR(ext_sock, &socket_set);
                        shutdown(ext_sock, SHUT_RDWR);
                        close(ext_sock);
                        ext_sock = -1;

                        /* Shut down all the external clients */
                        DG_CLIENT_COMM_shutdown_clients_of_type(DG_CLIENT_COMM_CLIENT_TYPE_EXT);
                    }
                    /* If the external interface was updated and we have the external socket open,
                       restart it */
                    else if ((status == DG_PAL_UTIL_SOCKET_UPDATE_EXT_UPDATE) &&
                             (ext_sock != -1))
                    {
                        /* Stop the external listening socket */
                        DG_DBG_TRACE("Restarting external listen socket");
                        max_fd = (int_sock > up_sock) ? int_sock : up_sock;
                        FD_CLR(ext_sock, &socket_set);
                        shutdown(ext_sock, SHUT_RDWR);
                        close(ext_sock);
                        ext_sock = -1;

                        /* Shut down all the external clients */
                        DG_CLIENT_COMM_shutdown_clients_of_type(DG_CLIENT_COMM_CLIENT_TYPE_EXT);

                        /* Start the external listening socket*/
                        if ((DG_PAL_UTIL_create_ext_diag_listen_sock(&ext_sock) == TRUE) &&
                            (ext_sock != -1))
                        {
                            dg_socket_listen_sock(ext_sock, &max_fd, &socket_set);
                        }
                    }
                }
            }
        }
    }

    /* Close any open listening sockets */
    if (ext_sock != -1)
    {
        shutdown(ext_sock, SHUT_RDWR);
        close(ext_sock);
    }

    if (int_sock != -1)
    {
        shutdown(int_sock, SHUT_RDWR);
        close(int_sock);
    }

    if (up_sock != -1)
    {
        shutdown(up_sock, SHUT_RDWR);
        close(up_sock);
    }
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Listens to a socket for an incoming request

@param[in]     sock       - Socket to listen to
@param[in,out] max_fd     - Max file descriptor in set
@param[out]    socket_set - Set of sockets we are listening to, 'sock' will be added in success case

@note
- On failure, the socket will not be added to socket_set and max_fd will not be updated
*//*==============================================================================================*/
void dg_socket_listen_sock(int sock, int* max_fd, fd_set* socket_set)
{
    /* listen for incoming requests */
    if (dg_socket_set_non_blocking_mode(sock, TRUE) == FALSE)
    {
        DG_DBG_ERROR("Error setting socket %d to non-blocking mode", sock);
    }
    else if (listen(sock,  DG_SOCKET_MAX_PENDING_CONNECTION_QUEUE) == -1)
    {
        DG_DBG_ERROR("Error listening on DIAG socket %d, errno = %d(%s)", sock, errno,
                     strerror(errno));
    }
    else
    {
        *max_fd = (sock > *max_fd) ? sock : *max_fd;
        FD_SET(sock, socket_set);
        DG_DBG_TRACE("Listening for DIAG clients on socket %d", sock);
    }
}

/*=============================================================================================*//**
@brief Accepts an incoming client connection

@param[in] listen_sock - Socket the incoming request is coming in on
*//*==============================================================================================*/
void dg_socket_accept_client_connect(int listen_sock, DG_CLIENT_COMM_CLIENT_TYPE_T type)
{
    BOOL            is_success  = FALSE;
    int             client_sock = -1;
    struct sockaddr serv_addr;
    socklen_t       sock_len = sizeof(serv_addr);
    pthread_t       tid;

    /* Accept the incoming request */
    DG_DBG_TRACE("Attempting to accept new client sock on listen sock %d", listen_sock);
    if ((client_sock = accept(listen_sock, &serv_addr, &sock_len)) == -1)
    {
        DG_DBG_ERROR("Error accepting incoming DIAG connection request: %s",
                     strerror(errno));
    }
    else
    {
        DG_DBG_TRACE("Accepted client socket request, client_sock = %d", client_sock);

        /* Ensure the incoming client is allowed */
        if (DG_PAL_UTIL_is_socket_allowed(client_sock) != TRUE)
        {
            DG_DBG_ERROR("Socket connection is not allowed (%d)", client_sock);
        }
        /* A client connected, set it to blocking mode */
        else if (dg_socket_set_non_blocking_mode(client_sock, FALSE) != TRUE)
        {
            DG_DBG_ERROR("Failed to enable blocking mode for the socket %d", client_sock);
        }
        /* Add client to the client list */
        else if (DG_CLIENT_COMM_add_client_to_list(client_sock, type) != TRUE)
        {
            /* Error adding socket, close it */
            DG_DBG_ERROR("Error adding connecting socket(%d) to list of client sockets",
                         client_sock);
        }
        else
        {
            /* Create a thread to handle the client's requests */
            if (pthread_create(&tid, NULL,
                               &DG_CLIENT_COMM_client_connection_handler,
                               (void*)(intptr_t)client_sock) != 0)
            {
                DG_DBG_ERROR("Error creating client thread for connection socket: %d",
                             client_sock);
                DG_MAIN_engine_exit_flag = TRUE;
            }
            else
            {
                DG_DBG_TRACE("Created client thread (%p) to handle connection socket: %d",
                             (void*)tid, client_sock);
                pthread_detach(tid);
                is_success = TRUE;
            }
        }
    }

    /* Close any client sockets where an error occured */
    if ((is_success == FALSE) && (client_sock != -1))
    {
        shutdown(client_sock, SHUT_RDWR);
        close(client_sock);
    }
}

/*=============================================================================================*//**
@brief Enable/disables non blocking mode on a socket

@param[in] fd     - Socket to set
@param[in] on_off - TRUE = enable non blocking, FALSE = disable
*//*==============================================================================================*/
BOOL dg_socket_set_non_blocking_mode(int fd, BOOL is_enable)
{
    BOOL is_success = FALSE;
    int  fd_flags   = 0;

    /* Get the socket's flags */
    if ((fd_flags = fcntl(fd, F_GETFL)) == -1)
    {
        DG_DBG_ERROR("Failed to read socket %d flags, errno = %d (%s)",
                     fd, errno, strerror(errno));
    }
    else
    {
        /* Set the non blocking flag as desired */
        if (is_enable == TRUE)
        {
            fd_flags |= O_NONBLOCK;
        }
        else
        {
            fd_flags &= (~O_NONBLOCK);
        }

        if (fcntl(fd, F_SETFL, fd_flags) != 0)
        {
            DG_DBG_ERROR("Failed to set socket %d flags to %d, errno = %d (%s)",
                         fd, fd_flags, errno, strerror(errno));
        }
        else
        {
            is_success = TRUE;
        }
    }

    return is_success;
}

/*=============================================================================================*//**
@brief This function handle the SIGINT signal

@param[in] sig - The signal

@note
  - This function is a way to let the select() api return thus we can exit thread safely
*//*==============================================================================================*/
void dg_socket_exit_handler(int sig)
{
    DG_DBG_TRACE("Diag socket listener got signaled: sig = %d", sig);
}

