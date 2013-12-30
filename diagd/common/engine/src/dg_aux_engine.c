/*==================================================================================================

    Module Name:  dg_aux_engine.c

    General Description: This file provides an interface to the next DIAG engine in the request
                         handling chain.

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
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include "dg_defs.h"
#include "dg_dbg.h"
#include "dg_aux_engine.h"
#include "dg_client_comm.h"
#include "dg_engine_util.h"
#include "dg_aux_util.h"
#include "dg_pal_aux_engine.h"

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                            LOCAL MACROS
==================================================================================================*/
#if !defined (DG_CFG_AUX_NUM)
    #error "Error, DG_CFG_AUX_NUM variable not set!"
#endif

#define DG_AUX_ENGINE_CLEANUP_PUSH(routine, arg) pthread_cleanup_push(routine, arg)
#define DG_AUX_ENGINE_CLEANUP_POP(execute) pthread_cleanup_pop(execute)

#define DG_AUX_ENGINE_EXPECT_RSP_DEFAULT  0xFFFF /**< Default value to initalize expected
                                                     response vars */
#define DG_AUX_ENGINE_CLEAN_POP_NO_EXEC   0      /* Value used to indicate function popped from cleanup
                                                stack should not be executed */

#define DG_AUX_ENGINE_OVERRIDE_DISABLED   -1

/*==================================================================================================
                             LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static DG_DEFS_STATUS_T    dg_aux_engine_send_req_to_aux(int aux_id, DG_DEFS_DIAG_REQ_T* diag);
static void*               dg_aux_engine_listener_thread(void* id);
static void                dg_aux_engine_listener_thread_exit_handler(int sig);
static DG_DEFS_DIAG_RSP_T* dg_aux_engine_read_aux_rsp(int aux_id);
static void                dg_aux_engine_handle_aux_cmd_impl(int                         aux_id,
                                                             DG_DEFS_DIAG_REQ_T*         diag,
                                                             DG_DEFS_DIAG_RSP_BUILDER_T* rsp,
                                                             UINT32                      time_out);
static void                dg_aux_engine_init_impl(int aux_id);
static void                dg_aux_engine_close_impl(int aux_id);
static int                 dg_aux_engine_handle_override(int aux_id);

/*==================================================================================================
                                      MODULE GLOBAL VARIABLES
==================================================================================================*/
static UINT16          dg_aux_engine_seq_tag        = 0;
static int             dg_aux_engine_override_id    = DG_AUX_ENGINE_OVERRIDE_DISABLED;
static pthread_mutex_t dg_aux_engine_override_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_t       dg_aux_engine_listener_thread_hndl[DG_CFG_AUX_NUM];
static BOOL            dg_aux_engine_listener_run_ctrl[DG_CFG_AUX_NUM];

/*==================================================================================================
                                          GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief This function will initialize the state of all aux engines to being unavailable
*//*==============================================================================================*/
void DG_AUX_ENGINE_init_available_state(void)
{
    int index;
    for (index = 0; index < DG_CFG_AUX_NUM; index++)
    {
        dg_aux_engine_listener_thread_hndl[index] = 0;
    }
}

/*=============================================================================================*//**
@brief This function will initialize everything needed to communicate with the aux DIAG engine

@param[in] aux_id - ID of the aux engine to init, use DG_AUX_ENGINE_AUX_ID_INIT_ALL to init all
*//*==============================================================================================*/
void DG_AUX_ENGINE_init(int aux_id)
{
    int index;

    if (aux_id >= DG_CFG_AUX_NUM)
    {
        DG_DBG_ERROR("Aux engine init error - invalid aux ID, aux_id = %d, max = %d",
                     aux_id, DG_CFG_AUX_NUM);
    }
    /* Init all aux engines */
    else if (aux_id == DG_AUX_ENGINE_AUX_ID_ALL)
    {
        for (index = 0; index < DG_CFG_AUX_NUM; index++)
        {
            dg_aux_engine_init_impl(index);
        }
    }
    else if (aux_id < 0)
    {
        DG_DBG_ERROR("Invalid aux id %d used", aux_id);
    }
    else
    {
        dg_aux_engine_init_impl(aux_id);
    }
}

