#ifndef _DG_CMN_DRV_INT_LOOP_H_
#define _DG_CMN_DRV_INT_LOOP_H_
/*==================================================================================================

    Module Name:  dg_cmn_drv_int_loop.h

    General Description: This file provides driver interface for Internal Loopback test

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup INT_LOOP_driver
@{

@par
Provide APIs for INT_LOOP
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

/** internal loopback path port definition */
enum
{
    DG_CMN_DRV_INT_LOOP_PORT_ALL    = 0x00, /** All the interfaces */
    DG_CMN_DRV_INT_LOOP_PORT_MGT    = 0x01, /** MGT Port           */
    DG_CMN_DRV_INT_LOOP_PORT_HA     = 0x02, /** HA Port            */
    DG_CMN_DRV_INT_LOOP_PORT_WTB0_1 = 0x03, /** WANPIM 0 Port 1    */
    DG_CMN_DRV_INT_LOOP_PORT_WTB0_2 = 0x04, /** WANPIM 0 Port 2    */
    DG_CMN_DRV_INT_LOOP_PORT_WTB1_1 = 0x05, /** WANPIM 1 Port 1    */
    DG_CMN_DRV_INT_LOOP_PORT_WTB1_2 = 0x06, /** WANPIM 1 Port 2    */
    DG_CMN_DRV_INT_LOOP_PORT_GE_0   = 0x10, /** GE_RJ45 Port 0     */
    DG_CMN_DRV_INT_LOOP_PORT_GE_1   = 0x11, /** GE_RJ45 Port 1     */
    DG_CMN_DRV_INT_LOOP_PORT_GE_2   = 0x12, /** GE_RJ45 Port 2     */
    DG_CMN_DRV_INT_LOOP_PORT_GE_3   = 0x13, /** GE_RJ45 Port 3     */
    DG_CMN_DRV_INT_LOOP_PORT_GE_4   = 0x14, /** GE_RJ45 Port 4     */
    DG_CMN_DRV_INT_LOOP_PORT_GE_5   = 0x15, /** GE_RJ45 Port 5     */
    DG_CMN_DRV_INT_LOOP_PORT_GE_6   = 0x16, /** GE_RJ45 Port 6     */
    DG_CMN_DRV_INT_LOOP_PORT_GE_7   = 0x17, /** GE_RJ45 Port 7     */
    DG_CMN_DRV_INT_LOOP_PORT_GE_8   = 0x20, /** GE_RJ45 Port 8     */
    DG_CMN_DRV_INT_LOOP_PORT_GE_9   = 0x21, /** GE_RJ45 Port 9     */
    DG_CMN_DRV_INT_LOOP_PORT_GE_10  = 0x22, /** GE_RJ45 Port 10    */
    DG_CMN_DRV_INT_LOOP_PORT_GE_11  = 0x23, /** GE_RJ45 Port 11    */
    DG_CMN_DRV_INT_LOOP_PORT_SFP_0  = 0x24, /** GE_SFP  Port 0     */
    DG_CMN_DRV_INT_LOOP_PORT_SFP_1  = 0x25, /** GE_SFP  Port 1     */
    DG_CMN_DRV_INT_LOOP_PORT_SFP_2  = 0x26, /** GE_SFP  Port 2     */
    DG_CMN_DRV_INT_LOOP_PORT_SFP_3  = 0x27, /** GE_SFP  Port 3     */
    DG_CMN_DRV_INT_LOOP_PORT_10GE_0 = 0x30, /** 10GE_SFP+ Port0    */
    DG_CMN_DRV_INT_LOOP_PORT_10GE_1 = 0x31, /** 10GE_SFP+ Port1    */
    DG_CMN_DRV_INT_LOOP_PORT_10GE_2 = 0x32, /** 10GE_SFP+ Port2    */
    DG_CMN_DRV_INT_LOOP_PORT_10GE_3 = 0x33, /** 10GE_SFP+ Port3    */
};
typedef UINT8 DG_CMN_DRV_INT_LOOP_PORT_T;

/** internal loopback network node definition */
enum
{
    DG_CMN_DRV_INT_LOOP_NODE_FPGA = 0x00,
    DG_CMN_DRV_INT_LOOP_NODE_MAC  = 0x01,
    DG_CMN_DRV_INT_LOOP_NODE_PHY  = 0x02,
    DG_CMN_DRV_INT_LOOP_NODE_PORT = 0x03, /** External loopback cable/SFP/SFP+ */
};
typedef UINT8 DG_CMN_DRV_INT_LOOP_NODE_T;

/** internal loopback interface speed configuration
 * this is not used yet, the driver would use the
 * default (1000M) config first */
enum
{
    DG_CMN_DRV_INT_LOOP_CFG_10M   = 0x00,
    DG_CMN_DRV_INT_LOOP_CFG_100M  = 0x01,
    DG_CMN_DRV_INT_LOOP_CFG_1000M = 0x02,
    DG_CMN_DRV_INT_LOOP_CFG_10G   = 0x03,
};
typedef UINT8 DG_CMN_DRV_INT_LOOP_CFG_T;

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/** INT_LOOP packet size */
typedef UINT16 DG_CMN_DRV_INT_LOOP_PACKET_SIZE_T;

/** INT_LOOP packets number */
typedef UINT16 DG_CMN_DRV_INT_LOOP_PACKET_NUM_T;

/** INT_LOOP data pattern type */
typedef UINT8 DG_CMN_DRV_INT_LOOP_PACKET_PATTERN_T;

/*==================================================================================================
                                   GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*=============================================================================================*//**
@brief Internal loopback test

@param[in] port    - the path to test on which port
@param[in] node    - where to loopback packet
@param[in] cfg     - port configuration
@param[in] size    - the loopback packet size
@param[in] num     - how many loopback packet to send
@param[in] pattern - content of each byte in the packet data field

*//*==============================================================================================*/
BOOL DG_CMN_DRV_INT_LOOP_test(DG_CMN_DRV_INT_LOOP_PORT_T           port,
                              DG_CMN_DRV_INT_LOOP_NODE_T           node,
                              DG_CMN_DRV_INT_LOOP_CFG_T            cfg,
                              DG_CMN_DRV_INT_LOOP_PACKET_SIZE_T    size,
                              DG_CMN_DRV_INT_LOOP_PACKET_NUM_T     num,
                              DG_CMN_DRV_INT_LOOP_PACKET_PATTERN_T pattern);

#ifdef __cplusplus
}
#endif

/** @} */
/** @} */

#endif /* _DG_CMN_DRV_INT_LOOP_H_  */

