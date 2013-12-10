/*==================================================================================================

    Module Name:  dg_client_api_wrapper.c

    General Description: Implements old LJ Client API functions using the common client API

====================================================================================================

                                 Motorola Confidential Proprietary
                          (c) Copyright Motorola 2008, All Rights Reserved
  
Revision History:
                            Modification     Tracking
Author                          Date          Number     Description of Changes
-------------------------   ------------    ----------   ------------------------------------------
Zachary Crosby   - azc095    2008/10/20     LIBrr34392   Creation, ported from R6743_G_74.10.29R

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <string.h>
#include "DG_Api.h"
#include "dg_client_api.h"

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                            LOCAL MACROS
==================================================================================================*/
#define DG_CLIENT_API_WRAPPER_ENGINE_TIMEOUT     5    /**< Arbitrary timeout length for connecting 
                                                           to the DIAG engine (in seconds)  */
#define DG_CLIENT_API_WRAPPER_OPCODE_RDELEM      0x20 /**< Opcode for RDELEM DIAG */

/** Macro to convert a UINT16 host to network */
#define DG_CLIENT_API_WRAPPER_UINT16_HTON(data) ( (((data) >> 8) & (0x00FF)) | \
                                                  (((data) << 8) & (0xFF00)) )

/*==================================================================================================
                             LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** Structure for a RDELEM diag request */
typedef struct
{
   UINT16  elem_id;       /**< The SEEM element identification number */
   UINT16  record_num;    /**< The SEEM record number to be read      */
   UINT16  record_offset; /**< The offset into the record to be read  */
   UINT16  read_length;   /**< Length of data to be read              */
} DG_CLIENT_API_WRAPPER_REQ_RDELEM_T;

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
@brief Begins a client session with the DIAG engine

@return File descriptor of the server connection on success, DG_API_ERROR on failure

@note
- Must close connection using #DG_API_stop_client
*//*==============================================================================================*/
int DG_API_start_client(void)
{
    int                    socket_fd = -1;
    DG_CLIENT_API_STATUS_T status;

    /* Connect to server, set return variable to correct value */
    if (DG_CLIENT_API_connect_to_server(DG_CLIENT_API_WRAPPER_ENGINE_TIMEOUT, &socket_fd) != 
        DG_CLIENT_API_STATUS_SUCCESS)
    {
        socket_fd = DG_API_ERROR;
    }
    
    return(socket_fd);
}

/*=============================================================================================*//**
@brief Ends a client session with the DIAG engine that was started with #DG_API_start_client

@param[in] socket - Socket connection to close
*//*==============================================================================================*/
void DG_API_stop_client(int socket)
{
    DG_CLIENT_API_disconnect_from_server(socket);
}

/*=============================================================================================*//**
@brief Sends a DIAG request to the DIAG engine over the specified socket

@param[in] socket    - Socket connection to send the request to
@param[in] opcode    - Opcode of the DIAG request
@param[in] timestamp - Timestamp to use for the DIAG request
@param[in] length    - Length of the data payload
@param[in] data      - Pointer to the data payload

@return Status of the function

@note
- Data payload must be in big endian
*//*==============================================================================================*/
DG_API_STATUS_T DG_API_send_cmd(int socket, UINT16 opcode, UINT8 timestamp, UINT32 length, 
                                void *data)
{
    /* Init status values to failure */
    DG_API_STATUS_T        wrapper_status = DG_API_ERROR; 
    DG_CLIENT_API_STATUS_T status         = DG_CLIENT_API_STATUS_ERROR;
    
    /* Send the DIAG request, set the wrapper return value correctly on success */
    status = DG_CLIENT_API_send_diag_req(socket, opcode, timestamp,
                                         length, (UINT8 *) data);

    if (status == DG_CLIENT_API_STATUS_SUCCESS)
    {
        wrapper_status = DG_API_SUCCESS;
    }

    return(wrapper_status);
}

