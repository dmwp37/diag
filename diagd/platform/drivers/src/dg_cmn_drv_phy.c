/*==================================================================================================

    Module Name:  dg_cmn_drv_phy.c

    General Description: Implements the PHY common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_phy.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup PHY_driver
@{
implementation of the PHY driver
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
@brief Reads register from a given PHY port and address

@param[in]  port   - The PHY port to read from
@param[in]  addr   - The PHY address to read from
@param[out] val    - The PHY reg value pointer read from the device

@note
- Read data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_PHY_read_reg(DG_CMN_DRV_PHY_PORT_T port,
                             DG_CMN_DRV_PHY_ADDR_T addr,
                             DG_CMN_DRV_PHY_REG_T* val)
{
    BOOL ret = FALSE;

    switch (port)
    {
    case DG_CMN_DRV_PHY_CPUMGT:
    case DG_CMN_DRV_PHY0_Port0:
    case DG_CMN_DRV_PHY0_Port1:
    case DG_CMN_DRV_PHY0_Port2:
    case DG_CMN_DRV_PHY0_Port3:
    case DG_CMN_DRV_PHY0_Port4:
    case DG_CMN_DRV_PHY0_Port5:
    case DG_CMN_DRV_PHY0_Port6:
    case DG_CMN_DRV_PHY0_Port7:
    case DG_CMN_DRV_PHY0_Port8:
    case DG_CMN_DRV_PHY1_Port0:
    case DG_CMN_DRV_PHY1_Port1:
    case DG_CMN_DRV_PHY1_Port2:
    case DG_CMN_DRV_PHY1_Port3:
    case DG_CMN_DRV_PHY1_Port4:
    case DG_CMN_DRV_PHY1_Port5:
    case DG_CMN_DRV_PHY1_Port6:
    case DG_CMN_DRV_PHY1_Port7:
    case DG_CMN_DRV_PHY1_Port8:
        *val = port;
        DG_DBG_TRACE("Read PHY port:0x%02x addr:0x%02x register value=0x%04x", port, addr, *val);
        ret = TRUE;
        break;

    default:
        DG_DRV_UTIL_set_error_string("Read PHY register failed, invalid port:0x%02x", port);
        break;
    }

    return ret;
}

/*=============================================================================================*//**
@brief Writes register to a given PHY port and address

@param[in]  port   - The PHY port to read from
@param[in]  addr   - The PHY address to read from
@param[in]  val    - The PHY register value to write to the device
*//*==============================================================================================*/
BOOL DG_CMN_DRV_PHY_write_reg(DG_CMN_DRV_PHY_PORT_T port,
                              DG_CMN_DRV_PHY_ADDR_T addr,
                              DG_CMN_DRV_PHY_REG_T  val)
{
    BOOL ret = FALSE;

    switch (port)
    {
    case DG_CMN_DRV_PHY_CPUMGT:
    case DG_CMN_DRV_PHY0_Port0:
    case DG_CMN_DRV_PHY0_Port1:
    case DG_CMN_DRV_PHY0_Port2:
    case DG_CMN_DRV_PHY0_Port3:
    case DG_CMN_DRV_PHY0_Port4:
    case DG_CMN_DRV_PHY0_Port5:
    case DG_CMN_DRV_PHY0_Port6:
    case DG_CMN_DRV_PHY0_Port7:
    case DG_CMN_DRV_PHY0_Port8:
    case DG_CMN_DRV_PHY1_Port0:
    case DG_CMN_DRV_PHY1_Port1:
    case DG_CMN_DRV_PHY1_Port2:
    case DG_CMN_DRV_PHY1_Port3:
    case DG_CMN_DRV_PHY1_Port4:
    case DG_CMN_DRV_PHY1_Port5:
    case DG_CMN_DRV_PHY1_Port6:
    case DG_CMN_DRV_PHY1_Port7:
    case DG_CMN_DRV_PHY1_Port8:
        /* *addr = val; */
        DG_DBG_TRACE("Write PHY port:0x%02x addr:0x%02x register value=0x%04x", port, addr, val);
        ret = TRUE;
        break;

    default:
        DG_DRV_UTIL_set_error_string("Write PHY register failed, invalid port:0x%02x", port);
        break;
    }

    return ret;
}


/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

