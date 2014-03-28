#ifndef _DG_LOOP_H_
#define _DG_LOOP_H_
/*==================================================================================================

    Module Name:  dg_loop.h

    General Description: This file provides dg_loop library interface

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

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
    DG_LOOP_PORT_MGT    = 0x01, /** MGT Port           */
    DG_LOOP_PORT_HA     = 0x02, /** HA Port            */
    DG_LOOP_PORT_WTB0_1 = 0x03, /** WANPIM 0 Port 1    */
    DG_LOOP_PORT_WTB0_2 = 0x04, /** WANPIM 0 Port 2    */
    DG_LOOP_PORT_WTB1_1 = 0x05, /** WANPIM 1 Port 1    */
    DG_LOOP_PORT_WTB1_2 = 0x06, /** WANPIM 1 Port 2    */
    DG_LOOP_PORT_GE_0   = 0x10, /** GE_RJ45 Port 0     */
    DG_LOOP_PORT_GE_1   = 0x11, /** GE_RJ45 Port 1     */
    DG_LOOP_PORT_GE_2   = 0x12, /** GE_RJ45 Port 2     */
    DG_LOOP_PORT_GE_3   = 0x13, /** GE_RJ45 Port 3     */
    DG_LOOP_PORT_GE_4   = 0x14, /** GE_RJ45 Port 4     */
    DG_LOOP_PORT_GE_5   = 0x15, /** GE_RJ45 Port 5     */
    DG_LOOP_PORT_GE_6   = 0x16, /** GE_RJ45 Port 6     */
    DG_LOOP_PORT_GE_7   = 0x17, /** GE_RJ45 Port 7     */
    DG_LOOP_PORT_GE_8   = 0x20, /** GE_RJ45 Port 8     */
    DG_LOOP_PORT_GE_9   = 0x21, /** GE_RJ45 Port 9     */
    DG_LOOP_PORT_GE_10  = 0x22, /** GE_RJ45 Port 10    */
    DG_LOOP_PORT_GE_11  = 0x23, /** GE_RJ45 Port 11    */
    DG_LOOP_PORT_SFP_0  = 0x24, /** GE_SFP  Port 0     */
    DG_LOOP_PORT_SFP_1  = 0x25, /** GE_SFP  Port 1     */
    DG_LOOP_PORT_SFP_2  = 0x26, /** GE_SFP  Port 2     */
    DG_LOOP_PORT_SFP_3  = 0x27, /** GE_SFP  Port 3     */
    DG_LOOP_PORT_10GE_0 = 0x30, /** 10GE_SFP+ Port0    */
    DG_LOOP_PORT_10GE_1 = 0x31, /** 10GE_SFP+ Port1    */
    DG_LOOP_PORT_10GE_2 = 0x32, /** 10GE_SFP+ Port2    */
    DG_LOOP_PORT_10GE_3 = 0x33, /** 10GE_SFP+ Port3    */
};
typedef UINT8 DG_LOOP_PORT_T;

/** loopback network node definition */
enum
{
    DG_LOOP_NODE_FPGA = 0x00,
    DG_LOOP_NODE_MAC  = 0x01,
    DG_LOOP_NODE_PHY  = 0x02,
    DG_LOOP_NODE_PORT = 0x03, /** External loopback cable/SFP/SFP+ */
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
    DG_LOOP_PORT_T tx_port; /* the port that will send data */
    DG_LOOP_PORT_T rx_port; /* the port that will recv data */
    UINT8          pattern; /* packet data pattern          */
    int            size;    /* packet size of each transfer */
    int            number;  /* how many times to send/recv  */
    void*          control; /* the internal control block   */
} DG_LOOP_TEST_T;

typedef struct
{
    int   fail_send;  /* failed send packets number     */
    int   fail_recv;  /* failed recv packets number     */
    int   total_send; /* total send packets number      */
    int   total_recv; /* total recv packets number      */
    char* send_err;   /* error message from send thread */
    char* recv_err;   /* error message from recv thread */
} DG_LOOP_TEST_STATISTIC_T;
/*==================================================================================================
                                   GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*=============================================================================================*//**
@brief open port for send/recv data

@param[in]  port    - the path to open on which port
@param[out] err_str - error string if any

@return the file descriptor, -1 if error happened

@note
- caller must free the *err_str
*//*==============================================================================================*/
int DG_LOOP_open(DG_LOOP_PORT_T port, char** err_str);

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
@param[out] err_str - error string if any

@return the file descriptor, -1 if error happened

@note
- caller must free the *err_str
*//*==============================================================================================*/
BOOL DG_LOOP_send(int fd, UINT8* buf, UINT32 len, char** err_str);

/*=============================================================================================*//**
@brief receive data from the loopback file descriptor

@param[in]  fd      - the fd that opened by DG_LOOP_open()
@param[out] buf     - the buffer to receive data
@param[in]  len     - the data length need to receive
@param[out] err_str - error string if any

@return the file descriptor, -1 if error happened

@note
- caller must free the *err_str
*//*==============================================================================================*/
BOOL DG_LOOP_recv(int fd, UINT8* buf, UINT32 len, char** err_str);

/*=============================================================================================*//**
@brief loopback node configuration

@param[in]  port    - the path to config on which port
@param[in]  node    - where to loopback packet
@param[in]  cfg     - configuration type
@param[out] err_str - error string if any

@return TRUE if success

@note
- if the port doesn't contains the node or doesn't support the configuration, FALSE will be returned
- caller must free the *err_str
*//*==============================================================================================*/
BOOL DG_LOOP_config(DG_LOOP_PORT_T port, DG_LOOP_NODE_T node, DG_LOOP_CFG_T cfg, char** err_str);

/*=============================================================================================*//**
@brief loopback test between a pair of ports

@param[in]  test    - the test parameter
@param[out] err_str - error string if any

@return TRUE if success

@note
- this function would start two threads in the background and return immediately
- one thread for sending the packets and one thread for receiving packets
- if test->number == DG_LOOP_RUN_IFINITE, the test would run forever
- user can all DG_Loop_stop_test() to stop the test and get the statistic result
- caller must free the *err_str
*//*==============================================================================================*/
BOOL DG_LOOP_start_test(DG_LOOP_TEST_T* test, char** err_str);

/*=============================================================================================*//**
@brief stop the loopback test

@param[in]  test    - which test to stop
@param[in]  result  - the statistic result of the test

@note
- this function would stop the two threads in the background that start by DG_LOOP_start_test()
- if test->number == DG_LOOP_RUN_IFINITE, it will stop the test immediately
- otherwise this function would block untill all the packets has been send/recv
- caller must free the result->send_err and result->recv_err
*//*==============================================================================================*/
void DG_LOOP_stop_test(DG_LOOP_TEST_T* test, DG_LOOP_TEST_STATISTIC_T* result);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* _DG_LOOP_H_ */

