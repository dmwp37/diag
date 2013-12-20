/*==================================================================================================

    Module Name:  dg_client_api.c

    General Description: Implements an API for clients to communicate with DIAG server/engine

====================================================================================================

Revision History:
                            Modification     Tracking
Author                          Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
Xudong Huang    - xudongh    2013/12/11     xxxxx-0000   Creation
Xudong Huang    - xudongh    2013/12/19     xxxxx-0001   Update diag rsp protocol
Xudong Huang    - xudongh    2013/12/20     xxxxx-0002   Update diag req protocol

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>

#include "dg_client_api.h"
#include "dg_defs.h"
#include "dg_pal_client_api.h"

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                            LOCAL MACROS
==================================================================================================*/
#ifdef DG_DEBUG
    /* printf-style debug message print */
    #define DG_CLIENT_API_TRACE(x...)  do { printf("DIAG API: "x); printf("\n"); } while (0)
    /* printf-style error message print */
    #define DG_CLIENT_API_ERROR(x...)  do { printf("DIAG API ERROR: "x); printf("\n"); } while (0)
#else
    #define DG_CLIENT_API_TRACE(x...)
    #define DG_CLIENT_API_ERROR(x...)
#endif


#define DG_CLIENT_API_READ_TCP_RETRY_NUM 3           /**< Number of times to retry reading a socket */
#define DG_CLIENT_API_SERVER_IP_ADDR     "127.0.0.1" /**< IP address of DIAG server */
#define DG_CLIENT_API_SERVER_PORT        11000       /**< Port of DIAG server */

#define DG_CLIENT_API_OPCODE_ECHO    0x0FFE  /**< Opcode for ECHO command */

/*==================================================================================================
                             LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** Structure for a ECHO diag request */
typedef struct
{
    UINT32 sub_command;   /**< sub echocommand always 0x00000000      */
    UINT8 echo_data[1];   /**< To be echoed data                      */
} DG_CLIENT_API_REQ_ECHO_T;


/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
=================================================================================================*/
static DG_CLIENT_API_STATUS_T dg_client_api_read_socket(int socket, UINT32 len_to_read,
                                                        UINT8* buff_ptr);
static void dg_client_api_req_hdr_hton(DG_DEFS_DIAG_REQ_HDR_T* hdr_in,
                                       DG_DEFS_DIAG_REQ_HDR_T* hdr_out);
static void dg_client_api_rsp_hdr_ntoh(DG_DEFS_DIAG_RSP_HDR_T* hdr_in,
                                       DG_DEFS_DIAG_RSP_HDR_T* hdr_out);

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
@brief Starts the DIAG server

@return Status of function
*//*==============================================================================================*/
DG_CLIENT_API_STATUS_T DG_CLIENT_API_launch_server(void)
{
    DG_CLIENT_API_STATUS_T status = DG_CLIENT_API_STATUS_ERROR;

    if (DG_PAL_CLIENT_API_start_diag_app() != TRUE)
    {
        DG_CLIENT_API_ERROR("Launching DIAG application failed!");
    }
    else
    {
        status = DG_CLIENT_API_STATUS_SUCCESS;
    }

    return status;
}

