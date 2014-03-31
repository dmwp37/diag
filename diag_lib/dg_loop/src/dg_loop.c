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
#include <pthread.h>
#include <errno.h>
#include "dg_platform_defs.h"
#include "dg_dbg.h"
#include "dg_loop.h"
#include "dg_loop_cfg.h"


/** @addtogroup libdg_loop
@{
*/
/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/
#define DG_LOOP_PORT_MAX (DG_LOOP_PORT_10GE_3 + 1)

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** internally use struct to control the send/revc thread */
typedef struct
{
    pthread_t send_thread; /* send thread        */
    pthread_t recv_thread; /* recv thread        */
    BOOL      b_run;       /* thread run control */
} DG_LOOP_TEST_CONTROL_T;

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static BOOL  dg_loop_check_port(DG_LOOP_PORT_T port);
static void* dg_loop_send_thread(void* arg);
static void* dg_loop_recv_thread(void* arg);
static BOOL  dg_loop_check_recv_data(UINT8* buf, UINT32 size, UINT8 pattern);

/*==================================================================================================
                                         GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/
/** internal real file descriptor array for each ports */
static int dg_loop_port_fd[DG_LOOP_PORT_MAX] = { -1 };

/*==================================================================================================
                                         GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief open port for send/recv data

@param[in]  port    - the path to open on which port

@return the file descriptor, -1 if error happened

@note
- if error happened, call DG_DBG_get_err_string() to get the last error
*//*==============================================================================================*/
int DG_LOOP_open(DG_LOOP_PORT_T port)
{
    int fd = -1;

    if (!dg_loop_check_port(port))
    {
        return -1;
    }

    if (dg_loop_port_fd[port] > 0)
    {
        /* already opened */
        return dg_loop_port_fd[port];
    }

    /* open the loop device to simulate the port */
    DG_DBG_TRACE("open loop port 0x%02x", port);

    fd = port;

    dg_loop_port_fd[port] = fd;

    return fd;
}

/*=============================================================================================*//**
@brief close port

@param[in] fd - the fd that opened by DG_LOOP_open()
*//*==============================================================================================*/
void DG_LOOP_close(int fd)
{
    int port = fd;

    if (dg_loop_check_port(port))
    {
        DG_DBG_TRACE("close loop port 0x%02x", port);
        if (dg_loop_port_fd[port] > 0)
        {
            /* close the fd*/
            /* close(dg_loop_port_fd[port]); */
            dg_loop_port_fd[port] = -1;
        }
    }
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
    int port = fd;

    if (!dg_loop_check_port(port))
    {
        return FALSE;
    }

    DG_DBG_TRACE("send to port 0x%02x, buf=%p, len=%d", port, buf, len);

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
    int port = fd;

    if (!dg_loop_check_port(port))
    {
        return FALSE;
    }

    DG_DBG_TRACE("recv from port 0x%02x, buf=%p, len=%d", port, buf, len);

    return TRUE;
}

/*=============================================================================================*//**
@brief loopback node configuration

@param[in]  port    - the path to config on which port
@param[in]  node    - where to loopback packet
@param[in]  cfg     - configuration type

@return TRUE if success

@note
- if the port doesn't contains the node or doesn't support the configuration, FALSE will be returned
- if error happened, call DG_DBG_get_err_string() to get the last error
*//*==============================================================================================*/
BOOL DG_LOOP_config(DG_LOOP_PORT_T port, DG_LOOP_NODE_T node, DG_LOOP_CFG_T cfg)
{
    if (!dg_loop_check_port(port))
    {
        return FALSE;
    }

    if (node > DG_LOOP_NODE_HDR)
    {
        DG_DBG_set_err_string("Invalid node selection, node=%d", node);
        return FALSE;
    }

    if (cfg > DG_LOOP_CFG_EXTERNAL)
    {
        DG_DBG_set_err_string("Invalid configuration, cfg=%d", cfg);
        return FALSE;
    }

    return DG_LOOP_cfg_impl(port, node, cfg);
}

/*=============================================================================================*//**
@brief print the last error string
*//*==============================================================================================*/
void DG_LOOP_print_err_string()
{
    char* err_str = DG_DBG_get_err_string();
    if (err_str != NULL)
    {
        printf("%s\n", err_str);
        free(err_str);
    }
}