/*=============================================================================================*//**
@brief This function will close everything needed to communicate with the aux DIAG engine

@param[in] aux_id - ID of the aux engine to init, use DG_AUX_ENGINE_AUX_ID_INIT_ALL to init all
*//*==============================================================================================*/
void DG_AUX_ENGINE_close(int aux_id)
{
    int index;

    if (aux_id >= DG_CFG_AUX_NUM)
    {
        DG_DBG_ERROR("Aux engine close error - invalid aux ID, aux_id = %d, max = %d",
                     aux_id, DG_CFG_AUX_NUM);
    }
    /* Init all aux engines */
    else if (aux_id == DG_AUX_ENGINE_AUX_ID_ALL)
    {
        for (index = 0; index < DG_CFG_AUX_NUM; index++)
        {
            dg_aux_engine_close_impl(index);
        }
    }
    else if (aux_id < 0)
    {
        DG_DBG_ERROR("Invalid aux id %d used", aux_id);
    }
    else
    {
        dg_aux_engine_close_impl(aux_id);
    }
}

/*=============================================================================================*//**
@brief This function will take a DIAG request, send it to the specified aux diag engine, and wait
       for its solicited response

@param[in]  aux_id - The aux engine to interact with
@param[in]  diag   - The DIAG request to send to the aux engine
@param[out] rsp    - The DIAG response, must be initialized already.

@note
  - The calling function is responsible for freeing the response.
  - This function will block until the aux engine response is received.  It is safe to cancel a
    thread with this function executing
  - Only 1 thread per aux engine may be using this function at once, this is controlled via mutex
    locks
  - this function is obsolete, please use DG_AUX_ENGINE_handle_aux_cmd_timeout()
*//*==============================================================================================*/
void DG_AUX_ENGINE_handle_aux_cmd(int aux_id, DG_DEFS_DIAG_REQ_T* diag, DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    int final_aux_id = dg_aux_engine_handle_override(aux_id);

    /* Ensure the aux ID is valid */
    if ((final_aux_id < 0) || (final_aux_id >= DG_CFG_AUX_NUM))
    {
        DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_RSP_MUX_ERR,
                                            "Invalid aux id %d used", final_aux_id);
    }
    else
    {
        dg_aux_engine_handle_aux_cmd_impl(final_aux_id, diag, rsp, 0);
    }
}

/*=============================================================================================*//**
@brief This function will take a DIAG request, send it to the specified aux diag engine, and wait
       for its solicited response

@param[in]  aux_id   - The aux engine to interact with
@param[in]  diag     - The DIAG request to send to the aux engine
@param[out] rsp      - The DIAG response, must be initialized already.
@param[in]  time_out -  the time out value (in seconds) to get a response, 0 means wait forever

@note
  - The calling function is responsible for freeing the response.
  - This function will block until the aux engine response is received.  It is safe to cancel a
    thread with this function executing
  - Only 1 thread per aux engine may be using this function at once, this is controlled via mutex
    locks
*//*==============================================================================================*/
void DG_AUX_ENGINE_handle_aux_cmd_timeout(int                         aux_id,
                                          DG_DEFS_DIAG_REQ_T*         diag,
                                          DG_DEFS_DIAG_RSP_BUILDER_T* rsp,
                                          UINT32                      time_out)
{
    int final_aux_id = dg_aux_engine_handle_override(aux_id);

    /* Ensure the aux ID is valid */
    if ((final_aux_id < 0) || (final_aux_id >= DG_CFG_AUX_NUM))
    {
        DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_RSP_MUX_ERR,
                                            "Invalid aux id %d used", final_aux_id);
    }
    else
    {
        dg_aux_engine_handle_aux_cmd_impl(final_aux_id, diag, rsp, time_out);
    }
}

