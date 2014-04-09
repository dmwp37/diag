/*==================================================================================================

    Module Name:  dg_loop_test.c

    General Description: Implements the dg_loop test interface

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
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
/* this value can't be too big, or it will lost packets */
#define DG_LOOP_CACHE_COUNT_MAX 4

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static void* dg_loop_send_thread(void* arg);
static void* dg_loop_recv_thread(void* arg);
static BOOL  dg_loop_check_recv_data(UINT8* buf, UINT32 size, UINT8 pattern);

/*==================================================================================================
                                         GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                         GLOBAL FUNCTIONS
==================================================================================================*/

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

    if (DG_LOOP_port_to_index(test->tx_port) < 0)
    {
        DG_DBG_set_err_string("Invalid tx_port to test, port=0x%02x", test->tx_port);
        return FALSE;
    }

    if (DG_LOOP_port_to_index(test->rx_port) < 0)
    {
        DG_DBG_set_err_string("Invalid rx_port to test, port=0x%02x", test->rx_port);
        return FALSE;
    }

    if (pthread_cond_init(&test->send_cond, NULL) != 0)
    {
        DG_DBG_set_err_string("can't init send condtion, errno=%d(%m)", errno);
        return FALSE;
    }

    if (pthread_cond_init(&test->recv_cond, NULL) != 0)
    {
        DG_DBG_set_err_string("can't init recv condtion, errno=%d(%m)", errno);
        return FALSE;
    }

    if (pthread_mutex_init(&test->mutex, NULL) != 0)
    {
        DG_DBG_set_err_string("can't init mutex, errno=%d(%m)", errno);
        return FALSE;
    }

    test->b_run  = TRUE;
    test->b_recv = TRUE; /* let the recv thread run, controlled by send thread */

    if (pthread_create(&test->recv_thread, NULL, dg_loop_recv_thread, test) != 0)
    {
        DG_DBG_ERROR("failed to start recv thread, rx_port=0x%02x, errno=%d(%m)",
                     test->rx_port, errno);
    }
    else if (pthread_create(&test->send_thread, NULL, dg_loop_send_thread, test) != 0)
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

    /* let the send/recv thread run first */
    sched_yield();
    if (!test->b_run)
    {
        ret = FALSE;
    }

    /* clean up */
    if (!ret)
    {
        /* tell the thread to stop */
        test->b_run  = FALSE;
        test->b_recv = FALSE;

        /* wait the thread to finish */
        if (test->send_thread != 0)
        {
            pthread_join(test->send_thread, NULL);
        }

        /* wait the thread to finish */
        if (test->recv_thread != 0)
        {
            pthread_join(test->recv_thread, NULL);
        }
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
    /* tell the thread to stop */
    test->b_run = FALSE;
}

/*=============================================================================================*//**
@brief query if the test is still running

@param[in]  test - which test to query

@return TRUE if the background send/recv thread is still running
*//*==============================================================================================*/
BOOL DG_LOOP_query_test(DG_LOOP_TEST_T* test)
{
    return test->b_run;
}