/*=============================================================================================*//**
@brief Begins a client session with the DIAG server/engine

@param [in]   timeout_in_msec - Timeout time for connection, in msec. Timeout of 0 means wait
                                forever.
@param [out]  socket_ptr      - Pointer to socket for server connection, only valid when
                                DG_CLIENT_API_STATUS_SUCCESS returned

@return Status of function
*//*==============================================================================================*/
DG_CLIENT_API_STATUS_T DG_CLIENT_API_connect_to_server(int                      timeout_in_msec,
                                                       DG_CLIENT_API_SESSION_T* socket_ptr)
{
    int                     socket_fd = -1;
    struct sockaddr_storage server;
    socklen_t               len       = 0;
    long                    orig_flags;
    fd_set                  fds;
    struct timeval          timeout;
    struct timeval*         timeout_ptr = NULL;
    int                     select_status;
    int                     error_value = 0;
    socklen_t               errval_size = sizeof(error_value);
    DG_CLIENT_API_STATUS_T  status      = DG_CLIENT_API_STATUS_ERROR;

    if (socket_ptr == NULL)
    {
        status = DG_CLIENT_API_STATUS_ERR_PARM;
    }
    else
    {
        socket_fd = DG_PAL_CLIENT_API_create_int_diag_socket(&server, &len);
        if (socket_fd != -1)
        {
            DG_CLIENT_API_TRACE("Attempting to connect to socket %d", socket_fd);

            orig_flags = fcntl(socket_fd, F_GETFL);
            if ((orig_flags == -1) ||
                (fcntl(socket_fd, F_SETFL, O_NONBLOCK) == -1))
            {
                DG_CLIENT_API_ERROR("Failed disabling blocking. errno=%d (%s)",
                                    errno, strerror(errno));
            }
            else
            {
                connect(socket_fd, (struct sockaddr*)&server, len);

                switch (errno)
                {
                    case 0:
                    {
                        /* success */
                        DG_CLIENT_API_TRACE("Successfully connected!");
                        status = DG_CLIENT_API_STATUS_SUCCESS;
                        break;
                    }
                    default:
                    {
                        DG_CLIENT_API_TRACE("waiting for connection. errno = %d (%s)",
                                            errno, strerror(errno));

                        /* Connection is taking some time.  Start timeout timer and wait for
                           connection */
                        FD_ZERO(&fds);
                        FD_SET(socket_fd, &fds);

                        /* Set timeout if one provided */
                        if (timeout_in_msec)
                        {
                            DG_CLIENT_API_TRACE("Using timeout of %d milliseconds",
                                                timeout_in_msec);
                            timeout.tv_sec  = (timeout_in_msec / 1000);
                            timeout.tv_usec = (timeout_in_msec % 1000) * 1000;
                            timeout_ptr     = &timeout;
                        }
                        else
                        {
                            DG_CLIENT_API_TRACE("No timeout, wait forver");
                            timeout_ptr = NULL;
                        }

                        DG_CLIENT_API_TRACE("Waiting for connect() to finish");
                        select_status = select(socket_fd + 1, NULL, &fds, NULL, timeout_ptr);
                        if (select_status == 0)
                        {
                            DG_CLIENT_API_ERROR("Timeout connecting to server");
                            status = DG_CLIENT_API_STATUS_TIMEOUT;
                        }
                        else if (select_status < 1)
                        {
                            /* A non-timeout error occurred */
                            DG_CLIENT_API_ERROR("Select failed, select status = %d: errno=%d (%s)",
                                                select_status, errno, strerror(errno));
                        }
                        else
                        {
                            /* determine if connect() was eventually successful */
                            if (getsockopt(socket_fd, SOL_SOCKET, SO_ERROR, &error_value,
                                           &errval_size) != 0)
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
                                status      = DG_CLIENT_API_STATUS_SUCCESS;
                                *socket_ptr = socket_fd;
                            }
                        }
                        break;
                    }
                }
            }
        }

        if (status == DG_CLIENT_API_STATUS_SUCCESS)
        {
            fcntl(socket_fd, F_SETFL, orig_flags);

            *socket_ptr = socket_fd;
        }
        else
        {
            close(socket_fd);
        }
    }

    return status;
}

/*=============================================================================================*//**
@brief Ends a client sessions initiated by DG_CLIENT_API_connect_to_server()

@param [in] socket - Socket to close

@return Status of function
*//*==============================================================================================*/
DG_CLIENT_API_STATUS_T DG_CLIENT_API_disconnect_from_server(DG_CLIENT_API_SESSION_T socket)
{
    DG_CLIENT_API_STATUS_T status = DG_CLIENT_API_STATUS_ERROR;

    if (close(socket) == 0)
    {
        DG_CLIENT_API_TRACE("Closed socket %d", socket);
        status = DG_CLIENT_API_STATUS_SUCCESS;
    }

    return status;
}

