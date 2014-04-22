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
    { DG_LOOP_PORT_MGT,    0xFF },
    { DG_LOOP_PORT_HA,     0xFF },
    { DG_LOOP_PORT_WTB0_1, 0xFF },
    { DG_LOOP_PORT_WTB0_2, 0xFF },
    { DG_LOOP_PORT_WTB1_1, 0xFF },
    { DG_LOOP_PORT_WTB1_2, 0xFF },
    { DG_LOOP_PORT_GE_0,   0xFF },
    { DG_LOOP_PORT_GE_1,   0xFF },
    { DG_LOOP_PORT_GE_2,   0xFF },
    { DG_LOOP_PORT_GE_3,   0xFF },
    { DG_LOOP_PORT_GE_4,   0xFF },
    { DG_LOOP_PORT_GE_5,   0xFF },
    { DG_LOOP_PORT_GE_6,   0xFF },
    { DG_LOOP_PORT_GE_7,   0xFF },
    { DG_LOOP_PORT_GE_8,   0xFF },
    { DG_LOOP_PORT_GE_9,   0xFF },
    { DG_LOOP_PORT_GE_10,  0xFF },
    { DG_LOOP_PORT_GE_11,  0xFF },
    { DG_LOOP_PORT_SFP_0,  0xFF },
    { DG_LOOP_PORT_SFP_1,  0xFF },
    { DG_LOOP_PORT_SFP_2,  0xFF },
    { DG_LOOP_PORT_SFP_3,  0xFF },
    { DG_LOOP_PORT_10GE_0, 0xFF },
    { DG_LOOP_PORT_10GE_1, 0xFF },
    { DG_LOOP_PORT_10GE_2, 0xFF },
    { DG_LOOP_PORT_10GE_3, 0xFF }
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
    int            index;
    int            fd;
    int            old_dbg_lvl;
    DG_LOOP_PORT_T tx_port;
    DG_LOOP_PORT_T rx_port;
    UINT8*         packet = malloc(DG_LOOP_DETECT_PACKET_SIZE);
    UINT8*         p_tx   = packet + DG_LOOP_DETECT_HDR_OFFSET;
    pthread_t      detect_thread[DG_LOOP_PORT_NUM] = { 0 };

    if (packet == NULL)
    {
        return NULL;
    }

    memset(packet, DG_LOOP_DETECT_FILL, DG_LOOP_DETECT_PACKET_SIZE);

    old_dbg_lvl = DG_DBG_get_dbg_level();
    DG_DBG_set_dbg_level(DG_DBG_LVL_DISABLE);

    /* start the detection threads */
    for (index = 0; index < DG_LOOP_PORT_NUM; index++)
    {
        rx_port = dg_loop_index_to_port(index);
        if (pthread_create(&detect_thread[index], NULL,
                           dg_loop_detect_thread, (void*)(intptr_t)index) != 0)
        {
            DG_DBG_ERROR("failed to start detect thread, rx_port=0x%02x, errno=%d(%m)",
                         rx_port, errno);
        }
    }

    /* send the detection packet */
    for (index = 0; index < DG_LOOP_PORT_NUM; index++)
    {
        tx_port = dg_loop_index_to_port(index);
        *p_tx   = tx_port;

        if ((fd = DG_LOOP_open(tx_port)) < 0)
        {
            DG_DBG_ERROR("failed to open tx port, tx_port=0x%02x", tx_port);
        }
        else if (!DG_LOOP_send(fd, packet, DG_LOOP_DETECT_PACKET_SIZE))
        {
            DG_DBG_ERROR("failed to send detection packet, tx_port=0x%02x", tx_port);
        }

        if (fd >= 0)
        {
            DG_LOOP_close(fd);
        }
    }

    /* wait the detection threads */
    for (index = 0; index < DG_LOOP_PORT_NUM; index++)
    {
        if (detect_thread[index] != 0)
        {
            pthread_join(detect_thread[index], NULL);
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
    int    index = (int)(intptr_t)arg;
    int    fd;
    UINT32 size     = DG_LOOP_DETECT_PACKET_SIZE;
    UINT8* recv_buf = NULL;

    DG_LOOP_PORT_T m_port = dg_loop_index_to_port(index);
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
            DG_DBG_TRACE("port 0x%02x failed to recv packet", m_port);
            break;
        }
        else if (!dg_loop_is_detect_packet(recv_buf, &tx_port, &rx_port))
        {
            DG_DBG_TRACE("port 0x%02x recv wrong packet", m_port);
        }
        else
        {
            int tx_index = DG_LOOP_check_port(tx_port);
            DG_DBG_TRACE("port 0x%02x recv a valid detection packet", m_port);
            /* save the detection */
            if (dg_loop_detect_info[tx_index].rx_port == 0xFF)
            {
                dg_loop_detect_info[tx_index].rx_port = m_port;
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