/*=============================================================================================*//**
@brief Behaves the same as  DG_AUX_ENGINE_handle_aux_cmd_timeout(), but handles disabled aux engines

@param[in]  aux_id     - The aux engine to interact with
@param[in]  diag       - The DIAG request to send to the aux engine
@param[out] rsp        - The DIAG response, must be initialized already.
@param[in]  time_out   - The time out value (in seconds) to get a response, 0 means wait forever
@param[out] is_enabled - If the aux engine is enabled (TRUE), or disabled (FALSE)

@note
  - This should only be used in cases where the execution of a request by an aux engine is
    considered to be optional.  This should not be used in cases where its critical that the command
    is handled by the aux engine!
  - If the requested aux engine is disabled, the 'rsp' will not be modified and 'is_enabled' will be
    set to false.
  - The calling function is responsible for freeing the response.
  - This function will block until the aux engine response is received.  It is safe to cancel a
    thread with this function executing
  - Only 1 thread per aux engine may be using this function at once, this is controlled via mutex
    locks
*//*==============================================================================================*/
void DG_AUX_ENGINE_handle_aux_cmd_timeout_opt(int                         aux_id,
                                              DG_DEFS_DIAG_REQ_T*         diag,
                                              DG_DEFS_DIAG_RSP_BUILDER_T* rsp,
                                              UINT32                      time_out,
                                              BOOL*                       is_enabled)
{
    int final_aux_id = dg_aux_engine_handle_override(aux_id);

    /* Ensure the aux ID is valid */
    if ((final_aux_id < 0) || (final_aux_id >= DG_CFG_AUX_NUM))
    {
        DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_RSP_MUX_ERR,
                                            "Invalid aux id %d used", final_aux_id);
        *is_enabled = FALSE;
    }
    else if (!DG_AUX_ENGINE_is_aux_enabled(final_aux_id))
    {
        *is_enabled = FALSE;
        DG_DBG_TRACE("Aux engine %d is not available, can not send request", final_aux_id);
    }
    else
    {
        *is_enabled = TRUE;
        dg_aux_engine_handle_aux_cmd_impl(final_aux_id, diag, rsp, time_out);
    }
}

/*=============================================================================================*//**
@brief This function will create a DIAG request (based upon a provided opcode and request data)
       and send it to the specified aux diag engine.

@param[in]  aux_id       - The aux engine to interact with
@param[in]  opcode       - Opcode of the DIAG to send
@param[in]  req_data_len - Length of request data of the diag to send
@param[in]  req_data_ptr - Request data of the diag to send
@param[out] rsp          - DIAG Response, must be already initialized.

@note
  - Calling function is responsible for freeing DIAG response
*//*==============================================================================================*/
void DG_AUX_ENGINE_create_and_send_aux_diag(int aux_id,
                                            DG_DEFS_OPCODE_T opcode, UINT32 req_data_len,
                                            UINT8* req_data_ptr, DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    DG_DEFS_DIAG_REQ_T diag_req;

    memset(&diag_req, 0, sizeof(diag_req));

    /* Build up the DIAG request */
    diag_req.header.seq_tag          = dg_aux_engine_seq_tag++;
    diag_req.header.opcode           = opcode;
    diag_req.header.no_rsp_reqd_flag = DG_DEFS_HDR_FLAG_RESPONSE_EXPECTED;
    diag_req.header.length           = req_data_len;
    diag_req.data_ptr                = req_data_ptr;

    /* Send the command and receive the response */
    DG_AUX_ENGINE_handle_aux_cmd(aux_id, &diag_req, rsp);
}

/*=============================================================================================*//**
@brief Behaves the same as DG_AUX_ENGINE_create_and_send_aux_diag(), but handles disabled aux
       engines

@param[in]  aux_id       - The aux engine to interact with
@param[in]  opcode       - Opcode of the DIAG to send
@param[in]  req_data_len - Length of request data of the diag to send
@param[in]  req_data_ptr - Request data of the diag to send
@param[out] rsp          - DIAG Response, must be already initialized.
@param[out] is_enabled   - If the aux engine is enabled (TRUE), or disabled (FALSE)

@note
  - This should only be used in cases where the execution of a request by an aux engine is
    considered to be optional.  This should not be used in cases where its critical that the command
    is handled by the aux engine!
  - If the requested aux engine is disabled, the 'rsp' will not be modified and 'is_enabled' will be
    set to false.
  - Calling function is responsible for freeing DIAG response
*//*==============================================================================================*/
void DG_AUX_ENGINE_create_and_send_aux_diag_opt(int aux_id, DG_DEFS_OPCODE_T opcode,
                                                UINT32 req_data_len,
                                                UINT8* req_data_ptr,
                                                DG_DEFS_DIAG_RSP_BUILDER_T* rsp, BOOL* is_enabled)
{
    int final_aux_id = dg_aux_engine_handle_override(aux_id);

    /* Ensure the aux ID is valid */
    if ((final_aux_id < 0) || (final_aux_id >= DG_CFG_AUX_NUM))
    {
        DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_RSP_MUX_ERR,
                                            "Invalid aux id %d used", final_aux_id);
        *is_enabled = FALSE;
    }
    else if (!DG_AUX_ENGINE_is_aux_enabled(final_aux_id))
    {
        *is_enabled = FALSE;
        DG_DBG_TRACE("Aux engine %d is not available, can not send opcode 0x%04x", final_aux_id, opcode);
    }
    else
    {
        *is_enabled = TRUE;
        DG_AUX_ENGINE_create_and_send_aux_diag(final_aux_id, opcode, req_data_len, req_data_ptr, rsp);
    }
}

