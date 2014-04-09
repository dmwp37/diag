/*==================================================================================================

    Module Name:  dg_loop.c

    General Description: Implements the dg_loop library

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <poll.h>
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
#define DG_LOOP_ETH_FRAME_LEN 9230

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
typedef struct
{
    DG_LOOP_PORT_T  port;  /* the stored port  */
    int             ref;   /* reference count  */
    int             tx_fd; /* the actual tx fd */
    int             rx_fd; /* the actual rx fd */
    char*           name;  /* the if name      */
    pthread_mutex_t mutex; /* mutex protection */
} DG_LOOP_PORT_FD_T;

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static BOOL dg_loop_open_impl(DG_LOOP_PORT_FD_T* fd);
static void dg_loop_close_impl(DG_LOOP_PORT_FD_T* fd);
static BOOL dg_loop_write_impl(DG_LOOP_PORT_FD_T* fd, UINT32 bytes_to_write, UINT8* data);
static BOOL dg_loop_read_impl(DG_LOOP_PORT_FD_T* fd, UINT32 bytes_to_read, UINT8* data);

/*==================================================================================================
                                         GLOBAL VARIABLES
==================================================================================================*/
DG_LOOP_MUTEX_UNLOCK_FUN_T dg_loop_mutex_unlock = (DG_LOOP_MUTEX_UNLOCK_FUN_T)&pthread_mutex_unlock;

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/
/** internal real file descriptor array for each ports */
static DG_LOOP_PORT_FD_T dg_loop_port_fd[DG_LOOP_PORT_NUM] =
{
    { DG_LOOP_PORT_I347_0, 0, -1, -1, "eth3", PTHREAD_MUTEX_INITIALIZER },
    { DG_LOOP_PORT_I347_1, 0, -1, -1, "eth2", PTHREAD_MUTEX_INITIALIZER },
    { DG_LOOP_PORT_I347_2, 0, -1, -1, "eth5", PTHREAD_MUTEX_INITIALIZER },
    { DG_LOOP_PORT_I347_3, 0, -1, -1, "eth4", PTHREAD_MUTEX_INITIALIZER },
};

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
        if (dg_loop_port_fd[index].port == port)
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
        port = dg_loop_port_fd[index].port;
    }

    return port;
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
    DG_COMPILE_UNUSED(port1);
    DG_COMPILE_UNUSED(port2);
    return TRUE;
}