/*=============================================================================================*//**
@brief stop and wait the loopback test finished

@param[in]  test - which test to wait

@note
- this function would stop and wait the two threads that started by DG_LOOP_start_test()
- it will block until all the thread released
*//*==============================================================================================*/
void DG_LOOP_wait_test(DG_LOOP_TEST_T* test)
{
    /* tell the thread to stop */
    test->b_run = FALSE;

    /* wait the thread to finish */
    if (test->send_thread != 0)
    {
        pthread_join(test->send_thread, NULL);
    }

    /* wait the thread to finish */
    if (test->recv_thread != 0)
    {
        pthread_join(test->recv_thread, NULL);
    }

    pthread_mutex_destroy(&test->mutex);
    pthread_cond_destroy(&test->send_cond);
    pthread_cond_destroy(&test->recv_cond);
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief send packet thread function

@param[in] arg - the test parameter

@return NULL returned
*//*==============================================================================================*/
void* dg_loop_send_thread(void* arg)
{
    DG_LOOP_TEST_T*           test   = (DG_LOOP_TEST_T*)arg;
    DG_LOOP_TEST_STATISTIC_T* result = &test->result;

    int    fd;
    int    number   = test->number;
    int    size     = test->size;
    UINT8* send_buf = NULL;

    DG_DBG_TRACE("enter into send thread: %p", (void*)pthread_self());

    /* open the port for sending data */
    if ((fd = DG_LOOP_open(test->tx_port)) < 0)
    {
        DG_DBG_ERROR("failed to open send port, tx_port=0x%02x", test->tx_port);
        goto send_finish;
    }

    /* prepare the data to send */
    if ((send_buf = malloc(size)) == NULL)
    {
        DG_DBG_set_err_string("failed to malloc send buf, size=%d", size);
        goto send_finish;
    }

    memset(send_buf, test->pattern, size);

    while (test->b_run)
    {
        BOOL b_send = TRUE;
        DG_LOOP_MUTEX_LOCK(&test->mutex);
        if (test->count >= DG_LOOP_CACHE_COUNT_MAX)
        {
            /* no data to send */
            pthread_cond_signal(&test->recv_cond);
            if (pthread_cond_wait(&test->send_cond, &test->mutex) != 0)
            {
                DG_DBG_ERROR("Error waiting on send condition, errno=%d(%m)", errno);
                b_send = FALSE;
            }
        }
        DG_LOOP_MUTEX_UNLOCK(&test->mutex);

        if (!b_send)
        {
            continue;
        }

        if (number < 0)
        {
            /* do nothing to run forever */
        }
        else if (number == 0)
        {
            DG_DBG_TRACE("send thread %p finished", (void*)pthread_self());
            /* tell the receive thread to stop */
            test->b_recv = FALSE;
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
            DG_LOOP_MUTEX_LOCK(&test->mutex);
            test->count++;
            pthread_cond_signal(&test->recv_cond);
            DG_LOOP_MUTEX_UNLOCK(&test->mutex);
        }
    }

send_finish:
    free(send_buf);

    if (fd >= 0)
    {
        DG_LOOP_close(fd);
    }

    test->b_run = FALSE;
    /* tell the receive thread to stop */
    test->b_recv = FALSE;

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
    DG_LOOP_TEST_T*           test   = (DG_LOOP_TEST_T*)arg;
    DG_LOOP_TEST_STATISTIC_T* result = &test->result;

    int    fd;
    int    number       = test->number;
    int    size         = test->size;
    UINT8* recv_buf     = NULL;
    UINT8  init_pattern = ~test->pattern;

    DG_DBG_TRACE("enter into recv thread: %p", (void*)pthread_self());

    /* open the port for receiving data */
    if ((fd = DG_LOOP_open(test->rx_port)) < 0)
    {
        DG_DBG_ERROR("failed to open recv port, rx_port=0x%02x", test->rx_port);
        goto recv_finish;
    }

    /* prepare the data to send */
    if ((recv_buf = malloc(size)) == NULL)
    {
        DG_DBG_set_err_string("failed to malloc recv buf, size=%d", size);
        goto recv_finish;
    }

    while ((test->count != 0) || test->b_recv)
    {
        BOOL b_recv = TRUE;
        DG_LOOP_MUTEX_LOCK(&test->mutex);
        if (test->count > 0)
        {
            /* we have data to recv */
        }
        else if (test->b_recv)
        {
            pthread_cond_signal(&test->send_cond);
            /* no data to recv */
            if (pthread_cond_wait(&test->recv_cond, &test->mutex) != 0)
            {
                DG_DBG_ERROR("Error waiting on recv condition, errno=%d(%m)", errno);
                b_recv = FALSE;
            }
        }
        else
        {
            b_recv = FALSE;
        }
        DG_LOOP_MUTEX_UNLOCK(&test->mutex);

        if (!b_recv)
        {
            continue;
        }

        if (number < 0)
        {
            /* do nothing to run forever */
        }
        else if (number == 0)
        {
            DG_DBG_TRACE("recv thread %p finished", (void*)pthread_self());
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

            /* if read time out we consider there is no data */
            DG_LOOP_MUTEX_LOCK(&test->mutex);
            test->count = 0;
            pthread_cond_signal(&test->send_cond);
            DG_LOOP_MUTEX_UNLOCK(&test->mutex);
        }
        else
        {
            DG_LOOP_MUTEX_LOCK(&test->mutex);
            test->count--;
            pthread_cond_signal(&test->send_cond);
            DG_LOOP_MUTEX_UNLOCK(&test->mutex);

            /* verify the data */
            if (!dg_loop_check_recv_data(recv_buf, size, test->pattern))
            {
                result->wrong_recv++;
                DG_DBG_set_err_string("receive wrong data");
            }
            result->total_recv++;
        }
    }

recv_finish:
    free(recv_buf);

    if (fd >= 0)
    {
        DG_LOOP_close(fd);
    }

    test->b_run  = FALSE;
    test->b_recv = FALSE;

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
    UINT8 v = 0;

    while ((size-- > 0) && (v == 0))
    {
        v = *(buf++) - pattern;
    }

    return v == 0;
}

/** @} */