/*=============================================================================================*//**
@brief loopback test between a pair of ports

@param[in]  test    - the test parameter

@return TRUE if success

@note
- this function would start two threads in the background and return immediately
- one thread for sending the packets and one thread for receiving packets
- if test->number == DG_LOOP_RUN_IFINITE, the test would run forever
- the statistic result is stored in test->result
- user can READ it any time to print out the result
- user can all DG_LOOP_stop_test() to stop the test
- if error happened, call DG_DBG_get_err_string() to get the last error
*//*==============================================================================================*/
BOOL DG_LOOP_start_test(DG_LOOP_TEST_T* test)
{
    BOOL ret = FALSE;

    DG_LOOP_TEST_CONTROL_T* p_control = NULL;

    if (!dg_loop_check_port(test->tx_port) ||
        !dg_loop_check_port(test->rx_port))
    {
        return FALSE;
    }

    /* init the test control block */
    if ((p_control = malloc(sizeof(DG_LOOP_TEST_CONTROL_T))) == NULL)
    {
        DG_DBG_set_err_string("failed to malloc loop test control block");
        return FALSE;
    }

    memset(p_control, 0, sizeof(DG_LOOP_TEST_CONTROL_T));
    p_control->b_run = TRUE;

    test->control = p_control;

    if (pthread_create(&p_control->recv_thread, NULL, dg_loop_recv_thread, test) != 0)
    {
        DG_DBG_ERROR("failed to start recv thread, rx_port=0x%02x, errno=%d(%m)",
                     test->rx_port, errno);
    }
    else if (pthread_create(&p_control->send_thread, NULL, dg_loop_send_thread, test) != 0)
    {
        DG_DBG_ERROR("failed to start send thread, tx_port=0x%02x, errno=%d(%m)",
                     test->tx_port, errno);
    }
    else
    {
        ret = TRUE;
        DG_DBG_TRACE("Successfully create send/recv thread pair, tx_port=0x%02x, tx_port=0x%02x",
                     test->tx_port, test->rx_port);
    }

    /* clean up */
    if (!ret)
    {
        /* tell the thread to stop */
        p_control->b_run = FALSE;

        /* wait the thread to finish */
        if (p_control->send_thread != 0)
        {
            pthread_join(p_control->send_thread, NULL);
        }

        /* wait the thread to finish */
        if (p_control->recv_thread != 0)
        {
            pthread_join(p_control->recv_thread, NULL);
        }

        free(p_control);
    }

    return ret;
}

/*=============================================================================================*//**
@brief stop the loopback test

@param[in]  test - which test to stop

@note
- this function would stop the two threads in the background that start by DG_LOOP_start_test()
*//*==============================================================================================*/
void DG_LOOP_stop_test(DG_LOOP_TEST_T* test)
{
    DG_LOOP_TEST_CONTROL_T* p_control = test->control;

    /* tell the thread to stop */
    p_control->b_run = FALSE;

    /* wait the thread to finish */
    if (p_control->send_thread != 0)
    {
        pthread_join(p_control->send_thread, NULL);
    }

    /* wait the thread to finish */
    if (p_control->recv_thread != 0)
    {
        pthread_join(p_control->recv_thread, NULL);
    }

    free(p_control);
}

