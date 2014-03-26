#ifndef _DG_CMN_DRV_LOOP_H_
#define _DG_CMN_DRV_LOOP_H_
/*==================================================================================================

    Module Name:  dg_cmn_drv_loop.h

    General Description: This file provides driver interface for normal loopback test

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup LOOP_driver
@{

@par
Provide APIs for normal loopback test
*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                              MACROS
==================================================================================================*/

/*==================================================================================================
                                               ENUMS
==================================================================================================*/
/** normal loopback path port definition */
enum
{
    DG_CMN_DRV_LOOP_PORT_MGT    = 0x01, /** MGT Port           */
    DG_CMN_DRV_LOOP_PORT_HA     = 0x02, /** HA Port            */
    DG_CMN_DRV_LOOP_PORT_WTB0_1 = 0x03, /** WANPIM 0 Port 1    */
    DG_CMN_DRV_LOOP_PORT_WTB0_2 = 0x04, /** WANPIM 0 Port 2    */
    DG_CMN_DRV_LOOP_PORT_WTB1_1 = 0x05, /** WANPIM 1 Port 1    */
    DG_CMN_DRV_LOOP_PORT_WTB1_2 = 0x06, /** WANPIM 1 Port 2    */
    DG_CMN_DRV_LOOP_PORT_GE_0   = 0x10, /** GE_RJ45 Port 0     */
    DG_CMN_DRV_LOOP_PORT_GE_1   = 0x11, /** GE_RJ45 Port 1     */
    DG_CMN_DRV_LOOP_PORT_GE_2   = 0x12, /** GE_RJ45 Port 2     */
    DG_CMN_DRV_LOOP_PORT_GE_3   = 0x13, /** GE_RJ45 Port 3     */
    DG_CMN_DRV_LOOP_PORT_GE_4   = 0x14, /** GE_RJ45 Port 4     */
    DG_CMN_DRV_LOOP_PORT_GE_5   = 0x15, /** GE_RJ45 Port 5     */
    DG_CMN_DRV_LOOP_PORT_GE_6   = 0x16, /** GE_RJ45 Port 6     */
    DG_CMN_DRV_LOOP_PORT_GE_7   = 0x17, /** GE_RJ45 Port 7     */
    DG_CMN_DRV_LOOP_PORT_GE_8   = 0x20, /** GE_RJ45 Port 8     */
    DG_CMN_DRV_LOOP_PORT_GE_9   = 0x21, /** GE_RJ45 Port 9     */
    DG_CMN_DRV_LOOP_PORT_GE_10  = 0x22, /** GE_RJ45 Port 10    */
    DG_CMN_DRV_LOOP_PORT_GE_11  = 0x23, /** GE_RJ45 Port 11    */
    DG_CMN_DRV_LOOP_PORT_SFP_0  = 0x24, /** GE_SFP  Port 0     */
    DG_CMN_DRV_LOOP_PORT_SFP_1  = 0x25, /** GE_SFP  Port 1     */
    DG_CMN_DRV_LOOP_PORT_SFP_2  = 0x26, /** GE_SFP  Port 2     */
    DG_CMN_DRV_LOOP_PORT_SFP_3  = 0x27, /** GE_SFP  Port 3     */
    DG_CMN_DRV_LOOP_PORT_10GE_0 = 0x30, /** 10GE_SFP+ Port0    */
    DG_CMN_DRV_LOOP_PORT_10GE_1 = 0x31, /** 10GE_SFP+ Port1    */
    DG_CMN_DRV_LOOP_PORT_10GE_2 = 0x32, /** 10GE_SFP+ Port2    */
    DG_CMN_DRV_LOOP_PORT_10GE_3 = 0x33, /** 10GE_SFP+ Port3    */
};
typedef UINT8 DG_CMN_DRV_LOOP_PORT_T;

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
/** narmal loopback packet size */
typedef UINT16 DG_CMN_DRV_LOOP_PACKET_SIZE_T;

/** LOOP packets number */
typedef UINT16 DG_CMN_DRV_LOOP_PACKET_NUM_T;

/** LOOP data pattern type */
typedef UINT8 DG_CMN_DRV_LOOP_PACKET_PATTERN_T;

/*==================================================================================================
                                   GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*=============================================================================================*//**
@brief Normal loopback test

@param[in] tx_port - the tx port selection
@param[in] rx_port - the rx port selection
@param[in] size    - the loopback packet size
@param[in] num     - how many loopback packet to send
@param[in] pattern - content of each byte in the packet data field

*//*==============================================================================================*/
BOOL DG_CMN_DRV_LOOP_test(DG_CMN_DRV_LOOP_PORT_T           tx_port,
                          DG_CMN_DRV_LOOP_PORT_T           rx_port,
                          DG_CMN_DRV_LOOP_PACKET_SIZE_T    size,
                          DG_CMN_DRV_LOOP_PACKET_NUM_T     num,
                          DG_CMN_DRV_LOOP_PACKET_PATTERN_T pattern);

#ifdef __cplusplus
}
#endif

/** @} */
/** @} */

#endif /* _DG_CMN_DRV_LOOP_H_  */

