#ifndef _DG_CMN_DRV_SFP_H_
#define _DG_CMN_DRV_SFP_H_
/*==================================================================================================

    Module Name:  dg_cmn_drv_sfp.h

    General Description: This file provides driver interface for SFP test

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup SFP_driver
@{

@par
Provide APIs for SFP
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

/** SFP ID type */
enum
{
    DG_CMN_DRV_SFP_ID_HA    = 0x00,
    DG_CMN_DRV_SFP_ID_SFP12 = 0x01,
    DG_CMN_DRV_SFP_ID_SFP13 = 0x02,
    DG_CMN_DRV_SFP_ID_SFP14 = 0x03,
    DG_CMN_DRV_SFP_ID_SFP15 = 0x04,
    DG_CMN_DRV_SFP_ID_SFP16 = 0x05,
    DG_CMN_DRV_SFP_ID_SFP17 = 0x06,
    DG_CMN_DRV_SFP_ID_SFP18 = 0x07,
    DG_CMN_DRV_SFP_ID_SFP19 = 0x08,
    DG_CMN_DRV_SFP_ID_MAX   = 0x08
};
typedef UINT8 DG_CMN_DRV_SFP_ID_T;

/** SFP status, Bits[4-7]: Reserved */
enum
{
    DG_CMN_DRV_SFP_STATUS_PRESENT_BIT  = 0x01,
    DG_CMN_DRV_SFP_STATUS_TX_FAULT_BIT = 0x02,
    DG_CMN_DRV_SFP_STATUS_RX_LOS_BIT   = 0x04,
    DG_CMN_DRV_SFP_STATUS_TX_EN_BIT    = 0x08,
};
typedef UINT8 DG_CMN_DRV_SFP_STATUS_T;

/** SFP registers */
enum
{
    DG_CMN_DRV_SFP_REG_A0 = 0x00,
    DG_CMN_DRV_SFP_REG_A2 = 0x01,
};
typedef UINT8 DG_CMN_DRV_SFP_REG_T;

/** SFP TX config */
enum
{
    DG_CMN_DRV_SFP_TX_DISABLE = 0x00,
    DG_CMN_DRV_SFP_TX_ENABLE  = 0x01,
};
typedef UINT8 DG_CMN_DRV_SFP_TX_CFG_T;

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/** SFP register data */
typedef struct
{
    UINT8 reg[256];
} DG_CMN_DRV_SFP_REG_DATA_T;

/*==================================================================================================
                                   GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*=============================================================================================*//**
@brief Get the SFP status

@param[in]  sfp    - selected SFP
@param[out] status - The SFP status

*//*==============================================================================================*/
BOOL DG_CMN_DRV_SFP_get_status(DG_CMN_DRV_SFP_ID_T sfp, DG_CMN_DRV_SFP_STATUS_T* status);

/*=============================================================================================*//**
@brief Configure SFP TX

@param[in] sfp - selected SFP
@param[in] cfg - new configuration of the SFP TX

*//*==============================================================================================*/
BOOL DG_CMN_DRV_SFP_tx_config(DG_CMN_DRV_SFP_ID_T sfp, DG_CMN_DRV_SFP_TX_CFG_T cfg);

/*=============================================================================================*//**
@brief Dump the SFP registers

@param[in]  sfp  - selected SFP
@param[in]  reg  - which register set to dump
@param[out] data - the dumped register data

*//*==============================================================================================*/
BOOL DG_CMN_DRV_SFP_dump_register(DG_CMN_DRV_SFP_ID_T        sfp,
                                  DG_CMN_DRV_SFP_REG_T       reg,
                                  DG_CMN_DRV_SFP_REG_DATA_T* data);

#ifdef __cplusplus
}
#endif

/** @} */
/** @} */

#endif /* _DG_CMN_DRV_SFP_H_  */

