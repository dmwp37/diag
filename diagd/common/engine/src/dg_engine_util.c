/*==================================================================================================

    Module Name:  dg_engine_util.c

    General Description:  Provides engine utility functions

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
#include <unistd.h>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <pthread.h>
#include "dg_defs.h"
#include "dg_dbg.h"
#include "dg_drv_util.h"
#include "dg_engine_util.h"
#include "dg_client_comm.h"
#include "dg_pal_util.h"
#include "dg_pal_security.h"

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                            LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                             LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
typedef struct
{
    DG_RSP_CODE_T      code;
    UINT8*             data_ptr;
    UINT32             data_cur_len;
    UINT32             data_max_len;
    DG_DEFS_RSP_FLAG_T flag;
    char*              err_text_string;
    UINT32             init_marker;
} dg_engine_util_diag_rsp_builder_t;

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
void dg_engine_util_send_response(DG_DEFS_DIAG_REQ_T* diag,  DG_RSP_CODE_T rsp_code,
                                  DG_DEFS_RSP_FLAG_T rsp_flags, UINT32 rsp_length, void* rsp_pdata);
/*==================================================================================================
                                          GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/
/** DIAG engine mode, default to normal */
static DG_DEFS_MODE_T dg_engine_util_engine_mode = DG_DEFS_MODE_NORMAL;

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Gets the current mode of the DIAG engine

@return The DIAG mode
*//*==============================================================================================*/
DG_DEFS_MODE_T DG_ENGINE_UTIL_get_engine_mode(void)
{
    return dg_engine_util_engine_mode;
}
/*=============================================================================================*//**
@brief Verifies the DIAG engine is in the desired mode

@param[in] desired_mode - Mode we want to be in
@param[out] rsp - Optional, set to error if we are not in the desired mode

@return TRUE = In the desired mode, FALSE = we are note
*//*==============================================================================================*/

BOOL DG_ENGINE_UTIL_check_engine_mode(DG_DEFS_MODE_T desired_mode, DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    BOOL           is_correct_mode = FALSE;
    DG_DEFS_MODE_T current_mode;

    /* Get the current mode, check it against the desired mode */
    current_mode = DG_ENGINE_UTIL_get_engine_mode();
    if ((current_mode & desired_mode) == desired_mode)
    {
        is_correct_mode = TRUE;
    }
    else
    {
        DG_DBG_ERROR("Target in wrong mode, desired = 0x%x, current = 0x%x",
                     desired_mode, current_mode);
        /* If a rsp pointer is give, set it to the correct error message */
        if (rsp != NULL)
        {
            if (desired_mode == DG_DEFS_MODE_TEST)
            {
                DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_ERR_MODE,
                                                    "Target must be in diag mode");
            }
            else if (desired_mode == DG_DEFS_MODE_NORMAL)
            {
                DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_ERR_MODE,
                                                    "Target must be in normal mode");
            }
            else
            {
                DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_PAR_ERR_MODE);
            }
        }
    }

    return is_correct_mode;
}

/*=============================================================================================*//**
@brief Get the ic type in this target

@return The ic type
*//*==============================================================================================*/
DG_DEFS_SEC_IC_STATE_T DG_ENGINE_UTIL_get_sec_ic_state(void)
{
    return DG_PAL_SECURITY_get_sec_ic_state();
}

/*=============================================================================================*//**
@brief Sets the current mode of the DIAG engine

@param[in] mode - The DIAG mode to set
*//*==============================================================================================*/
void DG_ENGINE_UTIL_set_engine_mode(DG_DEFS_MODE_T mode)
{
    dg_engine_util_engine_mode = mode;
}

/*=============================================================================================*//**
@brief Convert network byte order to host byte order for DIAG response headers

@param[in]  hdr_in  - Network byte order DIAG response header
@param[out] hdr_out - Host byte order DIAG response header
*//*==============================================================================================*/
void DG_ENGINE_UTIL_hdr_rsp_ntoh(DG_DEFS_DIAG_RSP_HDR_T* hdr_in,
                                 DG_DEFS_DIAG_RSP_HDR_T* hdr_out)
{
    memcpy(hdr_out, hdr_in, sizeof(DG_DEFS_DIAG_RSP_HDR_T));
    hdr_out->opcode  = ntohs(hdr_in->opcode);
    hdr_out->seq_tag = ntohs(hdr_in->seq_tag);
    hdr_out->length  = ntohl(hdr_in->length);
}

/*=============================================================================================*//**
@brief Convert host byte order to network byte order for DIAG response headers

@param[in]  hdr_in  - Host byte order DIAG response header
@param[out] hdr_out - Network byte order DIAG response header
*//*==============================================================================================*/
void DG_ENGINE_UTIL_hdr_rsp_hton(DG_DEFS_DIAG_RSP_HDR_T* hdr_in,
                                 DG_DEFS_DIAG_RSP_HDR_T* hdr_out)
{
    memcpy(hdr_out, hdr_in, sizeof(DG_DEFS_DIAG_RSP_HDR_T));
    hdr_out->opcode  = htons(hdr_in->opcode);
    hdr_out->seq_tag = htons(hdr_in->seq_tag);
    hdr_out->length  = htonl(hdr_in->length);
}

/*=============================================================================================*//**
@brief Convert network byte order to host byte order for DIAG request headers

@param[in]  hdr_in  - Network byte order DIAG request header
@param[out] hdr_out - Host byte order DIAG request header
*//*==============================================================================================*/
void DG_ENGINE_UTIL_hdr_req_ntoh(DG_DEFS_DIAG_REQ_HDR_T* hdr_in,
                                 DG_DEFS_DIAG_REQ_HDR_T* hdr_out)
{
    memcpy(hdr_out, hdr_in, sizeof(DG_DEFS_DIAG_REQ_HDR_T));
    hdr_out->opcode  = ntohs(hdr_in->opcode);
    hdr_out->seq_tag = ntohs(hdr_in->seq_tag);
    hdr_out->length  = ntohl(hdr_in->length);
}

/*=============================================================================================*//**
@brief Convert host byte order to network byte order for DIAG request headers

@param[in]  hdr_in  - Host byte order DIAG request header
@param[out] hdr_out - Network byte order DIAG request header
*//*==============================================================================================*/
void DG_ENGINE_UTIL_hdr_req_hton(DG_DEFS_DIAG_REQ_HDR_T* hdr_in,
                                 DG_DEFS_DIAG_REQ_HDR_T* hdr_out)
{
    memcpy(hdr_out, hdr_in, sizeof(DG_DEFS_DIAG_REQ_HDR_T));
    hdr_out->opcode  = htons(hdr_in->opcode);
    hdr_out->seq_tag = htons(hdr_in->seq_tag);
    hdr_out->length  = htonl(hdr_in->length);
}

/*=============================================================================================*//**
@brief Frees memory used by a DIAG request

@param[in]  diag - DIAG request to free
*//*==============================================================================================*/
void DG_ENGINE_UTIL_free_diag_req(DG_DEFS_DIAG_REQ_T* diag)
{
    if (diag != NULL)
    {
        pthread_mutex_destroy(&(diag->handled_mutex));
        pthread_cond_destroy(&(diag->handled_cond));
        pthread_mutex_destroy(&(diag->delete_mutex));
        pthread_cond_destroy(&(diag->delete_cond));

        free(diag->data_ptr);
        free(diag);
    }
}

/*=============================================================================================*//**
@brief Frees memory used by a DIAG response.  For reals.

@param[in] rsp - DIAG response to free
*//*==============================================================================================*/
void DG_ENGINE_UTIL_free_diag_rsp(DG_DEFS_DIAG_RSP_T* rsp)
{
    if (rsp != NULL)
    {
        free(rsp->data_ptr);
        free(rsp);
    }
}

