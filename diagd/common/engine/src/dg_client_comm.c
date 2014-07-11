/*==================================================================================================

    Module Name:  dg_client_comm.c

    General Description: Provides functions for interacting with DIAG clients

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
#include <sys/time.h>
#include <sys/socket.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <errno.h>
#include "dg_defs.h"
#include "dg_dbg.h"
#include "dg_client_comm.h"
#include "dg_aux_util.h"
#include "dg_drv_util.h"
#include "dg_engine_util.h"
#include "dg_socket.h"
#include "dg_main.h"
#include "dg_pal_util.h"

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/
#define DG_CLIENT_COMM_FD_READ_NUM_RETRY 3  /**< Number of times to retry reading a fd   */

/*==================================================================================================
                                            LOCAL MACROS
==================================================================================================*/
/* Macros for locking and unlocking access to the connected sockets list.  All code which
   refers to the connected sockets must lock/unlock at correct times */
#define DG_CLIENT_COMM_CONNECTED_CLIENTS_LOCK() \
    pthread_cleanup_push(DG_ENGINE_UTIL_generic_cleanup_unlock, \
                         (void*)&dg_client_comm_connected_clients_mutex); \
    pthread_mutex_lock(&dg_client_comm_connected_clients_mutex)

#define DG_CLIENT_COMM_CONNECTED_CLIENTS_UNLOCK() \
    pthread_mutex_unlock(&dg_client_comm_connected_clients_mutex); \
    pthread_cleanup_pop(0)

#define DG_CLIENT_COMM_HANDLER_TBL_LOCK() \
    pthread_cleanup_push(DG_ENGINE_UTIL_generic_cleanup_unlock, \
                         (void*)&dg_client_common_handler_tbl_mutex); \
    pthread_mutex_lock(&dg_client_common_handler_tbl_mutex)

#define DG_CLIENT_COMM_HANDLER_TBL_UNLOCK() \
    pthread_mutex_unlock(&dg_client_common_handler_tbl_mutex); \
    pthread_cleanup_pop(0)

/* Stub out thread cancel settings on platforms using alternate timers, not supported */
#define DG_CLIENT_COMM_SET_THREAD_CANCEL() \
    /*do \
    { \
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL); \
        pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL); \
    } while(0)*/

/*==================================================================================================
                             LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** Structure for connected client linked list node */
typedef struct dg_client_comm_client_node_int_t DG_CLIENT_COMM_CLIENT_NODE_T;
struct dg_client_comm_client_node_int_t
{
    DG_CLIENT_COMM_CLIENT_NODE_T* next;
    int                           socket;
    DG_CLIENT_COMM_CLIENT_TYPE_T  type;
};

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static const DG_DEFS_OPCODE_ENTRY_T* dg_client_comm_find_diag_handler(DG_DEFS_OPCODE_T opcode);
static void*                         dg_client_comm_diag_handler_exec(void* diag);
static void                          dg_client_comm_diag_handler_create_thread(DG_DEFS_DIAG_REQ_T* diag);
static DG_DEFS_DIAG_REQ_T*           dg_client_comm_read_diag_req(int fd);
static BOOL                          dg_client_comm_read_fd(int fd, void* buff, int len);
static void                          dg_client_comm_remove_client_from_list(int socket);
static void                          dg_client_comm_notify_client_update(BOOL is_add);
static void                          dg_client_comm_alt_timer_create_thread(DG_DEFS_DIAG_REQ_T* diag);

