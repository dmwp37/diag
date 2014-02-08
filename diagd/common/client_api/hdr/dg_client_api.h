#ifndef _DG_CLIENT_API_H
#define _DG_CLIENT_API_H
/*==================================================================================================

    Module Name:  dg_client_api.h

    General Description: Interface into DIAG service for DIAG clients

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup client_api
@{
*/

/*==================================================================================================
                                             CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                               MACROS
==================================================================================================*/

/*==================================================================================================
                                TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
typedef struct
{
    UINT16 opcode;
    UINT16 timestamp;
    UINT32 data_len;
    UINT8* data_ptr;
} DG_CLIENT_API_REQ_T;

typedef struct
{
    UINT16 opcode;
    UINT16 timestamp;
    UINT8  rsp_code;
    BOOL   is_fail;
    BOOL   is_unsol;
    UINT32 data_len;
    UINT8* data_ptr;
    UINT8* raw_rsp;
} DG_CLIENT_API_RSP_T;

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*=============================================================================================*//**
@brief Starts the DIAG server

@return TRUE for success
*//*==============================================================================================*/
BOOL DG_CLIENT_API_launch_server(void);

/*=============================================================================================*//**
@brief Begins a client session with the DIAG server/engine

@param [in] serv_addr - the diag server address, if NULL, internal socket would be used

@return socket for server connection, -1 if failed
*//*==============================================================================================*/
int DG_CLIENT_API_connect_to_server(const char* serv_addr);

/*=============================================================================================*//**
@brief Ends a client sessions initiated by DG_CLIENT_API_connect_to_server()

@param [in] socket - Socket to close

*//*==============================================================================================*/
void DG_CLIENT_API_disconnect_from_server(int socket);

/*=============================================================================================*//**
@brief Creates and sends a DIAG request to the server

@param [in] socket   - Server connection socket
@param [in] diag_req - diag request

@return TRUE for success
*//*==============================================================================================*/
BOOL DG_CLIENT_API_send_diag_req(int socket, DG_CLIENT_API_REQ_T* diag_req);

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
                                                 BOOL is_unsol, UINT32 timeout_in_ms);

/*=============================================================================================*//**
@brief Frees a given DIAG response

@param [in] diag_rsp - The DIAG response to free
*//*==============================================================================================*/
void DG_CLIENT_API_diag_rsp_free(DG_CLIENT_API_RSP_T* diag_rsp);

/*==================================================================================================
                                          GLOBAL VARIABLES
==================================================================================================*/

/** @} */
#ifdef __cplusplus
}
#endif
#endif

