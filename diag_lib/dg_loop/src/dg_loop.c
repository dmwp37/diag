/*==================================================================================================

    Module Name:  dg_loop.c

    General Description: Implements the dg_loop library

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include "dg_platform_defs.h"
#include "dg_dbg.h"
#include "dg_loop.h"

/** @addtogroup libdg_loop
@{
*/
/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/
#define DG_LOOP_MUTEX_LOCK(x) \
    pthread_cleanup_push(dg_loop_mutex_unlock, (x)); \
    pthread_mutex_lock((x))

#define DG_LOOP_MUTEX_UNLOCK(x) \
    pthread_mutex_unlock(x); \
    pthread_cleanup_pop(0)

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
typedef struct
{
    pthread_mutex_t mutex; /* mutex protection */

    int ref;               /* reference count  */
    int fd;                /* the actual fd    */
} DG_LOOP_PORT_FD_T;

typedef void (* DG_LOOP_MUTEX_UNLOCK_FUN_T) (void*);

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
                                         GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/
/* port number array */
static DG_LOOP_PORT_T dg_loop_port_array[] =
{
    DG_LOOP_PORT_MGT,
    DG_LOOP_PORT_HA,
    DG_LOOP_PORT_WTB0_1,
    DG_LOOP_PORT_WTB0_2,
    DG_LOOP_PORT_WTB1_1,
    DG_LOOP_PORT_WTB1_2,
    DG_LOOP_PORT_GE_0,
    DG_LOOP_PORT_GE_1,
    DG_LOOP_PORT_GE_2,
    DG_LOOP_PORT_GE_3,
    DG_LOOP_PORT_GE_4,
    DG_LOOP_PORT_GE_5,
    DG_LOOP_PORT_GE_6,
    DG_LOOP_PORT_GE_7,
    DG_LOOP_PORT_GE_8,
    DG_LOOP_PORT_GE_9,
    DG_LOOP_PORT_GE_10,
    DG_LOOP_PORT_GE_11,
    DG_LOOP_PORT_SFP_0,
    DG_LOOP_PORT_SFP_1,
    DG_LOOP_PORT_SFP_2,
    DG_LOOP_PORT_SFP_3,
    DG_LOOP_PORT_10GE_0,
    DG_LOOP_PORT_10GE_1,
    DG_LOOP_PORT_10GE_2,
    DG_LOOP_PORT_10GE_3,
};