/*==================================================================================================
                                          GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/
static const DG_DEFS_OPCODE_ENTRY_T* dg_client_comm_handler_tbl_ptr          = NULL;
static const DG_DEFS_OPCODE_ENTRY_T* dg_client_comm_override_handler_tbl_ptr = NULL;
static DG_CLIENT_COMM_CLIENT_NODE_T* dg_client_comm_client_list_head_ptr     = NULL;
static pthread_mutex_t               dg_client_comm_connected_clients_mutex  = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t               dg_client_common_handler_tbl_mutex      = PTHREAD_MUTEX_INITIALIZER;

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Shuts down the socket for all clients of the specified type

@param[in] type - Type of client

@note
- This function will just shutdown the socket for all clients.  That will cause each of the clients'
  blocking read to fail, and start the normal cleanup procedure
*//*==============================================================================================*/
void DG_CLIENT_COMM_shutdown_clients_of_type(DG_CLIENT_COMM_CLIENT_TYPE_T type)
{
    DG_CLIENT_COMM_CLIENT_NODE_T* node = NULL;

    DG_CLIENT_COMM_CONNECTED_CLIENTS_LOCK();

    /* Go through all connected clients, looking for clients of the given type */
    node = dg_client_comm_client_list_head_ptr;
    while (node != NULL)
    {
        if (node->type == type)
        {
            DG_DBG_TRACE("Shutting down socket %d", node->socket);
            shutdown(node->socket, SHUT_RDWR);
        }
        node = node->next;
    }
    DG_CLIENT_COMM_CONNECTED_CLIENTS_UNLOCK();
}

/*=============================================================================================*//**
@brief Listens for a client to connect to server

@param[in] arg - Ignored

@return NULL returned
*//*==============================================================================================*/
void* DG_CLIENT_COMM_client_connection_listener(void* arg)
{
    DG_COMPILE_UNUSED(arg);
    /* We are using sockets for our client connections, start socket listener */
    DG_SOCKET_connection_listener();
    return NULL;
}

/*=============================================================================================*//**
@brief Sends the supplied DIAG response to all connected clients

@param[in] rsp - DIAG Response to send
*//*==============================================================================================*/
void DG_CLIENT_COMM_send_rsp_to_all_clients(DG_DEFS_DIAG_RSP_T* rsp)
{
    DG_CLIENT_COMM_CLIENT_NODE_T* node = NULL;

    /* Lock access to the connected client list */
    DG_CLIENT_COMM_CONNECTED_CLIENTS_LOCK();

    /* Go through all connected clients, sending the response out */
    node = dg_client_comm_client_list_head_ptr;
    while (node != NULL)
    {
        if (!DG_CLIENT_COMM_client_write(node->socket, rsp))
        {
            DG_DBG_ERROR("Send response to fd %d failed", node->socket);
        }
        node = node->next;
    }
    DG_CLIENT_COMM_CONNECTED_CLIENTS_UNLOCK();
}

/*=============================================================================================*//**
@brief Handles requests from a connected DIAG client

@param[in] socket_void - fd for connected client

@return NULL always returned
*//*==============================================================================================*/
void* DG_CLIENT_COMM_client_connection_handler(void* socket_void)
{
    struct sigaction    sa;
    DG_DEFS_DIAG_REQ_T* diag   = NULL;
    int                 socket = *(int*)(&socket_void);

    DG_DBG_TRACE("Start client connection handler for socket %d", socket);
    dg_client_comm_notify_client_update(TRUE);

    /*
     * when time out the client may close the connection first, and send the rsp to the
     * closed socket will raise the SIGPIPE signal which would cause the whole server exist
     * ignore the signal can prevent the server exit.
     */
    memset(&sa, 0, sizeof(sa));
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = SIG_IGN;
    sa.sa_flags   = 0;
    if (sigaction(SIGPIPE, &sa, NULL) == -1)
    {
        DG_DBG_ERROR("failed to ignore SIGPIPE");
    }

    while (DG_MAIN_engine_exit_flag == FALSE)
    {
        diag = dg_client_comm_read_diag_req(socket);
        if (diag != NULL)
        {
            dg_client_comm_diag_handler_create_thread(diag);
        }
        else
        {
            DG_DBG_ERROR("Reading DIAG request failed, exiting client thread for socket %d", socket);
            break;
        }
    }

    dg_client_comm_remove_client_from_list(socket);
    shutdown(socket, SHUT_RDWR);
    close(socket);
    dg_client_comm_notify_client_update(FALSE);
    DG_DBG_TRACE("Client connection handler for socket %d dies", socket);
    pthread_exit((void*)0);

    return NULL;
}

