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
#include <semaphore.h>

/** @addtogroup libdg_loop
@{
*/
#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                              MACROS
==================================================================================================*/
#define DG_LOOP_PACKET_SIZE_MAX 9000
#define DG_LOOP_PACKET_SIZE_MIN 80
#define DG_LOOP_RUN_IFINITE     -1

/*==================================================================================================
                                               ENUMS
==================================================================================================*/

/** loopback path port definition */
enum
{
    DG_LOOP_PORT_mgt = 0,    /** MGT Port           */
    DG_LOOP_PORT_ha,         /** HA Port            */
    DG_LOOP_PORT_wtb0,       /** WANPIM0 Marvell pt */
    DG_LOOP_PORT_ge_16,      /** WANPIM0 BCM Port   */
    DG_LOOP_PORT_wtb1,       /** WANPIM0 Marvell Pt */
    DG_LOOP_PORT_ge_24,      /** WANPIM0 BCM Port   */
    DG_LOOP_PORT_ge_0,       /** GE_RJ45 Port 0     */
    DG_LOOP_PORT_ge_1,       /** GE_RJ45 Port 1     */
    DG_LOOP_PORT_ge_2,       /** GE_RJ45 Port 2     */
    DG_LOOP_PORT_ge_3,       /** GE_RJ45 Port 3     */
    DG_LOOP_PORT_ge_4,       /** GE_RJ45 Port 4     */
    DG_LOOP_PORT_ge_5,       /** GE_RJ45 Port 5     */
    DG_LOOP_PORT_ge_6,       /** GE_RJ45 Port 6     */
    DG_LOOP_PORT_ge_7,       /** GE_RJ45 Port 7     */
    DG_LOOP_PORT_ge_8,       /** GE_RJ45 Port 8     */
    DG_LOOP_PORT_ge_9,       /** GE_RJ45 Port 9     */
    DG_LOOP_PORT_ge_10,      /** GE_RJ45 Port 10    */
    DG_LOOP_PORT_ge_11,      /** GE_RJ45 Port 11    */
    DG_LOOP_PORT_ge_12,      /** GE_SFP  Port 0     */
    DG_LOOP_PORT_ge_13,      /** GE_SFP  Port 1     */
    DG_LOOP_PORT_ge_14,      /** GE_SFP  Port 2     */
    DG_LOOP_PORT_ge_15,      /** GE_SFP  Port 3     */
    DG_LOOP_PORT_xe_0,       /** 10GE_SFP+ Port0    */
    DG_LOOP_PORT_xe_1,       /** 10GE_SFP+ Port1    */
    DG_LOOP_PORT_xe_2,       /** 10GE_SFP+ Port2    */
    DG_LOOP_PORT_xe_3,       /** 10GE_SFP+ Port3    */
    DG_LOOP_PORT_NUM
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
/** the detected port pair */
typedef struct
{
    const DG_LOOP_PORT_T tx_port;
    DG_LOOP_PORT_T       rx_port;
} DG_LOOP_PORT_PAIR_T;

typedef struct
{
    int    fail_send;  /* failed send packets number */
    int    fail_recv;  /* failed recv packets number */
    UINT64 total_send; /* total send packets number  */
    UINT64 total_recv; /* total recv packets number  */
    int    wrong_recv; /* wrong recv packets number  */
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
    pthread_t send_thread; /* [pri] - send thread        */
    pthread_t recv_thread; /* [pri] - recv thread        */
    BOOL      b_run;       /* [pri] - thread run control */
    BOOL      b_recv;      /* [pri] - recv run control   */
    sem_t     send_sem;    /* [pri] - send semaphore     */
    sem_t     recv_sem;    /* [pri] - recv semaphore     */
} DG_LOOP_TEST_T;

/*==================================================================================================
                                   GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/
/*=============================================================================================*//**
@brief get the port number

@param[in]  name - the port name

@return -1 if invalid, otherwise the port number
*//*==============================================================================================*/
int DG_LOOP_get_port(const char* name);

/*=============================================================================================*//**
@brief get the port name

@param[in]  port - the port

@return NULL if invalid, otherwise the name of the port
*//*==============================================================================================*/
const char* DG_LOOP_port_name(DG_LOOP_PORT_T port);

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

/*=============================================================================================*//**
@brief auto detect the loop connection and fill the port pair

@return the port pair array, NULL if failed

*//*==============================================================================================*/
DG_LOOP_PORT_PAIR_T* DG_LOOP_auto_detect();

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* _DG_LOOP_H_ */