/*=============================================================================================*//**
@brief Attempts to receive a DIAG response from the specified socket

@param[in]  socket    - Socket connection read DIAG response from
@param[out] length    - Length of the complete DIAG response
@param[out] data      - Pointer to pointer of where the complete DIAG response is stored
@param[in]  timeout   - Timeout value to use while waiting for DIAG response (in msec)

@return Status of the function

@note
- This function will allocate memory for the DIAG response.  The calling function is responsible
  for freeing the memory.
*//*==============================================================================================*/
DG_API_STATUS_T DG_API_recv_resp(int socket, UINT32 *length, void **data, int timeout)
{
    /* Init status values to failure */
    DG_CLIENT_API_STATUS_T status         = DG_CLIENT_API_STATUS_ERROR;
    DG_API_STATUS_T        wrapper_status = DG_API_ERROR;
 
    /* Attempt to receive response */
    *data = DG_CLIENT_API_rcv_diag_rsp(socket, timeout, length, &status);
    if (status == DG_CLIENT_API_STATUS_SUCCESS)
    {
        /* Success */
        wrapper_status = DG_API_SUCCESS;
    }
    else if (status == DG_CLIENT_API_STATUS_TIMEOUT)
    {
        /* Timeout */
        wrapper_status = DG_API_TIMEOUT;
    }
    else
    {
        wrapper_status = DG_API_ERROR;
    }
    
    return(wrapper_status);
}

/*=============================================================================================*//**
@brief Parses a DIAG response

@param [in]  resp        - Pointer to DIAG response to parse
@param [in]  resp_len    - Length of DIAG response
@param [out] diag_fail   - Whether the response indicates a failure or not (optional)
@param [out] unsol_resp  - Whether the response is unsolicited or not (optional)
@param [out] timestamp   - Timestamp of response (optional)
@param [out] opcode      - Opcode of response (optional)
@param [out] resp_code   - Response code of response (optional)
@param [out] data_offset - Offset to response data part of response (optional)
@param [out] data_len    - Length of response data (optional)

@return If the response was properly parsed or not

@note 
 - For the optional parameters, the parameter will only be populated if the pointer is non-NULL and
   the function returns successfully 
*//*==============================================================================================*/
DG_API_STATUS_T DG_API_parse_resp( void *resp, UINT32 resp_len, BOOL *diag_fail, BOOL *unsol_resp,
                                     UINT8 *timestamp, UINT16 *opcode, UINT8 *resp_code,
                                     UINT8 *data_offset, UINT32 *data_len )
{
    /* Init status values to fail */
    DG_API_STATUS_T        wrapper_status = DG_API_ERROR;
    DG_CLIENT_API_STATUS_T status         = DG_CLIENT_API_STATUS_ERROR;
    UINT32 real_offset; /* Old API used UINT8 for offset, new used UINT32 */

    status = DG_CLIENT_API_parse_diag_rsp(resp, resp_len, diag_fail, unsol_resp, timestamp, opcode,
                                          resp_code, &real_offset, data_len);
    
    if (status == DG_CLIENT_API_STATUS_SUCCESS)
    {
        wrapper_status = DG_API_SUCCESS;
        
        /* Do typecasting of offset if needed, data should not be lost since this value is the size
           of the DIAG header */
        if (data_offset) *data_offset = (UINT8) real_offset;
    }

    return (wrapper_status);    
}

/*=============================================================================================*//**
@brief Reports if a given DIAG response indicates a failure

@param [in] resp      - The DIAG response to check
@param [in] resp_len  - Length of the DIAG response

@return TRUE if the response is a failure
*//*==============================================================================================*/
BOOL DG_API_check_resp_for_fail(void *resp, UINT32 resp_len)
{    
    return(DG_CLIENT_API_check_rsp_for_fail( (UINT8*) resp, resp_len));
}

