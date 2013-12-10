#ifndef _DG_CLIENT_API_H
#define _DG_CLIENT_API_H
/*==================================================================================================

    Module Name:  dg_client_api.h

    General Description: Interface into DIAG service for DIAG clients

====================================================================================================

Revision History:
                            Modification     Tracking
Author                          Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
Xudong Huang    - xudongh    2013/12/11     xxxxx-0000   Creation

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <dg_pal_client_platform_inc.h>

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
typedef int DG_CLIENT_API_SESSION_T;


/** Return values for DIAG Client API */
typedef enum
{
    DG_CLIENT_API_STATUS_MALLOC_FAIL = -4, /**< Malloc failed */
    DG_CLIENT_API_STATUS_ERR_PARM    = -3, /**< Invalid parameter used for function */
    DG_CLIENT_API_STATUS_TIMEOUT     = -2, /**< Timeout occurred */
    DG_CLIENT_API_STATUS_ERROR       = -1, /**< General error */
    DG_CLIENT_API_STATUS_SUCCESS     = 0   /**< Success */
} DG_CLIENT_API_STATUS_T;

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/
DG_CLIENT_API_STATUS_T DG_CLIENT_API_launch_server(void);
DG_CLIENT_API_STATUS_T DG_CLIENT_API_connect_to_server(int                      timeout_in_ms,
                                                       DG_CLIENT_API_SESSION_T* socket_ptr);
DG_CLIENT_API_STATUS_T DG_CLIENT_API_disconnect_from_server(DG_CLIENT_API_SESSION_T socket);
DG_CLIENT_API_STATUS_T DG_CLIENT_API_send_diag_req(DG_CLIENT_API_SESSION_T socket, UINT16 opcode,
                                                   UINT8 timestamp, UINT32 req_len,
                                                   UINT8* req_data_ptr);
DG_CLIENT_API_STATUS_T DG_CLIENT_API_send_diag_req_raw(DG_CLIENT_API_SESSION_T socket,
                                                       UINT32 raw_data_len, UINT8* raw_data_ptr);
UINT8* DG_CLIENT_API_rcv_desired_diag_rsp(DG_CLIENT_API_SESSION_T socket, UINT16 opcode,
                                          UINT8 timestamp, BOOL is_unsol, UINT32 timeout_in_ms,
                                          UINT32* rsp_len_ptr, DG_CLIENT_API_STATUS_T* status_ptr);
UINT8* DG_CLIENT_API_rcv_diag_rsp(DG_CLIENT_API_SESSION_T socket, UINT32 timeout_in_ms,
                                  UINT32* rsp_len_ptr, DG_CLIENT_API_STATUS_T* status_ptr);
DG_CLIENT_API_STATUS_T DG_CLIENT_API_parse_diag_rsp(UINT8* rsp_ptr, UINT32 rsp_len,
                                                    BOOL* diag_fail_ptr, BOOL* unsol_rsp_ptr,
                                                    UINT8* timestamp_ptr, UINT16* opcode_ptr,
                                                    UINT8* rsp_code_ptr,
                                                    UINT32* data_offset_val_ptr,
                                                    UINT32* data_len_ptr);
BOOL DG_CLIENT_API_check_rsp_for_fail(UINT8* diag_rsp, UINT32 rsp_len);
void DG_CLIENT_API_diag_rsp_free(UINT8* diag_rsp);
DG_CLIENT_API_STATUS_T DG_CLIENT_API_echo_data(UINT8* data, UINT32 size,
                                               UINT32 connect_timeout, UINT32 rsp_timeout);

/*==================================================================================================
                                          GLOBAL VARIABLES
==================================================================================================*/

/** @} */
#ifdef __cplusplus
}
#endif
#endif