/** internal real file descriptor array for each ports */
static DG_LOOP_PORT_FD_T dg_loop_port_fd[DG_LOOP_PORT_NUM] =
{
    { PTHREAD_MUTEX_INITIALIZER, 0, -1 }, /* DG_LOOP_PORT_MGT    */
    { PTHREAD_MUTEX_INITIALIZER, 0, -1 }, /* DG_LOOP_PORT_HA     */
    { PTHREAD_MUTEX_INITIALIZER, 0, -1 }, /* DG_LOOP_PORT_WTB0_1 */
    { PTHREAD_MUTEX_INITIALIZER, 0, -1 }, /* DG_LOOP_PORT_WTB0_2 */
    { PTHREAD_MUTEX_INITIALIZER, 0, -1 }, /* DG_LOOP_PORT_WTB1_1 */
    { PTHREAD_MUTEX_INITIALIZER, 0, -1 }, /* DG_LOOP_PORT_WTB1_2 */
    { PTHREAD_MUTEX_INITIALIZER, 0, -1 }, /* DG_LOOP_PORT_GE_0   */
    { PTHREAD_MUTEX_INITIALIZER, 0, -1 }, /* DG_LOOP_PORT_GE_1   */
    { PTHREAD_MUTEX_INITIALIZER, 0, -1 }, /* DG_LOOP_PORT_GE_2   */
    { PTHREAD_MUTEX_INITIALIZER, 0, -1 }, /* DG_LOOP_PORT_GE_3   */
    { PTHREAD_MUTEX_INITIALIZER, 0, -1 }, /* DG_LOOP_PORT_GE_4   */
    { PTHREAD_MUTEX_INITIALIZER, 0, -1 }, /* DG_LOOP_PORT_GE_5   */
    { PTHREAD_MUTEX_INITIALIZER, 0, -1 }, /* DG_LOOP_PORT_GE_6   */
    { PTHREAD_MUTEX_INITIALIZER, 0, -1 }, /* DG_LOOP_PORT_GE_7   */
    { PTHREAD_MUTEX_INITIALIZER, 0, -1 }, /* DG_LOOP_PORT_GE_8   */
    { PTHREAD_MUTEX_INITIALIZER, 0, -1 }, /* DG_LOOP_PORT_GE_9   */
    { PTHREAD_MUTEX_INITIALIZER, 0, -1 }, /* DG_LOOP_PORT_GE_10  */
    { PTHREAD_MUTEX_INITIALIZER, 0, -1 }, /* DG_LOOP_PORT_GE_11  */
    { PTHREAD_MUTEX_INITIALIZER, 0, -1 }, /* DG_LOOP_PORT_SFP_0  */
    { PTHREAD_MUTEX_INITIALIZER, 0, -1 }, /* DG_LOOP_PORT_SFP_1  */
    { PTHREAD_MUTEX_INITIALIZER, 0, -1 }, /* DG_LOOP_PORT_SFP_2  */
    { PTHREAD_MUTEX_INITIALIZER, 0, -1 }, /* DG_LOOP_PORT_SFP_3  */
    { PTHREAD_MUTEX_INITIALIZER, 0, -1 }, /* DG_LOOP_PORT_10GE_0 */
    { PTHREAD_MUTEX_INITIALIZER, 0, -1 }, /* DG_LOOP_PORT_10GE_1 */
    { PTHREAD_MUTEX_INITIALIZER, 0, -1 }, /* DG_LOOP_PORT_10GE_2 */
    { PTHREAD_MUTEX_INITIALIZER, 0, -1 }  /* DG_LOOP_PORT_10GE_3 */
};

static DG_LOOP_MUTEX_UNLOCK_FUN_T dg_loop_mutex_unlock =
    (DG_LOOP_MUTEX_UNLOCK_FUN_T)&pthread_mutex_unlock;

/*==================================================================================================
                                         GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief map the port id to index

@param[in]  port - the port number according to definition

@return the port index, -1 if invalid.

@note
- index is 0, 1 ... DG_LOOP_PORT_NUM-1
- use this function to check if the port is valid
*//*==============================================================================================*/
int DG_LOOP_port_to_index(DG_LOOP_PORT_T port)
{
    int ret = -1;
    int index;

    for (index = 0; index < DG_LOOP_PORT_NUM; index++)
    {
        if (dg_loop_port_array[index] == port)
        {
            ret = index;
            break;
        }
    }

    return ret;
}

/*=============================================================================================*//**
@brief map the index to port

@param[in]  port - the port number according to definition

@return the port index, 0xFF if invalid index.

@note
- index is 0, 1 ... DG_LOOP_PORT_NUM-1
*//*==============================================================================================*/
DG_LOOP_PORT_T DG_LOOP_index_to_port(int index)
{
    DG_LOOP_PORT_T port = 0xFF;

    if ((index >= 0) && (index < DG_LOOP_PORT_NUM))
    {
        port = dg_loop_port_array[index];
    }

    return port;
}