/*=============================================================================================*//**
@brief Adds a newly connected client to the client list

@param[in] socket - fd for connected client
@param[in] type   - Type of client

@return TRUE = Was able to add client to list, FALSE = Failed adding client to list.
*//*==============================================================================================*/
BOOL DG_CLIENT_COMM_add_client_to_list(int socket, DG_CLIENT_COMM_CLIENT_TYPE_T type)
{
    BOOL                          is_success  = FALSE;
    DG_CLIENT_COMM_CLIENT_NODE_T* client_node = NULL;

    /* Allocate memory for the client node */
    client_node = (DG_CLIENT_COMM_CLIENT_NODE_T*)malloc(sizeof(DG_CLIENT_COMM_CLIENT_NODE_T));
    if (client_node == NULL)
    {
        DG_DBG_ERROR("Error allocating memory for client node");
    }
    else
    {
        /* Save the socket & type in the linked list */
        client_node->socket = socket;
        client_node->type   = type;

        /* Lock the list and add the new node to the start */
        DG_CLIENT_COMM_CONNECTED_CLIENTS_LOCK();
        client_node->next                   = dg_client_comm_client_list_head_ptr;
        dg_client_comm_client_list_head_ptr = client_node;
        DG_DBG_TRACE("Successfully added client to list, socket = %d", socket);

        DG_CLIENT_COMM_CONNECTED_CLIENTS_UNLOCK();
        is_success = TRUE;
    }

    return is_success;
}

/*=============================================================================================*//**
@brief Sends a DIAG response to a client

@param[in] fd  - File descriptor to write to
@param[in] rsp - DIAG response

@return success/failure notification
*//*==============================================================================================*/
BOOL DG_CLIENT_COMM_client_write(int fd, DG_DEFS_DIAG_RSP_T* rsp)
{
    INT32  write_len = 0;
    BOOL   status    = FALSE;
    UINT8* write_buff;

    DG_DBG_TRACE("DIAG opcode = 0x%04x, length = %d, fd = %d",
                 rsp->header.opcode, rsp->header.length, fd);

    write_len = rsp->header.length + sizeof(rsp->header);
    if ((write_buff = (UINT8*)malloc(write_len)) == NULL)
    {
        DG_DBG_ERROR("out of memory - malloc failed on write_buff.");
    }
    else
    {
        /* Do endian conversion */
        DG_ENGINE_UTIL_hdr_rsp_hton(&rsp->header, (DG_DEFS_DIAG_RSP_HDR_T*)write_buff);
        if (rsp->header.length > 0)
        {
            memcpy(write_buff + sizeof(rsp->header), rsp->data_ptr, rsp->header.length);
        }
        DG_DBG_DUMP(write_buff, write_len);

        if (write(fd, write_buff, write_len) < 0)
        {
            DG_DBG_ERROR("Write data to fd = %d failed! Length attempted = %d", fd, write_len);
        }
        else
        {
            DG_DBG_TRACE("Sent %d byte(s) DIAG opcode = 0x%04x to fd:%d succeeded.",
                         write_len, rsp->header.opcode, fd);
            status = TRUE;
        }
        free(write_buff);
    }

    return status;
}

/*=============================================================================================*//**
@brief Sets the DIAG handler table

@param[in] tbl_ptr - Pointer to table
*//*==============================================================================================*/
void DG_CLIENT_COMM_set_handler_tbl(const DG_DEFS_OPCODE_ENTRY_T* tbl_ptr)
{
    DG_CLIENT_COMM_HANDLER_TBL_LOCK();
    dg_client_comm_handler_tbl_ptr = tbl_ptr;
    DG_CLIENT_COMM_HANDLER_TBL_UNLOCK();
}

