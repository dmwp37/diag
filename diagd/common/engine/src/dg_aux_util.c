/*==================================================================================================

    Module Name:  dg_aux_util.c

    General Description: This file provides utility functions for manage the AUX request.

====================================================================================================

Revision History:
                            Modification     Tracking
Author                          Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
Xudong Huang    - xudongh    2013/12/11     xxxxx-0000   Creation

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include "dg_defs.h"
#include "dg_dbg.h"
#include "dg_aux_engine.h"
#include "dg_aux_util.h"
#include "dg_client_comm.h"
#include "dg_engine_util.h"
#include "dg_pal_aux_engine.h"

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                            LOCAL MACROS
==================================================================================================*/
#define DG_AUX_UTIL_CLEANUP_PUSH(routine, arg) pthread_cleanup_push(routine, arg)
#define DG_AUX_UTIL_CLEANUP_POP(execute) pthread_cleanup_pop(execute)


/*==================================================================================================
                             LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
typedef struct
{
    UINT8               aux_id;
    UINT8               seq_tag;
    DG_DEFS_OPCODE_T    opcode;
    pthread_t           pid;
    sem_t               sem;
    DG_DEFS_DIAG_RSP_T* rsp;
} DG_AUX_UTIL_BP_REQ_T;

struct DG_AUX_UTIL_BP_REQ_NODE_T
{
    DG_AUX_UTIL_BP_REQ_T*             req;
    struct DG_AUX_UTIL_BP_REQ_NODE_T* next;
    struct DG_AUX_UTIL_BP_REQ_NODE_T* prev;
};

typedef struct DG_AUX_UTIL_BP_REQ_NODE_T DG_AUX_UTIL_BP_REQ_LIST_T;

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static DG_AUX_UTIL_BP_REQ_T* dg_aux_util_bp_req_malloc();
static void dg_aux_util_bp_req_free(DG_AUX_UTIL_BP_REQ_T* req);

static void dg_aux_util_wait_bp_rsp(DG_AUX_UTIL_BP_REQ_T* req, UINT32 time_out);
static void dg_aux_util_signal_bp_req(DG_AUX_UTIL_BP_REQ_T* req);

static BOOL dg_aux_util_bp_req_list_add(UINT8 aux_id, DG_DEFS_OPCODE_T opcode, UINT8 seq_tag);
static void dg_aux_util_bp_req_list_del(DG_AUX_UTIL_BP_REQ_LIST_T* node);

static DG_AUX_UTIL_BP_REQ_LIST_T* dg_aux_util_bp_req_list_find(UINT8            aux_id,
                                                               DG_DEFS_OPCODE_T opcode,
                                                               UINT8            seq_tag);
static DG_AUX_UTIL_BP_REQ_LIST_T* dg_aux_util_bp_req_list_find_pid(pthread_t pid);

/*==================================================================================================
                                      MODULE GLOBAL VARIABLES
==================================================================================================*/
static pthread_mutex_t           dg_aux_util_req_list_mutex = PTHREAD_MUTEX_INITIALIZER;
static DG_AUX_UTIL_BP_REQ_LIST_T dg_aux_util_req_list_head  =
{
    NULL,
    &dg_aux_util_req_list_head,
    &dg_aux_util_req_list_head
};

/*==================================================================================================
                                          GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Set a BP request into the list to indicate this request needs a response from BP

@param[in] aux_id  - The aux engine for the req
@param[in] opcode  - req opcode
@param[in] seq_tag - req seq_tag

@note
  - the work thread use this API when the BP request need a solicited response
  - the function add one bp request into the list
*//*==============================================================================================*/
void DG_AUX_UTIL_set_bp_req(UINT8 aux_id, DG_DEFS_OPCODE_T opcode, UINT8 seq_tag)
{
    if (!dg_aux_util_bp_req_list_add(aux_id, opcode, seq_tag))
    {
        DG_DBG_ERROR("Failed to add bp req to list, aux_id = %d, opcode = 0x%04x, seq_tag = 0x%02x",
                     aux_id, opcode, seq_tag);
    }
}