/*=============================================================================================*//**
@brief Creates and sends a DIAG request to the server

@param [in] socket        - Server connection socket
@param [in] opcode        - Opcode of DIAG request
@param [in] timestamp     - Timestamp of DIAG request
@param [in] req_len       - Length of request data
@param [in] req_data_ptr  - Pointer to request data, can be null if req_len is 0

@return Status of function
*//*==============================================================================================*/
DG_CLIENT_API_STATUS_T DG_CLIENT_API_send_diag_req(DG_CLIENT_API_SESSION_T socket, UINT16 opcode,
                                                   UINT16 timestamp, UINT32 req_len,
                                                   UINT8* req_data_ptr)
{
    int                    write_return_len = 0;
    DG_CLIENT_API_STATUS_T status           = DG_CLIENT_API_STATUS_ERROR;
    UINT32                 write_len        = 0;
    UINT8*                 write_buff       = NULL;
    DG_DEFS_DIAG_REQ_HDR_T cmd_header_h; /* DIAG Header in endianess of host */

    if ((req_len != 0) && (req_data_ptr == NULL))
    {
        status = DG_CLIENT_API_STATUS_ERR_PARM;
    }
    else
    {
        /* Init DIAG request header */
        memset(&cmd_header_h, 0, sizeof(cmd_header_h));
        cmd_header_h.seq_tag = timestamp;
        cmd_header_h.opcode  = opcode;
        cmd_header_h.length  = req_len;

        /* Calculate write length (command data + header) */
        write_len            = cmd_header_h.length + sizeof(cmd_header_h);
        if (!(write_buff = (UINT8*)malloc(write_len)))
        {
            DG_CLIENT_API_ERROR("Malloc failed for write_buff, attempted len = %d", write_len);
            status = DG_CLIENT_API_STATUS_MALLOC_FAIL;
        }
        else
        {
            /* Do endian swap into buffer */
            dg_client_api_req_hdr_hton(&cmd_header_h, (DG_DEFS_DIAG_REQ_HDR_T*)write_buff);

            /* Copy command data into buffer if it exists */
            if (cmd_header_h.length > 0)
            {
                memcpy((write_buff + sizeof(cmd_header_h)), req_data_ptr, cmd_header_h.length);
            }

            if ((write_return_len = write(socket, write_buff, write_len)) != (int)write_len)
            {
                DG_CLIENT_API_ERROR("Failed writing %d bytes to fd %d. errno=%d (%s)",
                                     write_len, socket, errno, strerror(errno));
            }
            else
            {
                status = DG_CLIENT_API_STATUS_SUCCESS;
                DG_CLIENT_API_TRACE("Successfully sent %d byte(s) DIAG opcode = 0x%04x to fd %d.",
                                    write_len, cmd_header_h.opcode, socket);
            }

            free(write_buff);
        }
    }

    return status;
}

/*=============================================================================================*//**
@brief Sends raw data to the DIAG server

@param [in] socket       - Server connection socket
@param [in] raw_data_len - Length of data to send
@param [in] raw_data_ptr - Pointer to data to send

@return Status of function

@note
 - This is a function is to be used only in cases where a client is directly given a completed
   DIAG request to pass to the DIAG server.  DG_CLIENT_API_send_diag_req() should be used in most
   cases to send a request.
*//*==============================================================================================*/
DG_CLIENT_API_STATUS_T DG_CLIENT_API_send_diag_req_raw(DG_CLIENT_API_SESSION_T socket,
                                                       UINT32                  raw_data_len,
                                                       UINT8*                  raw_data_ptr)
{
    int                    wrote_data_len = 0;
    DG_CLIENT_API_STATUS_T status         = DG_CLIENT_API_STATUS_ERROR;

    if ((raw_data_len == 0) || (raw_data_ptr == NULL))
    {
        status = DG_CLIENT_API_STATUS_ERR_PARM;
    }
    else
    {
        wrote_data_len = write(socket, raw_data_ptr, raw_data_len);
        if (wrote_data_len != (int)raw_data_len)
        {
            DG_CLIENT_API_ERROR("Failed writing %d bytes to fd %d, wrote %d bytes. errno=%d (%s)",
                                raw_data_len, socket, wrote_data_len, errno, strerror(errno));
        }
        else
        {
            status = DG_CLIENT_API_STATUS_SUCCESS;
            DG_CLIENT_API_TRACE("Successfully sent %d byte(s) to fd %d.",
                                raw_data_len, socket);
        }
    }

    return status;
}