/*=============================================================================================*//**
@brief Generic function for unlocking a mutex via POSIX's pthread_cleanup stack

@param[in] mutex - Mutex to unlock
*//*==============================================================================================*/
void DG_ENGINE_UTIL_generic_cleanup_unlock(void* mutex)
{
    DG_DBG_TRACE("Cleaning up locked mutex %p", mutex);
    pthread_mutex_unlock((pthread_mutex_t*)mutex);
}

/*=============================================================================================*//**
@brief Verifies if a request has at least a total of X number of bytes of request data

@param[in]  req         - The request to check
@param[in]  req_min_len - The minimum length to check against
@param[out] rsp         - Optional, response builder will be updated to indicate a length error
                          in a failure case

@return TRUE = length is valid, FALSE = error, invalid length
*//*==============================================================================================*/
BOOL DG_ENGINE_UTIL_req_len_check_at_least(DG_DEFS_DIAG_REQ_T* req, UINT32 req_min_len,
                                           DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    dg_engine_util_diag_rsp_builder_t* real_rsp     = (dg_engine_util_diag_rsp_builder_t*)rsp;
    BOOL                               is_len_valid = TRUE;

    DG_DBG_TRACE("Request length check, Min Length = %d, Request Length = %d",
                 req_min_len, req->header.length);
    if (req->header.length < req_min_len)
    {
        if (real_rsp)
        {
            DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_ERR_LENGTH,
                                                "Invalid Data Length, expected at least %d bytes",
                                                req_min_len);
            real_rsp->data_cur_len = 0;
        }
        is_len_valid = FALSE;
    }
    return is_len_valid;
}

/*=============================================================================================*//**
@brief Verifies if a request has exactly a total of X number of bytes of request data

@param[in]  req         - The request to check
@param[in]  req_min_len - The length to check
@param[out] rsp         - Optional, response builder will be updated to indicate a length error in a
                          failure case

@return TRUE = length is valid, FALSE = error, invalid length
*//*==============================================================================================*/
BOOL DG_ENGINE_UTIL_req_len_check_equal(DG_DEFS_DIAG_REQ_T* req, UINT32 req_min_len,
                                        DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    dg_engine_util_diag_rsp_builder_t* real_rsp     = (dg_engine_util_diag_rsp_builder_t*)rsp;
    BOOL                               is_len_valid = TRUE;

    DG_DBG_TRACE("Request length check, Equal Length = %d, Request Length = %d",
                 req_min_len, req->header.length);
    if (req->header.length != req_min_len)
    {
        if (real_rsp)
        {
            DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_ERR_LENGTH,
                                                "Invalid Data Length, expected %d bytes",
                                                req_min_len);
            real_rsp->data_cur_len = 0;
        }
        is_len_valid = FALSE;
    }
    return is_len_valid;
}

/*=============================================================================================*//**
@brief Verifies if a request has at least X number of bytes of request data remaining

@param[in]  req         - The request to check
@param[in]  req_min_len - The minimum length to check against
@param[out] rsp         - Optional, response builder will be updated to indicate a length error in a
                          failure case

@return TRUE = length is valid, FALSE = error, invalid length
*//*==============================================================================================*/
BOOL DG_ENGINE_UTIL_req_remain_len_check_at_least(DG_DEFS_DIAG_REQ_T* req, UINT32 req_min_len,
                                                  DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    dg_engine_util_diag_rsp_builder_t* real_rsp     = (dg_engine_util_diag_rsp_builder_t*)rsp;
    BOOL                               is_len_valid = TRUE;
    UINT32                             remain_len;

    if (req->header.length < req->data_offset)
    {
        remain_len = 0;
    }
    else
    {
        remain_len = req->header.length - req->data_offset;
    }

    DG_DBG_TRACE("Request length check, Min Length = %d, Remaining Request Length = %d",
                 req_min_len, remain_len);
    if (remain_len < req_min_len)
    {
        if (real_rsp)
        {
            DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_ERR_LENGTH,
                                                "Invalid Data Length: %d requested; %d remain",
                                                req_min_len, remain_len);
            real_rsp->data_cur_len = 0;
        }
        is_len_valid = FALSE;
    }
    return is_len_valid;
}

/*=============================================================================================*//**
@brief Verifies if a request has exactly X number of bytes of request data remaining

@param[in]  req         - The request to check
@param[in]  req_min_len - The length to check
@param[out] rsp         - Optional, response builder will be updated to indicate a length error in a
                          failure case

@return TRUE = length is valid, FALSE = error, invalid length
*//*==============================================================================================*/
BOOL DG_ENGINE_UTIL_req_remain_len_check_equal(DG_DEFS_DIAG_REQ_T* req, UINT32 req_min_len,
                                               DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    dg_engine_util_diag_rsp_builder_t* real_rsp     = (dg_engine_util_diag_rsp_builder_t*)rsp;
    BOOL                               is_len_valid = TRUE;
    UINT32                             remain_len;

    if (req->header.length < req->data_offset)
    {
        remain_len = 0;
    }
    else
    {
        remain_len = req->header.length - req->data_offset;
    }

    DG_DBG_TRACE("Request length check, Equal Length = %d, Remaining Request Length = %d",
                 req_min_len, remain_len);
    if (remain_len != req_min_len)
    {
        if (real_rsp)
        {
            DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_ERR_LENGTH,
                                                "Invalid Data Length: %d requested; %d remain",
                                                req_min_len, remain_len);
            real_rsp->data_cur_len = 0;
        }
        is_len_valid = FALSE;
    }
    return is_len_valid;
}

/*=============================================================================================*//**
@brief From a request, gets a 1 byte value

@param[in,out] req - The request to get value from

@return The value in host endian
*//*==============================================================================================*/
UINT8 DG_ENGINE_UTIL_req_parse_1_byte_ntoh(DG_DEFS_DIAG_REQ_T* req)
{
    UINT8 val = *(req->data_ptr + req->data_offset);

    req->data_offset += 1;
    return val;
}

/*=============================================================================================*//**
@brief From a request, converts a 2 byte value from network to host endian

@param[in,out] req - The request to get value from

@return The value in host endian
*//*==============================================================================================*/
UINT16 DG_ENGINE_UTIL_req_parse_2_bytes_ntoh(DG_DEFS_DIAG_REQ_T* req)
{
    UINT16 val = ntohs(*(UINT16*)(req->data_ptr + req->data_offset));

    req->data_offset += 2;
    return val;
}

/*=============================================================================================*//**
@brief From a request, converts a 4 byte value from network to host endian

@param[in,out] req - The request to get value from

@return The value in host endian
*//*==============================================================================================*/
UINT32 DG_ENGINE_UTIL_req_parse_4_bytes_ntoh(DG_DEFS_DIAG_REQ_T* req)
{
    UINT32 val = ntohl(*(UINT32*)(req->data_ptr + req->data_offset));

    req->data_offset += 4;
    return val;
}

/*=============================================================================================*//**
@brief Copys a x bytes of data from a request to dest

@param[in,out]  req       - The request to copy from
@param[in,out]  dest      - Where to copy buf to
@param[in]      num_bytes - The number of bytes to copy

@return The value in host endian
*//*==============================================================================================*/
void DG_ENGINE_UTIL_req_parse_buf(DG_DEFS_DIAG_REQ_T* req, UINT8* dest, UINT32 num_bytes)
{
    memcpy(dest, (req->data_ptr + req->data_offset), num_bytes);
    req->data_offset += num_bytes;
}

/*=============================================================================================*//**
@brief Returns the total length of the DIAG request data

@param[in] req - The DIAG request

@return The total length of the request data
*//*==============================================================================================*/
UINT32 DG_ENGINE_UTIL_req_get_len(DG_DEFS_DIAG_REQ_T* req)
{
    return req->header.length;
}

