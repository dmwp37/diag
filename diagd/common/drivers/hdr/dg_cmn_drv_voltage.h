#ifndef _DG_CMN_DRV_VOLTAGE_H_
#define _DG_CMN_DRV_VOLTAGE_H_
/*==================================================================================================

    Module Name:  dg_cmn_drv_voltage.h

    General Description: This file provides driver interface for Voltage test

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup VOLTAGE_driver
@{

@par
Provide APIs for Voltage
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
    DG_CMN_DRV_VOLTAGE_CB_0     = 0x00,
    DG_CMN_DRV_VOLTAGE_CB_1     = 0x01,
    DG_CMN_DRV_VOLTAGE_FEB_0    = 0x02,
    DG_CMN_DRV_VOLTAGE_FEB_1    = 0x03,
    DG_CMN_DRV_VOLTAGE_WTB_0    = 0x04,
    DG_CMN_DRV_VOLTAGE_WTB_1    = 0x05,
    DG_CMN_DRV_VOLTAGE_CHIP_MAX = 0x05
};
typedef UINT8 DG_CMN_DRV_VOLTAGE_CHIP_T;

enum
{
    DG_CMN_DRV_VOLTAGE_CHANNEL_0   = 0x00,
    DG_CMN_DRV_VOLTAGE_CHANNEL_1   = 0x01,
    DG_CMN_DRV_VOLTAGE_CHANNEL_2   = 0x02,
    DG_CMN_DRV_VOLTAGE_CHANNEL_3   = 0x03,
    DG_CMN_DRV_VOLTAGE_CHANNEL_4   = 0x04,
    DG_CMN_DRV_VOLTAGE_CHANNEL_5   = 0x05,
    DG_CMN_DRV_VOLTAGE_CHANNEL_6   = 0x06,
    DG_CMN_DRV_VOLTAGE_CHANNEL_7   = 0x07,
    DG_CMN_DRV_VOLTAGE_CHANNEL_MAX = 0x07
};
typedef UINT8 DG_CMN_DRV_VOLTAGE_CHANNEL_T;

enum
{
    DG_CMN_DRV_VOLTAGE_LEVEL_NORMAL = 0x00,
    DG_CMN_DRV_VOLTAGE_LEVEL_HIGH   = 0x01,
    DG_CMN_DRV_VOLTAGE_LEVEL_LOW    = 0x02,
    DG_CMN_DRV_VOLTAGE_LEVEL_MAX    = 0x02
};
typedef UINT8 DG_CMN_DRV_VOLTAGE_LEVEL_T;

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
/** VOLTAGE data in floating format */
typedef UINT32 DG_CMN_DRV_VOLTAGE_DATA_T;

/*==================================================================================================
                                   GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*=============================================================================================*//**
@brief Read voltage from selected chip and channel

@param[in]  chip    - voltage chip selection
@param[in]  channel - voltage channel selection
@param[out] data    - The voltage value in float format

@note
- the data is a UINT32 pointer, but it must contains the exact data as a float
- *data = *(UINT32*)&(float_voltage)
*//*==============================================================================================*/
BOOL DG_CMN_DRV_VOLTAGE_get(DG_CMN_DRV_VOLTAGE_CHIP_T    chip,
                            DG_CMN_DRV_VOLTAGE_CHANNEL_T channel,
                            DG_CMN_DRV_VOLTAGE_DATA_T*   data);

/*=============================================================================================*//**
@brief Set system power output level

@param[in] level - the System power output level

*//*==============================================================================================*/
BOOL DG_CMN_DRV_VOLTAGE_set(DG_CMN_DRV_VOLTAGE_LEVEL_T level);

#ifdef __cplusplus
}
#endif

/** @} */
/** @} */

#endif /* _DG_CMN_DRV_VOLTAGE_H_  */

