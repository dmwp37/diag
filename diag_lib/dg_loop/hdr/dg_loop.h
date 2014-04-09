#ifndef _DG_LOOP_H_
#define _DG_LOOP_H_
/*==================================================================================================

    Module Name:  dg_loop.h

    General Description: This file provides dg_loop library interface

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <pthread.h>

/** @addtogroup libdg_loop
@{
*/
#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                              MACROS
==================================================================================================*/
#define DG_LOOP_PORT_NUM        4
#define DG_LOOP_PACKET_SIZE_MAX 9230
#define DG_LOOP_PACKET_SIZE_MIN 60
#define DG_LOOP_RUN_IFINITE     -1

/*==================================================================================================
                                               ENUMS
==================================================================================================*/

/** loopback path port definition */
enum
{
    DG_LOOP_PORT_I347_0 = 0x01, /** Intel 347 Port0 for stargo */
    DG_LOOP_PORT_I347_1 = 0x02, /** Intel 347 Port1 for stargo */
    DG_LOOP_PORT_I347_2 = 0x03, /** Intel 347 Port2 for stargo */
    DG_LOOP_PORT_I347_3 = 0x04, /** Intel 347 Port3 for stargo */
};
typedef UINT8 DG_LOOP_PORT_T;

/** loopback network node definition */
enum
{
    DG_LOOP_NODE_FPGA = 0x00,
    DG_LOOP_NODE_MAC  = 0x01,
    DG_LOOP_NODE_PHY  = 0x02,
    DG_LOOP_NODE_HDR  = 0x03, /** External loopback cable/SFP/SFP+ */
};
typedef UINT8 DG_LOOP_NODE_T;

/** configuration type */
enum
{
    DG_LOOP_CFG_NORMAL   = 0x00,
    DG_LOOP_CFG_INTERNAL = 0x01,
    DG_LOOP_CFG_EXTERNAL = 0x02
};
typedef UINT8 DG_LOOP_CFG_T;

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef struct
{
    int fail_send;  /* failed send packets number     */
    int fail_recv;  /* failed recv packets number     */
    int total_send; /* total send packets number      */
    int total_recv; /* total recv packets number      */
    int wrong_recv; /* wrong recv packets number      */
} DG_LOOP_TEST_STATISTIC_T;

typedef struct
{
    /* public sector */
    DG_LOOP_PORT_T           tx_port;     /* [in]  - the port that will send data */
    DG_LOOP_PORT_T           rx_port;     /* [in]  - the port that will recv data */
    UINT8                    pattern;     /* [in]  - packet data pattern          */
    int                      size;        /* [in]  - packet size of each transfer */
    int                      number;      /* [in]  - how many times to send/recv  */
    DG_LOOP_TEST_STATISTIC_T result;      /* [out] - test result                  */
    /* private sector */
    pthread_t       send_thread; /* [pri] - send thread        */
    pthread_t       recv_thread; /* [pri] - recv thread        */
    BOOL            b_run;       /* [pri] - thread run control */
    BOOL            b_recv;      /* [pri] - recv run control   */
    int             count;       /* [pri] - packet to be recv  */
    pthread_mutex_t mutex;       /* [pri] - count mutex        */
    pthread_cond_t  send_cond;   /* [pri] - send condition     */
    pthread_cond_t  recv_cond;   /* [pri] - recv condition     */
} DG_LOOP_TEST_T;

/*==================================================================================================
                                   GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/
/*=============================================================================================*//**
@brief map the port id to index

@param[in]  port - the port number according to definition

@return the port index, -1 if invalid.

@note
- index is 0, 1 ... DG_LOOP_PORT_NUM-1
- use this function to check if the port is valid
*//*==============================================================================================*/
int DG_LOOP_port_to_index(DG_LOOP_PORT_T port);

/*=============================================================================================*//**
@brief map the index to port

@param[in]  port - the port number according to definition

@return the port index, 0xFF if invalid index.

@note
- index is 0, 1 ... DG_LOOP_PORT_NUM-1
*//*==============================================================================================*/
DG_LOOP_PORT_T DG_LOOP_index_to_port(int index);

/*=============================================================================================*//**
@brief connect two ports

@param[in] port1 - the path to open on which port

@return TRUE if success

@note
- this function is only for simulation
*//*==============================================================================================*/
BOOL DG_LOOP_connect(DG_LOOP_PORT_T port1, DG_LOOP_PORT_T port2);

/*=============================================================================================*//**
@brief disconnect all the ports

@note
- this function is only for simulation
*//*==============================================================================================*/
void DG_LOOP_disconnect_all();

/*=============================================================================================*//**
@brief open port for send/recv data

@param[in]  port    - the path to open on which port

@return the file descriptor, -1 if error happened

@note
- if error happened, call DG_DBG_get_err_string() to get the last error
*//*==============================================================================================*/
int DG_LOOP_open(DG_LOOP_PORT_T port);

/*=============================================================================================*//**
@brief close port

@param[in] fd - the fd that opened by DG_LOOP_open()
*//*==============================================================================================*/
void DG_LOOP_close(int fd);

/*=============================================================================================*//**
@brief send data over the loopback file descriptor

@param[in]  fd      - the fd that opened by DG_LOOP_open()
@param[in]  buf     - the buffer contains the data to send
@param[in]  len     - the data length need to send

@return the file descriptor, -1 if error happened

@note
- if error happened, call DG_DBG_get_err_string() to get the last error
*//*==============================================================================================*/
BOOL DG_LOOP_send(int fd, UINT8* buf, UINT32 len);

/*=============================================================================================*//**
@brief receive data from the loopback file descriptor

@param[in]  fd      - the fd that opened by DG_LOOP_open()
@param[out] buf     - the buffer to receive data
@param[in]  len     - the data length need to receive

@return the file descriptor, -1 if error happened

@note
- if error happened, call DG_DBG_get_err_string() to get the last error
*//*==============================================================================================*/
BOOL DG_LOOP_recv(int fd, UINT8* buf, UINT32 len);

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
BOOL DG_LOOP_config(DG_LOOP_PORT_T port, DG_LOOP_NODE_T node, DG_LOOP_CFG_T cfg);

/*=============================================================================================*//**
@brief configurate all the port node to normal mode

@return TRUE if success

@note
- if error happened, call DG_DBG_get_err_string() to get the last error
*//*==============================================================================================*/
BOOL DG_LOOP_config_all_normal();

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
BOOL DG_LOOP_start_test(DG_LOOP_TEST_T* test);

/*=============================================================================================*//**
@brief one send/recv thread version of DG_LOOP_start_test
*//*==============================================================================================*/
BOOL DG_LOOP_start_test1(DG_LOOP_TEST_T* test);

/*=============================================================================================*//**
@brief stop the loopback test

@param[in]  test - which test to stop

@note
- this function would stop the two threads in the background that start by DG_LOOP_start_test()
- the function would return immediately
*//*==============================================================================================*/
void DG_LOOP_stop_test(DG_LOOP_TEST_T* test);

/*=============================================================================================*//**
@brief query if the test is still running

@param[in]  test - which test to query

@return TRUE if the background send/recv thread is still running
*//*==============================================================================================*/
BOOL DG_LOOP_query_test(DG_LOOP_TEST_T* test);

/*=============================================================================================*//**
@brief wait the loopback test finished

@param[in]  test - which test to wait

@note
- this function would wait the two threads in the background that started by DG_LOOP_start_test()
- it should be used after DG_LOOP_stop_test(), and block until all the thread released
*//*==============================================================================================*/
void DG_LOOP_wait_test(DG_LOOP_TEST_T* test);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* _DG_LOOP_H_ */

