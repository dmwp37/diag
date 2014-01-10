#ifndef _DG_ENGINE_UTIL_H
#define _DG_ENGINE_UTIL_H
/*==================================================================================================

    Module Name:  dg_engine_util.h

    General Description: This file provides an interface to the DIAG engine

====================================================================================================

Revision History:
                            Modification     Tracking
Author                          Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
Xudong Huang    - xudongh    2013/12/11     xxxxx-0000   Creation
Xudong Huang    - xudongh    2013/12/20     xxxxx-0002   Update diag req protocol

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_defs.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup engine_interface
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

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/
BOOL                   DG_ENGINE_UTIL_check_engine_mode(DG_DEFS_MODE_T desired_mode, DG_DEFS_DIAG_RSP_BUILDER_T* rsp);
DG_DEFS_MODE_T         DG_ENGINE_UTIL_get_engine_mode(void);
void                   DG_ENGINE_UTIL_set_engine_mode(DG_DEFS_MODE_T mode);
DG_DEFS_SEC_IC_STATE_T DG_ENGINE_UTIL_get_sec_ic_state(void);

void DG_ENGINE_UTIL_hdr_rsp_ntoh(DG_DEFS_DIAG_RSP_HDR_T* hdr_in, DG_DEFS_DIAG_RSP_HDR_T* hdr_out);
void DG_ENGINE_UTIL_hdr_rsp_hton(DG_DEFS_DIAG_RSP_HDR_T* hdr_in, DG_DEFS_DIAG_RSP_HDR_T* hdr_out);
void DG_ENGINE_UTIL_hdr_req_ntoh(DG_DEFS_DIAG_REQ_HDR_T* hdr_in, DG_DEFS_DIAG_REQ_HDR_T* hdr_out);
void DG_ENGINE_UTIL_hdr_req_hton(DG_DEFS_DIAG_REQ_HDR_T* hdr_in, DG_DEFS_DIAG_REQ_HDR_T* hdr_out);

void DG_ENGINE_UTIL_free_diag_req(DG_DEFS_DIAG_REQ_T* diag);
void DG_ENGINE_UTIL_free_diag_rsp(DG_DEFS_DIAG_RSP_T* rsp);
void DG_ENGINE_UTIL_generic_cleanup_unlock(void* mutex);


/** @addtogroup request_handling
The following helper functions are meant to be used to parse & handle DIAG requests.  Only these
functions can be used to interact with a DIAG request.  A DIAG request can not be dereferenced
directly by a handler

@{
*/
BOOL DG_ENGINE_UTIL_req_len_check_at_least(DG_DEFS_DIAG_REQ_T* req, UINT32 req_min_len,
                                           DG_DEFS_DIAG_RSP_BUILDER_T* rsp);
BOOL DG_ENGINE_UTIL_req_len_check_equal(DG_DEFS_DIAG_REQ_T* req, UINT32 req_min_len,
                                        DG_DEFS_DIAG_RSP_BUILDER_T* rsp);
BOOL DG_ENGINE_UTIL_req_remain_len_check_at_least(DG_DEFS_DIAG_REQ_T* req, UINT32 req_min_len,
                                                  DG_DEFS_DIAG_RSP_BUILDER_T* rsp);
BOOL DG_ENGINE_UTIL_req_remain_len_check_equal(DG_DEFS_DIAG_REQ_T* req, UINT32 req_min_len,
                                               DG_DEFS_DIAG_RSP_BUILDER_T* rsp);
UINT32 DG_ENGINE_UTIL_req_get_len(DG_DEFS_DIAG_REQ_T* req);
UINT32 DG_ENGINE_UTIL_req_get_remain_len(DG_DEFS_DIAG_REQ_T* req);
UINT8* DG_ENGINE_UTIL_req_get_remain_data_ptr(DG_DEFS_DIAG_REQ_T* req);

UINT8  DG_ENGINE_UTIL_req_parse_1_byte_ntoh(DG_DEFS_DIAG_REQ_T* req);
UINT16 DG_ENGINE_UTIL_req_parse_2_bytes_ntoh(DG_DEFS_DIAG_REQ_T* req);
UINT32 DG_ENGINE_UTIL_req_parse_4_bytes_ntoh(DG_DEFS_DIAG_REQ_T* req);
void   DG_ENGINE_UTIL_req_parse_buf(DG_DEFS_DIAG_REQ_T* req, UINT8* dest, UINT32 num_bytes);
/** @} */

/** @addtogroup response_builder
The following helper functions are meant to be used to create DIAG responses.  Only these functions
can be used to create and send DIAG responses.

@{
*/
DG_DEFS_DIAG_RSP_BUILDER_T* DG_ENGINE_UTIL_rsp_init(void);

void DG_ENGINE_UTIL_rsp_free(DG_DEFS_DIAG_RSP_BUILDER_T* rsp);
BOOL DG_ENGINE_UTIL_rsp_data_alloc(DG_DEFS_DIAG_RSP_BUILDER_T* rsp, UINT32 max_size);
void DG_ENGINE_UTIL_rsp_append_1_byte_hton(DG_DEFS_DIAG_RSP_BUILDER_T* rsp, UINT8 val);
void DG_ENGINE_UTIL_rsp_append_2_bytes_hton(DG_DEFS_DIAG_RSP_BUILDER_T* rsp, UINT16 val);
void DG_ENGINE_UTIL_rsp_append_4_bytes_hton(DG_DEFS_DIAG_RSP_BUILDER_T* rsp, UINT32 val);
void DG_ENGINE_UTIL_rsp_append_buf(DG_DEFS_DIAG_RSP_BUILDER_T* rsp, UINT8* src, UINT32 num_bytes);
void DG_ENGINE_UTIL_rsp_replace_1_byte_hton(DG_DEFS_DIAG_RSP_BUILDER_T* rsp, UINT32 offset,
                                            UINT8 val);