/*=============================================================================================*//**
@brief Enables or disables the specified aux engine

@param[in] aux_id     - The aux engine to interact with
@param[in] set_enable - TRUE = enable, FALSE = disable

@return TRUE if successful

@note
  - Should only be used for debugging/bring-up purposes.
  - Allows aux engines to be enabled/disabled at run-time.
*//*==============================================================================================*/
BOOL DG_AUX_ENGINE_enable_aux(int aux_id, BOOL set_enable)
{
    DG_DBG_TRACE("Set aux_id %d enable status to %d", aux_id, set_enable);
    BOOL is_success = TRUE;

    /* Ensure the aux ID is valid */
    if ((aux_id < 0) || (aux_id >= DG_CFG_AUX_NUM))
    {
        is_success = FALSE;
        DG_DBG_ERROR("Invalid aux id %d used", aux_id);
    }
    else
    {
        if (set_enable == TRUE)
        {
            DG_AUX_ENGINE_init(aux_id);
        }
        else
        {
            DG_AUX_ENGINE_close(aux_id);
        }
    }

    return is_success;
}

/*=============================================================================================*//**
@brief Query if the specified aux engine is enabled

@param[in] aux_id - The aux engine to interact with

@return TRUE if the aux engine is ready to use

@note
*//*==============================================================================================*/
BOOL DG_AUX_ENGINE_is_aux_enabled(int aux_id)
{
    BOOL is_enalbed = TRUE;

    /* Ensure the aux ID is valid */
    if ((aux_id < 0) || (aux_id >= DG_CFG_AUX_NUM))
    {
        DG_DBG_ERROR("Invalid aux id %d queried", aux_id);
        is_enalbed = FALSE;
    }
    else
    {
        DG_AUX_ENGINE_init(aux_id);

        if (dg_aux_engine_listener_thread_hndl[aux_id] == 0)
        {
            is_enalbed = FALSE;
        }
    }

    return is_enalbed;
}

/*=============================================================================================*//**
@brief Enables or disables aux engine overide

@param[in] enable     - If the override should be enabled
@param[in] aux_id     - The aux engine to use for the override

@return TRUE if successful

@note
 - If enabled, will cause all commands being sent to ANY aux engine to be routed to the specified
   aux engine
*//*==============================================================================================*/
BOOL DG_AUX_ENGINE_override_aux(BOOL enable, int aux_id)
{
    BOOL is_success = FALSE;

    DG_AUX_ENGINE_CLEANUP_PUSH(DG_ENGINE_UTIL_generic_cleanup_unlock,
                               (void*)&dg_aux_engine_override_mutex);
    pthread_mutex_lock(&dg_aux_engine_override_mutex);

    if (enable == TRUE)
    {
        if ((aux_id < 0) || (aux_id >= DG_CFG_AUX_NUM))
        {
            DG_DBG_ERROR("Aux id %d invalid for override", aux_id);
        }
        else
        {
            dg_aux_engine_override_id = aux_id;
            is_success                = TRUE;
        }
    }
    else
    {
        dg_aux_engine_override_id = DG_AUX_ENGINE_OVERRIDE_DISABLED;
        is_success                = TRUE;
    }

    pthread_mutex_unlock(&dg_aux_engine_override_mutex);
    DG_AUX_ENGINE_CLEANUP_POP(DG_AUX_ENGINE_CLEAN_POP_NO_EXEC);

    return is_success;
}

