/*==================================================================================================

    Module Name:  dg_loop_api.c

    General Description: Implements the dg_loop library

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include "dg_platform_defs.h"
#include "dg_dbg.h"
#include "dg_loop.h"
#include "dg_loop_priv.h"

/** @addtogroup libdg_loop
@{
*/
/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
struct DG_LOOP_PORT_FD_S;

typedef struct
{
    BOOL (* open)(struct DG_LOOP_PORT_FD_S* fd);
    void (* close)(struct DG_LOOP_PORT_FD_S* fd);
    BOOL (* send)(struct DG_LOOP_PORT_FD_S* fd, UINT32 size, UINT8* data);
    BOOL (* recv)(struct DG_LOOP_PORT_FD_S* fd, UINT32 size, UINT8* data);
} DG_LOOP_PORT_OP_T;

typedef struct DG_LOOP_PORT_FD_S
{
    DG_LOOP_PORT_T port;   /* the stored port  */

    int ref;               /* reference count  */
    int tx_fd;             /* the actual tx fd */
    int rx_fd;             /* the actual rx fd */

    DG_LOOP_PORT_OP_T* op;
    pthread_mutex_t    mutex; /* mutex protection */
} DG_LOOP_PORT_FD_T;

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static BOOL dg_loop_open_sim(DG_LOOP_PORT_FD_T* fd);
static void dg_loop_close_sys(DG_LOOP_PORT_FD_T* fd);
static BOOL dg_loop_write_sim(DG_LOOP_PORT_FD_T* fd, UINT32 bytes_to_write, UINT8* data);
static BOOL dg_loop_read_sim(DG_LOOP_PORT_FD_T* fd, UINT32 bytes_to_read, UINT8* data);

/*==================================================================================================
                                         GLOBAL VARIABLES
==================================================================================================*/
DG_LOOP_MUTEX_UNLOCK_FUN_T dg_loop_mutex_unlock = (DG_LOOP_MUTEX_UNLOCK_FUN_T)&pthread_mutex_unlock;

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/
static DG_LOOP_PORT_OP_T dg_loop_port_sim_op =
{
    dg_loop_open_sim, dg_loop_close_sys, dg_loop_write_sim, dg_loop_read_sim
};