/*=============================================================================================*//**
@brief Attempts to receive the desired DIAG response from the specified socket.

@param [in] socket        - Server connection socket
@param [in] opcode        - Opcode of the expected response
@param [in] timestamp     - Timestamp of the expected response
@param [in] is_unsol      - If the expected response is unsolicited or not
@param [in] timeout_in_ms - Time to wait for a DIAG response (in msec), 0 means wait forever
@param [out] rsp_len_ptr  - Length of received DIAG response
@param [out] status_ptr   - Status of function

@return Pointer to DIAG response

@note
  - All non-desired DIAG responses that are received will be deleted
  - The calling function is responsible for freeing the memory pointed to by the returned pointer
    by using DG_CLIENT_API_diag_rsp_free()
  - A valid diag response pointer is only returned in a success case
  - If the expected response is unsolicited, the timestamp parameter is ignored
*//*==============================================================================================*/
UINT8* DG_CLIENT_API_rcv_desired_diag_rsp(DG_CLIENT_API_SESSION_T socket, UINT16 opcode,
                                          UINT16 timestamp, BOOL is_unsol,
                                          UINT32 timeout_in_ms, UINT32* rsp_len_ptr,
                                          DG_CLIENT_API_STATUS_T* status_ptr)
{
    BOOL   is_rcvd_unsol   = FALSE;
    UINT16  rcvd_timestamp = 0;
    UINT16 rcvd_opcode     = 0;
    UINT8* diag_rsp        = NULL;
    BOOL   got_rsp         = FALSE;

    *status_ptr = DG_CLIENT_API_STATUS_SUCCESS;

    /* While we have not received the desired response and there are no errors, wait for response */
    while ((got_rsp == FALSE) && (*status_ptr == DG_CLIENT_API_STATUS_SUCCESS))
    {
        diag_rsp = DG_CLIENT_API_rcv_diag_rsp(socket, timeout_in_ms, rsp_len_ptr, status_ptr);
        if ((*status_ptr == DG_CLIENT_API_STATUS_SUCCESS) && (diag_rsp != NULL))
        {
            if ((*status_ptr = DG_CLIENT_API_parse_diag_rsp(diag_rsp, *rsp_len_ptr, NULL,
                                                            &is_rcvd_unsol, &rcvd_timestamp,
                                                            &rcvd_opcode, NULL, NULL, NULL))
                == DG_CLIENT_API_STATUS_SUCCESS)
            {
                /* Make sure the received response matches what was request.  If we are waiting for
                   an unsolicited response, the timestamp will be set to 0 in the response. */
                if (((rcvd_opcode == opcode) && (rcvd_timestamp == timestamp)
                     && (is_rcvd_unsol == is_unsol) && (is_unsol == FALSE)) ||
                    ((rcvd_opcode == opcode) && (rcvd_timestamp == 0)
                     && (is_rcvd_unsol == is_unsol) && (is_unsol == TRUE)))
                {
                    DG_CLIENT_API_TRACE("Got the response we are looking for");
                    got_rsp = TRUE;
                }
            }
        }

        if (got_rsp == FALSE)
        {
            DG_CLIENT_API_TRACE("Got the wrong response!!");
            free(diag_rsp);
            diag_rsp = NULL;
        }
    }

    return diag_rsp;
}