/*=============================================================================================*//**
@brief Query how many aux engine enabled (ready to use)

@param[out] first_enabled_aux - The first enabled aux engine (could be NULL if we don't care)

@return number of enabled aux engine

@note
*//*==============================================================================================*/
int DG_AUX_ENGINE_get_total_enabled_aux(int* first_enabled_aux)
{
    int count = 0;
    int index = DG_CFG_AUX_NUM - 1;

    while (index >= 0)
    {
        if (DG_AUX_ENGINE_is_aux_enabled(index))
        {
            count++;
            if (first_enabled_aux != NULL)
            {
                *first_enabled_aux = index;
            }
        }

        index--;
    }

    return count;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief This function will take a DIAG request, send it to the specified aux diag engine, and wait
       for its solicited response

@param[in]  aux_id   - The aux engine to interact with
@param[in]  diag     - The DIAG request to send to the aux engine
@param[out] rsp      - The DIAG response, must be initialized already.
@param[in]  time_out - The time out value (in seconds) to get a response, 0 means wait forever

@note
  - The calling function is responsible for freeing the response.
  - This function will block until the aux engine response is received.  It is safe to cancel a
    thread with this function executing
  - Only 1 thread per aux engine may be using this function at once, this is controlled via mutex
    locks
*//*==============================================================================================*/
void dg_aux_engine_handle_aux_cmd_impl(int                         aux_id,
                                       DG_DEFS_DIAG_REQ_T*         diag,
                                       DG_DEFS_DIAG_RSP_BUILDER_T* rsp,
                                       UINT32                      time_out)
{
    DG_DEFS_DIAG_RSP_T* resp = NULL;
    DG_DEFS_STATUS_T    send_status;
    DG_DEFS_OPCODE_T    opcode;
    UINT16              seq_tag;

    if (!DG_AUX_ENGINE_is_aux_enabled(aux_id))
    {
        /* Report error in this case */
        DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_RSP_MUX_ERR,
                                            "Aux DIAG engine %d not available", aux_id);
    }
    else if (diag != NULL)
    {
        DG_DBG_TRACE("Handle command opcode = 0x%04x on aux engine %d",
                     diag->header.opcode, aux_id);

        opcode  = diag->header.opcode;
        seq_tag = diag->header.seq_tag;

        /* If a response is required, push the req to the bp req list */
        if (diag->header.no_rsp_reqd_flag == DG_DEFS_HDR_FLAG_RESPONSE_EXPECTED)
        {
            DG_AUX_UTIL_set_bp_req(aux_id, opcode, seq_tag);
        }

        if ((send_status = dg_aux_engine_send_req_to_aux(aux_id, diag)) != DG_DEFS_STATUS_SUCCESS)
        {
            DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_RSP_MUX_ERR,
                                                "Failed to send request to aux diag engine %d, status = %d",
                                                aux_id, send_status);
        }
        else if (diag->header.no_rsp_reqd_flag != DG_DEFS_HDR_FLAG_RESPONSE_EXPECTED)
        {
            /* No response is required, set current one to success */
            DG_DBG_TRACE("No response required.");
            DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
        }
        else
        {
            /* Response is required, wait for the response */
            resp = DG_AUX_UTIL_get_bp_rsp(aux_id, opcode, seq_tag, time_out);
            if (resp == NULL)
            {
                DG_DBG_ERROR("the BP response is time out or just canceled by father thread");
                DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_RSP_TIMEOUT,
                                                    "Aux DIAG engine %d wait response time out",
                                                    aux_id);
            }
            else
            {
                DG_DBG_TRACE("Successfully received the expected response on aux engine %d",
                             aux_id);

                /* Take DIAG response and convert it to a response builder */
                if (resp->header.unsol_rsp_flag)
                {
                    DG_ENGINE_UTIL_rsp_set_flag(rsp, DG_DEFS_RSP_FLAG_UNSOL);
                }

                /* Copy response data into response builder */
                if ((resp->header.length > 0) &&
                    (DG_ENGINE_UTIL_rsp_data_alloc(rsp, resp->header.length) != TRUE))
                {
                    DG_DBG_ERROR("Unable to allocate %d for response", resp->header.length);
                }
                else
                {
                    DG_ENGINE_UTIL_rsp_set_code(rsp, (DG_RSP_CODE_T)resp->header.rsp_code);
                    DG_ENGINE_UTIL_rsp_append_buf(rsp, resp->data_ptr, resp->header.length);
                }

                DG_ENGINE_UTIL_free_diag_rsp(resp);
            }
        }
    }
}

