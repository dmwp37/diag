#ifndef _DG_CMN_DRV_PRBS_H_
#define _DG_CMN_DRV_PRBS_H_
/*==================================================================================================

    Module Name:  dg_cmn_drv_prbs.h

    General Description: This file provides driver interface for PRBS test

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup PRBS_driver
@{

@par
Provide APIs for PRBS
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
    DG_CMN_DRV_PRBS_BUS_HiGig0  = 0x00,
    DG_CMN_DRV_PRBS_BUS_HiGig1  = 0x01,
    DG_CMN_DRV_PRBS_BUS_QSGMII0 = 0x10,
    DG_CMN_DRV_PRBS_BUS_QSGMII1 = 0x11,
    DG_CMN_DRV_PRBS_BUS_QSGMII2 = 0x12,
    DG_CMN_DRV_PRBS_BUS_QSGMII3 = 0x13,
    DG_CMN_DRV_PRBS_BUS_SFI0    = 0x20,
    DG_CMN_DRV_PRBS_BUS_SFI1    = 0x21,
    DG_CMN_DRV_PRBS_BUS_SFI2    = 0x22,
    DG_CMN_DRV_PRBS_BUS_SFI3    = 0x23,
};
typedef UINT8 DG_CMN_DRV_PRBS_BUS_T;

enum
{
    DG_CMN_DRV_PRBS_ORDER_7TH  = 0x00,
    DG_CMN_DRV_PRBS_ORDER_15TH = 0x01,
    DG_CMN_DRV_PRBS_ORDER_23TH = 0x02,
    DG_CMN_DRV_PRBS_ORDER_31TH = 0x03,
};
typedef UINT8 DG_CMN_DRV_PRBS_ORDER_T;

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef UINT32 DG_CMN_DRV_PRBS_COUNT_T;


/*==================================================================================================
                                   GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*=============================================================================================*//**
@brief PRBS test operate

@param[in]  bus   - bus need to be test
@param[in]  order - order need to be use
@param[out] count - PRBS error count value
*//*==============================================================================================*/
BOOL DG_CMN_DRV_PRBS_operate(DG_CMN_DRV_PRBS_BUS_T    bus,
                             DG_CMN_DRV_PRBS_ORDER_T  order,
                             DG_CMN_DRV_PRBS_COUNT_T* count);

#ifdef __cplusplus
}
#endif

/** @} */
/** @} */

#endif /* _DG_CMN_DRV_PRBS_H_  */

