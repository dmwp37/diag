/*==================================================================================================

    Module Name:  dg_pal_aux_engine.c

    General Description: Implements the PAL auxiliary engine layer for the test platform

====================================================================================================


Revision History:
                            Modification     Tracking
Author                          Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
Xudong Huang    - xudongh    2012/04/27     xxxxxx-000   Creation
Xudong Huang    - xudongh    2013/12/19     xxxxx-0001   Update diag rsp protocol

====================================================================================================
                                            INCLUDE FILES
==================================================================================================*/
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>

#include "dg_defs.h"
#include "dg_dbg.h"
#include "dg_pal_aux_engine.h"
#include "dg_engine_util.h"

/*==================================================================================================
                                           LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                            LOCAL MACROS
==================================================================================================*/
#define DG_PAL_AUX_ENGINE_FD_NOT_INIT -1

/*==================================================================================================
                             LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                      LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static void* dg_pal_aux_engine_aux_thread(void*);

/*==================================================================================================
                                          GLOBAL VARIABLES
==================================================================================================*/
#if DG_CFG_AUX_NUM > 0
/** the aux simulator thread handler */
static pthread_t dg_pal_aux_engine_thread_hndl[DG_CFG_AUX_NUM] = { 0 };
/** the aux simulator thread control flag */
static BOOL      dg_pal_aux_engine_run_ctrl[DG_CFG_AUX_NUM]    = { FALSE };
/** Socket the aux simulator thread uses */
static int       dg_pal_aux_engine_socket_aux[DG_CFG_AUX_NUM]  = { DG_PAL_AUX_ENGINE_FD_NOT_INIT };
/** Socket the DIAG engine uses */
static int       dg_pal_aux_engine_socket_diag[DG_CFG_AUX_NUM] = { DG_PAL_AUX_ENGINE_FD_NOT_INIT };
#else
/** the aux simulator thread handler */
static pthread_t dg_pal_aux_engine_thread_hndl[DG_CFG_AUX_NUM];
/** the aux simulator thread control flag */
static BOOL      dg_pal_aux_engine_run_ctrl[DG_CFG_AUX_NUM];
/** Socket the aux simulator thread uses */
static int       dg_pal_aux_engine_socket_aux[DG_CFG_AUX_NUM];
/** Socket the DIAG engine uses */
static int       dg_pal_aux_engine_socket_diag[DG_CFG_AUX_NUM];
#endif

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Initializes the communication interface with the specified auxiliary DIAG engine

@param[in] aux_id - The aux engine to init

@return Status of initialization

@note
 - If no auxiliary DIAG engine is present, DG_PAL_AUX_ENGINE_INIT_NOT_PRESENT must be returned
 - This function should NOT support an aux id of DG_AUX_ENGINE_AUX_ID_INIT_ALL
*//*==============================================================================================*/
DG_PAL_AUX_ENGINE_INIT_T DG_PAL_AUX_ENGINE_init(int aux_id)
{
    int                      sockets[2];
    int                      thread_status;
    DG_PAL_AUX_ENGINE_INIT_T status = DG_PAL_AUX_ENGINE_INIT_FAIL;

    if (aux_id >= DG_CFG_AUX_NUM)
    {
        status = DG_PAL_AUX_ENGINE_INIT_NOT_PRESENT;
        DG_DBG_ERROR("Tried to init non existing aux engine %d", aux_id);
    }
    else if (dg_pal_aux_engine_thread_hndl[aux_id] != 0)
    {
        status = DG_PAL_AUX_ENGINE_INIT_SUCCESS;
        DG_DBG_TRACE("Already init existing aux engine %d", aux_id);
    }
    /* Create a connected socket pair */
    else if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) != 0)
    {
        DG_DBG_ERROR("Failed to create connected sockets, errno = %d (%s)",
                     errno, strerror(errno));
    }
    else
    {
        /* Save the sockets */
        dg_pal_aux_engine_socket_aux[aux_id]  = sockets[0];
        dg_pal_aux_engine_socket_diag[aux_id] = sockets[1];

        /* set the aux simulate thread run flag */
        dg_pal_aux_engine_run_ctrl[aux_id]    = TRUE;

        /* Spawn a thread to simulate an aux engine */
        if ((thread_status = pthread_create(&dg_pal_aux_engine_thread_hndl[aux_id], NULL,
                                            dg_pal_aux_engine_aux_thread, (void*)aux_id)) != 0)
        {
            DG_DBG_ERROR("Failed to create aux simulation thread, errno = %d (%s)",
                         errno, strerror(errno));
            close(dg_pal_aux_engine_socket_aux[aux_id]);
            close(dg_pal_aux_engine_socket_diag[aux_id]);
            dg_pal_aux_engine_thread_hndl[aux_id] = 0;
            dg_pal_aux_engine_run_ctrl[aux_id]    = FALSE;
        }
        else
        {
            status = DG_PAL_AUX_ENGINE_INIT_SUCCESS;
        }
    }
    return status;
}