/*=============================================================================================*//**
@brief open port for send/recv data

@param[in]  port    - the path to open on which port

@return the file descriptor, -1 if error happened

@note
- if error happened, call DG_DBG_get_err_string() to get the last error
*//*==============================================================================================*/
int DG_LOOP_open(DG_LOOP_PORT_T port)
{
    int index;

    pthread_mutex_t* mutex;

    if ((index = DG_LOOP_port_to_index(port)) < 0)
    {
        DG_DBG_set_err_string("Invalid Port to open, port=0x%02x", port);
        return -1;
    }

    mutex = &dg_loop_port_fd[index].mutex;
    DG_LOOP_MUTEX_LOCK(mutex);
    if (dg_loop_port_fd[index].ref <= 0)
    {
        /* open the loop device to simulate the port */
        DG_DBG_TRACE("open loop port 0x%02x", port);
        dg_loop_port_fd[index].fd  = port;
        dg_loop_port_fd[index].ref = 1;
    }
    else
    {
        /* already opened */
        dg_loop_port_fd[index].ref++;
    }
    DG_LOOP_MUTEX_UNLOCK(mutex);

    return index;
}

/*=============================================================================================*//**
@brief close port

@param[in] fd - the fd that opened by DG_LOOP_open()
*//*==============================================================================================*/
void DG_LOOP_close(int fd)
{
    int index = fd;

    pthread_mutex_t* mutex;
    DG_LOOP_PORT_T   port;

    if ((port = DG_LOOP_index_to_port(index)) == 0xFF)
    {
        DG_DBG_set_err_string("Invalid Port fd to close, fd=%d", fd);
        return;
    }

    mutex = &dg_loop_port_fd[index].mutex;
    DG_LOOP_MUTEX_LOCK(mutex);
    if (dg_loop_port_fd[index].ref > 0)
    {
        dg_loop_port_fd[index].ref--;
    }

    if (dg_loop_port_fd[index].ref == 0)
    {
        /* close the fd */
        /* close(dg_loop_port_fd[index].fd); */
        DG_DBG_TRACE("close loop port 0x%02x", port);
        dg_loop_port_fd[index].fd = -1;
    }
    DG_LOOP_MUTEX_UNLOCK(mutex);
}

/*=============================================================================================*//**
@brief send data over the loopback file descriptor

@param[in]  fd      - the fd that opened by DG_LOOP_open()
@param[in]  buf     - the buffer contains the data to send
@param[in]  len     - the data length need to send

@return the file descriptor, -1 if error happened

@note
- if error happened, call DG_DBG_get_err_string() to get the last error
*//*==============================================================================================*/
BOOL DG_LOOP_send(int fd, UINT8* buf, UINT32 len)
{
    int index = fd;

    DG_LOOP_PORT_T port;

    if ((port = DG_LOOP_index_to_port(index)) == 0xFF)
    {
        DG_DBG_set_err_string("Invalid Port fd to send, fd=%d", fd);
        return FALSE;
    }
    else if (dg_loop_port_fd[index].ref <= 0)
    {
        DG_DBG_set_err_string("the fd is closed, fd=%d", fd);
        return FALSE;
    }

    DG_DBG_TRACE("send to port 0x%02x, buf=%p, len=%d", port, buf, len);

    usleep(1000);

    return TRUE;
}

/*=============================================================================================*//**
@brief receive data from the loopback file descriptor

@param[in]  fd      - the fd that opened by DG_LOOP_open()
@param[out] buf     - the buffer to receive data
@param[in]  len     - the data length need to receive

@return the file descriptor, -1 if error happened

@note
- if error happened, call DG_DBG_get_err_string() to get the last error
*//*==============================================================================================*/
BOOL DG_LOOP_recv(int fd, UINT8* buf, UINT32 len)
{
    int index = fd;

    DG_LOOP_PORT_T port;

    if ((port = DG_LOOP_index_to_port(index)) == 0xFF)
    {
        DG_DBG_set_err_string("Invalid Port fd to recv, fd=%d", fd);
        return FALSE;
    }
    else if (dg_loop_port_fd[index].ref <= 0)
    {
        DG_DBG_set_err_string("the fd is closed, fd=%d", fd);
        return FALSE;
    }

    DG_DBG_TRACE("recv from port 0x%02x, buf=%p, len=%d", port, buf, len);

    usleep(1000);

    return TRUE;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */

