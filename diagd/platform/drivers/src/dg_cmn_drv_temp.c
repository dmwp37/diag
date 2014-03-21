/*==================================================================================================

    Module Name:  dg_cmn_drv_temp.c

    General Description: Implements the Temperature common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_temp.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup TEMP_driver
@{
implementation of the Temperature driver
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
@brief Get temperature from specified temp sensor

@param[in]  sensor      - temperature sensor selection
@param[out] temperature - The temperature from the sensor (convert from float)

@note
- the temp is a UINT32 pointer, but it must contains the exact data as a float
- *temp = *(UINT32*)&(float_temperature)
*//*==============================================================================================*/
BOOL DG_CMN_DRV_TEMP_get(DG_CMN_DRV_TEMP_SENSOR_T sensor, DG_CMN_DRV_TEMP_DATA_T* temperature)
{
    BOOL ret = FALSE;

    float f_temp = 23.4;

    switch (sensor)
    {
    case DG_CMN_DRV_TEMP_CB_0:
    case DG_CMN_DRV_TEMP_CB_1:
    case DG_CMN_DRV_TEMP_FEB_0:
    case DG_CMN_DRV_TEMP_FEB_1:
    case DG_CMN_DRV_TEMP_FEB_2:
    case DG_CMN_DRV_TEMP_CPU:
    case DG_CMN_DRV_TEMP_PCH:
    case DG_CMN_DRV_TEMP_PSU_0:
    case DG_CMN_DRV_TEMP_PSU_1:
    case DG_CMN_DRV_TEMP_WTB_1:
    case DG_CMN_DRV_TEMP_WTB_2:
    case DG_CMN_DRV_TEMP_PHY_0:
    case DG_CMN_DRV_TEMP_PHY_1:
    case DG_CMN_DRV_TEMP_FPGA:
        DG_DBG_TRACE("TEMP sensor %d got temperature: %+.2f°C", sensor, f_temp);
        *temperature = *(UINT32*)&f_temp;
        ret          = TRUE;
        break;

    default:
        DG_DRV_UTIL_set_error_string("Invalid temperature sensor=%d", sensor);
        break;
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