/*=============================================================================================*//**
@brief Sets the DIAG Override handler table

@param[in] tbl_ptr - Pointer to table

@note
 - If used, this table will be used in place of the one specified via
   DG_CLIENT_COMM_set_handler_tbl()
 - To disable, call this function with a NULL pointer
*//*==============================================================================================*/
void DG_CLIENT_COMM_set_override_handler_tbl(const DG_DEFS_OPCODE_ENTRY_T* tbl_ptr)
{
    DG_CLIENT_COMM_HANDLER_TBL_LOCK();
    dg_client_comm_override_handler_tbl_ptr = tbl_ptr;
    DG_CLIENT_COMM_HANDLER_TBL_UNLOCK();
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Finds the DIAG handler for a given opcode

@param[in] opcode - Opcode to find handler for

@return Pointer to handler table entry, NULL on failure
*//*==============================================================================================*/
const DG_DEFS_OPCODE_ENTRY_T* dg_client_comm_find_diag_handler(DG_DEFS_OPCODE_T opcode)
{
    UINT32                        i           = 0;
    const DG_DEFS_OPCODE_ENTRY_T* handler_ptr = NULL;
    const DG_DEFS_OPCODE_ENTRY_T* tbl_ptr     = NULL;

    DG_CLIENT_COMM_HANDLER_TBL_LOCK();
    tbl_ptr = dg_client_comm_handler_tbl_ptr;

    /* Use the override table if feature enabled */
    if (dg_client_comm_override_handler_tbl_ptr != NULL)
    {
        DG_DBG_TRACE("Using handler table override!");
        tbl_ptr = dg_client_comm_override_handler_tbl_ptr;
    }

    if (tbl_ptr != NULL)
    {
        while ((tbl_ptr + i)->opcode != DG_DEFS_HANDLER_TABLE_OPCODE_END)
        {
            if ((tbl_ptr + i)->opcode == opcode)
            {
                break;
            }
            i++;
        }

        handler_ptr = &tbl_ptr[i];
    }

    DG_CLIENT_COMM_HANDLER_TBL_UNLOCK();
    return handler_ptr;
}

/*=============================================================================================*//**
@brief Executes the DIAG handler for the given DIAG request

@param[in] diag_void - DIAG request to handle

@return NULL always, not used
*//*==============================================================================================*/
void* dg_client_comm_diag_handler_exec(void* diag_void)
{
    const DG_DEFS_OPCODE_ENTRY_T* diag_handler = NULL;
    DG_DEFS_DIAG_RSP_BUILDER_T*   rsp          = NULL;
    DG_DEFS_DIAG_REQ_T*           diag         = (DG_DEFS_DIAG_REQ_T*)diag_void;

    rsp = DG_ENGINE_UTIL_rsp_init();
    DG_CLIENT_COMM_SET_THREAD_CANCEL();

    /* Find the handler for the diag, ensure the handler can be executed */
    diag_handler = dg_client_comm_find_diag_handler(diag->header.opcode);
    if ((diag_handler == NULL) || (diag_handler->fptr == NULL))
    {
        DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_ERR_OPCODE,
                                            "Opcode 0x%04x was not found",
                                            diag->header.opcode);
    }
    else if ((diag_handler->mode != DG_DEFS_MODE_ALL) &&
             (diag_handler->mode != DG_ENGINE_UTIL_get_engine_mode()))
    {
        DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_ERR_MODE,
                                            "DIAG Mode Error!  cur_mode=%d, desire_mode=%d",
                                            DG_ENGINE_UTIL_get_engine_mode(), diag_handler->mode);
    }
    else
    {
        /* Call the handler function for the diag, this will block until the diag is complete */
        DG_DBG_TRACE("For thread %p, executing function pointer %p for DIAG opcode 0x%04x",
                     pthread_self(), diag_handler->fptr, diag->header.opcode);

        /*
         * here we got the chance to set the connection between diag rsp and pthread_self()
         * we save this information in the thread specific data
         * so that the driver layer could get the error string correctly for its own thread
         */
        DG_DRV_UTIL_init_error_string();

        diag_handler->fptr(diag);
    }

    /* Only send a response in a failure case */
    if (DG_ENGINE_UTIL_rsp_is_failure(rsp) == TRUE)
    {
        DG_ENGINE_UTIL_rsp_send(rsp, diag);
    }

    DG_ENGINE_UTIL_rsp_free(rsp);

    /* Special case for platforms using alternate timers, see
       dg_client_comm_alt_timer_create_thread() for more info */

    /* Indicate to client request thread that the command has been handled */
    pthread_mutex_lock(&(diag->handled_mutex));
    diag->is_handled = TRUE;
    pthread_cond_signal(&(diag->handled_cond));
    pthread_mutex_unlock(&(diag->handled_mutex));

    /* Wait for client request thread to indicate handler thread can delete DIAG request */
    pthread_mutex_lock(&diag->delete_mutex);
    while (diag->can_delete == FALSE)
    {
        if (pthread_cond_wait(&(diag->delete_cond), &(diag->delete_mutex)) != 0)
        {
            DG_DBG_ERROR("Error waiting on delete condition, errno=%d(%m)", errno);
        }
    }
    pthread_mutex_unlock(&(diag->delete_mutex));
    DG_DBG_TRACE("Delete DIAG request for DIAG opcode 0x%04x", diag->header.opcode);
    DG_ENGINE_UTIL_free_diag_req(diag);

    return NULL;
}