/** internal real file descriptor array for each ports */
static DG_LOOP_PORT_FD_T dg_loop_port_fd[DG_LOOP_PORT_NUM] =
{
    { DG_LOOP_PORT_MGT,    0, -1, -1, &dg_loop_port_sim_op, PTHREAD_MUTEX_INITIALIZER },
    { DG_LOOP_PORT_HA,     0, -1, -1, &dg_loop_port_sim_op, PTHREAD_MUTEX_INITIALIZER },
    { DG_LOOP_PORT_WTB0_1, 0, -1, -1, &dg_loop_port_sim_op, PTHREAD_MUTEX_INITIALIZER },
    { DG_LOOP_PORT_WTB0_2, 0, -1, -1, &dg_loop_port_sim_op, PTHREAD_MUTEX_INITIALIZER },
    { DG_LOOP_PORT_WTB1_1, 0, -1, -1, &dg_loop_port_sim_op, PTHREAD_MUTEX_INITIALIZER },
    { DG_LOOP_PORT_WTB1_2, 0, -1, -1, &dg_loop_port_sim_op, PTHREAD_MUTEX_INITIALIZER },
    { DG_LOOP_PORT_GE_0,   0, -1, -1, &dg_loop_port_sim_op, PTHREAD_MUTEX_INITIALIZER },
    { DG_LOOP_PORT_GE_1,   0, -1, -1, &dg_loop_port_sim_op, PTHREAD_MUTEX_INITIALIZER },
    { DG_LOOP_PORT_GE_2,   0, -1, -1, &dg_loop_port_sim_op, PTHREAD_MUTEX_INITIALIZER },
    { DG_LOOP_PORT_GE_3,   0, -1, -1, &dg_loop_port_sim_op, PTHREAD_MUTEX_INITIALIZER },
    { DG_LOOP_PORT_GE_4,   0, -1, -1, &dg_loop_port_sim_op, PTHREAD_MUTEX_INITIALIZER },
    { DG_LOOP_PORT_GE_5,   0, -1, -1, &dg_loop_port_sim_op, PTHREAD_MUTEX_INITIALIZER },
    { DG_LOOP_PORT_GE_6,   0, -1, -1, &dg_loop_port_sim_op, PTHREAD_MUTEX_INITIALIZER },
    { DG_LOOP_PORT_GE_7,   0, -1, -1, &dg_loop_port_sim_op, PTHREAD_MUTEX_INITIALIZER },
    { DG_LOOP_PORT_GE_8,   0, -1, -1, &dg_loop_port_sim_op, PTHREAD_MUTEX_INITIALIZER },
    { DG_LOOP_PORT_GE_9,   0, -1, -1, &dg_loop_port_sim_op, PTHREAD_MUTEX_INITIALIZER },
    { DG_LOOP_PORT_GE_10,  0, -1, -1, &dg_loop_port_sim_op, PTHREAD_MUTEX_INITIALIZER },
    { DG_LOOP_PORT_GE_11,  0, -1, -1, &dg_loop_port_sim_op, PTHREAD_MUTEX_INITIALIZER },
    { DG_LOOP_PORT_SFP_0,  0, -1, -1, &dg_loop_port_sim_op, PTHREAD_MUTEX_INITIALIZER },
    { DG_LOOP_PORT_SFP_1,  0, -1, -1, &dg_loop_port_sim_op, PTHREAD_MUTEX_INITIALIZER },
    { DG_LOOP_PORT_SFP_2,  0, -1, -1, &dg_loop_port_sim_op, PTHREAD_MUTEX_INITIALIZER },
    { DG_LOOP_PORT_SFP_3,  0, -1, -1, &dg_loop_port_sim_op, PTHREAD_MUTEX_INITIALIZER },
    { DG_LOOP_PORT_10GE_0, 0, -1, -1, &dg_loop_port_sim_op, PTHREAD_MUTEX_INITIALIZER },
    { DG_LOOP_PORT_10GE_1, 0, -1, -1, &dg_loop_port_sim_op, PTHREAD_MUTEX_INITIALIZER },
    { DG_LOOP_PORT_10GE_2, 0, -1, -1, &dg_loop_port_sim_op, PTHREAD_MUTEX_INITIALIZER },
    { DG_LOOP_PORT_10GE_3, 0, -1, -1, &dg_loop_port_sim_op, PTHREAD_MUTEX_INITIALIZER }
};

/*==================================================================================================
                                         GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief check the validation of the port

@param[in]  port - the port number according to definition

@return -1 if invalid, otherwise the internal index start from 0

@note
- index is 0, 1 ... DG_LOOP_PORT_NUM-1
- use this function to check if the port is valid
*//*==============================================================================================*/
int DG_LOOP_check_port(DG_LOOP_PORT_T port)
{
    /* port index map */
    enum
    {
        DG_LOOP_INDEX_MGT = 0,
        DG_LOOP_INDEX_HA,
        DG_LOOP_INDEX_WTB0_1,
        DG_LOOP_INDEX_WTB0_2,
        DG_LOOP_INDEX_WTB1_1,
        DG_LOOP_INDEX_WTB1_2,
        DG_LOOP_INDEX_GE_0,
        DG_LOOP_INDEX_GE_1,
        DG_LOOP_INDEX_GE_2,
        DG_LOOP_INDEX_GE_3,
        DG_LOOP_INDEX_GE_4,
        DG_LOOP_INDEX_GE_5,
        DG_LOOP_INDEX_GE_6,
        DG_LOOP_INDEX_GE_7,
        DG_LOOP_INDEX_GE_8,
        DG_LOOP_INDEX_GE_9,
        DG_LOOP_INDEX_GE_10,
        DG_LOOP_INDEX_GE_11,
        DG_LOOP_INDEX_SFP_0,
        DG_LOOP_INDEX_SFP_1,
        DG_LOOP_INDEX_SFP_2,
        DG_LOOP_INDEX_SFP_3,
        DG_LOOP_INDEX_10GE_0,
        DG_LOOP_INDEX_10GE_1,
        DG_LOOP_INDEX_10GE_2,
        DG_LOOP_INDEX_10GE_3,
    };

#define INDEX_MAPPER(x) case DG_LOOP_PORT_ ## x: \
    return DG_LOOP_INDEX_ ## x
    switch (port)
    {
        INDEX_MAPPER(MGT);
        INDEX_MAPPER(HA);
        INDEX_MAPPER(WTB0_1);
        INDEX_MAPPER(WTB0_2);
        INDEX_MAPPER(WTB1_1);
        INDEX_MAPPER(WTB1_2);
        INDEX_MAPPER(GE_0);
        INDEX_MAPPER(GE_1);
        INDEX_MAPPER(GE_2);
        INDEX_MAPPER(GE_3);
        INDEX_MAPPER(GE_4);
        INDEX_MAPPER(GE_5);
        INDEX_MAPPER(GE_6);
        INDEX_MAPPER(GE_7);
        INDEX_MAPPER(GE_8);
        INDEX_MAPPER(GE_9);
        INDEX_MAPPER(GE_10);
        INDEX_MAPPER(GE_11);
        INDEX_MAPPER(SFP_0);
        INDEX_MAPPER(SFP_1);
        INDEX_MAPPER(SFP_2);
        INDEX_MAPPER(SFP_3);
        INDEX_MAPPER(10GE_0);
        INDEX_MAPPER(10GE_1);
        INDEX_MAPPER(10GE_2);
        INDEX_MAPPER(10GE_3);
    }
#undef INDEX_MAPPER

    return -1;
}