/*=============================================================================================*//**
@brief Returns the length of the remaining, unparsed, DIAG request data

@param[in] req - The DIAG request

@return The length of the remaining data
*//*==============================================================================================*/
UINT32 DG_ENGINE_UTIL_req_get_remain_len(DG_DEFS_DIAG_REQ_T* req)
{
    UINT32 remain_len = 0;

    if (req->header.length > req->data_offset)
    {
        remain_len = req->header.length - req->data_offset;
    }

    return remain_len;
}


/*=============================================================================================*//**
@brief Returns a pointer into request data at current parsing offset

@param[in]  req       - The DIAG request

@return Pointer to the request data at current parsing offset
*//*==============================================================================================*/
UINT8* DG_ENGINE_UTIL_req_get_remain_data_ptr(DG_DEFS_DIAG_REQ_T* req)
{
    return req->data_ptr + req->data_offset;
}


/*=============================================================================================*//**
@brief Initializes a #DG_DEFS_DIAG_RSP_BUILDER_T response builder for use in a handler.

@return The initialized response builder

@note
  - This function MUST be called prior to using the response builder
  - DG_ENGINE_UTIL_rsp_free() MUST be called when the response builder is no longer needed
*//*==============================================================================================*/
DG_DEFS_DIAG_RSP_BUILDER_T* DG_ENGINE_UTIL_rsp_init(void)
{
    dg_engine_util_diag_rsp_builder_t* real_rsp = NULL;

    if ((real_rsp = (dg_engine_util_diag_rsp_builder_t*)
                    malloc(sizeof(dg_engine_util_diag_rsp_builder_t))) == NULL)
    {
        DG_PAL_UTIL_panic("Failed to init response builder!");
    }
    else
    {
        real_rsp->code            = DG_RSP_CODE_NOT_SET;
        real_rsp->data_ptr        = NULL;
        real_rsp->data_cur_len    = 0;
        real_rsp->data_max_len    = 0;
        real_rsp->flag            = DG_DEFS_RSP_FLAG_NONE;
        real_rsp->err_text_string = NULL;
        real_rsp->init_marker     = 0x005A4143;
    }
    return (DG_DEFS_DIAG_RSP_BUILDER_T*)real_rsp;
}

/*=============================================================================================*//**
@brief Frees a #DG_DEFS_DIAG_RSP_BUILDER_T response builder

@param[out] rsp - The response builder to free

@note
  - DG_ENGINE_UTIL_rsp_free() MUST be called when the response builder is no longer needed
*//*==============================================================================================*/
void DG_ENGINE_UTIL_rsp_free(DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    dg_engine_util_diag_rsp_builder_t* real_rsp = (dg_engine_util_diag_rsp_builder_t*)rsp;

    if (real_rsp != NULL)
    {
        free(real_rsp->data_ptr);
        real_rsp->data_ptr = NULL;

        free(real_rsp->err_text_string);
        real_rsp->err_text_string = NULL;

        real_rsp->data_max_len = 0;
        real_rsp->data_cur_len = 0;
        real_rsp->init_marker  = 0x00000000;
        free(rsp);
    }
}

/*=============================================================================================*//**
@brief Allocates memory for response builder data

@param[out] rsp      - The response builder to allocate memory for
@param[in]  max_size - The size of memory to allocate

@return TRUE = allocation successful, FALSE = failed

@note
  - On failure, the response builder is updated to indicate a malloc fail
  - Do not use this to allocate memory for generic buffers, use DG_ENGINE_UTIL_alloc_mem() for
    that purpose
  - It is an error to call this function twice with the same response builder.
*//*==============================================================================================*/
BOOL DG_ENGINE_UTIL_rsp_data_alloc(DG_DEFS_DIAG_RSP_BUILDER_T* rsp, UINT32 max_size)
{
    dg_engine_util_diag_rsp_builder_t* real_rsp   = (dg_engine_util_diag_rsp_builder_t*)rsp;
    BOOL                               is_success = FALSE;
    UINT8*                             buffer     = NULL;

    /* If the response builder data pointer is already set, return an error */
    if (real_rsp->data_ptr != NULL)
    {
        DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_MALLOC_FAIL,
                                            "Attempted to allocate response data twice");
    }
    else
    {
        buffer = (UINT8*)malloc(max_size);
        if (buffer == NULL)
        {
            DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_MALLOC_FAIL,
                                                "Allocate %d bytes for response builder failed",
                                                max_size);

            real_rsp->data_cur_len = 0;
        }
        else
        {
            real_rsp->data_ptr     = buffer;
            real_rsp->data_max_len = max_size;
            is_success             = TRUE;
        }
    }

    return is_success;
}

/*=============================================================================================*//**
@brief Gets a 1 byte value and appends it to a #DG_DEFS_DIAG_RSP_BUILDER_T response builder

@param[in,out] rsp - The response builder to append to
@param[in]     val - The value to append

@note
  - rsp must have been initialized by DG_ENGINE_UTIL_rsp_init() before using this function
  - rsp must have had sufficient memory allocated by DG_ENGINE_UTIL_rsp_data_alloc() before using
    this function
*//*==============================================================================================*/
void DG_ENGINE_UTIL_rsp_append_1_byte_hton(DG_DEFS_DIAG_RSP_BUILDER_T* rsp, UINT8 val)
{
    dg_engine_util_diag_rsp_builder_t* real_rsp = (dg_engine_util_diag_rsp_builder_t*)rsp;

    DG_ENGINE_UTIL_buf_append_1_byte_hton(real_rsp->data_ptr, &real_rsp->data_cur_len, val);
}

/*=============================================================================================*//**
@brief Converts a 2 byte value from host to network endian and appends it to a
       #DG_DEFS_DIAG_RSP_BUILDER_T response builder

@param[in,out] rsp - The response builder to append to
@param[in]     val - The value to append

@note
  - rsp must have been initialized by DG_ENGINE_UTIL_rsp_init() before using this function
  - rsp must have had sufficient memory allocated by DG_ENGINE_UTIL_rsp_data_alloc() before using
    this function
*//*==============================================================================================*/
void DG_ENGINE_UTIL_rsp_append_2_bytes_hton(DG_DEFS_DIAG_RSP_BUILDER_T* rsp, UINT16 val)
{
    dg_engine_util_diag_rsp_builder_t* real_rsp = (dg_engine_util_diag_rsp_builder_t*)rsp;

    DG_ENGINE_UTIL_buf_append_2_bytes_hton(real_rsp->data_ptr, &real_rsp->data_cur_len, val);
}

/*=============================================================================================*//**
@brief Converts a 4 byte value from host to network endian and appends it to a
       #DG_DEFS_DIAG_RSP_BUILDER_T response builder

@param[in,out] rsp - The response builder to append to
@param[in]     val - The value to append

@note
  - rsp must have been initialized by DG_ENGINE_UTIL_rsp_init() before using this function
  - rsp must have had sufficient memory allocated by DG_ENGINE_UTIL_rsp_data_alloc() before using
    this function
*//*==============================================================================================*/
void DG_ENGINE_UTIL_rsp_append_4_bytes_hton(DG_DEFS_DIAG_RSP_BUILDER_T* rsp, UINT32 val)
{
    dg_engine_util_diag_rsp_builder_t* real_rsp = (dg_engine_util_diag_rsp_builder_t*)rsp;

    DG_ENGINE_UTIL_buf_append_4_bytes_hton(real_rsp->data_ptr, &real_rsp->data_cur_len, val);
}

