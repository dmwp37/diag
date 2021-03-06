/*==================================================================================================

    Module Name:  dg_cmn_drv_cpld.c

    General Description: Implements the CPLD common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_cpld.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup CPLD_driver
@{
implementation of the CPLD driver
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
@brief Get the CPLD register

@param[in]  id     - The CPLD ID
@param[in]  offset - The register offset position in the CPLD
@param[out] val    - The register value

*//*==============================================================================================*/
BOOL DG_CMN_DRV_CPLD_get(DG_CMN_DRV_CPLD_ID_T     id,
                         DG_CMN_DRV_CPLD_OFFSET_T offset,
                         DG_CMN_DRV_CPLD_VALUE_T* val)
{
    BOOL ret = FALSE;

    if ((id != DG_CMN_DRV_CPLD_CB) && (id != DG_CMN_DRV_CPLD_FEB))
    {
        DG_DRV_UTIL_set_error_string("Invalid CPLD id=%d", id);
    }
    else
    {
        *val = id;

        DG_DBG_TRACE("CPLD ID %d offset 0x%04x got value: 0x%02x", id, offset, *val);

        ret = TRUE;
    }

    return ret;
}

/*=============================================================================================*//**
@brief Set the CPLD register

@param[in]  id     - The CPLD ID
@param[in]  offset - The register offset position in the CPLD
@param[out] val    - The register value

*//*==============================================================================================*/
BOOL DG_CMN_DRV_CPLD_set(DG_CMN_DRV_CPLD_ID_T     id,
                         DG_CMN_DRV_CPLD_OFFSET_T offset,
                         DG_CMN_DRV_CPLD_VALUE_T  val)
{
    BOOL ret = FALSE;

    if ((id != DG_CMN_DRV_CPLD_CB) && (id != DG_CMN_DRV_CPLD_FEB))
    {
        DG_DRV_UTIL_set_error_string("Invalid CPLD id=%d", id);
    }
    else
    {
        DG_DBG_TRACE("CPLD ID %d offset 0x%04x set value: %d", id, offset, val);

        ret = TRUE;
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