/*=============================================================================================*//**
@brief Creates a thread to a handle a DIAG request

@param[in] diag - DIAG request to handle
*//*==============================================================================================*/
void dg_client_comm_diag_handler_create_thread(DG_DEFS_DIAG_REQ_T* diag)
{
    /* Platforms using alternate timers need to create its threads specially */
    dg_client_comm_alt_timer_create_thread(diag);
}

/*=============================================================================================*//**
@brief Reads a DIAG request from the client

@param[in] fd - File descriptor to read from

@return The DIAG request that was read, NULL in case of error.

@note
 - Calling function is responsible for freeing request using DG_ENGINE_UTIL_free_diag_req()
*//*==============================================================================================*/
DG_DEFS_DIAG_REQ_T* dg_client_comm_read_diag_req(int fd)
{
    DG_DEFS_DIAG_REQ_HDR_T diag_hdr;
    BOOL                   is_success = FALSE;
    DG_DEFS_DIAG_REQ_T*    diag_req   = NULL;

    /* Allocate memory for DIAG request */
    if ((diag_req = (DG_DEFS_DIAG_REQ_T*)malloc(sizeof(DG_DEFS_DIAG_REQ_T))) == NULL)
    {
        DG_DBG_ERROR("Out of memory - malloc failed on diag");
    }
    else
    {
        /* Set the sender of the command request */
        diag_req->sender_id   = fd;
        diag_req->data_ptr    = NULL;
        diag_req->data_offset = 0;

        /* Init mutexes and conditional variables */
        pthread_mutex_init(&(diag_req->handled_mutex), NULL);
        pthread_cond_init(&(diag_req->handled_cond), NULL);
        pthread_mutex_init(&(diag_req->delete_mutex), NULL);
        pthread_cond_init(&(diag_req->delete_cond), NULL);
        diag_req->can_delete = FALSE;
        diag_req->is_handled = FALSE;

        if (dg_client_comm_read_fd(fd, &diag_hdr, sizeof(diag_hdr)))
        {
            /* Do endian conversion */
            DG_ENGINE_UTIL_hdr_req_ntoh(&diag_hdr, &(diag_req->header));
            DG_DBG_TRACE("DIAG Header Read - opcode = 0x%04x, length = %d",
                         diag_req->header.opcode, diag_req->header.length);

            /* Read request data if its present */
            if (diag_req->header.length != 0)
            {
                if ((diag_req->data_ptr = (UINT8*)malloc(diag_req->header.length)) == NULL)
                {
                    DG_DBG_ERROR("Out of memory - malloc failed response data size of %d",
                                 diag_req->header.length);
                }
                else
                {
                    if (!dg_client_comm_read_fd(fd, diag_req->data_ptr, diag_req->header.length))
                    {
                        DG_DBG_ERROR("Read payload data failed");
                    }
                    else
                    {
                        is_success = TRUE;
                    }
                }
            }
            else
            {
                is_success = TRUE;
            }
        }
    }

    if (is_success == FALSE)
    {
        DG_ENGINE_UTIL_free_diag_req(diag_req);
        diag_req = NULL;
    }

    return diag_req;
}