/*=============================================================================================*//**
@brief close the communication interface with the specified auxiliary DIAG engine

@param[in] aux_id - The aux engine to close

@note
 - This function should NOT support an aux id of DG_AUX_ENGINE_AUX_ID_INIT_ALL
*//*==============================================================================================*/
void DG_PAL_AUX_ENGINE_close(int aux_id)
{
    if (aux_id >= DG_CFG_AUX_NUM)
    {
        DG_DBG_ERROR("Tried to close non existing aux engine %d", aux_id);
    }
    else if (dg_pal_aux_engine_socket_aux[aux_id] < 0 ||
             dg_pal_aux_engine_socket_diag[aux_id] < 0)
    {
        DG_DBG_ERROR("Tried to close non opened aux engine %d", aux_id);
    }
    else if (dg_pal_aux_engine_thread_hndl[aux_id] == 0)
    {
        DG_DBG_TRACE("Tried to close already closed aux engine %d", aux_id);
    }
    else
    {
        /* Keep trying to connect to the aux engine until we are successful, or we hit the
           retry max */
        DG_DBG_TRACE("Try to close Aux engine (id = %d)", aux_id);

        /* tell the aux simulate thread to exit */
        dg_pal_aux_engine_run_ctrl[aux_id] = FALSE;

        close(dg_pal_aux_engine_socket_aux[aux_id]);
        close(dg_pal_aux_engine_socket_diag[aux_id]);

        /* wait the thread to finish */
        pthread_join(dg_pal_aux_engine_thread_hndl[aux_id], NULL);

        dg_pal_aux_engine_thread_hndl[aux_id] = 0;
        dg_pal_aux_engine_socket_aux[aux_id]  = DG_PAL_AUX_ENGINE_FD_NOT_INIT;
        dg_pal_aux_engine_socket_diag[aux_id] = DG_PAL_AUX_ENGINE_FD_NOT_INIT;

        DG_DBG_TRACE("Successfully close simulated Aux engine (id = %d)", aux_id);
    }
}

/*=============================================================================================*//**
@brief Reads the specified number of bytes from the specified auxiliary DIAG engine

@param[in]  aux_id        - The aux engine to read from
@param[in]  bytes_to_read - The number of bytes to read
@param[out] data          - Data read

@return TRUE = success, FALSE = failure

@note
 - The read is synchronous, the function will block until the requested number of bytes are read
*//*==============================================================================================*/
BOOL DG_PAL_AUX_ENGINE_read(int aux_id, UINT32 bytes_to_read, UINT8* data)
{
    BOOL is_success = FALSE;

    /* Return error if the aux engine handle is not init'd */
    if ((aux_id >= DG_CFG_AUX_NUM) ||
        (dg_pal_aux_engine_socket_diag[aux_id] == DG_PAL_AUX_ENGINE_FD_NOT_INIT))
    {
        DG_DBG_ERROR("Aux engine device is not open, aux_id = %d!", aux_id);
    }
    else if (read(dg_pal_aux_engine_socket_diag[aux_id], data, bytes_to_read) ==
             (ssize_t)bytes_to_read)
    {
        is_success = TRUE;
    }
    return is_success;
}