/*=============================================================================================*//**
@brief Attempts to receive a DIAG response from the specified socket

@param [in]  socket        - Server connection socket
@param [in]  timeout_in_ms - Time to wait for a DIAG response (in msec), 0 means wait forever
@param [out] rsp_len_ptr   - Length of received DIAG response
@param [out] status_ptr    - Status of function

@return Pointer to DIAG response

@note
  - The calling function is responsible for freeing the memory used for the DIAG response by using
    DG_CLIENT_API_diag_rsp_free()
  - A valid diag response pointer and response legnth is only returned in a success case
*//*==============================================================================================*/
UINT8* DG_CLIENT_API_rcv_diag_rsp(DG_CLIENT_API_SESSION_T socket, UINT32 timeout_in_ms,
                                  UINT32* rsp_len_ptr, DG_CLIENT_API_STATUS_T* status_ptr)
{
    UINT32                 total_len;
    UINT8*                 diag_rsp    = NULL;
    struct timeval         timeout;
    struct timeval*        timeout_ptr = NULL;
    fd_set                 read_fd_set;
    DG_DEFS_DIAG_RSP_HDR_T orig_hdr;
    DG_DEFS_DIAG_RSP_HDR_T converted_hdr;
    int                    select_status;

    if (status_ptr != NULL)
    {
        *status_ptr = DG_CLIENT_API_STATUS_ERROR;
        if (rsp_len_ptr == NULL)
        {
            *status_ptr = DG_CLIENT_API_STATUS_ERR_PARM;
        }
        else
        {
            DG_CLIENT_API_TRACE("Receive response from server connection %d, timeout = %d",
                                socket, timeout_in_ms);

            /* Convert the timeout (in milliseconds) to seconds & microseconds if timeout used */
            if (timeout_in_ms)
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
            FD_ZERO(&read_fd_set);
            FD_SET(socket, &read_fd_set);
            select_status = select(socket + 1, &read_fd_set, NULL, NULL, timeout_ptr);
            if (select_status == 0)
            {
                DG_CLIENT_API_TRACE("Timeout occurred!");
                *status_ptr = DG_CLIENT_API_STATUS_TIMEOUT;
            }
            else if (select_status != 1)
            {
                DG_CLIENT_API_ERROR("Select failed, select_status = %d. errno=%d (%s)",
                                     select_status, errno, strerror(errno));
            }
            else
            {
                /* Data is ready on the socket, attempt to read the DIAG response header */
                DG_CLIENT_API_TRACE("Attempt to read response header.");
                if ((*status_ptr =
                         dg_client_api_read_socket(socket, sizeof(orig_hdr), (UINT8*)&orig_hdr))
                    == DG_CLIENT_API_STATUS_SUCCESS)
                {
                    /* Convert the endianess of the header and determine the total length of
                       the response */
                    dg_client_api_rsp_hdr_ntoh(&orig_hdr, &converted_hdr);
                    total_len = sizeof(orig_hdr) + converted_hdr.length;
                    DG_CLIENT_API_TRACE("Total length of response: %d", total_len);
                    if ((diag_rsp = (UINT8*)malloc(total_len)) != NULL)
                    {
                        *rsp_len_ptr = total_len;
                        memcpy(diag_rsp, &converted_hdr, sizeof(converted_hdr));

                        /* Attempt to read the response data if present */
                        if (converted_hdr.length > 0)
                        {
                           *status_ptr = dg_client_api_read_socket(socket, converted_hdr.length,
                                                                   diag_rsp + sizeof(orig_hdr));
                            if (*status_ptr != DG_CLIENT_API_STATUS_SUCCESS)
                            {
                                /* Reading the data failed, cleanup */
                                free(diag_rsp);
                                diag_rsp     = NULL;
                                *rsp_len_ptr = 0;
                            }
                        }
                    }
                    else
                    {
                        /* Memory allocation failed, return error */
                        DG_CLIENT_API_ERROR("Allocating %d bytes failed.", total_len);
                        *rsp_len_ptr = 0;
                        *status_ptr  = DG_CLIENT_API_STATUS_MALLOC_FAIL;
                    }
                }
            }
        }
    }
    return diag_rsp;
}

