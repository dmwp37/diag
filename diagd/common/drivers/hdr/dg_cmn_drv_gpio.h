#ifndef _DG_CMN_DRV_GPIO_H_
#define _DG_CMN_DRV_GPIO_H_
/*==================================================================================================

    Module Name:  dg_cmn_drv_gpio.h

    General Description: This file provides driver interface for GPIO test

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup GPIO_driver
@{

@par
Provide APIs for GPIO
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

/* GPIO config type */
enum
{
    DG_CMN_DRV_GPIO_CFG_INPUT  = 0x00,
    DG_CMN_DRV_GPIO_CFG_OUTPUT = 0x01,
};
typedef UINT8 DG_CMN_DRV_GPIO_CFG_T;

/** GPIO value */
enum
{
    DG_CMN_DRV_GPIO_ACTIVE   = 0x01,
    DG_CMN_DRV_GPIO_INACTIVE = 0x00,
};
typedef UINT8 DG_CMN_DRV_GPIO_VALUE_T;

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/** GPIO port */
typedef UINT8 DG_CMN_DRV_GPIO_PORT_T;

/*==================================================================================================
                                   GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*=============================================================================================*//**
@brief Get the GPIO port value

@param[in]  port - The GPIO port
@param[out] val  - The GPIO port value

*//*==============================================================================================*/
BOOL DG_CMN_DRV_GPIO_get(DG_CMN_DRV_GPIO_PORT_T port, DG_CMN_DRV_GPIO_VALUE_T* val);

/*=============================================================================================*//**
@brief Set the GPIO port value

@param[in] port - The GPIO port
@param[in] val  - The GPIO port value

*//*==============================================================================================*/
BOOL DG_CMN_DRV_GPIO_set(DG_CMN_DRV_GPIO_PORT_T port, DG_CMN_DRV_GPIO_VALUE_T val);

/*=============================================================================================*//**
@brief Configure the GPIO port as input or output

@param[in] port - The GPIO port
@param[in] cfg  - The GPIO port configuration

*//*==============================================================================================*/
BOOL DG_CMN_DRV_GPIO_set_cfg(DG_CMN_DRV_GPIO_PORT_T port, DG_CMN_DRV_GPIO_CFG_T cfg);

/*=============================================================================================*//**
@brief Get the GPIO port configuration

@param[in]  port - The GPIO port
@param[out] cfg  - The GPIO port configuration

*//*==============================================================================================*/
BOOL DG_CMN_DRV_GPIO_get_cfg(DG_CMN_DRV_GPIO_PORT_T port, DG_CMN_DRV_GPIO_CFG_T* cfg);

#ifdef __cplusplus
}
#endif

/** @} */
/** @} */

#endif /* _DG_CMN_DRV_GPIO_H_  */