/*=============================================================================================*//**
@brief Writes the specified number of bytes to the specified auxiliary DIAG engine

@param[in]  aux_id         - The aux engine to write to
@param[in]  bytes_to_write - The number of bytes to write
@param[out] data           - Data to write

@return TRUE = success, FALSE = failure

@note
 - The write is synchronous, the function will block until the requested number of bytes are written
*//*==============================================================================================*/
BOOL DG_PAL_AUX_ENGINE_write(int aux_id, UINT32 bytes_to_write, UINT8* data)
{
    BOOL is_success = FALSE;

    /* Return error if the aux engine handle is not init'd */
    if ((aux_id >= DG_CFG_AUX_NUM) ||
        (dg_pal_aux_engine_socket_diag[aux_id] == DG_PAL_AUX_ENGINE_FD_NOT_INIT))
    {
        DG_DBG_ERROR("Aux engine device is not open, aux_id = %d!", aux_id);
    }
    else if (write(dg_pal_aux_engine_socket_diag[aux_id], data, bytes_to_write) ==
             (ssize_t)bytes_to_write)
    {
        is_success = TRUE;
    }

    return is_success;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief This function runs as its own thread, simulating another DIAG engine for testing the aux
       engine

@param[in]  id - The aux engine id this thread is simulating

@return Not used

@note
 - This thread will watch for any DIAG request which is sent to it.  It will read the request, and
   always echo back the requests's opcode + data as a DIAG response
*//*==============================================================================================*/
static void* dg_pal_aux_engine_aux_thread(void* id)
{
    DG_DEFS_DIAG_RSP_HDR_T rsp_hdr;
    DG_DEFS_DIAG_RSP_HDR_T n_rsp_hdr;
    DG_DEFS_DIAG_REQ_HDR_T req_hdr;
    DG_DEFS_DIAG_REQ_HDR_T n_req_hdr;

    UINT8*                 payload = NULL;
    int                    read_size;
    int                    write_size;
    int                    aux_id  = *(int*)(&id);

    DG_DBG_TRACE("AUX SIM[%d] Starting aux thread", aux_id);

    while (dg_pal_aux_engine_run_ctrl[aux_id])
    {
        /* Read the DIAG header */
        read_size = read(dg_pal_aux_engine_socket_aux[aux_id], &n_req_hdr, sizeof(n_req_hdr));
        if (read_size != sizeof(n_req_hdr))
        {
            DG_DBG_TRACE("AUX SIM[%d] - Failed to read DIAG hedaer, read %d bytes",
                         aux_id, read_size);
        }
        else
        {
            BOOL is_success = TRUE;

            /* Endian swap the header */
            DG_ENGINE_UTIL_hdr_req_ntoh(&n_req_hdr, &req_hdr);

            DG_DBG_TRACE("AUX SIM[%d] - Header opcode = 0x%04x, length = 0x%08x",
                         aux_id, req_hdr.opcode, req_hdr.length);

            /* If the header indicates a data payload, read it */
            if (req_hdr.length > 0)
            {
                if ((payload = (UINT8*)malloc(req_hdr.length)) == NULL)
                {
                    is_success = FALSE;
                    DG_DBG_ERROR("AUX SIM[%d] - Failed to allocate %d bytes",
                                 aux_id, req_hdr.length);
                }
                else
                {
                    read_size = (int)read(dg_pal_aux_engine_socket_aux[aux_id], payload,
                                          req_hdr.length);
                    if (read_size != (int)req_hdr.length)
                    {
                        is_success = FALSE;
                        DG_DBG_TRACE("AUX SIM[%d] - Failed to read payload, read %d bytes",
                                     aux_id, read_size);
                    }
                }
            }

            if (is_success == TRUE)
            {
                /* Set the DIAG response header */
                memset(&rsp_hdr, 0, sizeof(rsp_hdr));
                rsp_hdr.seq_tag      = req_hdr.seq_tag;
                rsp_hdr.opcode       = req_hdr.opcode;
                rsp_hdr.rsp_code     = DG_RSP_CODE_CMD_RSP_GENERIC;
                rsp_hdr.length       = req_hdr.length + 1; /* +1 for the aux id added in response data */

                /* Endian swap the header */
                DG_ENGINE_UTIL_hdr_rsp_hton(&rsp_hdr, &n_rsp_hdr);

                /* Write the header + payload (if needed) */
                write_size = write(dg_pal_aux_engine_socket_aux[aux_id], &n_rsp_hdr, sizeof(n_rsp_hdr));

                /* Add the aux id in the response data */
                write(dg_pal_aux_engine_socket_aux[aux_id], &aux_id, sizeof(UINT8));
                if (req_hdr.length > 0)
                {
                    write_size += write(dg_pal_aux_engine_socket_aux[aux_id], payload, req_hdr.length);
                }
                DG_DBG_TRACE("AUX SIM[%d] - Wrote %d bytes",
                             aux_id, write_size);

            }

            if (payload != NULL)
            {
                free(payload);
                payload = NULL;
            }
        }
    }

    DG_DBG_TRACE("AUX SIM[%d] aux thread stopped", aux_id);

    return NULL;
}
/** @} */
/** @} */