/*=============================================================================================*//**
@brief This function will take a DIAG request and send it to the specified aux engine.

@param[in] aux_id - The aux engine to interact with
@param[in] diag   - The DIAG request to send to the aux engine

@return Status

@note
  - This function assumes the aux engine interface has properly been opened
*//*==============================================================================================*/
DG_DEFS_STATUS_T dg_aux_engine_send_req_to_aux(int aux_id, DG_DEFS_DIAG_REQ_T* diag)
{
    DG_DEFS_STATUS_T        retval     = DG_DEFS_STATUS_GEN_ERROR;
    int                     write_len  = 0;
    UINT8*                  write_buff = NULL;
    DG_DEFS_DIAG_REQ_HDR_T* cmd_header = NULL;

    cmd_header = &(diag->header);
    DG_DBG_TRACE("DIAG opcode = 0x%04x, length = %d", cmd_header->opcode, cmd_header->length);

    /* Determine size of complete request to write */
    write_len  = cmd_header->length + sizeof(DG_DEFS_DIAG_REQ_HDR_T);
    if ((write_buff = (UINT8*)malloc(write_len)) == NULL)
    {
        DG_DBG_ERROR("Out of memory - malloc failed on write_buff, length = %d", write_len);
        retval = DG_DEFS_STATUS_MALLOC_FAIL;
    }
    else
    {
        /* Convert endianess of DIAG header */
        DG_ENGINE_UTIL_hdr_req_hton(cmd_header, (DG_DEFS_DIAG_REQ_HDR_T*)write_buff);
        if (cmd_header->length > 0)
        {
            memcpy(write_buff + sizeof(*cmd_header), diag->data_ptr, cmd_header->length);
        }

        DG_DBG_TRACE("Sending %d bytes to aux engine %d: ", write_len, aux_id);
        DG_DBG_DUMP(write_buff, write_len);

        if (DG_PAL_AUX_ENGINE_write(aux_id, write_len, write_buff) != TRUE)
        {
            DG_DBG_ERROR("Write data to aux engine %d failed!", aux_id);
        }
        else
        {
            DG_DBG_TRACE("Transferred %d byte(s) DIAG opcode = 0x%04x to aux engine %d succeeded.",
                         write_len, cmd_header->opcode, aux_id);
            retval = DG_DEFS_STATUS_SUCCESS;
        }

        free(write_buff);
    }

    return retval;
}

/*=============================================================================================*//**
@brief This function will constantly attempt to read aux engine responses.

@param[in] id - Aux engine ID this thread is for, id is an int typecasted as void *

@return NULL - Not used

@note
  - This function is meant to run as its own thread.
  - This function will never return
  - All unsolicited responses are sent to all connected clients
  - Solicited Responses will be handled as follows:
    - If an expected solicited response is received, the awaiting handler thread is notified
    - If an expected solicited response is received, and there is already an older, unconsumed
      expected solicited response, the older response is deleted.
    - If a solicited response is received, but no handler expects it, the response is deleted.
*//*==============================================================================================*/
void* dg_aux_engine_listener_thread(void* id)
{
    DG_DEFS_DIAG_RSP_T* resp;
    int                 aux_id        = *(int*)(&id);
    int                 failure_count = 0;
    DG_DEFS_OPCODE_T    opcode;
    UINT8               seq_tag;

    struct sigaction    actions;
    memset(&actions, 0, sizeof(actions));
    sigemptyset(&actions.sa_mask);
    actions.sa_flags   = 0;
    actions.sa_handler = dg_aux_engine_listener_thread_exit_handler;
    sigaction(SIGUSR1, &actions, NULL);

    DG_DBG_TRACE("AUX Thread[%d] - Listener started", aux_id);
    /* Loop forever */
    while (dg_aux_engine_listener_run_ctrl[aux_id])
    {
        /* Attempt to read an aux engine response */
        resp = dg_aux_engine_read_aux_rsp(aux_id);
        if (resp == NULL)
        {
            DG_DBG_ERROR("AUX Thread[%d] - Failure to read aux engine", aux_id);
            failure_count++;
        }
        /* If this is an unsolicited response, send response to all connected clients */
        else if (resp->header.unsol_rsp_flag)
        {
            failure_count = 0;
            DG_CLIENT_COMM_send_rsp_to_all_clients(resp);
            DG_ENGINE_UTIL_free_diag_rsp(resp);
        }
        else
        {
            failure_count = 0;

            seq_tag       = resp->header.seq_tag;
            opcode        = resp->header.opcode;

            if (!DG_AUX_UTIL_set_bp_rsp(aux_id, opcode, seq_tag, resp))
            {
                /* We received an unexpected solicited aux engine response, log it and delete it */
                DG_DBG_ERROR("AUX Thread[%d] - Received unexpected solicited aux engine response! "
                             "opcode = 0x%04x. seq_tag = 0x%02x", aux_id, opcode, seq_tag);
                DG_ENGINE_UTIL_free_diag_rsp(resp);
            }
        }

        if (failure_count > 10)   /* give up after 10 times retry and reset the connection */
        {
            DG_DBG_TRACE("RESET AUX ID %d", aux_id);
            /* when next time send a bp req, the aux engine would be re-initialized */
            dg_aux_engine_listener_thread_hndl[aux_id] = 0;
            dg_aux_engine_listener_run_ctrl[aux_id]    = FALSE;
            DG_PAL_AUX_ENGINE_close(aux_id);
            break;
        }
    }

    DG_DBG_TRACE("AUX Thread[%d] - Listener stopped", aux_id);
    return NULL;
}