void DG_ENGINE_UTIL_rsp_replace_2_bytes_hton(DG_DEFS_DIAG_RSP_BUILDER_T* rsp, UINT32 offset,
                                             UINT16 val);
void DG_ENGINE_UTIL_rsp_replace_4_bytes_hton(DG_DEFS_DIAG_RSP_BUILDER_T* rsp, UINT32 offset,
                                             UINT32 val);
void DG_ENGINE_UTIL_rsp_set_len_zero(DG_DEFS_DIAG_RSP_BUILDER_T* rsp);
void DG_ENGINE_UTIL_rsp_send(DG_DEFS_DIAG_RSP_BUILDER_T* rsp, DG_DEFS_DIAG_REQ_T* diag);
void DG_ENGINE_UTIL_rsp_set_code(DG_DEFS_DIAG_RSP_BUILDER_T* rsp, DG_RSP_CODE_T code);
BOOL DG_ENGINE_UTIL_rsp_is_failure(DG_DEFS_DIAG_RSP_BUILDER_T* rsp);
void DG_ENGINE_UTIL_rsp_set_flag(DG_DEFS_DIAG_RSP_BUILDER_T* rsp, DG_DEFS_RSP_FLAG_T flag);
void DG_ENGINE_UTIL_rsp_set_error_string(DG_DEFS_DIAG_RSP_BUILDER_T* rsp,
                                         DG_RSP_CODE_T code, const char* format, ...);
void DG_ENGINE_UTIL_rsp_set_error_string_drv(DG_DEFS_DIAG_RSP_BUILDER_T* rsp,
                                             DG_RSP_CODE_T code,
                                             const char* format, ...);
DG_RSP_CODE_T DG_ENGINE_UTIL_rsp_get_code(DG_DEFS_DIAG_RSP_BUILDER_T* rsp);
UINT32        DG_ENGINE_UTIL_rsp_get_curr_len(DG_DEFS_DIAG_RSP_BUILDER_T* rsp);
const UINT8*  DG_ENGINE_UTIL_rsp_get_data_ptr(DG_DEFS_DIAG_RSP_BUILDER_T* rsp);

DG_DEFS_RSP_FLAG_T DG_ENGINE_UTIL_rsp_get_flag(DG_DEFS_DIAG_RSP_BUILDER_T* rsp);
/** @} */

void DG_ENGINE_UTIL_buf_append_1_byte_hton(UINT8* buf, UINT32* cur_buf_len, UINT8 val);
void DG_ENGINE_UTIL_buf_append_2_bytes_hton(UINT8* buf, UINT32* cur_buf_len, UINT16 val);
void DG_ENGINE_UTIL_buf_append_4_bytes_hton(UINT8* buf, UINT32* cur_buf_len, UINT32 val);
void DG_ENGINE_UTIL_buf_append_buf(UINT8* dest_buff, UINT32* cur_buf_len, void* src,
                                   UINT32 num_bytes);
void    DG_ENGINE_UTIL_buf_replace_1_byte_hton(UINT8* buf, UINT32 offset, UINT8 val);
void    DG_ENGINE_UTIL_buf_replace_2_bytes_hton(UINT8* buf, UINT32 offset, UINT16 val);
void    DG_ENGINE_UTIL_buf_replace_4_bytes_hton(UINT8* buf, UINT32 offset, UINT32 val);
UINT8   DG_ENGINE_UTIL_buf_parse_1_byte_ntoh(UINT8** buf_ptr);
UINT16  DG_ENGINE_UTIL_buf_parse_2_bytes_ntoh(UINT8** buf_ptr);
UINT32  DG_ENGINE_UTIL_buf_parse_4_bytes_ntoh(UINT8** buf_ptr);
void    DG_ENGINE_UTIL_buf_uint16_hton(UINT16* buf, UINT32 len);
void    DG_ENGINE_UTIL_buf_parse_buf(UINT8* dest, UINT8** buf_ptr, UINT32 num_bytes);
UINT8*  DG_ENGINE_UTIL_alloc_mem(UINT32 max_size, DG_DEFS_DIAG_RSP_BUILDER_T* rsp);
W_CHAR* DG_ENGINE_UTIL_process_unicode_filename(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp);
char*   DG_ENGINE_UTIL_process_ascii_filename(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp);
char*   DG_ENGINE_UTIL_prepend_default_path_ascii(char* src, DG_DEFS_DIAG_RSP_BUILDER_T* rsp);
W_CHAR* DG_ENGINE_UTIL_process_unicode_filename_no_null(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp);
W_CHAR* DG_ENGINE_UTIL_prepend_default_path_unicode(W_CHAR* src, DG_DEFS_DIAG_RSP_BUILDER_T* rsp);

UINT32 DG_ENGINE_UTIL_convert_sign_magnitude_4bytes(INT32 x);
UINT16 DG_ENGINE_UTIL_convert_sign_magnitude_2bytes(INT16 x);
/*==================================================================================================
                                          GLOBAL VARIABLES
==================================================================================================*/

/** @}*/
#ifdef __cplusplus
}
#endif
#endif

