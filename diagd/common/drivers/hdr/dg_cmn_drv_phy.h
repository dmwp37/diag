#ifndef _DG_CMN_DRV_PHY_H_
#define _DG_CMN_DRV_PHY_H_
/*==================================================================================================

    Module Name:  dg_cmn_drv_phy.h

    General Description: This file provides driver interface for PHY test

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup PHY_driver
@{

@par
Provide APIs for PHY
*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                              MACROS
==================================================================================================*/
/*
See also datasheet page 90.
The BCM54685E has nine unique PHY addresses for MII management. Eight for PHY address pins and one
for the QSGMII interface. PHY address for the ports are set to the following:
 Port 1 is set through the PHYA[4:0] pins
 Port 2 is the PHY address pins + 1
 Port 3 is the PHY address pins + 2
 Port 4 is the PHY address pins + 3
 Port 5 is the PHY address pins + 4
 Port 6 is the PHY address pins + 5
 Port 7 is the PHY address pins + 6
 Port 8 is the PHY address pins + 7
 QSGMII is the PHY address pins + 8
*/
enum
{
    DG_CMN_DRV_PHY_CPUMGT = 0x00,
    DG_CMN_DRV_PHY0_Port0 = 0x10,
    DG_CMN_DRV_PHY0_Port1 = 0x11,
    DG_CMN_DRV_PHY0_Port2 = 0x12,
    DG_CMN_DRV_PHY0_Port3 = 0x13,
    DG_CMN_DRV_PHY0_Port4 = 0x14,
    DG_CMN_DRV_PHY0_Port5 = 0x15,
    DG_CMN_DRV_PHY0_Port6 = 0x16,
    DG_CMN_DRV_PHY0_Port7 = 0x17,
    DG_CMN_DRV_PHY0_Port8 = 0x18, /* QSGMII PHY */
    DG_CMN_DRV_PHY1_Port0 = 0x20,
    DG_CMN_DRV_PHY1_Port1 = 0x21,
    DG_CMN_DRV_PHY1_Port2 = 0x22,
    DG_CMN_DRV_PHY1_Port3 = 0x23,
    DG_CMN_DRV_PHY1_Port4 = 0x24,
    DG_CMN_DRV_PHY1_Port5 = 0x25,
    DG_CMN_DRV_PHY1_Port6 = 0x26,
    DG_CMN_DRV_PHY1_Port7 = 0x27,
    DG_CMN_DRV_PHY1_Port8 = 0x28, /* QSGMII PHY */
};
typedef UINT8 DG_PHY_PORT_T;


/*==================================================================================================
                                               ENUMS
==================================================================================================*/

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef UINT8 DG_CMN_DRV_PHY_PORT_T;
typedef UINT8 DG_CMN_DRV_PHY_ADDR_T;
typedef UINT16 DG_CMN_DRV_PHY_REG_T;

/*==================================================================================================
                                   GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*=============================================================================================*//**
@brief Reads register from a given PHY port and address

@param[in]  port   - The PHY port to read from
@param[in]  addr   - The PHY address to read from
@param[out] val    - The PHY reg value pointer read from the device

@note
- Read data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_PHY_read_reg(DG_CMN_DRV_PHY_PORT_T port,
                             DG_CMN_DRV_PHY_ADDR_T addr,
                             DG_CMN_DRV_PHY_REG_T* val);

/*=============================================================================================*//**
@brief Writes register to a given PHY port and address

@param[in]  port   - The PHY port to read from
@param[in]  addr   - The PHY address to read from
@param[in]  val    - The PHY register value to write to the device
*//*==============================================================================================*/
BOOL DG_CMN_DRV_PHY_write_reg(DG_CMN_DRV_PHY_PORT_T port,
                              DG_CMN_DRV_PHY_ADDR_T addr,
                              DG_CMN_DRV_PHY_REG_T  val);


#ifdef __cplusplus
}
#endif

/** @} */
/** @} */

#endif /* _DG_CMN_DRV_PHY_H_  */