/*=============================================================================================*//**
@brief disconnect all the ports

@note
- this function is only for simulation
*//*==============================================================================================*/
void DG_LOOP_disconnect_all()
{
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

    if ((index = DG_LOOP_port_to_index(port)) < 0)
    {
        DG_DBG_set_err_string("Invalid Port to open, port=0x%02x", port);
        return -1;
    }

    mutex = &dg_loop_port_fd[index].mutex;
    DG_LOOP_MUTEX_LOCK(mutex);
    if (dg_loop_port_fd[index].ref <= 0)
    {
        if (dg_loop_open_impl(&dg_loop_port_fd[index]))
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
        dg_loop_close_impl(&dg_loop_port_fd[index]);
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

    if (dg_loop_write_impl(&dg_loop_port_fd[index], len, buf))
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

    if (dg_loop_read_impl(&dg_loop_port_fd[index], len, buf))
    {
        ret = TRUE;
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief open port implementation

@param[out] fd - the fd

@return TRUE if success

@note
- if error happened, call DG_DBG_get_err_string() to get the last error
*//*==============================================================================================*/
BOOL dg_loop_open_impl(DG_LOOP_PORT_FD_T* fd)
{
    struct ifreq       ifr;
    struct sockaddr_ll socket_address;
    struct packet_mreq mr;

    if ((fd->tx_fd = socket(PF_PACKET, SOCK_RAW, 0)) < 0)
    {
        DG_DBG_set_err_string("dg_loop_open_impl tx socket create error, port=0x%02x", fd->port);
        goto on_error;
    }

    /* Open RAW socket to send on */
    if ((fd->rx_fd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_DIAG))) < 0)
    {
        DG_DBG_set_err_string("dg_loop_open_impl rx socket create error, port=0x%02x", fd->port);
        goto on_error;
    }

    /* Get the index of the interface to receive on */
    memset(&ifr, 0, sizeof(struct ifreq));
    strncpy(ifr.ifr_name, fd->name, IFNAMSIZ);
    if (ioctl(fd->tx_fd, SIOCGIFINDEX, &ifr) < 0)
    {
        DG_DBG_set_err_string("get if index error, port=0x%02x", fd->port);
        goto on_error;
    }

    memset(&socket_address, 0, sizeof(socket_address));
    /* Index of the network device */
    socket_address.sll_ifindex = ifr.ifr_ifindex;
    /* RAW communication */
    socket_address.sll_family = AF_PACKET;

    if (bind(fd->rx_fd, (struct sockaddr*)&socket_address, sizeof(socket_address)) < 0)
    {
        DG_DBG_set_err_string("rx socket bind error, port=0x%02x", fd->port);
        goto on_error;
    }

    if (bind(fd->tx_fd, (struct sockaddr*)&socket_address, sizeof(socket_address)))
    {
        DG_DBG_set_err_string("tx socket bind error, port=0x%02x", fd->port);
        goto on_error;
    }

    memset(&mr, 0, sizeof(mr));
    mr.mr_ifindex = ifr.ifr_ifindex;
    mr.mr_type    = PACKET_MR_PROMISC;

    if (setsockopt(fd->rx_fd, SOL_PACKET, PACKET_ADD_MEMBERSHIP, &mr, sizeof(mr)) < 0)
    {
        DG_DBG_set_err_string("set rx socket to PROMISC failed, port=0x%02x. errno=%d(%m)",
                              fd->port, errno);
        goto on_error;
    }

    /* open the loop device to simulate the port */
    DG_DBG_TRACE("open loop port=0x%02x", fd->port);

    return TRUE;

on_error:
    if (fd->tx_fd > 0)
    {
        close(fd->tx_fd);
        fd->tx_fd = -1;
    }

    if (fd->tx_fd > 0)
    {
        close(fd->rx_fd);
        fd->rx_fd = -1;
    }
    return FALSE;
}

/*=============================================================================================*//**
@brief close port implementation

@param[int] fd   - the fd
*//*==============================================================================================*/
void dg_loop_close_impl(DG_LOOP_PORT_FD_T* fd)
{
    /* close the fd */
    if (fd->tx_fd > 0)
    {
        close(fd->tx_fd);
        fd->tx_fd = -1;
    }

    if (fd->tx_fd > 0)
    {
        close(fd->rx_fd);
        fd->rx_fd = -1;
    }
    DG_DBG_TRACE("close loop port=0x%02x", fd->port);
}

/*=============================================================================================*//**
@brief Writes the specified number of bytes to the specified port

@param[in] fd             - The port fd
@param[in] bytes_to_write - The number of bytes to write
@param[in] data           - Data to write

@return TRUE = success, FALSE = failure

@note
 - The write is synchronous, the function will block until the requested number of bytes are written
*//*==============================================================================================*/
BOOL dg_loop_write_impl(DG_LOOP_PORT_FD_T* fd, UINT32 len, UINT8* buf)
{
    BOOL                 ret              = FALSE;
    struct ether_header* eh               = (struct ether_header*)buf;
    unsigned char        mac_broadcast[6] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

    /* Construct the Ethernet header */
    memset(buf, 0, ETH_HLEN);
    memcpy(eh->ether_dhost, mac_broadcast, ETH_ALEN);
    eh->ether_type = htons(ETH_P_DIAG);

    if (send(fd->tx_fd, buf, len, 0) < 0)
    {
        DG_DBG_set_err_string("DG_LOOP_send send error, port=0x%02x. errno=%d(%m)",
                              fd->port, errno);
    }
    else
    {
        ret = TRUE;
    }

    return ret;
}

/*=============================================================================================*//**
@brief Reads the specified number of bytes from the specified port

@param[in]  fd            - port fd
@param[in]  bytes_to_read - The number of bytes to read
@param[out] data          - Data read

@return TRUE = success, FALSE = failure

@note
 - The read is synchronous, use select/port before read
*//*==============================================================================================*/
BOOL dg_loop_read_impl(DG_LOOP_PORT_FD_T* fd, UINT32 len, UINT8* buf)
{
    UINT32 rx_len;
    int    status;
    BOOL   ret = FALSE;

    struct pollfd pf = { fd->rx_fd, POLLIN | POLLERR | POLLRDNORM, 0 };

    status = poll(&pf, 1, 500);

    if (status < 0)
    {
        DG_DBG_set_err_string("recv poll error, port=0x%02x. errno=%d(%m)",
                              fd->port, errno);
    }
    else if (status == 0)
    {
        DG_DBG_set_err_string("recv poll timed out! port=0x%02x", fd->port);
    }
    else
    {
        rx_len = recv(fd->rx_fd, buf, DG_LOOP_ETH_FRAME_LEN, MSG_TRUNC);
        if (len != rx_len)
        {
            DG_DBG_set_err_string("port=0x%02x received %d bytes, should receive %d bytes",
                                  fd->port, rx_len, len);
        }
        else
        {
            ret = TRUE;
        }
    }

    return ret;
}

/** @} */

