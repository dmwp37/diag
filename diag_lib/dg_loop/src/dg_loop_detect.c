/*==================================================================================================

    Module Name:  dg_loop_detect.c

    General Description: Implements the loop connection auto detection

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
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
#define DG_LOOP_DETECT_PACKET_SIZE 666
#define DG_LOOP_DETECT_HDR_OFFSET  400
#define DG_LOOP_DETECT_FILL        0xBE

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static void* dg_loop_detect_thread(void* arg);
static BOOL  dg_loop_is_detect_packet(UINT8* buf, UINT8* tx_port, UINT8* rx_port);

/*==================================================================================================
                                         GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/
static DG_LOOP_PORT_PAIR_T dg_loop_detect_info[DG_LOOP_PORT_NUM] =
{
#define PORT_PAIR_INIT(x) [DG_LOOP_PORT_ ## x] = { DG_LOOP_PORT_ ## x, 0xFF }

    PORT_PAIR_INIT(mgt),
    PORT_PAIR_INIT(ha),
    PORT_PAIR_INIT(wtb0),
    PORT_PAIR_INIT(wtb1),
    PORT_PAIR_INIT(ge_0),
    PORT_PAIR_INIT(ge_1),
    PORT_PAIR_INIT(ge_2),
    PORT_PAIR_INIT(ge_3),
    PORT_PAIR_INIT(ge_4),
    PORT_PAIR_INIT(ge_5),
    PORT_PAIR_INIT(ge_6),
    PORT_PAIR_INIT(ge_7),
    PORT_PAIR_INIT(ge_8),
    PORT_PAIR_INIT(ge_9),
    PORT_PAIR_INIT(ge_10),
    PORT_PAIR_INIT(ge_11),
    PORT_PAIR_INIT(ge_12),
    PORT_PAIR_INIT(ge_13),
    PORT_PAIR_INIT(ge_14),
    PORT_PAIR_INIT(ge_15),
    PORT_PAIR_INIT(ge_16),
    PORT_PAIR_INIT(ge_24),
    PORT_PAIR_INIT(xe_0),
    PORT_PAIR_INIT(xe_1),
    PORT_PAIR_INIT(xe_2),
    PORT_PAIR_INIT(xe_3)

#undef PORT_PAIR_INIT
};

/*==================================================================================================
                                         GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief auto detect the loop connection and fill the port pair

@return the port pair array, NULL if failed

*//*==============================================================================================*/
DG_LOOP_PORT_PAIR_T* DG_LOOP_auto_detect()
{
    int       port;
    int       fd;
    int       old_dbg_lvl;
    UINT8*    packet = malloc(DG_LOOP_DETECT_PACKET_SIZE);
    UINT8*    p_tx   = packet + DG_LOOP_DETECT_HDR_OFFSET;
    pthread_t detect_thread[DG_LOOP_PORT_NUM] = { 0 };

    if (packet == NULL)
    {
        return NULL;
    }

    memset(packet, DG_LOOP_DETECT_FILL, DG_LOOP_DETECT_PACKET_SIZE);

    old_dbg_lvl = DG_DBG_get_dbg_level();
    DG_DBG_set_dbg_level(DG_DBG_LVL_DISABLE);

    /* start the detection threads */
    for (port = 0; port < DG_LOOP_PORT_NUM; port++)
    {
        if (pthread_create(&detect_thread[port], NULL,
                           dg_loop_detect_thread, (void*)(intptr_t)port) != 0)
        {
            DG_DBG_ERROR("failed to start detect thread, rx_port=0x%02x, errno=%d(%m)", port, errno);
        }
    }

    /* send the detection packet */
    for (port = 0; port < DG_LOOP_PORT_NUM; port++)
    {
        *p_tx = port;

        if ((fd = DG_LOOP_open(port)) < 0)
        {
            DG_DBG_ERROR("failed to open tx port, tx_port=0x%02x", port);
        }
        else if (!DG_LOOP_send(fd, packet, DG_LOOP_DETECT_PACKET_SIZE))
        {
            DG_DBG_ERROR("failed to send detection packet, tx_port=0x%02x", port);
        }

        if (fd >= 0)
        {
            DG_LOOP_close(fd);
        }
    }

    /* wait the detection threads */
    for (port = 0; port < DG_LOOP_PORT_NUM; port++)
    {
        if (detect_thread[port] != 0)
        {
            pthread_join(detect_thread[port], NULL);
        }
    }

    DG_DBG_set_dbg_level(old_dbg_lvl);

    return dg_loop_detect_info;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief recv packet thread function

@param[in] arg - the test parameter

@return NULL returned
*//*==============================================================================================*/
void* dg_loop_detect_thread(void* arg)
{
    int    fd;
    UINT32 size     = DG_LOOP_DETECT_PACKET_SIZE;
    UINT8* recv_buf = NULL;

    DG_LOOP_PORT_T m_port = (DG_LOOP_PORT_T)(intptr_t)arg;
    DG_LOOP_PORT_T tx_port;
    DG_LOOP_PORT_T rx_port = m_port;

    DG_DBG_TRACE("enter into detect thread: %p", (void*)pthread_self());

    /* open the port for receiving data */
    if ((fd = DG_LOOP_open(rx_port)) < 0)
    {
        DG_DBG_ERROR("failed to open recv port, rx_port=0x%02x", rx_port);
        goto detect_finish;
    }

    /* prepare the data to send */
    if ((recv_buf = malloc(size)) == NULL)
    {
        DG_DBG_ERROR("failed to malloc recv buf, size=%d", size);
        goto detect_finish;
    }

    while (TRUE)
    {
        /* init the buffer with different data first */
        memset(recv_buf, 0, size);

        if (!DG_LOOP_recv(fd, recv_buf, size))
        {
            DG_DBG_TRACE("port %s failed to recv packet", DG_LOOP_port_name(m_port));
            break;
        }
        else if (!dg_loop_is_detect_packet(recv_buf, &tx_port, &rx_port))
        {
            DG_DBG_TRACE("port %s recv wrong packet", DG_LOOP_port_name(m_port));
        }
        else
        {
            DG_DBG_TRACE("port %s recv a valid detection packet", DG_LOOP_port_name(m_port));
            /* save the detection */
            if (dg_loop_detect_info[tx_port].rx_port == 0xFF)
            {
                dg_loop_detect_info[tx_port].rx_port = m_port;
            }
        }
    }

detect_finish:
    free(recv_buf);

    if (fd >= 0)
    {
        DG_LOOP_close(fd);
    }

    DG_DBG_TRACE("leave detect thread: %p", (void*)pthread_self());
    return NULL;
}

/*=============================================================================================*//**
@brief judge if the packet is a detection packet

@param[in]  buf     - the packet buf
@param[in]  size    - the packet size
@param[out] tx_port - the tx port
@param[out] rx_port - the rx port

@return TRUE if the packet is a detect packet
*//*==============================================================================================*/
BOOL dg_loop_is_detect_packet(UINT8* buf, UINT8* tx_port, UINT8* rx_port)
{
    UINT8  v      = 0;
    UINT32 size   = DG_LOOP_DETECT_PACKET_SIZE;
    UINT8* packet = buf;

    *tx_port = *(packet + DG_LOOP_DETECT_HDR_OFFSET);
    *rx_port = *(packet + DG_LOOP_DETECT_HDR_OFFSET + 1);

    /* fill the tx/rx */
    *(packet + DG_LOOP_DETECT_HDR_OFFSET)     = DG_LOOP_DETECT_FILL;
    *(packet + DG_LOOP_DETECT_HDR_OFFSET + 1) = DG_LOOP_DETECT_FILL;

    /* skip the eth header */
    buf  += 14;
    size -= 14;

    while ((size-- > 0) && (v == 0))
    {
        v = *(buf++) - DG_LOOP_DETECT_FILL;
    }

    return v == 0;
}

/** @} */