/*=============================================================================================*//**
@brief Reads the specified number of bytes from the given file descriptor

@param[in]  fd   - File descriptor to read from
@param[out] buff - Buffer to read into
@param[in]  len  - Number of bytes to read

@return Success/failure of read
*//*==============================================================================================*/
BOOL dg_client_comm_read_fd(int fd, void* buff, int len)
{
    BOOL status             = TRUE;
    int  total_bytes_read   = 0;
    int  current_bytes_read = 0;
    int  try_count          = 0;

    DG_DBG_TRACE("Attempt to read %d bytes from client fd %d", len, fd);
    /* Continue to read until an error occurs or we read the desired number of bytes */
    while (status && (total_bytes_read != len))
    {
        current_bytes_read = read(fd, ((UINT8*)buff + total_bytes_read), (len - total_bytes_read));
        if (current_bytes_read > 0)
        {
            DG_DBG_DUMP(((UINT8*)buff + total_bytes_read), current_bytes_read);
            total_bytes_read += current_bytes_read;
        }
        else
        {
            /* Only print errors if the current_bytes_read is NOT 0.  0 bytes most likely
               indicates client closed its connection */
            if (current_bytes_read != 0)
            {
                DG_DBG_ERROR("Read failed, current_bytes_read = %d, errno=%d(%m)",
                             current_bytes_read, errno);
            }

            if (++try_count == DG_CLIENT_COMM_FD_READ_NUM_RETRY)
            {
                DG_DBG_ERROR("Read Failed on fd %d, over number of retries.", fd);
                status = FALSE;
            }
        }
    }

    return status;
}

/*=============================================================================================*//**
@brief Removes the client from the connected clients list

@param[in] socket - The socket of the client to remove
*//*==============================================================================================*/
void dg_client_comm_remove_client_from_list(int socket)
{
    /* Nodes to traverse linked list, must be init'd to NULL! */
    DG_CLIENT_COMM_CLIENT_NODE_T* cur_node  = NULL;
    DG_CLIENT_COMM_CLIENT_NODE_T* prev_node = NULL;

    DG_DBG_TRACE("Attempting to remove socket %d from the client list", socket);

    /* Lock the connected clients list */
    DG_CLIENT_COMM_CONNECTED_CLIENTS_LOCK();

    /* Loop through all nodes, looking for the specified socket */
    cur_node = dg_client_comm_client_list_head_ptr;
    while (cur_node != NULL)
    {
        if (cur_node->socket == socket)
        {
            /* Found the node!  If the previous node is still NULL, then the node being removed is
               the head of the list.  Set the head to the next node */
            if (prev_node == NULL)
            {
                dg_client_comm_client_list_head_ptr = cur_node->next;
            }
            else
            {
                /* If the head is not being removed, set the previous node's next ptr to current
                   node's next */
                prev_node->next = cur_node->next;
            }
            free(cur_node);
            break;
        }
        else
        {
            /* Continue traversing the linked list */
            prev_node = cur_node;
            cur_node  = cur_node->next;
        }
    }
    DG_CLIENT_COMM_CONNECTED_CLIENTS_UNLOCK();
}


