/*==================================================================================================

    Module Name:  dg_cmn_drv_gpio.c

    General Description: Implements the GPIO common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_gpio.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup GPIO_driver
@{
implementation of the GPIO driver
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
static DG_CMN_DRV_GPIO_VALUE_T dg_cmn_drv_gpio_data[DG_CMN_DRV_GPIO_PORT_MAX] = { 0 };
static DG_CMN_DRV_GPIO_CFG_T   dg_cmn_drv_gpio_cfg[DG_CMN_DRV_GPIO_PORT_MAX] = { 0 };

/*==================================================================================================
                                         GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Get the GPIO port value

@param[in]  port - The GPIO port
@param[out] val  - The GPIO port value

*//*==============================================================================================*/
BOOL DG_CMN_DRV_GPIO_get(DG_CMN_DRV_GPIO_PORT_T port, DG_CMN_DRV_GPIO_VALUE_T* val)
{
    BOOL ret = FALSE;

    if (port > DG_CMN_DRV_GPIO_PORT_MAX)
    {
        DG_DRV_UTIL_set_error_string("Invalid port=%d", port);
    }
    else
    {
        *val = dg_cmn_drv_gpio_data[port];

        DG_DBG_TRACE("GPIO Port %d got value: %d", port, *val);

        ret = TRUE;
    }

    return ret;
}


/*=============================================================================================*//**
@brief Set the GPIO port value

@param[in] port - The GPIO port
@param[in] val  - The GPIO port value

*//*==============================================================================================*/
BOOL DG_CMN_DRV_GPIO_set(DG_CMN_DRV_GPIO_PORT_T port, DG_CMN_DRV_GPIO_VALUE_T val)
{
    BOOL ret = FALSE;

    if (port > DG_CMN_DRV_GPIO_PORT_MAX)
    {
        DG_DRV_UTIL_set_error_string("Invalid port=%d", port);
    }
    else if ((val != DG_CMN_DRV_GPIO_ACTIVE) && (val != DG_CMN_DRV_GPIO_INACTIVE))
    {
        DG_DRV_UTIL_set_error_string("GPIO value is not valid. val=%d", val);
    }
    else if (dg_cmn_drv_gpio_cfg[port] != DG_CMN_DRV_GPIO_CFG_OUTPUT)
    {
        DG_DRV_UTIL_set_error_string("port %d is not configured as output", port);
    }
    else
    {
        dg_cmn_drv_gpio_data[port] = val;

        DG_DBG_TRACE("GPIO Port %d set value: %d", port, val);

        ret = TRUE;
    }

    return ret;
}

/*=============================================================================================*//**
@brief Configure the GPIO port as input or output

@param[in] port - The GPIO port
@param[in] cfg  - The GPIO port configuration

*//*==============================================================================================*/
BOOL DG_CMN_DRV_GPIO_cfg(DG_CMN_DRV_GPIO_PORT_T port, DG_CMN_DRV_GPIO_CFG_T cfg)
{
    BOOL ret = FALSE;

    if (port > DG_CMN_DRV_GPIO_PORT_MAX)
    {
        DG_DRV_UTIL_set_error_string("Invalid port=%d", port);
    }
    else if ((cfg != DG_CMN_DRV_GPIO_CFG_INPUT) && (cfg != DG_CMN_DRV_GPIO_CFG_OUTPUT))
    {
        DG_DRV_UTIL_set_error_string("GPIO config is not valid. cfg=%d", cfg);
    }
    else
    {
        dg_cmn_drv_gpio_cfg[port] = cfg;

        DG_DBG_TRACE("GPIO Port %d configured as %s", port,
                     (cfg == DG_CMN_DRV_GPIO_CFG_INPUT) ? "input" : "output");

        ret = TRUE;
    }

    return ret;
}
/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

