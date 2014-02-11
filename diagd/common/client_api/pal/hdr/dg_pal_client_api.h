#ifndef _DG_PAL_CLIENT_API_H
#define _DG_PAL_CLIENT_API_H
/*==================================================================================================

    Module Name:  dg_pal_client_api.h

    General Description: DIAG PAL - Client API

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Client_PAL
@{

@par
Functions for supporting the DIAG Client API

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
@brief Launches the DIAG application

@return TRUE = successfully started the DIAG application, FALSE = failed to start application
*//*==============================================================================================*/
BOOL DG_PAL_CLIENT_API_start_diag_app(void);

/*=============================================================================================*//**
@brief Creates socket for use by internal DIAG clients

@return File descriptor for the socket, -1 on fail
*//*==============================================================================================*/
int DG_PAL_CLIENT_API_create_int_diag_socket();

/*=============================================================================================*//**
@brief Creates socket for use by external DIAG clients

@param [in] serv_addr - the diag server address

@return File descriptor for the socket, -1 on fail
*//*==============================================================================================*/
int DG_PAL_CLIENT_API_create_ext_diag_socket(const char* serv_addr);

/*=============================================================================================*//**
@brief Close the client socket

@param [in] socket       - Socket to close

*//*==============================================================================================*/
void DG_PAL_CLIENT_API_close_diag_socket(int socket);

/*=============================================================================================*//**
@brief Wait the socket until there is data available

@param [in] socket        - Socket to wait
@param [in] is_read       - TURE for waiting read, FALSE for waiting write
@param [in] timeout_in_ms - Time to wait for a DIAG response (in msec), 0 means wait forever

@return TRUE if success
*//*==============================================================================================*/
BOOL DG_PAL_CLIENT_API_wait(int socket, BOOL is_read, UINT32 timeout_in_ms);

/*=============================================================================================*//**
@brief Reads specified number of bytes data from a socket

@param [in] socket      - Socket to read from
@param [in] len_to_read - Number of bytes to read
@param [out] buff_ptr   - Buffer to read data into

@return TRUE if success
*//*==============================================================================================*/
BOOL DG_PAL_CLIENT_API_read(int socket, UINT32 len_to_read, UINT8* buff_ptr);

/*=============================================================================================*//**
@brief Write specified number of bytes data to a socket

@param [in] socket       - Socket to write to
@param [in] len_to_write - Number of bytes to write
@param [in] buff_ptr     - Buffer to write data from

@return TRUE if success
*//*==============================================================================================*/
BOOL DG_PAL_CLIENT_API_write(int socket, UINT8* buf, UINT32 len);

/*=============================================================================================*//**
@brief Converts a request header in host-endian to network-endian

@param [in]  hdr_in  - Host-endian DIAG request header
@param [out] hdr_out - Network-endian DIAG request header
*//*==============================================================================================*/
void DG_PAL_CLIENT_API_req_hdr_hton(DG_DEFS_DIAG_REQ_HDR_T* hdr_in, DG_DEFS_DIAG_REQ_HDR_T* hdr_out);

/*=============================================================================================*//**
@brief Converts a response header in network-endian to host-endian

@param [in]  hdr_in  - Network-endian DIAG response header
@param [out] hdr_out - Host-endian DIAG response header
*//*==============================================================================================*/
void DG_PAL_CLIENT_API_rsp_hdr_ntoh(DG_DEFS_DIAG_RSP_HDR_T* hdr_in, DG_DEFS_DIAG_RSP_HDR_T* hdr_out);

/** @} */

#ifdef __cplusplus
}
#endif

#endif

