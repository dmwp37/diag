/*==================================================================================================

    Module Name:  dg_client_api.c

    General Description: Implements an API for clients to communicate with DIAG server/engine

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dg_defs.h"
#include "dg_pal_client_api.h"
#include "dg_pal_client_platform_inc.h"
#include "dg_client_api.h"


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
=================================================================================================*/
static UINT8* dg_client_api_recv_diag_rsp(int socket, UINT32 timeout_in_ms);

static void dg_client_api_parse_diag_rsp(UINT8* raw_rsp_ptr, DG_CLIENT_API_RSP_T* diag_rsp);

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

@return TRUE for success
*//*==============================================================================================*/
BOOL DG_CLIENT_API_launch_server(void)
{
    BOOL status = FALSE;

    if (DG_PAL_CLIENT_API_start_diag_app() != TRUE)
    {
        DG_CLIENT_API_ERROR("Launching DIAG application failed!");
    }
    else
    {
        status = TRUE;
    }

    return status;
}

/*=============================================================================================*//**
@brief Begins a client session with the DIAG server/engine

@param [in] serv_addr - the diag server address, if NULL, internal socket would be used

@return socket for server connection, -1 if failed
*//*==============================================================================================*/
int DG_CLIENT_API_connect_to_server(const char* serv_addr)
{
    int socket_fd = -1;

    if (serv_addr == NULL)
    {
        socket_fd = DG_PAL_CLIENT_API_create_int_diag_socket();
    }
    else
    {
        socket_fd = DG_PAL_CLIENT_API_create_ext_diag_socket(serv_addr);
    }

    return socket_fd;
}

/*=============================================================================================*//**
@brief Ends a client sessions initiated by DG_CLIENT_API_connect_to_server()

@param [in] socket - Socket to close

*//*==============================================================================================*/
void DG_CLIENT_API_disconnect_from_server(int socket)
{
    DG_CLIENT_API_TRACE("Closed socket %d", socket);
    DG_PAL_CLIENT_API_close_diag_socket(socket);
}

/*=============================================================================================*//**
@brief Creates and sends a DIAG request to the server

@param [in] socket   - Server connection socket
@param [in] diag_req - diag request

@return TRUE for success
*//*==============================================================================================*/
BOOL DG_CLIENT_API_send_diag_req(int socket, DG_CLIENT_API_REQ_T* diag_req)
{
    BOOL                   status     = FALSE;
    UINT32                 write_len  = 0;
    UINT8*                 write_buff = NULL;
    DG_DEFS_DIAG_REQ_HDR_T cmd_header_h; /* DIAG Header in endianess of host */

    if (diag_req == NULL)
    {
        DG_CLIENT_API_ERROR("diag_req is NULL to send");
    }
    else if ((diag_req->data_len != 0) && (diag_req->data_ptr == NULL))
    {
        DG_CLIENT_API_ERROR("diag_req's data is invalid");
    }
    else
    {
        /* Init DIAG request header */
        memset(&cmd_header_h, 0, sizeof(cmd_header_h));
        cmd_header_h.seq_tag = diag_req->timestamp;
        cmd_header_h.opcode  = diag_req->opcode;
        cmd_header_h.length  = diag_req->data_len;

        /* Calculate write length (command data + header) */
        write_len = cmd_header_h.length + sizeof(cmd_header_h);
        if (!(write_buff = (UINT8*)malloc(write_len)))
        {
            DG_CLIENT_API_ERROR("Malloc failed for write_buff, attempted len = %d", write_len);
        }
        else
        {
            /* Do endian swap into buffer */
            DG_PAL_CLIENT_API_req_hdr_hton(&cmd_header_h, (DG_DEFS_DIAG_REQ_HDR_T*)write_buff);

            /* Copy command data into buffer if it exists */
            if (cmd_header_h.length > 0)
            {
                memcpy(write_buff + sizeof(cmd_header_h), diag_req->data_ptr, cmd_header_h.length);
            }

            if (DG_PAL_CLIENT_API_write(socket, write_buff, write_len))
            {
                status = TRUE;
                DG_CLIENT_API_TRACE("Successfully sent %d byte(s) DIAG opcode = 0x%04x to fd %d.",
                                    write_len, cmd_header_h.opcode, socket);
            }

            free(write_buff);
        }
    }

    return status;
}