/*=============================================================================================*//**
@brief This function handle the aux listen thread signal

@param[in] sig - The signal

@note
  - This function is a way to implement pthread_cancel()
  - usually the signal will force the block read() return -1 and errno = EINTR
  - so we got a chance to exit the listener thread
*//*==============================================================================================*/
void dg_aux_engine_listener_thread_exit_handler(int sig)
{
    int aux_id;
    DG_DBG_TRACE("AUX listener thead got signaled: sig = %d", sig);
    for (aux_id = 0; aux_id < DG_CFG_AUX_NUM; aux_id++)
    {
        if ((dg_aux_engine_listener_thread_hndl[aux_id] != 0) &&
            (dg_aux_engine_listener_run_ctrl[aux_id] == FALSE))
        {
            /* Call PAL to close the aux engine */
            DG_PAL_AUX_ENGINE_close(aux_id);
        }
    }
}

/*=============================================================================================*//**
@brief This function will read a response from the specified aux engine

@param[in] aux_id - The aux engine to interact with

@return Pointer to the response, NULL in case of error

@note
  - This function will block until the specified aux engine response is read or an error occurs.
  - Functions calling this function must free the response using DG_ENGINE_UTIL_free_diag_rsp()
*//*==============================================================================================*/
DG_DEFS_DIAG_RSP_T* dg_aux_engine_read_aux_rsp(int aux_id)
{
    UINT32                 data_length;
    BOOL                   is_success = FALSE;
    DG_DEFS_DIAG_RSP_T*    resp       = NULL;
    DG_DEFS_DIAG_RSP_HDR_T dg_rsp_hdr;

    /* Allocate memory for DIAG response */
    if ((resp = (DG_DEFS_DIAG_RSP_T*)malloc(sizeof(DG_DEFS_DIAG_RSP_T))) == NULL)
    {
        DG_DBG_ERROR("Out of memory - malloc failed on diag");
    }
    else
    {
        resp->data_ptr = NULL;

        DG_DBG_TRACE("Attempt to read aux engine %d response header", aux_id);
        if (DG_PAL_AUX_ENGINE_read(aux_id, sizeof(dg_rsp_hdr), (UINT8*)&dg_rsp_hdr) != TRUE)
        {
            DG_DBG_ERROR("Reading header on aux engine %d failed!", aux_id);
        }
        else
        {
            /* Network byte order to host byte order... */
            DG_ENGINE_UTIL_hdr_rsp_ntoh(&dg_rsp_hdr, &(resp->header));
            DG_DBG_TRACE("Successfully read DIAG header on aux engine %d. DIAG opcode = 0x%04x, length = %d: ",
                         aux_id, resp->header.opcode, resp->header.length);
            DG_DBG_DUMP(&dg_rsp_hdr, sizeof(dg_rsp_hdr));

            /* If there is response data, read it as well */
            data_length = resp->header.length;
            if (data_length > 0)
            {
                if ((resp->data_ptr = (UINT8*)malloc(data_length)) == NULL)
                {
                    DG_DBG_ERROR("Out of memory - malloc failed on data pointer");
                }
                else
                {
                    if (DG_PAL_AUX_ENGINE_read(aux_id, data_length, resp->data_ptr) != TRUE)
                    {
                        DG_DBG_ERROR("Read response data on aux engine %d failed!", aux_id);
                    }
                    else
                    {
                        DG_DBG_TRACE("Read response data on aux engine %d success, data length = %d: ",
                                     aux_id, data_length);
                        DG_DBG_DUMP(resp->data_ptr, data_length);
                        is_success = TRUE;
                    }
                }
            }
            else
            {
                /* There is no response data, indicate we got header successfully */
                is_success = TRUE;
            }
        }
    }

    if (is_success == FALSE)
    {
        DG_ENGINE_UTIL_free_diag_rsp(resp);
        resp = NULL;
    }

    return resp;
}