/*=============================================================================================*//**
@brief Creates a thread to a handle a DIAG request, special function for platforms using alternate
       timers

@param[in] diag - DIAG request to handle

@note
 - Due to certain limitations on multiple platforms, timeout timers must be handled specially.
   On these platforms, DIAG request threads/timeouts are handled in this manner:
   1) Client thread finds opcode in opcode table, this needed to determine the desired timeout value
   2) Client thread spawns handler thread to handle request
   3) Client thread does a timed block wait on the 'is_handled' conditional variable.
   4) Once handler thread is done handling DIAG request, it sets 'is_handled' to indicate it is
      finished.
   5) Handler thread blocks waiting on the 'can_delete' conditional variable.
   6) Client thread unblocks, sets 'can_delete' to inidcate handler thread can delete request
   7) Client thread waits for next request
   8) Handler thread unblocks, deletes request and dies
 - For step #3, if a timeout occurs, the client thread sends the timeout response and lets
   handler thread know it can delete the request
*//*==============================================================================================*/
void dg_client_comm_alt_timer_create_thread(DG_DEFS_DIAG_REQ_T* diag)
{
    const DG_DEFS_OPCODE_ENTRY_T* diag_handler = NULL;
    DG_DEFS_DIAG_RSP_BUILDER_T*   rsp          = NULL;
    int                           status;
    struct timeval                time_of_day;
    struct timespec               timeout_time;
    pthread_t                     thread_id;
    BOOL                          is_client_thread_created = FALSE;

    DG_DBG_TRACE("Creating thread for DIAG 0x%04x", diag->header.opcode);

    /* Init response variable */
    rsp = DG_ENGINE_UTIL_rsp_init();

    /* Find the opcode in the opcode table, needed to determine DIAG timeout time */
    diag_handler = dg_client_comm_find_diag_handler(diag->header.opcode);
    if ((diag_handler == NULL) || (diag_handler->fptr == NULL))
    {
        DG_DBG_ERROR("Opcode 0x%04x was not found", diag->header.opcode);
        DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_PAR_ERR_OPCODE);
    }
    else
    {
        /* Get the current time of day */
        if (gettimeofday(&time_of_day, NULL) != 0)
        {
            DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                "Failed to get time of day, errno=%d(%m)", errno);
        }
        else
        {
            /* Add the timeout time to the time of day to get absolute timeout time */
            timeout_time.tv_sec  = time_of_day.tv_sec;
            timeout_time.tv_nsec = time_of_day.tv_usec * 1000;
            timeout_time.tv_sec += (diag_handler->timeout_msec / 1000);

            /* Create the thread */
            status = pthread_create(&thread_id, NULL, dg_client_comm_diag_handler_exec, (void*)diag);
            if (status != 0)
            {
                DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                    "Creating handler thread failed, errno=%d(%m)",
                                                    errno);
            }
            else
            {
                /* Detach the new thread */
                pthread_detach(thread_id);

                /* Indicate that the client thread exists */
                is_client_thread_created = TRUE;

                /* Do a timed wait until handler thread indicates it is do handling the request */
                pthread_mutex_lock(&diag->handled_mutex);
                while (diag->is_handled == FALSE)
                {
                    status = pthread_cond_timedwait(&(diag->handled_cond),
                                                    &(diag->handled_mutex), &timeout_time);

                    if (status == ETIMEDOUT)
                    {
                        /* If a time out occurred, return a timeout response */
                        DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_RSP_TIMEOUT,
                                                            "Handler thread timed out, time out=%d seconds",
                                                            (diag_handler->timeout_msec / 1000));
                        break;
                    }
                    else if (status != 0)
                    {
                        /* If an error other than time out occurred, send an error response */
                        DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Waiting for handler thread failed, errno=%d(%m)",
                                                            errno);
                        break;
                    }

                }
                pthread_mutex_unlock(&(diag->handled_mutex));

            }
        }
    }

    if (DG_ENGINE_UTIL_rsp_is_failure(rsp) == TRUE)
    {
        DG_ENGINE_UTIL_rsp_send(rsp, diag);
        /* error happens and cancel possible BP request after send out the response */
        if (is_client_thread_created == TRUE)
        {
            DG_AUX_UTIL_cancel_thread_bp_req(thread_id);
        }
    }
    DG_ENGINE_UTIL_rsp_free(rsp);

    /* If the client thread was created, indicate to the handler thread that the request can be
       deleted now.  This must occur at a time when the client thread is done with the diag
       request (ie. after the response was sent) */
    if (is_client_thread_created == TRUE)
    {
        pthread_mutex_lock(&(diag->delete_mutex));
        diag->can_delete = TRUE;
        pthread_cond_signal(&(diag->delete_cond));
        pthread_mutex_unlock(&(diag->delete_mutex));
    }
}

/*=============================================================================================*//**
@brief Notifies glue layer of a client being added/removed

@param[in] is_add  - If a client is being added, TRUE = added, FALSE = removed
*//*==============================================================================================*/
void dg_client_comm_notify_client_update(BOOL is_add)
{
    DG_CLIENT_COMM_CLIENT_NODE_T* node    = NULL;
    int                           num_int = 0;
    int                           num_ext = 0;

    DG_CLIENT_COMM_CONNECTED_CLIENTS_LOCK();

    /* Go through all connected clients, looking for clients of the given type */
    node = dg_client_comm_client_list_head_ptr;
    while (node != NULL)
    {
        if (node->type == DG_CLIENT_COMM_CLIENT_TYPE_INT)
        {
            num_int++;
        }
        else
        {
            num_ext++;
        }
        node = node->next;
    }

    DG_PAL_UTIL_notify_client_update(is_add, num_int, num_ext);
    DG_CLIENT_COMM_CONNECTED_CLIENTS_UNLOCK();
}