/*=============================================================================================*//**
@brief Parses a DIAG response

@param [in]  rsp_ptr             - Pointer to DIAG response to parse
@param [in]  rsp_len             - Length of DIAG response
@param [out] diag_fail_ptr       - Whether the response indicates a failure or not (optional)
@param [out] unsol_rsp_ptr       - Whether the response is unsolicited or not (optional)
@param [out] timestamp_ptr       - Timestamp of response (optional)
@param [out] opcode_ptr          - Opcode of response (optional)
@param [out] rsp_code_ptr        - Response code of response (optional)
@param [out] data_offset_val_ptr - Offset to response data part of response (optional)
@param [out] data_len_ptr        - Length of response data (optional)

@return If the response was properly parsed or not

@note
 - For the optional parameters, the parameter will only be populated if the pointer is non-NULL and
   the function returns successfully
*//*==============================================================================================*/
DG_CLIENT_API_STATUS_T DG_CLIENT_API_parse_diag_rsp(UINT8* rsp_ptr, UINT32 rsp_len,
                                                    BOOL* diag_fail_ptr, BOOL* unsol_rsp_ptr,
                                                    UINT16* timestamp_ptr, UINT16* opcode_ptr,
                                                    UINT8* rsp_code_ptr,
                                                    UINT32* data_offset_val_ptr,
                                                    UINT32* data_len_ptr)
{
    DG_CLIENT_API_STATUS_T status = DG_CLIENT_API_STATUS_ERROR;
    DG_DEFS_DIAG_RSP_HDR_T hdr    = *(DG_DEFS_DIAG_RSP_HDR_T*)rsp_ptr;

    if (rsp_len == (hdr.length + sizeof(hdr)))
    {
        status = DG_CLIENT_API_STATUS_SUCCESS;

        /* Set the fields only if a valid pointer was passed */
        if (unsol_rsp_ptr)
        {
            *unsol_rsp_ptr = (hdr.unsol_rsp_flag) ? TRUE : FALSE;
        }
        if (diag_fail_ptr)
        {
            *diag_fail_ptr = (hdr.rsp_code != DG_RSP_CODE_CMD_RSP_GENERIC) ? TRUE : FALSE;
        }
        if (timestamp_ptr)
        {
            *timestamp_ptr = hdr.seq_tag;
        }
        if (opcode_ptr)
        {
            *opcode_ptr = hdr.opcode;
        }
        if (rsp_code_ptr)
        {
            *rsp_code_ptr = hdr.rsp_code;
        }
        if (data_offset_val_ptr)
        {
            *data_offset_val_ptr = sizeof(hdr);
        }
        if (data_len_ptr)
        {
            *data_len_ptr = hdr.length;
        }
    }

    return status;
}

/*=============================================================================================*//**
@brief Reports if a given DIAG response indicates a failure

@param [in] diag_rsp - The DIAG response to check
@param [in] rsp_len  - Length of the DIAG response

@return TRUE if the response is a failure
*//*==============================================================================================*/
BOOL DG_CLIENT_API_check_rsp_for_fail(UINT8* diag_rsp, UINT32 rsp_len)
{
    BOOL is_rsp_failure = FALSE;

    if (DG_CLIENT_API_parse_diag_rsp(diag_rsp, rsp_len, &is_rsp_failure, NULL,
                                     NULL, NULL, NULL, NULL, NULL) != DG_CLIENT_API_STATUS_SUCCESS)
    {
        is_rsp_failure = TRUE;
    }
    return is_rsp_failure;
}

/*=============================================================================================*//**
@brief Frees a given DIAG response

@param [in] diag_rsp - The DIAG response to free
*//*==============================================================================================*/
void DG_CLIENT_API_diag_rsp_free(UINT8* diag_rsp)
{
    free(diag_rsp);
}