/*=============================================================================================*//**
@brief Appends a buffer of X bytes to a #DG_DEFS_DIAG_RSP_BUILDER_T response builder

@param[in,out] rsp        - The response builder to append to
@param[in]     src        - The buffer to append
@param[in]     num_bytes  - Number of bytes to append

@note
  - rsp must have been initialized by DG_ENGINE_UTIL_rsp_init() before using this function
  - rsp must have had sufficient memory allocated by DG_ENGINE_UTIL_rsp_data_alloc() before using
    this function
*//*==============================================================================================*/
void DG_ENGINE_UTIL_rsp_append_buf(DG_DEFS_DIAG_RSP_BUILDER_T* rsp, UINT8* src, UINT32 num_bytes)
{
    dg_engine_util_diag_rsp_builder_t* real_rsp = (dg_engine_util_diag_rsp_builder_t*)rsp;

    DG_ENGINE_UTIL_buf_append_buf(real_rsp->data_ptr, &real_rsp->data_cur_len, src, num_bytes);
}

/*=============================================================================================*//**
@brief Takes a 1 byte value and replaces the #DG_DEFS_DIAG_RSP_BUILDER_T response builder data at
       offset x

@param[in,out] rsp    - The response builder to set
@param[in]     offset - The byte offset into response builder data to set
@param[in]     val    - The value to set

@note
  - rsp must have been initialized by DG_ENGINE_UTIL_rsp_init() before using this function
  - rsp must have had sufficient memory allocated by DG_ENGINE_UTIL_rsp_data_alloc() before using
    this function
*//*==============================================================================================*/
void DG_ENGINE_UTIL_rsp_replace_1_byte_hton(DG_DEFS_DIAG_RSP_BUILDER_T* rsp, UINT32 offset, UINT8 val)
{
    dg_engine_util_diag_rsp_builder_t* real_rsp = (dg_engine_util_diag_rsp_builder_t*)rsp;

    DG_ENGINE_UTIL_buf_replace_1_byte_hton(real_rsp->data_ptr, offset, val);
}

/*=============================================================================================*//**
@brief Converts a 2 byte value from host to network endian and replaces the
       #DG_DEFS_DIAG_RSP_BUILDER_T response builder data at offset x

@param[in,out] rsp    - The response builder to set
@param[in]     offset - The byte offset into response builder data to set
@param[in]     val    - The value to set

@note
  - rsp must have been initialized by DG_ENGINE_UTIL_rsp_init() before using this function
  - rsp must have had sufficient memory allocated by DG_ENGINE_UTIL_rsp_data_alloc() before using
    this function
*//*==============================================================================================*/
void DG_ENGINE_UTIL_rsp_replace_2_bytes_hton(DG_DEFS_DIAG_RSP_BUILDER_T* rsp, UINT32 offset, UINT16 val)
{
    dg_engine_util_diag_rsp_builder_t* real_rsp = (dg_engine_util_diag_rsp_builder_t*)rsp;

    DG_ENGINE_UTIL_buf_replace_2_bytes_hton(real_rsp->data_ptr, offset, val);
}

/*=============================================================================================*//**
@brief Converts a 4 byte value from host to network endian and replace the
       #DG_DEFS_DIAG_RSP_BUILDER_T response builder data at offset x

@param[in,out] rsp    - The response builder to set
@param[in]     offset - The byte offset into response builder data to set
@param[in]     val    - The value to set

@note
  - rsp must have been initialized by DG_ENGINE_UTIL_rsp_init() before using this function
  - rsp must have had sufficient memory allocated by DG_ENGINE_UTIL_rsp_data_alloc() before using
    this function
*//*==============================================================================================*/
void DG_ENGINE_UTIL_rsp_replace_4_bytes_hton(DG_DEFS_DIAG_RSP_BUILDER_T* rsp, UINT32 offset, UINT32 val)
{
    dg_engine_util_diag_rsp_builder_t* real_rsp = (dg_engine_util_diag_rsp_builder_t*)rsp;

    DG_ENGINE_UTIL_buf_replace_4_bytes_hton(real_rsp->data_ptr, offset, val);
}
/*=============================================================================================*//**
@brief sets the current length of the response buffer data to zero

@param[in,out] rsp                - The response builder to set

@note
  - rsp must have been initialized by DG_ENGINE_UTIL_rsp_init() before using this function
  - rsp must have had sufficient memory allocated by DG_ENGINE_UTIL_rsp_data_alloc() before using
    this function
*//*==============================================================================================*/
void DG_ENGINE_UTIL_rsp_set_len_zero(DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    dg_engine_util_diag_rsp_builder_t* real_rsp = (dg_engine_util_diag_rsp_builder_t*)rsp;

    real_rsp->data_cur_len = 0;
}
/*=============================================================================================*//**
@brief Gets a 1 byte value and appends it to a buffer

@param[in,out] buf         - The buf to append to
@param[in,out] cur_buf_len - The current length of the buffer, will be updated with new length
@param[in]     val         - The value to append
*//*==============================================================================================*/
void DG_ENGINE_UTIL_buf_append_1_byte_hton(UINT8* buf, UINT32* cur_buf_len, UINT8 val)
{
    *(buf + (*cur_buf_len)++) = val;
}

/*=============================================================================================*//**
@brief Converts a 2 byte value from host to network endian and appends it to a buffer

@param[in,out] buf         - The buf to append to
@param[in,out] cur_buf_len - The current length of the buffer, will be updated with new length
@param[in]     val         - The value to append
*//*==============================================================================================*/
void DG_ENGINE_UTIL_buf_append_2_bytes_hton(UINT8* buf, UINT32* cur_buf_len, UINT16 val)
{
    *(UINT16*)(buf + *cur_buf_len) = htons(val);

    *cur_buf_len += 2;
}

/*=============================================================================================*//**
@brief Converts a 4 byte value from host to network endian and appends it to a buffer

@param[in,out] buf         - The buf to append to
@param[in,out] cur_buf_len - The current length of the buffer, will be updated with new length
@param[in]     val         - The value to append
*//*==============================================================================================*/
void DG_ENGINE_UTIL_buf_append_4_bytes_hton(UINT8* buf, UINT32* cur_buf_len, UINT32 val)
{
    *(UINT32*)(buf + *cur_buf_len) = htonl(val);

    *cur_buf_len += 4;
}

/*=============================================================================================*//**
@brief Converts 16 bit data in a big buffer from host to network endian

@param[in,out] buf    - The buf to set
@param[in]     len    - length of buf data
*//*==============================================================================================*/
void DG_ENGINE_UTIL_buf_uint16_hton(UINT16* buf, UINT32 len)
{
    UINT32 offset;

    for (offset = 0; offset < len; offset++)
    {
        buf[offset] = htons(buf[offset]);
    }
}

/*=============================================================================================*//**
@brief Appends a buffer to another buffer

@param[in,out] dest_buf    - The buf to append to
@param[in,out] cur_buf_len - The current length of the buffer, will be updated with new length
@param[in]     src         - The buf to append from
@param[in]     num_bytes   - The number of byte to append
*//*==============================================================================================*/
void DG_ENGINE_UTIL_buf_append_buf(UINT8* dest_buf, UINT32* cur_buf_len, void* src, UINT32 num_bytes)
{
    memcpy((dest_buf + *cur_buf_len), src, num_bytes);
    *cur_buf_len += num_bytes;
}

/*=============================================================================================*//**
@brief Gets a 1 byte value and replaces the buf at offset x

@param[in,out] buf    - The buf to set
@param[in]     offset - The byte offset into buf data to set
@param[in]     val    - The value to set
*//*==============================================================================================*/
void DG_ENGINE_UTIL_buf_replace_1_byte_hton(UINT8* buf, UINT32 offset, UINT8 val)
{
    *(buf + offset) = val;
}