/*=============================================================================================*//**
@brief query if the test is still running

@param[in]  test - which test to query

@return TRUE if the background send/recv thread is still running
*//*==============================================================================================*/
BOOL DG_LOOP_query_test(DG_LOOP_TEST_T* test)
{
    DG_LOOP_TEST_CONTROL_T* p_control = test->control;

    return p_control->b_run;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief check loopback port

@param[in]  port    - the loopback port

@return TRUE if success
*//*==============================================================================================*/
BOOL dg_loop_check_port(DG_LOOP_PORT_T port)
{
    switch (port)
    {
    case DG_LOOP_PORT_MGT:
    case DG_LOOP_PORT_HA:
    case DG_LOOP_PORT_WTB0_1:
    case DG_LOOP_PORT_WTB0_2:
    case DG_LOOP_PORT_WTB1_1:
    case DG_LOOP_PORT_WTB1_2:
    case DG_LOOP_PORT_GE_0:
    case DG_LOOP_PORT_GE_1:
    case DG_LOOP_PORT_GE_2:
    case DG_LOOP_PORT_GE_3:
    case DG_LOOP_PORT_GE_4:
    case DG_LOOP_PORT_GE_5:
    case DG_LOOP_PORT_GE_6:
    case DG_LOOP_PORT_GE_7:
    case DG_LOOP_PORT_GE_8:
    case DG_LOOP_PORT_GE_9:
    case DG_LOOP_PORT_GE_10:
    case DG_LOOP_PORT_GE_11:
    case DG_LOOP_PORT_SFP_0:
    case DG_LOOP_PORT_SFP_1:
    case DG_LOOP_PORT_SFP_2:
    case DG_LOOP_PORT_SFP_3:
    case DG_LOOP_PORT_10GE_0:
    case DG_LOOP_PORT_10GE_1:
    case DG_LOOP_PORT_10GE_2:
    case DG_LOOP_PORT_10GE_3:
        break;

    default:
        DG_DBG_set_err_string("Invalid Port selection, port=0x%02x", port);
        return FALSE;
    }

    return TRUE;
}

/*=============================================================================================*//**
@brief send packet thread function

@param[in] arg - the test parameter

@return NULL returned
*//*==============================================================================================*/
void* dg_loop_send_thread(void* arg)
{
    DG_LOOP_TEST_T*           test      = (DG_LOOP_TEST_T*)arg;
    DG_LOOP_TEST_CONTROL_T*   p_control = test->control;
    DG_LOOP_TEST_STATISTIC_T* result    = &test->result;

    int    fd;
    int    number   = test->number;
    int    size     = test->size;
    UINT8* send_buf = NULL;

    DG_DBG_TRACE("enter into send thread: %p", (void*)pthread_self());

    /* open the port for sending data */
    if ((fd = DG_LOOP_open(test->tx_port)) < 0)
    {
        DG_DBG_ERROR("failed to open send port, tx_port=0x%02x", test->tx_port);
        return NULL;
    }

    /* prepare the data to send */
    if ((send_buf = malloc(size)) == NULL)
    {
        DG_DBG_set_err_string("failed to malloc send buf, size=%d", size);
        return NULL;
    }

    memset(send_buf, test->pattern, size);

    while (p_control->b_run)
    {
        if (number < 0)
        {
            /* do nothing to run forever */
        }
        else if (number == 0)
        {
            DG_DBG_TRACE("send thread %p finished", (void*)pthread_self());
            break;
        }
        else
        {
            number--;
        }

        if (!DG_LOOP_send(fd, send_buf, size))
        {
            result->fail_send++;
        }
        else
        {
            result->total_send++;
        }
    }

    free(send_buf);

    DG_DBG_TRACE("leave send thread: %p", (void*)pthread_self());

    return NULL;
}

/*=============================================================================================*//**
@brief recv packet thread function

@param[in] arg - the test parameter

@return NULL returned
*//*==============================================================================================*/
void* dg_loop_recv_thread(void* arg)
{
    DG_LOOP_TEST_T*           test      = (DG_LOOP_TEST_T*)arg;
    DG_LOOP_TEST_CONTROL_T*   p_control = test->control;
    DG_LOOP_TEST_STATISTIC_T* result    = &test->result;

    int    fd;
    int    number       = test->number;
    int    size         = test->size;
    UINT8* recv_buf     = NULL;
    UINT8  init_pattern = ~test->pattern;

    DG_DBG_TRACE("enter into recv thread: %p", (void*)pthread_self());

    /* open the port for sending data */
    if ((fd = DG_LOOP_open(test->tx_port)) < 0)
    {
        DG_DBG_ERROR("failed to open send port, tx_port=0x%02x", test->tx_port);
        return NULL;
    }

    /* prepare the data to send */
    if ((recv_buf = malloc(size)) == NULL)
    {
        DG_DBG_set_err_string("failed to malloc recv buf, size=%d", size);
        return NULL;
    }

    while (p_control->b_run)
    {
        if (number < 0)
        {
            /* do nothing to run forever */
        }
        else if (number == 0)
        {
            DG_DBG_TRACE("recv thread %p finished", (void*)pthread_self());
            /* only let the recv thread update the control */
            p_control->b_run = FALSE;
            break;
        }
        else
        {
            number--;
        }

        /* init the buffer with different data first */
        memset(recv_buf, init_pattern, size);

        if (!DG_LOOP_recv(fd, recv_buf, size))
        {
            result->fail_recv++;
        }
        else
        {
            /* verify the data */
            if (!dg_loop_check_recv_data(recv_buf, size, test->pattern))
            {
                result->wrong_recv++;
                DG_DBG_set_err_string("receive wrong data");
            }
            result->total_recv++;
        }
    }

    free(recv_buf);

    DG_DBG_TRACE("leave recv thread: %p", (void*)pthread_self());
    return NULL;
}

/*=============================================================================================*//**
@brief check the received data

@param[in] buf      - the buffer of data to check
@param[in] size     - the buffer size
@param[in] pattern  - the check pattern

@return TURE if success
*//*==============================================================================================*/
BOOL dg_loop_check_recv_data(UINT8* buf, UINT32 size, UINT8 pattern)
{
    BOOL   ret = TRUE;
    UINT32 i;

    for (i = 0; i < size; i++)
    {
        if (buf[i] != pattern)
        {
            ret = FALSE;
            break;
        }
    }

    return ret;
}

/** @} */

