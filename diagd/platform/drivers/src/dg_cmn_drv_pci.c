/*==================================================================================================

    Module Name:  dg_cmn_drv_pci.c

    General Description: Implements the PCI common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_pci.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup PCI_driver
@{
implementation of the PCI driver
*/

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

/*==================================================================================================
                                         GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                         GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Reads data from a given address on the given PCI config space

@param[in]  domain    - The PCI domain ID to read from
@param[in]  bus       - The PCI bus ID to read from
@param[in]  dev       - The PCI device ID to read from
@param[in]  func      - The PCI function ID to read from
@param[in]  offset    - The device register offset to read from
@param[in]  len       - The amount of data to read in bytes
@param[out] read_data - The data read from the device

@note
- Read data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_PCI_cfg_read(DG_CMN_DRV_PCI_DOMAIN_T domain,
                             DG_CMN_DRV_PCI_BUS_T    bus,
                             DG_CMN_DRV_PCI_DEV_T    dev,
                             DG_CMN_DRV_PCI_FUNC_T   func,
                             DG_CMN_DRV_PCI_OFFSET_T offset,
                             DG_CMN_DRV_PCI_LEN_T    len,
                             UINT8*                  read_data)
{
    BOOL ret = FALSE;

    if (domain == 0)
    {
        memset(read_data, offset, len);
        DG_DBG_TRACE("Read PCI config space: domain=0x%02x, bus=0x%02x, dev=0x%02x, func=0x%02x, "
                     "offset=0x%08x, len=0x%08x", domain, bus, dev, func, offset, len);
        ret = TRUE;
    }
    else
    {
        DG_DRV_UTIL_set_error_string("Read PCI config space failed: domain=0x%02x, bus=0x%02x, "
                                     "dev=0x%02x, func=0x%02x, offset=0x%08x, len=0x%08x",
                                     domain, bus, dev, func, offset, len);
    }

    return ret;
}

/*=============================================================================================*//**
@brief Writes data to a given address on the given PCI memory space

@param[in]  domain    - The PCI domain ID to read from
@param[in]  bus       - The PCI bus ID to read from
@param[in]  dev       - The PCI device ID to read from
@param[in]  func      - The PCI function ID to read from
@param[in]  offset    - The device register offset to read from
@param[in]  len       - The amount of data to read in bytes
@param[out] read_data - The data read from the device
*//*==============================================================================================*/
BOOL DG_CMN_DRV_PCI_mem_read(DG_CMN_DRV_PCI_DOMAIN_T domain,
                             DG_CMN_DRV_PCI_BUS_T    bus,
                             DG_CMN_DRV_PCI_DEV_T    dev,
                             DG_CMN_DRV_PCI_FUNC_T   func,
                             DG_CMN_DRV_PCI_OFFSET_T offset,
                             DG_CMN_DRV_PCI_LEN_T    len,
                             UINT8*                  read_data)
{
    BOOL ret = FALSE;

    if (domain == 0)
    {
        memset(read_data, offset, len);
        DG_DBG_TRACE("Read PCI memory space: domain=0x%02x, bus=0x%02x, dev=0x%02x, func=0x%02x, "
                     "offset=0x%08x, len=0x%08x", domain, bus, dev, func, offset, len);
        ret = TRUE;
    }
    else
    {
        DG_DRV_UTIL_set_error_string("Read PCI memory space failed: domain=0x%02x, bus=0x%02x, "
                                     "dev=0x%02x, func=0x%02x, offset=0x%08x, len=0x%08x",
                                     domain, bus, dev, func, offset, len);
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