/*=============================================================================================*//**
@brief Attempts to receive the desired DIAG response from the specified socket.

@param [in] socket        - Server connection socket
@param [in] diag_req      - the diag request for getting the response
@param [in] is_unsol      - If the expected response is unsolicited or not
@param [in] timeout_in_ms - Time to wait for a DIAG response (in msec), 0 means wait forever

@return Pointer to DIAG response

@note
  - All non-desired DIAG responses that are received will be deleted
  - The calling function is responsible for freeing the memory pointed to by the returned pointer
    by using DG_CLIENT_API_diag_rsp_free()
  - A valid diag response pointer is only returned in a success case
  - If the expected response is unsolicited, the timestamp parameter is ignored
*//*==============================================================================================*/
DG_CLIENT_API_RSP_T* DG_CLIENT_API_recv_diag_rsp(int socket, DG_CLIENT_API_REQ_T* diag_req,
                                                 BOOL is_unsol, UINT32 timeout_in_ms)
{
    DG_CLIENT_API_RSP_T* diag_rsp;

    UINT8* raw_diag_rsp = NULL;
    BOOL   got_rsp      = FALSE;

    if ((diag_rsp = (DG_CLIENT_API_RSP_T*)malloc(sizeof(DG_CLIENT_API_RSP_T))) == NULL)
    {
        DG_CLIENT_API_ERROR("Malloc diag_rsp failed to recv!");
        return NULL;
    }

    /* While we have not received the desired response and there are no errors, wait for response */
    while (got_rsp == FALSE)
    {
        raw_diag_rsp = dg_client_api_recv_diag_rsp(socket, timeout_in_ms);

        if (raw_diag_rsp == NULL)
        {
            DG_CLIENT_API_ERROR("Failed to recv diag response!");
            free(diag_rsp);
            return NULL;
        }
        else
        {
            dg_client_api_parse_diag_rsp(raw_diag_rsp, diag_rsp);

            /* Make sure the received response matches what was request.  If we are waiting for
               an unsolicited response, the timestamp will be set to 0 in the response. */

            if (is_unsol == FALSE)
            {
                if ((diag_rsp->timestamp == diag_req->timestamp) &&
                    (diag_rsp->opcode == diag_req->opcode) &&
                    (diag_rsp->is_unsol == FALSE))
                {
                    DG_CLIENT_API_TRACE("Got the response we are looking for");
                    got_rsp = TRUE;
                }
            }
            else
            {
                if ((diag_rsp->opcode == diag_req->opcode) &&
                    (diag_rsp->is_unsol == TRUE))
                {
                    DG_CLIENT_API_TRACE("Got the unsolicited response we are looking for");
                    got_rsp = TRUE;
                }
            }

            if (got_rsp == FALSE)
            {
                DG_CLIENT_API_TRACE("Got the wrong response!!");
                free(raw_diag_rsp);
                raw_diag_rsp = NULL;
            }
        }
    }

    if (got_rsp == FALSE)
    {
        free(diag_rsp);
        diag_rsp = NULL;
    }

    return diag_rsp;
}

/*=============================================================================================*//**
@brief Frees a given DIAG response

@param [in] diag_rsp - The DIAG response to free
*//*==============================================================================================*/
void DG_CLIENT_API_diag_rsp_free(DG_CLIENT_API_RSP_T* diag_rsp)
{
    if (diag_rsp != NULL)
    {
        free(diag_rsp->raw_rsp);
        free(diag_rsp);
    }
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Attempts to receive a DIAG response from the specified socket

@param [in]  socket        - Server connection socket
@param [in]  timeout_in_ms - Time to wait for a DIAG response (in msec), 0 means wait forever

@return Pointer to raw DIAG response

@note
  - The calling function is responsible for freeing the memory used for the raw DIAG response
  - A valid diag response pointer and response legnth is only returned in a success case
*//*==============================================================================================*/
UINT8* dg_client_api_recv_diag_rsp(int socket, UINT32 timeout_in_ms)
{
    UINT32                 total_len;
    UINT8*                 raw_diag_rsp = NULL;
    DG_DEFS_DIAG_RSP_HDR_T orig_hdr;
    DG_DEFS_DIAG_RSP_HDR_T converted_hdr;

    DG_CLIENT_API_TRACE("Receive response from server connection %d, timeout = %d",
                        socket, timeout_in_ms);

    if (DG_PAL_CLIENT_API_wait(socket, TRUE, timeout_in_ms))
    {
        /* Data is ready on the socket, attempt to read the DIAG response header */
        DG_CLIENT_API_TRACE("Attempt to read response header.");
        if (DG_PAL_CLIENT_API_read(socket, sizeof(orig_hdr), (UINT8*)&orig_hdr))
        {
            /* Convert the endianess of the header and determine the total length of
               the response */
            DG_PAL_CLIENT_API_rsp_hdr_ntoh(&orig_hdr, &converted_hdr);
            total_len = sizeof(orig_hdr) + converted_hdr.length;
            DG_CLIENT_API_TRACE("Total length of response: %d", total_len);
            if ((raw_diag_rsp = (UINT8*)malloc(total_len)) != NULL)
            {
                memcpy(raw_diag_rsp, &converted_hdr, sizeof(converted_hdr));

                /* Attempt to read the response data if present */
                if (converted_hdr.length > 0)
                {
                    if (!DG_PAL_CLIENT_API_read(socket, converted_hdr.length,
                                                raw_diag_rsp + sizeof(orig_hdr)))
                    {
                        /* Reading the data failed, cleanup */
                        free(raw_diag_rsp);
                        raw_diag_rsp = NULL;
                    }
                }
            }
            else
            {
                /* Memory allocation failed, return error */
                DG_CLIENT_API_ERROR("Allocating %d bytes failed.", total_len);
            }
        }
    }

    return raw_diag_rsp;
}

/*=============================================================================================*//**
@brief Parses a DIAG response

@param [in]  raw_rsp_ptr    - Pointer to raw DIAG response to parse
@param [out] diag_rsp       - diag response
                                                                                                  *
*//*==============================================================================================*/
void dg_client_api_parse_diag_rsp(UINT8* raw_rsp_ptr, DG_CLIENT_API_RSP_T* diag_rsp)
{
    DG_DEFS_DIAG_RSP_HDR_T* hdr = (DG_DEFS_DIAG_RSP_HDR_T*)raw_rsp_ptr;

    diag_rsp->raw_rsp   = raw_rsp_ptr;
    diag_rsp->opcode    = hdr->opcode;
    diag_rsp->rsp_code  = hdr->rsp_code;
    diag_rsp->timestamp = hdr->seq_tag;
    diag_rsp->data_len  = hdr->length;
    diag_rsp->data_ptr  = raw_rsp_ptr + sizeof(DG_DEFS_DIAG_RSP_HDR_T);

    diag_rsp->is_unsol = (hdr->unsol_rsp_flag == DG_DEFS_RSP_FLAG_UNSOL) ? TRUE : FALSE;
    diag_rsp->is_fail  = (hdr->rsp_code != DG_RSP_CODE_CMD_RSP_GENERIC) ? TRUE : FALSE;

}