/*=============================================================================================*//**
@brief Initializes the specified aux engine for use

@param[in] aux_id  - The aux engine to interact with
*//*==============================================================================================*/
void dg_aux_engine_init_impl(int aux_id)
{
    DG_PAL_AUX_ENGINE_INIT_T aux_status;

    /* Make sure the aux engine hasn't be initialized already */
    if (dg_aux_engine_listener_thread_hndl[aux_id] != 0)
    {
        DG_DBG_TRACE("Aux engine %d already initialized", aux_id);
    }
    else
    {
        /* Call PAL to init the aux engine */
        aux_status = DG_PAL_AUX_ENGINE_init(aux_id);
        if (aux_status == DG_PAL_AUX_ENGINE_INIT_NOT_PRESENT)
        {
            DG_DBG_TRACE("Aux engine %d is not present, skipping aux engine setup", aux_id);
        }
        else if (aux_status != DG_PAL_AUX_ENGINE_INIT_SUCCESS)
        {
            DG_DBG_ERROR("Failed to init the aux engine %d! aux_status = %d", aux_id, aux_status);
        }
        else
        {
            dg_aux_engine_listener_run_ctrl[aux_id] = TRUE;
            /* Create a new thread to receive responses from aux diag engine. */
            if (!(pthread_create(&dg_aux_engine_listener_thread_hndl[aux_id], NULL,
                                 dg_aux_engine_listener_thread, (void*)(intptr_t)aux_id)))
            {
                DG_DBG_TRACE("Successfully init aux engine %d interface!", aux_id);
            }
            else
            {
                dg_aux_engine_listener_thread_hndl[aux_id] = 0;
                dg_aux_engine_listener_run_ctrl[aux_id]    = FALSE;
                DG_DBG_ERROR("Aux engine %d listener thread failed to start, errno = %d (%s)",
                             aux_id, errno, strerror(errno));
            }
        }
    }
}

/*=============================================================================================*//**
@brief Close the specified aux engine for use

@param[in] aux_id  - The aux engine to interact with
*//*==============================================================================================*/
void dg_aux_engine_close_impl(int aux_id)
{
    /* Make sure the aux engine has been initialized already */
    if (dg_aux_engine_listener_thread_hndl[aux_id] == 0)
    {
        DG_DBG_TRACE("Aux engine %d already closed", aux_id);
    }
    else
    {
        DG_DBG_TRACE("Trying to close aux engine %d ", aux_id);
        /* tell the aux listen thread to stop */
        dg_aux_engine_listener_run_ctrl[aux_id] = FALSE;

        /* signal the thread so that the read could break */
        pthread_kill(dg_aux_engine_listener_thread_hndl[aux_id], SIGUSR1);

        /* wait the aux listen thread to finish */
        pthread_join(dg_aux_engine_listener_thread_hndl[aux_id], NULL);

        DG_DBG_TRACE("Successfully shut down aux engine %d listener thread!", aux_id);
        dg_aux_engine_listener_thread_hndl[aux_id] = 0;
    }
}

/*=============================================================================================*//**
@brief Updates the given aux engine id to account for aux override

@param[in] aux_id - The aux engine to interact with

@return The new aux id to use

@note
 - If aux override is disabled, the input parameter is returned.
*//*==============================================================================================*/
int dg_aux_engine_handle_override(int aux_id)
{
    int new_aux_id = aux_id; /* Default to given aux id */

    DG_AUX_ENGINE_CLEANUP_PUSH(DG_ENGINE_UTIL_generic_cleanup_unlock,
                               (void*)&dg_aux_engine_override_mutex);
    pthread_mutex_lock(&dg_aux_engine_override_mutex);

    if (dg_aux_engine_override_id != DG_AUX_ENGINE_OVERRIDE_DISABLED)
    {
        DG_DBG_TRACE("Aux ID overridden from %d to %d", aux_id, dg_aux_engine_override_id);
        new_aux_id = dg_aux_engine_override_id;
    }
    pthread_mutex_unlock(&dg_aux_engine_override_mutex);
    DG_AUX_ENGINE_CLEANUP_POP(DG_AUX_ENGINE_CLEAN_POP_NO_EXEC);

    return new_aux_id;
}