/*=============================================================================================*//**
@brief connect two ports

@param[in] port1 - the path to open on which port

@return TRUE if success

@note
- this function is only for simulation
*//*==============================================================================================*/
BOOL DG_LOOP_connect(DG_LOOP_PORT_T port1, DG_LOOP_PORT_T port2)
{
    int index1;
    int index2;

    if ((index1 = DG_LOOP_check_port(port1)) < 0)
    {
        DG_DBG_set_err_string("invalid connect: port1=0x%02x", port1);
        return FALSE;
    }

    if ((index2 = DG_LOOP_check_port(port2)) < 0)
    {
        DG_DBG_set_err_string("invalid connect: port2=0x%02x", port2);
        return FALSE;
    }

    if ((dg_loop_port_fd[index1].tx_fd > 0) ||
        (dg_loop_port_fd[index1].rx_fd > 0) ||
        (dg_loop_port_fd[index2].tx_fd > 0) ||
        (dg_loop_port_fd[index2].rx_fd > 0))
    {
        DG_DBG_TRACE("already connected. port1=0x%02x port2=0x%02x", port1, port2);
        return TRUE;
    }

    /* only simulation port support this action */
    if ((dg_loop_port_fd[index1].op != &dg_loop_port_sim_op) ||
        (dg_loop_port_fd[index2].op != &dg_loop_port_sim_op))
    {
        return FALSE;
    }

    if ((DG_LOOP_open(port1) < 0) || (DG_LOOP_open(port2) < 0))
    {
        DG_LOOP_close(index1);
        DG_LOOP_close(index2);
        return FALSE;
    }

    DG_DBG_TRACE("port1=0x%02x port2=0x%02x connected", port1, port2);
    /* Save the sockets */
    if (port1 != port2)
    {
        /* save one socket pair */
        int socket0 = dg_loop_port_fd[index1].tx_fd;
        int socket1 = dg_loop_port_fd[index1].rx_fd;

        /* close one socket pair */
        close(dg_loop_port_fd[index2].tx_fd);
        close(dg_loop_port_fd[index2].rx_fd);

        dg_loop_port_fd[index1].tx_fd = socket0;
        dg_loop_port_fd[index2].rx_fd = socket1;

        dg_loop_port_fd[index2].tx_fd = socket1;
        dg_loop_port_fd[index1].rx_fd = socket0;
    }

    return TRUE;
}

/*=============================================================================================*//**
@brief disconnect all the ports

@note
- this function is only for simulation
*//*==============================================================================================*/
void DG_LOOP_disconnect_all()
{
    int index;

    for (index = 0; index < DG_LOOP_PORT_NUM; index++)
    {
        DG_LOOP_close(index);
    }
}

