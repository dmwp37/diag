/*==================================================================================================

    Module Name:  dg_pal_client_api.c

    General Description: Implements the PAL for Client API

====================================================================================================

====================================================================================================
                                            INCLUDE FILES
==================================================================================================*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <WinSock2.h>
#include "dg_pal_client_platform_inc.h"
#include "dg_defs.h"
#include "dg_pal_client_api.h"

#pragma comment(lib, "Ws2_32.lib")


/*==================================================================================================
                                           LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                            LOCAL MACROS
==================================================================================================*/
#define DG_PAL_CLIENT_API_READ_TCP_RETRY_NUM 3     /**< Number of times to retry reading a socket  */
#define DG_PAL_CLIENT_API_CONNECT_WAIT       5000  /**< Max wait time in ms to connect diag socket */
#define DG_PAL_CLIENT_API_SERVER_PORT        11000 /**< Port of DIAG server       */

/*==================================================================================================
                             LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
typedef int socklen_t;

/*==================================================================================================
                                      LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static BOOL dg_pal_client_api_connect_socket(int socket, struct sockaddr* addr, socklen_t len);

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

@return File descriptor for the socket, -1 on fail
*//*==============================================================================================*/
int DG_PAL_CLIENT_API_create_int_diag_socket()
{
    int socket_fd = -1;
    DG_CLIENT_API_ERROR("Windows does not support unix socket, use external socket!");
    return socket_fd;
}

/*=============================================================================================*//**
@brief Creates socket for use by external DIAG clients

@param [in] serv_addr - the diag server address

@return File descriptor for the socket, -1 on fail
*//*==============================================================================================*/
int DG_PAL_CLIENT_API_create_ext_diag_socket(const char* serv_addr)
{
    int socket_fd = -1;

    struct hostent*    he;
    struct sockaddr_in address;

    WORD    wVersionRequested;
    WSADATA wsaData;
    int     err;

    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0)
    {
        DG_CLIENT_API_ERROR("Initializing Socket!");
        return -1;
    }

    /* Confirm that the WinSock DLL supports 2.2.        */
    /* Note that if the DLL supports versions greater    */
    /* than 2.2 in addition to 2.2, it will still return */
    /* 2.2 in wVersion since that is the version we      */
    /* requested.                                        */
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        /* Tell the user that we could not find a usable WinSock DLL.  */
        DG_CLIENT_API_ERROR("Could not find a usable WinSock DLL!");
        WSACleanup();
        return -1;
    }

    /* resolve hostname */
    if ((he = gethostbyname(serv_addr)) == NULL)
    {
        DG_CLIENT_API_ERROR("Invalid server addrres: %s", serv_addr);
        return -1;
    }

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        DG_CLIENT_API_ERROR("Can't create INET socket, errno=%d (%s)", errno, strerror(errno));
        return -1;
    }

    memcpy(&address.sin_addr, he->h_addr_list[0], he->h_length);
    address.sin_family = AF_INET;
    address.sin_port   = htons(DG_PAL_CLIENT_API_SERVER_PORT);

    if (!dg_pal_client_api_connect_socket(socket_fd, (struct sockaddr*)&address, sizeof(address)))
    {
        DG_PAL_CLIENT_API_close_diag_socket(socket_fd);
        socket_fd = -1;
    }

    return socket_fd;
}

/*=============================================================================================*//**
@brief Close the client socket

@param [in] socket       - Socket to close

*//*==============================================================================================*/
void DG_PAL_CLIENT_API_close_diag_socket(int socket)
{
    closesocket(socket);
    WSACleanup();
}

/*=============================================================================================*//**
@brief Wait the socket until there is data available

@param [in] socket        - Socket to wait
@param [in] is_read       - TURE for waiting read, FALSE for waiting write
@param [in] timeout_in_ms - Time to wait for a DIAG response (in msec), 0 means wait forever

@return TRUE if success
*//*==============================================================================================*/
BOOL DG_PAL_CLIENT_API_wait(int socket, BOOL is_read, UINT32 timeout_in_ms)
{
    BOOL status = FALSE;

    struct timeval  timeout;
    struct timeval* timeout_ptr = NULL;
    fd_set          fds;
    int             select_status;

    /* Convert the timeout (in milliseconds) to seconds & microseconds if timeout used */
    if (timeout_in_ms != 0)
    {
        DG_CLIENT_API_TRACE("Response timeout = %d milliseconds", timeout_in_ms);
        timeout.tv_sec  = timeout_in_ms / 1000;
        timeout.tv_usec = (timeout_in_ms % 1000) * 1000;
        timeout_ptr     = &timeout;
    }
    else
    {
        DG_CLIENT_API_TRACE("No response timeout, wait forever");
        timeout_ptr = NULL;
    }

    /* Wait for data to be available to read */
    FD_ZERO(&fds);
    FD_SET(socket, &fds);

    if (is_read)
    {
        select_status = select(socket + 1, &fds, NULL, NULL, timeout_ptr);
    }
    else
    {
        select_status = select(socket + 1, NULL, &fds, NULL, timeout_ptr);
    }
    if (select_status == 0)
    {
        DG_CLIENT_API_TRACE("Timeout occurred!");
    }
    else if (select_status != 1)
    {
        DG_CLIENT_API_ERROR("Select failed, select_status = %d. errno=%d (%s)",
                            select_status, errno, strerror(errno));
    }
    else
    {
        status = TRUE;
    }

    return status;
}

/*=============================================================================================*//**
@brief Reads specified number of bytes data from a socket

@param [in] socket      - Socket to read from
@param [in] len_to_read - Number of bytes to read
@param [out] buff_ptr   - Buffer to read data into

@return TRUE if success
*//*==============================================================================================*/
BOOL DG_PAL_CLIENT_API_read(int socket, UINT32 len_to_read, UINT8* buff_ptr)
{
    BOOL  status             = TRUE;
    INT32 total_bytes_read   = 0;
    INT32 current_bytes_read = 0;
    INT32 try_count          = 0;

    DG_CLIENT_API_TRACE("Attempting to read %d bytes from %d.", len_to_read, socket);

    /* Continue to read until an error occurs or we read the desired number of bytes */
    while (status && (total_bytes_read != (int)len_to_read))
    {
        current_bytes_read = recv(socket, (buff_ptr + total_bytes_read),
                                  (len_to_read - total_bytes_read), 0);
        if (current_bytes_read > 0)
        {
            total_bytes_read += current_bytes_read;
        }
        else if ((current_bytes_read == -1) && ((errno == EAGAIN)
                                                || (errno == EWOULDBLOCK) || (errno == ETIMEDOUT)))
        {
            DG_CLIENT_API_ERROR("Timeout occurred while waiting for DIAG response. errno=%d (%s)",
                                errno, strerror(errno));
            status = FALSE;
        }
        else if (++try_count == DG_PAL_CLIENT_API_READ_TCP_RETRY_NUM)
        {
            DG_CLIENT_API_ERROR("Read Failed, over number of retries.");
            status = FALSE;
        }
    }

    return status;
}

/*=============================================================================================*//**
@brief Write specified number of bytes data to a socket

@param [in] socket       - Socket to write to
@param [in] buf          - Buffer to write data from
@param [in] len          - Number of bytes to write

@return TRUE if success
*//*==============================================================================================*/
BOOL DG_PAL_CLIENT_API_write(int socket, UINT8* buf, UINT32 len)
{
    BOOL status = TRUE;

    if (send(socket, buf, len, 0) != (int)len)
    {
        DG_CLIENT_API_ERROR("Failed to send %d bytes to socket %d. errno=%d (%s)",
                            len, socket, errno, strerror(errno));

        status = FALSE;
    }

    return status;
}

/*=============================================================================================*//**
@brief Converts a request header in host-endian to network-endian

@param [in]  hdr_in  - Host-endian DIAG request header
@param [out] hdr_out - Network-endian DIAG request header
*//*==============================================================================================*/
void DG_PAL_CLIENT_API_req_hdr_hton(DG_DEFS_DIAG_REQ_HDR_T* hdr_in, DG_DEFS_DIAG_REQ_HDR_T* hdr_out)
{
    memcpy(hdr_out, hdr_in, sizeof(DG_DEFS_DIAG_REQ_HDR_T));
    hdr_out->opcode  = htons(hdr_in->opcode);
    hdr_out->seq_tag = htons(hdr_in->seq_tag);
    hdr_out->length  = htonl(hdr_in->length);
}

/*=============================================================================================*//**
@brief Converts a response header in network-endian to host-endian

@param [in]  hdr_in  - Network-endian DIAG response header
@param [out] hdr_out - Host-endian DIAG response header
*//*==============================================================================================*/
void DG_PAL_CLIENT_API_rsp_hdr_ntoh(DG_DEFS_DIAG_RSP_HDR_T* hdr_in, DG_DEFS_DIAG_RSP_HDR_T* hdr_out)
{
    memcpy(hdr_out, hdr_in, sizeof(DG_DEFS_DIAG_RSP_HDR_T));

    hdr_out->opcode  = ntohs(hdr_in->opcode);
    hdr_out->seq_tag = ntohs(hdr_in->seq_tag);
    hdr_out->length  = ntohl(hdr_in->length);
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Connect to the diag socket

@param [in]  socket - Network-endian DIAG response header
@param [in]  addr   - Socket address
@param [in]  len    - Length of the socket address

@return TRUE if success
*//*==============================================================================================*/
BOOL dg_pal_client_api_connect_socket(int socket, struct sockaddr* addr, socklen_t len)
{
    BOOL      status      = FALSE;
    u_long    orig_flags  = 1;
    char      error_value = 0;
    socklen_t errval_size = sizeof(error_value);

    if (ioctlsocket(socket, FIONBIO, &orig_flags) != NO_ERROR)
    {
        DG_CLIENT_API_ERROR("Failed to disable blocking. errno=%d (%s)", errno, strerror(errno));
    }
    else
    {
        DG_CLIENT_API_TRACE("Attempting to connect to socket %d", socket);
        connect(socket, addr, len);

        switch (errno)
        {
        case 0:
        {
            /* success */
            DG_CLIENT_API_TRACE("Successfully connected!");
            status = TRUE;
        }
        break;

        default:
        {
            DG_CLIENT_API_TRACE("waiting for connection. errno = %d (%s)", errno, strerror(errno));

            if (DG_PAL_CLIENT_API_wait(socket, FALSE, DG_PAL_CLIENT_API_CONNECT_WAIT))
            {
                /* determine if connect() was eventually successful */
                if (getsockopt(socket, SOL_SOCKET, SO_ERROR, &error_value, &errval_size) != 0)
                {
                    DG_CLIENT_API_ERROR("cannot connect to server. errno=%d (%s)",
                                        errno, strerror(errno));
                }
                else if (error_value != 0)
                {
                    DG_CLIENT_API_ERROR("cannot connect to server, error_value = %d. errno=%d (%s)",
                                        error_value, errno, strerror(errno));
                }
                else
                {
                    DG_CLIENT_API_TRACE("Successfully connected!");
                    orig_flags = 0;
                    ioctlsocket(socket, FIONBIO, &orig_flags);
                    status = TRUE;
                }
            }
        }
        break;
        }
    }

    return status;
}

