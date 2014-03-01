#ifndef _DG_CMN_DRV_EXT_LOOP_H_
#define _DG_CMN_DRV_EXT_LOOP_H_
/*==================================================================================================

    Module Name:  dg_cmn_drv_ext_loop.h

    General Description: This file provides driver interface for external loopback test

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup EXT_LOOP_driver
@{

@par
Provide APIs for EXT_LOOP
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
enum
{
    DG_CMN_DRV_EXT_LOOP_ACTION_CLEAR = 0x00,
    DG_CMN_DRV_EXT_LOOP_ACTION_SET   = 0x01,
};
typedef UINT8 DG_CMN_DRV_EXT_LOOP_ACTION_T;

enum
{
    DG_CMN_DRV_EXT_LOOP_NODE_PCH     = 0x00,
    DG_CMN_DRV_EXT_LOOP_NODE_FPGA    = 0x01,
    DG_CMN_DRV_EXT_LOOP_NODE_SWITCH  = 0x02,
    DG_CMN_DRV_EXT_LOOP_NODE_PHY_MGT = 0x10,
    DG_CMN_DRV_EXT_LOOP_NODE_PHY0_P1 = 0x11,
    DG_CMN_DRV_EXT_LOOP_NODE_PHY0_P2 = 0x12,
    DG_CMN_DRV_EXT_LOOP_NODE_PHY0_P3 = 0x13,
    DG_CMN_DRV_EXT_LOOP_NODE_PHY0_P4 = 0x14,
    DG_CMN_DRV_EXT_LOOP_NODE_PHY0_P5 = 0x15,
    DG_CMN_DRV_EXT_LOOP_NODE_PHY0_P6 = 0x16,
    DG_CMN_DRV_EXT_LOOP_NODE_PHY0_P7 = 0x17,
    DG_CMN_DRV_EXT_LOOP_NODE_PHY0_P8 = 0x18,
    DG_CMN_DRV_EXT_LOOP_NODE_PHY1_P1 = 0x19,
    DG_CMN_DRV_EXT_LOOP_NODE_PHY1_P2 = 0x1A,
    DG_CMN_DRV_EXT_LOOP_NODE_PHY1_P3 = 0x1B,
    DG_CMN_DRV_EXT_LOOP_NODE_PHY1_P4 = 0x1C,
};
typedef UINT8 DG_CMN_DRV_EXT_LOOP_NODE_T;

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/


/*==================================================================================================
                                   GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*=============================================================================================*//**
@brief external loopback operate

@param[in]  action   - set/clear
@param[out] node     - node need to be operate

@note
- result is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_EXT_LOOP_operate(DG_CMN_DRV_EXT_LOOP_ACTION_T action,
                                 DG_CMN_DRV_EXT_LOOP_NODE_T   node);

#ifdef __cplusplus
}
#endif

/** @} */
/** @} */

#endif /* _DG_CMN_DRV_EXT_LOOP_H_  */