/*=============================================================================================*//**
@brief Get the solicited bp response for the expected request

@param[in] aux_id   - The aux engine for the req
@param[in] opcode   - req opcode
@param[in] seq_tag  - req seq_tag
@param[in] time_out - max timeout value to block

@return the expected solicited response or NULL if error

@note
  - the work thread use this API when the BP request need a solicited response
  - the function seeks the request from the BP req list
  - the function will block until the response comes back or timeout happened
*//*==============================================================================================*/
DG_DEFS_DIAG_RSP_T* DG_AUX_UTIL_get_bp_rsp(UINT8 aux_id, DG_DEFS_OPCODE_T opcode, UINT8 seq_tag,
                                           UINT32 time_out)
{
    DG_DEFS_DIAG_RSP_T*        ret  = NULL;
    DG_AUX_UTIL_BP_REQ_LIST_T* iter = dg_aux_util_bp_req_list_find(aux_id, opcode, seq_tag);
    if (iter != NULL)
    {
        dg_aux_util_wait_bp_rsp(iter->req, time_out);
        ret = iter->req->rsp;
        dg_aux_util_bp_req_list_del(iter);
    }
    else
    {
        DG_DBG_ERROR("Can't find BP req to get rsp: aux_id = %d, opcode = 0x%04x, seq_tag = 0x%02x",
                     aux_id, opcode, seq_tag);
    }

    return ret;
}

/*=============================================================================================*//**
@brief Set the solicited response for the expected request

@param[in] aux_id  - The aux engine for the req
@param[in] opcode  - req opcode
@param[in] seq_tag - req seq_tag
@param[in] rsp     - the solicited bp response

@return TRUE if success

@note
  - the BP listener thread use this API when a solicited response comes
  - this function will tell the work thread that the expected response is ready and let it go
*//*==============================================================================================*/
BOOL DG_AUX_UTIL_set_bp_rsp(UINT8 aux_id, DG_DEFS_OPCODE_T opcode, UINT8 seq_tag,
                            DG_DEFS_DIAG_RSP_T* rsp)
{
    BOOL                       ret  = FALSE;

    DG_AUX_UTIL_BP_REQ_LIST_T* iter = dg_aux_util_bp_req_list_find(aux_id, opcode, seq_tag);
    if (iter != NULL)
    {
        iter->req->rsp = rsp;
        dg_aux_util_signal_bp_req(iter->req);
        ret            = TRUE;
    }
    else
    {
        DG_DBG_ERROR("Can't find BP req to set rsp: aux_id = %d, opcode = 0x%04x, seq_tag = 0x%02x",
                     aux_id, opcode, seq_tag);
    }

    return ret;
}

/*=============================================================================================*//**
@brief Cancel the BP request for specified thread

@param[in] pid  - the specified thread ID

@note
  - the monitor thread use this API to cancel it's bp req
  - when timeout happens the engine should call this function
  - this function will tell the work thread that the expected response is ready and let it go
*//*==============================================================================================*/
void DG_AUX_UTIL_cancel_thread_bp_req(pthread_t pid)
{
    /* just signal the request and let the consumer to return
       the consumer will delete the bp req from list */
    DG_AUX_UTIL_BP_REQ_LIST_T* iter = dg_aux_util_bp_req_list_find_pid(pid);
    if (iter != NULL)
    {
        dg_aux_util_signal_bp_req(iter->req);
    }
}


/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Malloc a solicited bp request

@return the pointer of the bp req
*//*==============================================================================================*/
DG_AUX_UTIL_BP_REQ_T* dg_aux_util_bp_req_malloc()
{
    DG_AUX_UTIL_BP_REQ_T* req = (DG_AUX_UTIL_BP_REQ_T*)malloc(sizeof(DG_AUX_UTIL_BP_REQ_T));
    if (req == NULL)
    {
        DG_DBG_ERROR("Out of memory - failed to malloc DG_AUX_UTIL_BP_REQ_T");
    }
    else if (sem_init(&req->sem, 0, 0) != 0)
    {
        DG_DBG_ERROR("Failed to init semphore for BP req, errno = %d (%s)",
                     errno, strerror(errno));
        free(req);
        req = NULL;
    }
    else
    {
        req->pid = pthread_self();
        req->rsp = NULL;
    }

    return req;
}

/*=============================================================================================*//**
@brief Free a solicited bp request

@param[in] req - the pointer of the bp req
*//*==============================================================================================*/
void dg_aux_util_bp_req_free(DG_AUX_UTIL_BP_REQ_T* req)
{
    if (req != NULL)
    {
        sem_destroy(&req->sem);
        free(req);
    }
}


