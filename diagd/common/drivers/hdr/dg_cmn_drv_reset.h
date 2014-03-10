#ifndef _DG_CMN_DRV_RESET_H_
#define _DG_CMN_DRV_RESET_H_
/*==================================================================================================

    Module Name:  dg_cmn_drv_reset.h

    General Description: This file provides driver interface for RESET test

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup RESET_driver
@{

@par
Provide APIs for RESET
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
/** RESET Chip Selection */
enum
{
    DG_CMN_DRV_RESET_CPU           = 0x00,
    DG_CMN_DRV_RESET_FEB           = 0x01,
    DG_CMN_DRV_RESET_CB_RENESAS    = 0x02,
    DG_CMN_DRV_RESET_I2C           = 0x03,
    DG_CMN_DRV_RESET_PHY0          = 0x10,
    DG_CMN_DRV_RESET_PHY1          = 0x11,
    DG_CMN_DRV_RESET_SWITCH        = 0x12,
    DG_CMN_DRV_RESET_MGT_PHY       = 0x13,
    DG_CMN_DRV_RESET_FPGA          = 0x14,
    DG_CMN_DRV_RESET_FPGA_PCIE     = 0x15,
    DG_CMN_DRV_RESET_FEB_RENESAS   = 0x16,
    DG_CMN_DRV_RESET_FPGA_FLASH    = 0x17,
    DG_CMN_DRV_RESET_SWITCH_PERST0 = 0x20,
    DG_CMN_DRV_RESET_SWITCH_PERST1 = 0x21,
    DG_CMN_DRV_RESET_I2C_MUX       = 0x22,
    DG_CMN_DRV_RESET_WANPIM1       = 0x23,
    DG_CMN_DRV_RESET_WANPIM2       = 0x24,
    DG_CMN_DRV_RESET_SYS           = 0xFF,
};
typedef UINT8 DG_CMN_DRV_RESET_COMP_T;

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
@brief Reads data from a given address on the given RESET bus

@param[in]  comp       - Which component to reset
@param[in]  is_reset   - TRUE to reset and FALSE to recover

@note
- Read data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_RESET_reset(DG_CMN_DRV_RESET_COMP_T comp, BOOL is_reset);

#ifdef __cplusplus
}
#endif

/** @} */
/** @} */

#endif /* _DG_CMN_DRV_RESET_H_  */

