/*==================================================================================================

    Module Name:  dg_cmn_drv_prbs.c

    General Description: Implements the PRBS common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_prbs.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup PRBS_driver
@{
implementation of the PRBS driver
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
@brief PRBS test operate

@param[in]  bus     - bus need to be test
@param[in]  order   - order need to be use
@param[out] count - PRBS error count value
*//*==============================================================================================*/
BOOL DG_CMN_DRV_PRBS_operate(DG_CMN_DRV_PRBS_BUS_T    bus,
                             DG_CMN_DRV_PRBS_ORDER_T  order,
                             DG_CMN_DRV_PRBS_COUNT_T* count)
{
    BOOL ret = FALSE;

    switch (bus)
    {
    case DG_CMN_DRV_PRBS_BUS_HiGig0:
    case DG_CMN_DRV_PRBS_BUS_HiGig1:
    case DG_CMN_DRV_PRBS_BUS_QSGMII0:
    case DG_CMN_DRV_PRBS_BUS_QSGMII1:
    case DG_CMN_DRV_PRBS_BUS_QSGMII2:
    case DG_CMN_DRV_PRBS_BUS_QSGMII3:
    case DG_CMN_DRV_PRBS_BUS_SFI0:
    case DG_CMN_DRV_PRBS_BUS_SFI1:
    case DG_CMN_DRV_PRBS_BUS_SFI2:
    case DG_CMN_DRV_PRBS_BUS_SFI3:
        switch (order)
        {
        case DG_CMN_DRV_PRBS_ORDER_7TH:
        case DG_CMN_DRV_PRBS_ORDER_15TH:
        case DG_CMN_DRV_PRBS_ORDER_23TH:
        case DG_CMN_DRV_PRBS_ORDER_31TH:
            *count = 0;
            DG_DBG_TRACE("PRBS test bus:0x%02x, order:0x%02x", bus, order);
            ret = TRUE;
            break;

        default:
            DG_DRV_UTIL_set_error_string("Invalid bus 0x%02x", bus);
            break;
        }
        break;

    default:
        DG_DRV_UTIL_set_error_string("Invalid order 0x%02x", order);
        break;
    }

    return ret;
}


/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