/*=============================================================================================*//**
@brief Wait the solicited response for the request

@param[in] req     - the pointer of the bp req
@param[in] timeout - timeout value to wait

@note
 - the pthread condition is not stable enough, use semaphore instead
*//*==============================================================================================*/
void dg_aux_util_wait_bp_rsp(DG_AUX_UTIL_BP_REQ_T* req, UINT32 time_out)
{
    int             status = 0;
    struct timeval  time_of_day;
    struct timespec timeout_time;

    DG_DBG_TRACE("Waiting BP rsp: aux_id = %d, opcode = 0x%04x, seq_tag = 0x%02x",
                 req->aux_id, req->opcode, req->seq_tag);

    DG_DBG_TRACE("Waiting semphore = %p", &req->sem);

    if (time_out != 0)
    {
        if (gettimeofday(&time_of_day, NULL) != 0)
        {
            DG_DBG_ERROR("Failed to get time of day, errno = %d (%s)", errno, strerror(errno));
            DG_DBG_TRACE("Waiting BP rsp: Change to INFINIT wait");
            time_out = 0;
        }
        else
        {
            /* Add the timeout time to the time of day to get absolute timeout time */
            timeout_time.tv_sec  = time_of_day.tv_sec;
            timeout_time.tv_nsec = time_of_day.tv_usec * 1000;
            timeout_time.tv_sec += time_out;
        }
    }

    if (time_out == 0)
    {
        status = sem_wait(&req->sem);
    }
    else
    {
        status = sem_timedwait(&req->sem, &timeout_time);
    }

    if (status < 0)
    {
        if (errno == ETIMEDOUT)
        {
            /* If a time out occurred, return a timeout response */
            DG_DBG_ERROR("Waiting for aux response time out on aux %d, time out = %d seconds",
                         req->aux_id, time_out);
        }
        else
        {
            /* If an error other than time out occurred, send an error response */
            DG_DBG_ERROR("Waiting for aux response failed on aux %d, errno = %d (%s)",
                         req->aux_id, errno, strerror(errno));
        }
    }
    else
    {
        DG_DBG_TRACE("Wait BP rsp successfully: aux_id = %d, opcode = 0x%04x, seq_tag = 0x%02x",
                     req->aux_id, req->opcode, req->seq_tag);
    }
}

/*=============================================================================================*//**
@brief Signal the request when the response is ready

@param[in] req     - the pointer of the bp req
*//*==============================================================================================*/
void dg_aux_util_signal_bp_req(DG_AUX_UTIL_BP_REQ_T* req)
{
    DG_DBG_TRACE("BP req signaled: aux_id = %d, opcode = 0x%04x, seq_tag = 0x%02x",
                 req->aux_id, req->opcode, req->seq_tag);

    DG_DBG_TRACE("Signaled semphore = %p", &req->sem);

    sem_post(&req->sem);
}

/*=============================================================================================*//**
@brief Add one bp request to the list

@param[in] aux_id  - The aux engine for the req
@param[in] opcode  - req opcode
@param[in] seq_tag - req seq_tag

@return TRUE is success
*//*==============================================================================================*/
BOOL dg_aux_util_bp_req_list_add(UINT8 aux_id, DG_DEFS_OPCODE_T opcode, UINT8 seq_tag)
{
    BOOL                       ret = FALSE;
    DG_AUX_UTIL_BP_REQ_LIST_T* node;
    DG_AUX_UTIL_BP_REQ_LIST_T* next;
    DG_AUX_UTIL_BP_REQ_LIST_T* prev;

    node = (DG_AUX_UTIL_BP_REQ_LIST_T*)malloc(sizeof(DG_AUX_UTIL_BP_REQ_LIST_T));

    if (node == NULL)
    {
        DG_DBG_ERROR("can't malloc DG_AUX_UTIL_BP_REQ_LIST_T!");
    }
    else if ((node->req = dg_aux_util_bp_req_malloc()) == NULL)
    {
        DG_DBG_ERROR("can't malloc DG_AUX_UTIL_BP_REQ_T!");
        free(node);
        node = NULL;
    }
    else
    {
        node->req->aux_id  = aux_id;
        node->req->opcode  = opcode;
        node->req->seq_tag = seq_tag;

        DG_AUX_UTIL_CLEANUP_PUSH(DG_ENGINE_UTIL_generic_cleanup_unlock,
                                 (void*)&dg_aux_util_req_list_mutex);
        pthread_mutex_lock(&dg_aux_util_req_list_mutex);

        next = &dg_aux_util_req_list_head;
        prev = dg_aux_util_req_list_head.prev;

        next->prev = node;
        node->next = next;
        node->prev = prev;
        prev->next = node;

        pthread_mutex_unlock(&dg_aux_util_req_list_mutex);
        DG_AUX_UTIL_CLEANUP_POP(0);


        DG_DBG_TRACE("Add BP req to the list: aux_id = %d, opcode = 0x%04x, seq_tag = 0x%02x",
                     aux_id, opcode, seq_tag);

        ret = TRUE;
    }

    return ret;
}

