#ifndef _DG_CMN_DRV_TEMP_H_
#define _DG_CMN_DRV_TEMP_H_
/*==================================================================================================

    Module Name:  dg_cmn_drv_temp.h

    General Description: This file provides driver interface for Temperature test

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup TEMP_driver
@{

@par
Provide APIs for Temperature test
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
enum
{
    DG_CMN_DRV_TEMP_CB_0  = 0x00,
    DG_CMN_DRV_TEMP_CB_1  = 0x01,
    DG_CMN_DRV_TEMP_FEB_0 = 0x02,
    DG_CMN_DRV_TEMP_FEB_1 = 0x03,
    DG_CMN_DRV_TEMP_CPU   = 0x04,
    DG_CMN_DRV_TEMP_PCH   = 0x05,
    DG_CMN_DRV_TEMP_PSU_0 = 0x06,
    DG_CMN_DRV_TEMP_PSU_1 = 0x07,
    DG_CMN_DRV_TEMP_WTB_0 = 0x08,
    DG_CMN_DRV_TEMP_WTB_1 = 0x09,
    DG_CMN_DRV_TEMP_PHY_0 = 0x0a,
    DG_CMN_DRV_TEMP_PHY_1 = 0x0b,
    DG_CMN_DRV_TEMP_FPGA  = 0x0c,
};
typedef UINT8 DG_CMN_DRV_TEMP_SENSOR_T;

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
/** Temperature data */
typedef UINT32 DG_CMN_DRV_TEMP_DATA_T;

/*==================================================================================================
                                   GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*=============================================================================================*//**
@brief Get temperature from specified temp sensor

@param[in]  sensor      - temperature sensor selection
@param[out] temperature - The temperature from the sensor (convert from float)

@note
- the temp is a UINT32 pointer, but it must contains the exact data as a float
- *temp = *(UINT32*)&(float_temperature)
*//*==============================================================================================*/
BOOL DG_CMN_DRV_TEMP_get(DG_CMN_DRV_TEMP_SENSOR_T sensor, DG_CMN_DRV_TEMP_DATA_T* temperature);

#ifdef __cplusplus
}
#endif

/** @} */
/** @} */

#endif /* _DG_CMN_DRV_TEMP_H_  */