/*=============================================================================================*//**
@brief Reads the requested SEEM element

@param[in]  elem_id     - SEEM element ID to read
@param[in]  record_num  - Record number of element to read
@param[in]  offset      - Offset into element/record to read
@param[in]  read_length - Number of bytes to read & must be non-zero.
@param[out] data_buffer - Buffer to store read SEEM data, must be at least size of 'read_length'

@return Status of the function

@note
- Calling function is responsible for allocating 'data_buffer' to be at least size of 'read_length'
*//*==============================================================================================*/
DG_API_STATUS_T DG_API_read_seem_data(UINT16 elem_id, UINT16 record_num, 
                                      UINT16 offset, UINT16 read_length, UINT8 *data_buffer)
{
   DG_CLIENT_API_WRAPPER_REQ_RDELEM_T rdelem_req;
   DG_API_STATUS_T          wrapper_status = DG_API_ERROR;
   DG_CLIENT_API_STATUS_T   status         = DG_CLIENT_API_STATUS_ERROR;
   DG_CLIENT_API_SESSION_T  diag_session;
   UINT8                   *diag_rsp       = NULL;
   UINT32                   diag_rsp_len   = 0;
   UINT32                   data_length    = 0;
   UINT32                   data_offset    = 0;
   BOOL                     is_failed      = TRUE;
   
   /* Attempt to connect to the DIAG server, 30 sec timeout */
   if (DG_CLIENT_API_connect_to_server(30000, &diag_session) == DG_CLIENT_API_STATUS_SUCCESS)
   {
       /* Create RDELEM DIAG request and send to DIAG server */ 
       rdelem_req.elem_id       = DG_CLIENT_API_WRAPPER_UINT16_HTON(elem_id);
       rdelem_req.record_num    = DG_CLIENT_API_WRAPPER_UINT16_HTON(record_num);
       rdelem_req.record_offset = DG_CLIENT_API_WRAPPER_UINT16_HTON(offset);
       rdelem_req.read_length   = DG_CLIENT_API_WRAPPER_UINT16_HTON(read_length);

       /* Send RDELEM request, timestamp of 0 */
       if (DG_CLIENT_API_send_diag_req(diag_session, DG_CLIENT_API_WRAPPER_OPCODE_RDELEM, 0, 
                                       sizeof(rdelem_req),
                                       (UINT8*) &rdelem_req) == DG_CLIENT_API_STATUS_SUCCESS)
       {
           /* Get the RDELEM resposne, timestamp of 0 */
           diag_rsp = DG_CLIENT_API_rcv_desired_diag_rsp(diag_session, 
                                                         DG_CLIENT_API_WRAPPER_OPCODE_RDELEM, 0,
                                                         FALSE, DG_API_RSP_TIMEOUT, &diag_rsp_len,
                                                         &status);

           /* Return timeout if occurred */
           if (status == DG_CLIENT_API_STATUS_TIMEOUT)
           {
               wrapper_status = DG_API_TIMEOUT;
           }
           else if (status == DG_CLIENT_API_STATUS_SUCCESS)
           {
               /* If successfully got response, parse it.  If response is success, and the data 
                  length (minus 1 for the RDELEM status byte) is equal to the requested read 
                  length, copy the response data and set status to success */
               if ((DG_CLIENT_API_parse_diag_rsp(diag_rsp, diag_rsp_len, &is_failed, NULL, NULL, NULL,
                                                 NULL, &data_offset, &data_length) ==
                    DG_CLIENT_API_STATUS_SUCCESS) && 
                   (is_failed == FALSE) &&
                   ( (data_length - 1) == read_length) )
               {
                   memcpy(data_buffer, (diag_rsp + data_offset + 1), read_length);
                   wrapper_status = DG_API_SUCCESS;
               }
               
               DG_CLIENT_API_diag_rsp_free(diag_rsp);
           }
       }
       DG_CLIENT_API_disconnect_from_server(diag_session);
   }
   
   /* Return status code */
   return(wrapper_status);
}