/*=============================================================================================*//**
@brief Delete one bp request from the list

@param[in] node - the req node in the list
*//*==============================================================================================*/
void dg_aux_util_bp_req_list_del(DG_AUX_UTIL_BP_REQ_LIST_T* node)
{
    DG_AUX_UTIL_BP_REQ_LIST_T* next;
    DG_AUX_UTIL_BP_REQ_LIST_T* prev;

    /* this would never happen if (node != &dg_aux_util_req_list_head) */

    DG_DBG_TRACE("Delete BP req from the list: "
                 "aux_id = %d, opcode = 0x%04x, seq_tag = 0x%02x",
                 node->req->aux_id, node->req->opcode, node->req->seq_tag);


    DG_AUX_UTIL_CLEANUP_PUSH(DG_ENGINE_UTIL_generic_cleanup_unlock,
                             (void*)&dg_aux_util_req_list_mutex);
    pthread_mutex_lock(&dg_aux_util_req_list_mutex);

    next = node->next;
    prev = node->prev;

    next->prev = prev;
    prev->next = next;

    dg_aux_util_bp_req_free(node->req);
    free(node);

    pthread_mutex_unlock(&dg_aux_util_req_list_mutex);
    DG_AUX_UTIL_CLEANUP_POP(0);
}

/*=============================================================================================*//**
@brief Find the bp request from the list

@param[in] aux_id  - The aux engine for the req
@param[in] opcode  - req opcode
@param[in] seq_tag - req seq_tag

@return the list node of the request, NULL if not found
*//*==============================================================================================*/
DG_AUX_UTIL_BP_REQ_LIST_T* dg_aux_util_bp_req_list_find(UINT8            aux_id,
                                                        DG_DEFS_OPCODE_T opcode,
                                                        UINT8            seq_tag)
{
    DG_AUX_UTIL_BP_REQ_LIST_T* ret = NULL;
    DG_AUX_UTIL_BP_REQ_LIST_T* iter;

    DG_AUX_UTIL_CLEANUP_PUSH(DG_ENGINE_UTIL_generic_cleanup_unlock,
                             (void*)&dg_aux_util_req_list_mutex);
    pthread_mutex_lock(&dg_aux_util_req_list_mutex);

    iter = dg_aux_util_req_list_head.next;

    /* iterate over the list */
    while (iter != &dg_aux_util_req_list_head)
    {
        if (iter->req->aux_id == aux_id &&
            iter->req->seq_tag == seq_tag &&
            iter->req->opcode == opcode)
        {
            ret = iter;
            break;
        }
        iter = iter->next;
    }

    pthread_mutex_unlock(&dg_aux_util_req_list_mutex);
    DG_AUX_UTIL_CLEANUP_POP(0);

    return ret;
}

/*=============================================================================================*//**
@brief Find the bp request from the list by thread id

@param[in] pid  - the thread id for the request

@return the list node of the request, NULL if not found
*//*==============================================================================================*/
DG_AUX_UTIL_BP_REQ_LIST_T* dg_aux_util_bp_req_list_find_pid(pthread_t pid)
{
    DG_AUX_UTIL_BP_REQ_LIST_T* ret = NULL;
    DG_AUX_UTIL_BP_REQ_LIST_T* iter;

    DG_AUX_UTIL_CLEANUP_PUSH(DG_ENGINE_UTIL_generic_cleanup_unlock,
                             (void*)&dg_aux_util_req_list_mutex);
    pthread_mutex_lock(&dg_aux_util_req_list_mutex);

    iter = dg_aux_util_req_list_head.next;

    /* iterate over the list */
    while (iter != &dg_aux_util_req_list_head)
    {
        if (iter->req->pid == pid)
        {
            DG_DBG_TRACE("found BP req by pid 0x%x: aux_id = %d, opcode = 0x%04x, seq_tag = 0x%02x",
                         (int)pid, iter->req->aux_id, iter->req->opcode, iter->req->seq_tag);
            ret = iter;
            break;
        }
        iter = iter->next;
    }

    pthread_mutex_unlock(&dg_aux_util_req_list_mutex);
    DG_AUX_UTIL_CLEANUP_POP(0);

    return ret;
}