/*=============================================================================================*//**
@brief Converts a 2 byte value from host to network endian and replaces the buf at offset x

@param[in,out] buf    - The buf to set
@param[in]     offset - The byte offset into buf data to set
@param[in]     val    - The value to set
*//*==============================================================================================*/
void DG_ENGINE_UTIL_buf_replace_2_bytes_hton(UINT8* buf, UINT32 offset, UINT16 val)
{
    *(UINT16*)(buf + offset) = htons(val);
}

/*=============================================================================================*//**
@brief Converts a 4 byte value from host to network endian and sets the replaces at offset x

@param[in,out] buf    - The buf to set
@param[in]     offset - The byte offset into buf data to set
@param[in]     val    - The value to set
*//*==============================================================================================*/
void DG_ENGINE_UTIL_buf_replace_4_bytes_hton(UINT8* buf, UINT32 offset, UINT32 val)
{
    *(UINT32*)(buf + offset) = htonl(val);
}
/*=============================================================================================*//**
@brief From a buf, gets a 1 byte value.

@param[in,out] buf_ptr - The buf to get value from, buf_ptr updated to past converted value

@return The value in host endian
*//*==============================================================================================*/
UINT8 DG_ENGINE_UTIL_buf_parse_1_byte_ntoh(UINT8** buf_ptr)
{
    UINT8 val = **buf_ptr;

    *buf_ptr += 1;
    return val;
}

/*=============================================================================================*//**
@brief From a buf, converts a 2 byte value from network to host endian

@param[in,out] buf_ptr - The buf to get value from, buf_ptr updated to past converted value

@return The value in host endian
*//*==============================================================================================*/
UINT16 DG_ENGINE_UTIL_buf_parse_2_bytes_ntoh(UINT8** buf_ptr)
{
    UINT16 val = ntohs(*(UINT16*)(*buf_ptr));

    *buf_ptr += 2;
    return val;
}

/*=============================================================================================*//**
@brief From a buf, converts a 4 byte value from network to host endian

@param[in,out] buf_ptr - The buf to get value from, buf_ptr updated to past converted value

@return The value in host endian
*//*==============================================================================================*/
UINT32 DG_ENGINE_UTIL_buf_parse_4_bytes_ntoh(UINT8** buf_ptr)
{
    UINT32 val = ntohl(*(UINT32*)(*buf_ptr));

    *buf_ptr += 4;
    return val;
}

/*=============================================================================================*//**
@brief Copys a x bytes of data from buf_ptr to dest, buf_ptr updated past copied bytes

@param[in,out]  dest      - Where to copy buf to
@param[in,out]  buf_ptr   - The buf to copy from, buf_ptr updated to past copied bytes
@param[in]      num_bytes - The number of bytes to copy

@return The value in host endian
*//*==============================================================================================*/
void DG_ENGINE_UTIL_buf_parse_buf(UINT8* dest, UINT8** buf_ptr, UINT32 num_bytes)
{
    memcpy(dest, *buf_ptr, num_bytes);
    *buf_ptr += num_bytes;
}