/*=============================================================================================*//**
@brief open port for send/recv data

@param[in]  port - the path to open on which port

@return the file descriptor, -1 if error happened

@note
- if error happened, call DG_DBG_get_err_string() to get the last error
*//*==============================================================================================*/
int DG_LOOP_open(DG_LOOP_PORT_T port)
{
    int index;

    pthread_mutex_t* mutex;

    if ((index = DG_LOOP_check_port(port)) < 0)
    {
        DG_DBG_set_err_string("Invalid Port to open, port=0x%02x", port);
        return -1;
    }

    if (dg_loop_port_fd[index].op == NULL)
    {
        DG_DBG_set_err_string("Unsupported Port to open, port=0x%02x", port);
        return -1;
    }

    mutex = &dg_loop_port_fd[index].mutex;
    DG_LOOP_MUTEX_LOCK(mutex);
    if (dg_loop_port_fd[index].ref <= 0)
    {
        if (dg_loop_port_fd[index].op->open(&dg_loop_port_fd[index]))
        {
            DG_DBG_TRACE("open loop port 0x%02x", port);
            dg_loop_port_fd[index].ref = 1;
        }
        else
        {
            DG_DBG_ERROR("failed to open, port=0x%02x", port);
            index = -1;
        }
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

    if ((port = dg_loop_index_to_port(index)) == 0xFF)
    {
        DG_DBG_set_err_string("Invalid Port fd to close, fd=%d", fd);
        return;
    }

    if (dg_loop_port_fd[index].op == NULL)
    {
        DG_DBG_set_err_string("Unsupported Port to close, port=0x%02x", port);
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
        dg_loop_port_fd[index].op->close(&dg_loop_port_fd[index]);
        DG_DBG_TRACE("close loop port 0x%02x", port);
    }
    DG_LOOP_MUTEX_UNLOCK(mutex);
}

/*=============================================================================================*//**
@brief send data over the loopback file descriptor

@param[in]  fd  - the fd that opened by DG_LOOP_open()
@param[in]  buf - the buffer contains the data to send
@param[in]  len - the data length need to send

@return the file descriptor, -1 if error happened

@note
- if error happened, call DG_DBG_get_err_string() to get the last error
*//*==============================================================================================*/
BOOL DG_LOOP_send(int fd, UINT8* buf, UINT32 len)
{
    BOOL ret   = FALSE;
    int  index = fd;

    DG_LOOP_PORT_T port;

    if ((port = dg_loop_index_to_port(index)) == 0xFF)
    {
        DG_DBG_set_err_string("Invalid Port fd to send, fd=%d", fd);
        return FALSE;
    }
    else if (dg_loop_port_fd[index].op == NULL)
    {
        DG_DBG_set_err_string("Unsupported Port to send, port=0x%02x", port);
        return FALSE;
    }
    else if (dg_loop_port_fd[index].ref <= 0)
    {
        DG_DBG_set_err_string("the fd is closed, fd=%d", fd);
        return FALSE;
    }

    if (dg_loop_port_fd[index].op->send(&dg_loop_port_fd[index], len, buf))
    {
        ret = TRUE;
    }

    return ret;
}

/*=============================================================================================*//**
@brief receive data from the loopback file descriptor

@param[in]  fd  - the fd that opened by DG_LOOP_open()
@param[out] buf - the buffer to receive data
@param[in]  len - the data length need to receive

@return the file descriptor, -1 if error happened

@note
- if error happened, call DG_DBG_get_err_string() to get the last error
*//*==============================================================================================*/
BOOL DG_LOOP_recv(int fd, UINT8* buf, UINT32 len)
{
    BOOL ret   = FALSE;
    int  index = fd;

    DG_LOOP_PORT_T port;

    if ((port = dg_loop_index_to_port(index)) == 0xFF)
    {
        DG_DBG_set_err_string("Invalid Port fd to recv, fd=%d", fd);
        return FALSE;
    }
    else if (dg_loop_port_fd[index].op == NULL)
    {
        DG_DBG_set_err_string("Unsupported Port to recv, port=0x%02x", port);
        return FALSE;
    }
    else if (dg_loop_port_fd[index].ref <= 0)
    {
        DG_DBG_set_err_string("the fd is closed, fd=%d", fd);
        return FALSE;
    }

    if (dg_loop_port_fd[index].op->recv(&dg_loop_port_fd[index], len, buf))
    {
        ret = TRUE;
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief map the index to port

@param[in]  port - the port number according to definition

@return the port index, 0xFF if invalid index.

@note
- index is 0, 1 ... DG_LOOP_PORT_NUM-1
*//*==============================================================================================*/
DG_LOOP_PORT_T dg_loop_index_to_port(int index)
{
    DG_LOOP_PORT_T port = 0xFF;

    if ((index >= 0) && (index < DG_LOOP_PORT_NUM))
    {
        port = dg_loop_port_fd[index].port;
    }

    return port;
}

/*=============================================================================================*//**
@brief open simulation port implementation

@param[out] fd - the fd

@return TRUE if success

@note
- if error happened, call DG_DBG_get_err_string() to get the last error
*//*==============================================================================================*/
BOOL dg_loop_open_sim(DG_LOOP_PORT_FD_T* fd)
{
    int sockets[2];

    if ((fd->tx_fd > 0) || (fd->rx_fd > 0))
    {
        DG_DBG_TRACE("already opened. port=0x%02x", fd->port);
        return TRUE;
    }

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) != 0)
    {
        DG_DBG_set_err_string("Failed to create connected sockets, errno=%d(%m)", errno);
        return FALSE;
    }

    DG_DBG_TRACE("port=0x%02x opened", fd->port);

    /* self connected by default */
    fd->tx_fd = sockets[0];
    fd->rx_fd = sockets[1];

    return TRUE;
}

/*=============================================================================================*//**
@brief close port implementation

@param[int] fd   - the fd
*//*==============================================================================================*/
void dg_loop_close_sys(DG_LOOP_PORT_FD_T* fd)
{
    if (fd->tx_fd > 0)
    {
        close(fd->tx_fd);
        fd->tx_fd = -1;
    }

    if (fd->rx_fd > 0)
    {
        close(fd->rx_fd);
        fd->rx_fd = -1;
    }
}

/*=============================================================================================*//**
@brief Writes the specified number of bytes to simulation port

@param[in] fd             - The port fd
@param[in] bytes_to_write - The number of bytes to write
@param[in] data           - Data to write

@return TRUE = success, FALSE = failure

@note
 - The write is synchronous, the function will block until the requested number of bytes are written
*//*==============================================================================================*/
BOOL dg_loop_write_sim(DG_LOOP_PORT_FD_T* fd, UINT32 bytes_to_write, UINT8* data)
{
    BOOL is_success = FALSE;

    if (write(fd->tx_fd, data, bytes_to_write) == (ssize_t)bytes_to_write)
    {
        is_success = TRUE;
    }

    return is_success;
}

/*=============================================================================================*//**
@brief Reads the specified number of bytes from simulation port

@param[in]  fd            - port fd
@param[in]  bytes_to_read - The number of bytes to read
@param[out] data          - Data read

@return TRUE = success, FALSE = failure

@note
 - The read is synchronous, use select/port before read
*//*==============================================================================================*/
BOOL dg_loop_read_sim(DG_LOOP_PORT_FD_T* fd, UINT32 bytes_to_read, UINT8* data)
{
    BOOL           is_success         = TRUE;
    int            total_bytes_read   = 0;
    int            current_bytes_read = 0;
    struct timeval timeout;
    fd_set         fd_set;
    int            select_status;

    timeout.tv_sec  = 0;
    timeout.tv_usec = 100000;

    /* Wait for data to be available to read */
    FD_ZERO(&fd_set);
    FD_SET(fd->rx_fd, &fd_set);

    /* Continue to read until an error occurs or we read the desired number of bytes */
    while (is_success && (total_bytes_read != (int)bytes_to_read))
    {
        select_status = select(fd->rx_fd + 1, &fd_set, NULL, NULL, &timeout);

        if (select_status == 0)
        {
            DG_DBG_ERROR("read timeout occurred!");
            return FALSE;
        }
        else if (select_status != 1)
        {
            DG_DBG_ERROR("Select failed, select_status=%d. errno=%d(%m)", select_status, errno);
            return FALSE;
        }

        current_bytes_read = read(fd->rx_fd,
                                  ((UINT8*)data + total_bytes_read),
                                  (bytes_to_read - total_bytes_read));
        if (current_bytes_read > 0)
        {
            total_bytes_read += current_bytes_read;
        }
        else
        {
            /* Only print errors if the current_bytes_read is NOT 0.  0 bytes most likely
               indicates client closed its connection */
            if (current_bytes_read != 0)
            {
                DG_DBG_ERROR("Read port 0x%02x failed, current_bytes_read=%d, errno=%d(%m)",
                             fd->port, current_bytes_read, errno);
                is_success = FALSE;
            }
        }
    }

    return is_success;
}

/** @} */