/*=============================================================================================*//**
@brief Echo data

@param[in/out]  data        - data buffer to echo, when returned will fill responsed echo data
@param[in]      size        - the size of the data buffer
@param[in]  connect_timeout - Amount of time to wait to connect to DIAG engine (in msec, 0 means
                              wait forever)
@param[in]  rsp_timeout     - Amount of time to wait for DIAG response (in msec, 0 means wait
                              forever)

@return Status of the function

@note
- Calling function is responsible for allocating (and freeing)
*//*==============================================================================================*/
DG_CLIENT_API_STATUS_T DG_CLIENT_API_echo_data(UINT8* data, UINT32 size,
                                               UINT32 connect_timeout, UINT32 rsp_timeout)
{
    DG_CLIENT_API_REQ_ECHO_T* echo_req;
    DG_CLIENT_API_STATUS_T    status       = DG_CLIENT_API_STATUS_ERROR;
    DG_CLIENT_API_SESSION_T   diag_session;
    UINT8*                    diag_rsp     = NULL;
    UINT32                    diag_rsp_len = 0;
    UINT32                    rsp_data_off = 0;
    UINT32                    rsp_data_len = 0;
    UINT32                    cmd_len      = 0;
    BOOL                      is_failed    = TRUE;

    DG_CLIENT_API_TRACE("Requested echo data size = %d", size);

    cmd_len = offsetof(DG_CLIENT_API_REQ_ECHO_T, echo_data) + size;

    if (data == NULL)
    {
        DG_CLIENT_API_ERROR("Invalid pass in parameter!");
        status = DG_CLIENT_API_STATUS_ERR_PARM;
    }
    else if ((status = DG_CLIENT_API_connect_to_server(connect_timeout, &diag_session))
             != DG_CLIENT_API_STATUS_SUCCESS)
    {
        DG_CLIENT_API_ERROR("Connect to server failed!");
        status = DG_CLIENT_API_STATUS_ERROR;
    }
    else if ((echo_req = (DG_CLIENT_API_REQ_ECHO_T*)malloc(cmd_len)) == NULL)
    {
        DG_CLIENT_API_ERROR("Malloc ECHO req failed! length = %d", cmd_len);
        DG_CLIENT_API_disconnect_from_server(diag_session);
        status = DG_CLIENT_API_STATUS_MALLOC_FAIL;
    }
    else
    {
        echo_req->sub_command = htonl(0x00000000); /* for echo request */
        memcpy((UINT8*)(echo_req->echo_data), data, size);

        /* Send echo request */
        status = DG_CLIENT_API_send_diag_req(diag_session, DG_CLIENT_API_OPCODE_ECHO,
                                             0, cmd_len, (UINT8*)echo_req);

        /* after send the request we release the resource */
        free(echo_req);

        if (status != DG_CLIENT_API_STATUS_SUCCESS)
        {
            DG_CLIENT_API_ERROR("Send DIAG ECHO request failed!");
            status = DG_CLIENT_API_STATUS_ERROR;
        }
        else
        {
            /* Get the ECHO response */
            diag_rsp = DG_CLIENT_API_rcv_desired_diag_rsp(
                diag_session, DG_CLIENT_API_OPCODE_ECHO, 0,
                FALSE, rsp_timeout, &diag_rsp_len, &status);
            if (status != DG_CLIENT_API_STATUS_SUCCESS)
            {
                DG_CLIENT_API_ERROR("Receive DIAG ECHO response failed!");
                status = DG_CLIENT_API_STATUS_ERROR;
            }
            else
            {
                /* If successfully got response, parse it.
                If response is success, check the data length */
                status = DG_CLIENT_API_parse_diag_rsp(diag_rsp, diag_rsp_len, &is_failed,
                                                      NULL, NULL, NULL, NULL,
                                                      &rsp_data_off, &rsp_data_len);

                DG_CLIENT_API_TRACE("Parse status = %d, is_failed = %d", status, is_failed);

                if (status != DG_CLIENT_API_STATUS_SUCCESS)
                {
                    DG_CLIENT_API_ERROR("Parse DIAG ECHO response failed!");
                    status = DG_CLIENT_API_STATUS_ERROR;
                }
                else if (is_failed == TRUE)
                {
                    DG_CLIENT_API_ERROR("ECHO data failed!");
                    status = DG_CLIENT_API_STATUS_ERROR;
                }
                else if (rsp_data_len != size)
                {
                    DG_CLIENT_API_ERROR("ECHO data size mismatch! origin: %d, receive: %d",
                                        size, rsp_data_len);
                    status = DG_CLIENT_API_STATUS_ERROR;
                }
                else
                {
                    memcpy(data, diag_rsp + rsp_data_off, size);
                }

                DG_CLIENT_API_diag_rsp_free(diag_rsp);
            }
        }

        DG_CLIENT_API_disconnect_from_server(diag_session);
    }

    /* Return status code */
    DG_CLIENT_API_TRACE("Return status = %d", status);
    return status;
}