/*=============================================================================================*//**
@brief Allocates memory for a generic buffer

@param[in]      max_size - The size of memory to allocate
@param[in,out]  rsp      - Optional, if memory allocation fails, response builder updated to indicate
                           malloc failed

@return Pointer to memory, NULL on fail

@note
  - Do not use this to allocate memory for response builder, use DG_ENGINE_UTIL_rsp_data_alloc()
    for that purpose
  - If rsp is passed it, rsp must have been initialized by DG_ENGINE_UTIL_rsp_init() before using
    this function
*//*==============================================================================================*/
UINT8* DG_ENGINE_UTIL_alloc_mem(UINT32 max_size, DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    dg_engine_util_diag_rsp_builder_t* real_rsp = (dg_engine_util_diag_rsp_builder_t*)rsp;
    UINT8*                             buffer   = (UINT8*)malloc(max_size);

    if (buffer == NULL)
    {
        if (real_rsp)
        {
            DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_MALLOC_FAIL,
                                                "Allocate %d bytes for generic buffer failed",
                                                max_size);
            real_rsp->data_cur_len = 0;
        }
    }

    return buffer;
}
/*=============================================================================================*//**
@brief process the wchar filename ,once the filename is not absolute ,need prepend default 12m path.

@param[in]  req  - The request to check
@param[out] rsp  - Optional, response builder will be updated to indicate a length error in a failure case
@return Pointer to processed filename,null termination.
@note
 - returned filename must be NULL terminated,the loading function must be responsible to free the memory
 - this funtion is designed to the case if there is no null terminal at the end of the input filename ,the function
 - will append a null terminal.
*//*==============================================================================================*/
W_CHAR* DG_ENGINE_UTIL_process_unicode_filename_no_null(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    DG_PAL_UTIL_ABSOLUTE_PATH_T filename_check = DG_PAL_UTIL_ABSOLUTE_PATH_ERROR;
    W_CHAR*                     filename       = NULL;      /*input filename*/
    W_CHAR*                     ret_wchar      = NULL;      /*return value,must be initialized to NULL*/
    UINT32                      size           = 0;         /*Store filename size*/
    UINT32                      i              = 0;

    size = DG_ENGINE_UTIL_req_get_remain_len(req);
    /* + 2 for append null termination */
    if ((filename = (W_CHAR*)DG_ENGINE_UTIL_alloc_mem(size + 2, rsp)) != NULL)
    {
        for (i = 0; i < (size / 2); i++)
        {
            filename[i] = DG_ENGINE_UTIL_req_parse_2_bytes_ntoh(req);
        }
        if (filename[size / 2 - 1] != 0x0000)
        {
            DG_DBG_TRACE("Append NULL terminal");
            filename[size / 2] = 0x0000;
        }
        filename_check = DG_PAL_UTIL_check_absolute_path(filename);
        if (filename_check == DG_PAL_UTIL_ABSOLUTE_PATH_YES)
        {
            ret_wchar = filename;
        }
        else if (filename_check == DG_PAL_UTIL_ABSOLUTE_PATH_NO)
        {
            ret_wchar = DG_ENGINE_UTIL_prepend_default_path_unicode(filename, rsp);
            free(filename);

        }
        else
        {
            free(filename);
            DG_ENGINE_UTIL_rsp_set_error_string(rsp,
                                                DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                "Check absolute path error!");
        }
    }
    return ret_wchar;
}
/*=============================================================================================*//**
@brief process the unicode filename ,once the filename is not absolute ,need prepend default path.

@param[in]  req     - The request to check
@param[out] rsp     - Optional, response builder will be updated to indicate a length error in a
                      failure case
@return Pointer to processed filename
@note
 - returned filename must be NULL terminated,the loading function must be responsible to free the memory
*//*==============================================================================================*/
W_CHAR* DG_ENGINE_UTIL_process_unicode_filename(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    DG_PAL_UTIL_ABSOLUTE_PATH_T filename_check = DG_PAL_UTIL_ABSOLUTE_PATH_ERROR;
    W_CHAR*                     filename       = NULL;      /*input filename*/
    W_CHAR*                     ret_wchar      = NULL;      /*return value,must be initialized to NULL*/
    UINT32                      size           = 0;         /*Store filename size*/
    UINT32                      i              = 0;

    size = DG_ENGINE_UTIL_req_get_remain_len(req);
    if ((filename = (W_CHAR*)DG_ENGINE_UTIL_alloc_mem(size, rsp)) != NULL)
    {
        for (i = 0; i < (size / 2); i++)
        {
            filename[i] = DG_ENGINE_UTIL_req_parse_2_bytes_ntoh(req);
        }
        if (filename[size / 2 - 1] != 0x0000)
        {
            free(filename);
            filename = NULL;
            DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_ERR_PARM,
                                                "NULL missing in filename");
        }
        else
        {
            filename_check = DG_PAL_UTIL_check_absolute_path(filename);
            if (filename_check == DG_PAL_UTIL_ABSOLUTE_PATH_YES)
            {
                ret_wchar = filename;
            }
            else if (filename_check == DG_PAL_UTIL_ABSOLUTE_PATH_NO)
            {
                ret_wchar = DG_ENGINE_UTIL_prepend_default_path_unicode(filename, rsp);
                free(filename);
            }
            else
            {
                free(filename);
                DG_ENGINE_UTIL_rsp_set_error_string(rsp,
                                                    DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                    "Check absolute path error!");
            }
        }
    }
    return ret_wchar;
}
/*=============================================================================================*//**
@brief process the unicode filename ,once the filename is not absolute ,need prepend default path.

@param[in]  req         - The request to check
@param[out] rsp         - Optional, response builder will be updated to indicate a length error in a
                          failure case

@return - the processed filename,must be NULL terminated,the loading function must be responsible to
 -free the memory,once the filename is not NULL pointer
*//*==============================================================================================*/
char* DG_ENGINE_UTIL_process_ascii_filename(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{

    DG_PAL_UTIL_ABSOLUTE_PATH_T ret = DG_PAL_UTIL_ABSOLUTE_PATH_ERROR;
    /** all of the pointers must be set to NULL*/
    W_CHAR* filename       = NULL;
    W_CHAR* full_filename  = NULL;
    W_CHAR* alias_filename = NULL;
    W_CHAR* ret_wchar      = NULL;
    char*   ret_char       = NULL;
    UINT32  size           = 0;
    UINT16  i              = 0;

    /**file size including the end NULL */
    size = DG_ENGINE_UTIL_req_get_remain_len(req);

    if ((filename = (W_CHAR*)DG_ENGINE_UTIL_alloc_mem(size * 2, rsp)) != NULL)
    {
        for (i = 0; i < size; i++)
        {
            filename[i] = (W_CHAR)(DG_ENGINE_UTIL_req_parse_1_byte_ntoh(req));
        }
        if (filename[size - 1] != 0x0000)
        {
            free(filename);
            filename = NULL;
            DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_ERR_PARM,
                                                "NULL missing in filename");
        }
        else
        {
            ret = DG_PAL_UTIL_check_absolute_path(filename);
            if (ret == DG_PAL_UTIL_ABSOLUTE_PATH_ERROR)
            {
                free(filename);
                filename = NULL;
                DG_ENGINE_UTIL_rsp_set_error_string(rsp,
                                                    DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                    "Check absolute path error!");
            }
            else if (ret == DG_PAL_UTIL_ABSOLUTE_PATH_YES)
            {
                ret_wchar = filename;
            }
            else if (ret == DG_PAL_UTIL_ABSOLUTE_PATH_NO)
            {
                if ((full_filename = (W_CHAR*)DG_ENGINE_UTIL_alloc_mem
                                         (size * 2 + DG_PAL_UTIL_DEFAULT_DIR_MAX_SIZE, rsp)) != NULL)
                {
                    if (DG_PAL_UTIL_get_default_path(full_filename) == FALSE)
                    {
                        free(full_filename);
                        full_filename = NULL;
                        DG_ENGINE_UTIL_rsp_set_error_string(rsp,
                                                            DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Get Default path error!");
                    }
                    else
                    {
                        alias_filename = full_filename;
                        while (*alias_filename != 0x0000)
                        {
                            alias_filename++;
                        }
                        for (i = 0; i < size; i++)
                        {
                            *alias_filename++ = filename[i];
                        }
                        alias_filename = full_filename;
                        ret_wchar      = full_filename;

                    }
                }
                if (filename != NULL)
                {
                    free(filename);
                    filename = NULL;
                }

            }
        }
    }
    /**Here the ret_wchar may have two type of buffer ,one case is the
     * filename buffer if the filename is absolute,another case is
     * the full_filename buffer if the filename is not absolute and have
     * been prepend default path*/
    if (ret_wchar != NULL)
    {
        /** convert filename form W_CHAR* to char* */
        alias_filename = ret_wchar;
        size           = 0;
        while (ret_wchar[size] != 0x0000)
        {
            size++;
        }
        /**file size include the end NULL*/
        size++;
        DG_DBG_TRACE("Processed W_CHAR filename size is: %d", size);
        if ((ret_char = (char*)DG_ENGINE_UTIL_alloc_mem(size, rsp)) != NULL)
        {
            size = 0;
            while (*alias_filename != 0x0000)
            {
                ret_char[size] = (char)(*alias_filename);
                alias_filename++;
                size++;
            }
            ret_char[size] = 0x00;
            DG_DBG_TRACE("Processed ascii filename is: %s", ret_char);
        }
        if (ret_wchar != NULL)
        {
            free(ret_wchar);
            ret_wchar = NULL;
        }
    }
    return ret_char;
}
/*=============================================================================================*//**
@brief Prepends the default 12m path to an unicode string

@param[in]      src - Null terminated unicode string to prepend the default 12m path to.
@param[in,out]  rsp - Optional, if memory allocation fails, response builder updated to indicate
                      malloc failed

@return Pointer to the new null terminated unicode string, NULL on failure

@note
 - Calling function responsible for freeing the returned string on success cases
*//*==============================================================================================*/
W_CHAR* DG_ENGINE_UTIL_prepend_default_path_unicode(W_CHAR* src, DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    int     src_len = 0;
    int     i       = 0;
    W_CHAR* ret_val = NULL;

    while (src[src_len] != 0x0000)
    {
        src_len++;
    }
    src_len++;
    DG_DBG_TRACE("src_len : %d", src_len);
    ret_val = (W_CHAR*)
              DG_ENGINE_UTIL_alloc_mem(src_len * 2 + DG_PAL_UTIL_DEFAULT_DIR_MAX_SIZE, rsp);
    if (ret_val != NULL)
    {
        if (DG_PAL_UTIL_get_default_path(ret_val) == FALSE)
        {
            /* Set error response if given */
            free(ret_val);
            ret_val = NULL;
            if (rsp != NULL)
            {
                DG_ENGINE_UTIL_rsp_set_error_string(rsp,
                                                    DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                    "Get default path error!");
            }
        }
        else
        {
            /* Move index to the end of the default 12m path*/
            while (ret_val[i] != 0x0000)
            {
                i++;
            }
            /* Append filename to the default 12m path*/
            src_len = 0;
            while (src[src_len] != 0x0000)
            {
                ret_val[i + src_len] = src[src_len];
                src_len++;
            }
            /*Append NULL terminal*/
            ret_val[i + src_len] = 0x0000;
            DG_DBG_TRACE("Prepended wchar filename length including NULL terminal: %d", (i + src_len + 1));
        }

    }
    return ret_val;
}
/*=============================================================================================*//**
@brief Prepends the default 12m path to an ASCII string

@param[in]      src - Null terminated ASCII string to prepend the default 12m path to.
@param[in,out]  rsp - Optional, if memory allocation fails, response builder updated to indicate
                      malloc failed

@return Pointer to the new null terminated ASCII string, NULL on failure

@note
 - Calling function responsible for freeing the returned string on success cases
*//*==============================================================================================*/
char* DG_ENGINE_UTIL_prepend_default_path_ascii(char* src, DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    int     index          = 0;
    char*   new_string     = NULL;
    W_CHAR* default_string = NULL;

    /* Allocate memory to hold the default string */
    if ((default_string =
             (W_CHAR*)DG_ENGINE_UTIL_alloc_mem(DG_PAL_UTIL_DEFAULT_DIR_MAX_SIZE, rsp)) != NULL)
    {
        /* Get the default string */
        if (DG_PAL_UTIL_get_default_path(default_string) == FALSE)
        {
            /* Set error response if given */
            if (rsp != NULL)
            {
                DG_ENGINE_UTIL_rsp_set_error_string(rsp,
                                                    DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                    "Get default path error!");
            }
        }
        else
        {
            /* Allocate space for the new string, +1 for NULL */
            new_string = (char*)
                         DG_ENGINE_UTIL_alloc_mem(strlen(src) + 1 + DG_PAL_UTIL_DEFAULT_DIR_MAX_SIZE, rsp);

            if (new_string != NULL)
            {
                /* Copy the default string to the new string, converting UNICODE => ASCII */
                while (default_string[index] != 0x0000)
                {
                    new_string[index] = (char)default_string[index];
                    index++;
                }
                /* Copy the NULL */
                new_string[index] = (char)default_string[index];

                /* Concat the src string to the end of the new string */
                strcat(new_string, src);
            }
        }
        free(default_string);
    }

    return new_string;
}

/*=============================================================================================*//**
@brief Sends a DIAG response builder to the client

@param[in] rsp  - Response Builder to send
@param[in] diag - The DIAG request which orginated this response

@note
  - rsp must have been initialized by DG_ENGINE_UTIL_rsp_init() before using this function
*//*==============================================================================================*/
void DG_ENGINE_UTIL_rsp_send(DG_DEFS_DIAG_RSP_BUILDER_T* rsp, DG_DEFS_DIAG_REQ_T* diag)
{
    dg_engine_util_diag_rsp_builder_t* real_rsp = (dg_engine_util_diag_rsp_builder_t*)rsp;
    UINT8*                             rsp_data_ptr;
    UINT32                             rsp_len;

    /* If there is an error string, that takes precedence */
    if (real_rsp->err_text_string)
    {
        if (DG_PAL_SECURITY_is_diag_error_strings_allowed() == TRUE)
        {
            DG_DBG_TRACE("DG_PAL_SECURITY_is_diag_error_strings_allowed == TRUE");
            rsp_data_ptr = (UINT8*)real_rsp->err_text_string;
            rsp_len      = strlen(real_rsp->err_text_string) + 1; /* +1 to include NULL */
        }
        else
        {
            DG_DBG_TRACE("DG_PAL_SECURITY_is_diag_error_strings_allowed == FALSE");
            rsp_data_ptr = NULL;
            rsp_len      = 0;
        }
    }
    else
    {
        rsp_data_ptr = real_rsp->data_ptr;
        rsp_len      = real_rsp->data_cur_len;
    }

    dg_engine_util_send_response(diag, real_rsp->code, real_rsp->flag, rsp_len, rsp_data_ptr);
}

/*=============================================================================================*//**
@brief Sets the response code of a response builder

@param[in,out] rsp  - The response builder to update
@param[in] code - Code to set

@note
  - rsp must have been initialized by DG_ENGINE_UTIL_rsp_init() before using this function
*//*==============================================================================================*/
void DG_ENGINE_UTIL_rsp_set_code(DG_DEFS_DIAG_RSP_BUILDER_T* rsp, DG_RSP_CODE_T code)
{
    dg_engine_util_diag_rsp_builder_t* real_rsp = (dg_engine_util_diag_rsp_builder_t*)rsp;

    real_rsp->code = code;
}

/*=============================================================================================*//**
@brief Indicates if the response indicates a failure

@param[in] rsp  - The response builder to check for failure

@return TRUE = response indicates failure, FALSE = command indicates success/not set

@note
  - To determine if the response is a failure, the response code must be set to a failure, or the
    fail flag must be set
*//*==============================================================================================*/
BOOL DG_ENGINE_UTIL_rsp_is_failure(DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    dg_engine_util_diag_rsp_builder_t* real_rsp   = (dg_engine_util_diag_rsp_builder_t*)rsp;
    BOOL                               is_failure = FALSE;

    /* If response code is set to a failure, indicate the response is a
       failure */
    if ((real_rsp->code != DG_RSP_CODE_NOT_SET) &&
        (real_rsp->code != DG_RSP_CODE_CMD_RSP_GENERIC))
    {
        is_failure = TRUE;
    }

    return is_failure;
}

/*=============================================================================================*//**
@brief Sets the specified flag in the response builder.

@param[in,out] rsp  - The response builder to update
@param[in] flag - Flag to set, all previously set flags are maintained, except in clear case.
                  To clear all flags, use DG_DEFS_RSP_FLAG_NONE.

@note
  - rsp must have been initialized by DG_ENGINE_UTIL_rsp_init() before using this function
*//*==============================================================================================*/
void DG_ENGINE_UTIL_rsp_set_flag(DG_DEFS_DIAG_RSP_BUILDER_T* rsp, DG_DEFS_RSP_FLAG_T flag)
{
    dg_engine_util_diag_rsp_builder_t* real_rsp = (dg_engine_util_diag_rsp_builder_t*)rsp;

    if (flag == DG_DEFS_RSP_FLAG_NONE)
    {
        /* If the specified flag is DG_DEFS_RSP_FLAG_NONE, remove all flags */
        real_rsp->flag = flag;
    }
    else
    {
        /* Set the new flag, leaving all other flags set */
        real_rsp->flag |= flag;
    }
}

/*=============================================================================================*//**
@brief Sets a response builder to contain an ASCII error string

@param[in] rsp    - The response builder to update
@param[in] code   - The response code to set
@param[in] format - printf style format string for error message
@param[in] ...    - Variable argument, used to popluated format string

@note
  - rsp must have been initialized by DG_ENGINE_UTIL_rsp_init() before using this function
  - This will cause any previous response builder information to be overwritten
*//*==============================================================================================*/
void DG_ENGINE_UTIL_rsp_set_error_string(DG_DEFS_DIAG_RSP_BUILDER_T* rsp,
                                         DG_RSP_CODE_T code, const char* format, ...)
{
    dg_engine_util_diag_rsp_builder_t* real_rsp = (dg_engine_util_diag_rsp_builder_t*)rsp;

    char*   ascii_error_string = NULL; /* Temporary buffer for error string in ascii */
    va_list args;                      /* Variable arg list                          */

    /* Init variable arg list */
    va_start(args, format);

    if (real_rsp->err_text_string != NULL)
    {
        /* If an string exists, ignore it and use the already set one */
        DG_DBG_ERROR("Tried to set error string while one already existed.");
        DG_DBG_ERROR("Try to set: %s, Current: %s", format, real_rsp->err_text_string);
    }
    else if (vasprintf(&ascii_error_string, format, args) < 0)
    {
        real_rsp->code = code;
        DG_DBG_ERROR("vasprintf() set string failed. errno=%d(%m)", errno);
    }
    else
    {
        real_rsp->code = code;
        DG_DBG_TRACE("Error string set to: %s", ascii_error_string);
        real_rsp->err_text_string = ascii_error_string;
    }

    va_end(args);
}

/*=============================================================================================*//**
@brief Sets a response builder to contain an ASCII error string which will append a driver
       error string

@param[in] rsp    - The response builder to update
@param[in] code   - The response code to set
@param[in] format - printf style format string for error message
@param[in] ...    - Variable argument, used to popluated format string

@note
  - rsp must have been initialized by DG_ENGINE_UTIL_rsp_init() before using this function
  - This will cause any previous response builder information to be overwritten
  - This function would retrieve the correct driver error string from thread specific data
*//*==============================================================================================*/
void DG_ENGINE_UTIL_rsp_set_error_string_drv(DG_DEFS_DIAG_RSP_BUILDER_T* rsp,
                                             DG_RSP_CODE_T code,
                                             const char* format, ...)
{
    char*   ascii_error_string = NULL; /* Temporary buffer for error string in ascii */
    va_list args;                      /* Variable arg list                          */

    /* Init variable arg list */
    va_start(args, format);

    if (vasprintf(&ascii_error_string, format, args) < 0)
    {
        DG_DBG_ERROR("vasprintf() set ascii error string failed. errno=%d(%m)", errno);
    }
    else
    {
        /* Temporary buffer for driver error string in ascii */
        char* drv_error_string = DG_DRV_UTIL_get_error_string();

        /* If driver error string is null, just set the passed in error string */
        if (drv_error_string == NULL)
        {
            DG_ENGINE_UTIL_rsp_set_error_string(rsp, code,
                                                "%s; Driver error string not set",
                                                ascii_error_string);
        }
        else
        {
            DG_ENGINE_UTIL_rsp_set_error_string(rsp, code, "%s; %s",
                                                ascii_error_string, drv_error_string);
        }

        free(ascii_error_string);
    }

    va_end(args);
}


/*=============================================================================================*//**
@brief Gets the response code of a response builder

@param[in] rsp  - The response builder to read

@return The response code of the DIAG response builder

@note
  - rsp must have been initialized by DG_ENGINE_UTIL_rsp_init() before using this function
  - if the response code was not set it will return DG_RSP_CODE_NOT_SET.
*//*==============================================================================================*/
DG_RSP_CODE_T DG_ENGINE_UTIL_rsp_get_code(DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    dg_engine_util_diag_rsp_builder_t* real_rsp = (dg_engine_util_diag_rsp_builder_t*)rsp;

    return real_rsp->code;
}

/*=============================================================================================*//**
@brief Returns the current length of the DIAG response builder

@param[in] rsp - The DIAG response builder

@return The current length of the DIAG response builder
*//*==============================================================================================*/
UINT32 DG_ENGINE_UTIL_rsp_get_curr_len(DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    dg_engine_util_diag_rsp_builder_t* real_rsp = (dg_engine_util_diag_rsp_builder_t*)rsp;

    return real_rsp->data_cur_len;
}

/*=============================================================================================*//**
@brief Returns the data pointer of the DIAG response builder

@param[in]  rsp - The DIAG response builder

@return Pointer to the beginning of the data pointer of the DIAG response builder

@note This pointer can only be used to look at the response data, cannot be updated
*//*==============================================================================================*/
const UINT8* DG_ENGINE_UTIL_rsp_get_data_ptr(DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    dg_engine_util_diag_rsp_builder_t* real_rsp = (dg_engine_util_diag_rsp_builder_t*)rsp;

    return real_rsp->data_ptr;
}

/*=============================================================================================*//**
@brief Returns the response flags of the DIAG response builder

@param[in]  rsp - The DIAG response builder

@return The response flags
*//*==============================================================================================*/
DG_DEFS_RSP_FLAG_T DG_ENGINE_UTIL_rsp_get_flag(DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    dg_engine_util_diag_rsp_builder_t* real_rsp = (dg_engine_util_diag_rsp_builder_t*)rsp;

    return real_rsp->flag;
}

/*=============================================================================================*//**
@brief Convert a 4byte integer to sign-magnitude format with MSB the sign bit.

@param[in]  x - integer

@return The converted value
*//*==============================================================================================*/
UINT32 DG_ENGINE_UTIL_convert_sign_magnitude_4bytes(INT32 x)
{
    return (x >= 0) ? (UINT32)(x) : (0x80000000 | (UINT32)abs(x));
}

/*=============================================================================================*//**
@brief Convert a 2byte integer to sign-magnitude format with MSB the sign bit.

@param[in]  x - integer

@return The converted value
*//*==============================================================================================*/
UINT16 DG_ENGINE_UTIL_convert_sign_magnitude_2bytes(INT16 x)
{
    return (UINT16)((x >= 0) ? (x) : (0x8000 | abs(x)));
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Sends a DIAG response

@param[in] diag       - The originating DIAG request
@param[in] rsp_code   - Response code
@param[in] rsp_flags  - Response flag bitmask
@param[in] rsp_length - Length of response data (not including response header)
@param[in] rsp_pdata  - Pointer to response data (not including response header)

@note
  - For unsolicited responses, they will be sent to all connected clients
*//*==============================================================================================*/
void dg_engine_util_send_response(DG_DEFS_DIAG_REQ_T* diag,
                                  DG_RSP_CODE_T rsp_code,
                                  DG_DEFS_RSP_FLAG_T rsp_flags,
                                  UINT32 rsp_length, void* rsp_pdata)
{
    DG_DEFS_DIAG_RSP_T rsp;
    DG_RSP_CODE_T      final_rsp_code   = rsp_code;
    DG_DEFS_RSP_FLAG_T final_rsp_flags  = rsp_flags;
    UINT32             final_rsp_length = rsp_length;
    void*              final_rsp_pdata  = rsp_pdata;

    /* If there is a a response data length but no response data pointer, report an error */
    if ((final_rsp_length > 0) && (final_rsp_pdata == NULL))
    {
        final_rsp_code   = DG_RSP_CODE_CMD_INTL_ERR;
        final_rsp_length = 0;
    }

    rsp.header.unsol_rsp_flag = (final_rsp_flags & DG_DEFS_RSP_FLAG_UNSOL) ?
                                DG_DEFS_HDR_FLAG_RESPONSE_UNSOLICITED : DG_DEFS_HDR_FLAG_RESPONSE_SOLICITED;

    rsp.header.diag_version = DG_DEFS_HDR_DIAG_VERSION_VALUE;
    rsp.header.seq_tag      = diag->header.seq_tag;
    rsp.header.opcode       = diag->header.opcode;
    rsp.header.rsp_code     = final_rsp_code;
    rsp.header.length       = final_rsp_length;

    DG_DBG_TRACE("Response Flags - unsol = %d, data = %d, fail = %d, seq = %d",
                 rsp.header.unsol_rsp_flag,
                 rsp.header.length > 0,
                 rsp.header.rsp_code != DG_RSP_CODE_CMD_RSP_GENERIC,
                 rsp.header.seq_tag);

    if (rsp.header.rsp_code != DG_RSP_CODE_CMD_RSP_GENERIC)
    {
        DG_PAL_DBG_dump_fs_log(rsp.header.opcode);
    }

    /* If the request is marked for deletion, then the response should be suppressed */
    pthread_mutex_lock(&(diag->delete_mutex));
    if (diag->can_delete == TRUE)
    {
        DG_DBG_ERROR("Error, tried to send DIAG response for a request marked to be deleted. "
                     "Setting header to indicate no response required");
        diag->header.no_rsp_reqd_flag = DG_DEFS_HDR_FLAG_RESPONSE_NOT_EXPECTED;
    }
    pthread_mutex_unlock(&(diag->delete_mutex));

    if (diag->header.no_rsp_reqd_flag == DG_DEFS_HDR_FLAG_RESPONSE_NOT_EXPECTED)
    {
        DG_DBG_TRACE("DIAG Header indicates no response needed");
    }
    else
    {
        rsp.data_ptr = (UINT8*)rsp_pdata;
        if (final_rsp_flags & DG_DEFS_RSP_FLAG_UNSOL)
        {
            /* For unsolicited responses, set sequence tag to zero since there is no matching
               request */
            rsp.header.seq_tag = 0;

            DG_CLIENT_COMM_send_rsp_to_all_clients(&rsp);
        }
        else
        {
            /* For solicited responses, send to client who originated the request */
            DG_DBG_TRACE("Sending res_code = 0x%02x, res_length = %d to sender_id = %d",
                         rsp.header.rsp_code, rsp.header.length, diag->sender_id);
            if (!DG_CLIENT_COMM_client_write(diag->sender_id, &rsp))
            {
                DG_DBG_ERROR("Send response to fd: %d failed", diag->sender_id);
            }
        }
    }
}

