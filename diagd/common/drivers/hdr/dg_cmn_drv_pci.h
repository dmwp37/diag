#ifndef _DG_CMN_DRV_PCI_H_
#define _DG_CMN_DRV_PCI_H_
/*==================================================================================================

    Module Name:  dg_cmn_drv_pci.h

    General Description: This file provides driver interface for PCI test

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup PCI_driver
@{

@par
Provide APIs for PCI
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

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef UINT8 DG_CMN_DRV_PCI_DOMAIN_T;
typedef UINT8 DG_CMN_DRV_PCI_BUS_T;
typedef UINT8 DG_CMN_DRV_PCI_DEV_T;
typedef UINT8 DG_CMN_DRV_PCI_FUNC_T;
typedef UINT32 DG_CMN_DRV_PCI_OFFSET_T;
typedef UINT32 DG_CMN_DRV_PCI_LEN_T;

/*==================================================================================================
                                   GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*=============================================================================================*//**
@General PCIe bus config space register read

@param[in]  domain    - The PCI domain ID to read from
@param[in]  bus       - The PCI bus ID to read from
@param[in]  dev       - The PCI device ID to read from
@param[in]  func      - The PCI function ID to read from
@param[in]  offset    - The device register offset to read from
@param[in]  len       - The amount of data to read in bytes
@param[out] data      - The data read from the device

@note
- Read data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_PCI_cfg_read(UINT8 domain, UINT8 bus, UINT8 dev, UINT8 func,
                             UINT32 offset, UINT32 len, UINT8* data);

/*=============================================================================================*//**
@General PCIe bus memory space register read

@param[in]  domain    - The PCI domain ID to read from
@param[in]  bus       - The PCI bus ID to read from
@param[in]  dev       - The PCI device ID to read from
@param[in]  func      - The PCI function ID to read from
@param[in]  offset    - The device register offset to read from
@param[in]  len       - The amount of data to read in bytes
@param[out] data      - The data read from the device

@note
- Read data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_PCI_mem_read(UINT8 domain, UINT8 bus, UINT8 dev, UINT8 func,
                             UINT32 offset, UINT32 len, UINT8* data);

#ifdef __cplusplus
}
#endif

/** @} */
/** @} */

#endif /* _DG_CMN_DRV_DBG_LVL_H_  */

