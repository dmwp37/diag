/*==================================================================================================

    Module Name:  dg_cmn_drv_pttm.c

    General Description: Implements the PTTM common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_pttm.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup PTTM_driver
@{
implementation of the PTTM driver
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
static DG_CMN_DRV_PTTM_DATA_T dg_cmn_drv_pttm_data[3] = { 0 }; /* for 3 PTTM chips simulation */

/*==================================================================================================
                                         GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Reads RDAC register from PTTM

@param[in]  chip - PTTM chip selection
@param[out] data - The RDAC register data from PTTM

*//*==============================================================================================*/
BOOL DG_CMN_DRV_PTTM_get(DG_CMN_DRV_PTTM_CHIP_T chip, DG_CMN_DRV_PTTM_DATA_T* data)
{
    BOOL ret = FALSE;

    if ((chip != DG_CMN_DRV_PTTM_AD5247) &&
        (chip != DG_CMN_DRV_PTTM_ISL90727) &&
        (chip != DG_CMN_DRV_PTTM_ISL90728))
    {
        DG_DRV_UTIL_set_error_string("Invalid chip=%d", chip);
    }
    else
    {
        *data = dg_cmn_drv_pttm_data[chip];

        DG_DBG_TRACE("PTTM chip %d got value: %d", chip, *data);

        ret = TRUE;
    }

    return ret;
}

/*=============================================================================================*//**
@brief Set RDAC register to PTTM

@param[in] chip - PTTM chip selection
@param[in] data - The RDAC register data set to PTTM

*//*==============================================================================================*/
BOOL DG_CMN_DRV_PTTM_set(DG_CMN_DRV_PTTM_CHIP_T chip, DG_CMN_DRV_PTTM_DATA_T data)
{
    BOOL ret = FALSE;

    if ((chip != DG_CMN_DRV_PTTM_AD5247) &&
        (chip != DG_CMN_DRV_PTTM_ISL90727) &&
        (chip != DG_CMN_DRV_PTTM_ISL90728))
    {
        DG_DRV_UTIL_set_error_string("Invalid chip=%d", chip);
    }
    else if (data > DG_CMN_DRV_PTTM_DATE_MAX)
    {
        DG_DRV_UTIL_set_error_string("PTTM data is not valid. val=%d", data);
    }
    else
    {
        dg_cmn_drv_pttm_data[chip] = data;

        DG_DBG_TRACE("PTTM chip %d set value: %d", chip, data);

        ret = TRUE;
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