/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Reads specified number of bytes data from a socket

@param [in] socket      - Socket to read from
@param [in] len_to_read - Number of bytes to read
@param [out] buff_ptr   - Buffer to read data into

@return Status of function
*//*==============================================================================================*/
DG_CLIENT_API_STATUS_T dg_client_api_read_socket(int socket, UINT32 len_to_read, UINT8* buff_ptr)
{
    DG_CLIENT_API_STATUS_T status             = DG_CLIENT_API_STATUS_SUCCESS;
    INT32                  total_bytes_read   = 0;
    INT32                  current_bytes_read = 0;
    INT32                  try_count          = 0;

    DG_CLIENT_API_TRACE("Attempting to read %d bytes from %d.", len_to_read, socket);

    /* Continue to read until an error occurs or we read the desired number of bytes */
    while ((status == DG_CLIENT_API_STATUS_SUCCESS) && (total_bytes_read != (int)len_to_read))
    {
        current_bytes_read = read(socket, (buff_ptr + total_bytes_read),
                                  (len_to_read - total_bytes_read));
        if (current_bytes_read > 0)
        {
            total_bytes_read += current_bytes_read;
        }
        else if ((current_bytes_read == -1) && ((errno == EAGAIN)
                 || (errno == EWOULDBLOCK) || (errno == ETIMEDOUT)))
        {
            DG_CLIENT_API_ERROR("Timeout occurred while waiting for DIAG response. errno=%d (%s)",
                                errno, strerror(errno));
            status = DG_CLIENT_API_STATUS_TIMEOUT;
        }
        else if (++try_count == DG_CLIENT_API_READ_TCP_RETRY_NUM)
        {
            DG_CLIENT_API_ERROR("Read Failed, over number of retries.");
            status = DG_CLIENT_API_STATUS_ERROR;
        }
    }

    return status;
}

/*=============================================================================================*//**
@brief Converts a request header in host-endian to network-endian

@param [in]  hdr_in  - Host-endian DIAG request header
@param [out] hdr_out - Network-endian DIAG request header
*//*==============================================================================================*/
void dg_client_api_req_hdr_hton(DG_DEFS_DIAG_REQ_HDR_T* hdr_in, DG_DEFS_DIAG_REQ_HDR_T* hdr_out)
{
    memcpy(hdr_out, hdr_in, sizeof(DG_DEFS_DIAG_REQ_HDR_T));
    hdr_out->opcode = htons(hdr_in->opcode);
    hdr_out->seq_tag = htons(hdr_in->seq_tag);
    hdr_out->length = htonl(hdr_in->length);
}

/*=============================================================================================*//**
@brief Converts a response header in network-endian to host-endian

@param [in]  hdr_in  - Network-endian DIAG response header
@param [out] hdr_out - Host-endian DIAG response header
*//*==============================================================================================*/
void dg_client_api_rsp_hdr_ntoh(DG_DEFS_DIAG_RSP_HDR_T* hdr_in, DG_DEFS_DIAG_RSP_HDR_T* hdr_out)
{
    memcpy(hdr_out, hdr_in, sizeof(DG_DEFS_DIAG_RSP_HDR_T));
    
    hdr_out->opcode  = ntohs(hdr_in->opcode);
    hdr_out->seq_tag = ntohs(hdr_in->seq_tag);
    hdr_out->length  = ntohl(hdr_in->length);
}

