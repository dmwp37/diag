#ifndef _DG_CMN_DRV_PSU_H_
#define _DG_CMN_DRV_PSU_H_
/*==================================================================================================

    Module Name:  dg_cmn_drv_psu.h

    General Description: This file provides driver interface for PSU test

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup PSU_driver
@{

@par
Provide APIs for PSU
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
/** PSU slot type */
enum
{
    DG_CMN_DRV_PSU_SLOT_0   = 0x00,
    DG_CMN_DRV_PSU_SLOT_1   = 0x01,
    DG_CMN_DRV_PSU_SLOT_MAX = 0x01
};
typedef UINT8 DG_CMN_DRV_PSU_SLOT_T;

/** PSU channel type */
enum
{
    DG_CMN_DRV_PSU_CHANNEL_EEPROM = 0x00,
    DG_CMN_DRV_PSU_CHANNEL_PSMI   = 0x01,
};
typedef UINT8 DG_CMN_DRV_PSU_CHANNEL_T;

/** PSU config type */
enum
{
    DG_CMN_DRV_PSU_CFG_TURN_OFF = 0x00,
    DG_CMN_DRV_PSU_CFG_TURN_ON  = 0x01,
};
typedef UINT8 DG_CMN_DRV_PSU_CFG_T;

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
/** PSU information type */
typedef struct
{
    UINT8 data[512];
} DG_CMN_DRV_PSU_INFO_T;

/** PSU address type */
typedef UINT8 DG_CMN_DRV_PSU_ADDR_T;

/** PSU data type */
typedef UINT16 DG_CMN_DRV_PSU_DATA_T;

/** PSU status type */
typedef UINT8 DG_CMN_DRV_PSU_STATUS_T;

/*==================================================================================================
                                   GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*=============================================================================================*//**
@brief Dump the PSU information value

@param[in]  slot    - The PSU slot
@param[in]  channel - The PSU channel
@param[out] info    - The PSU dumped information

@note
- Fill unavailable data with 0xFF
*//*==============================================================================================*/
BOOL DG_CMN_DRV_PSU_dump_info(DG_CMN_DRV_PSU_SLOT_T    slot,
                              DG_CMN_DRV_PSU_CHANNEL_T channel,
                              DG_CMN_DRV_PSU_INFO_T*   info);

/*=============================================================================================*//**
@brief write PSU information

@param[in] slot    - The PSU slot
@param[in] channel - The PSU channel
@param[in] addr    - The PSU address to write
@param[in] data    - The PSU data to write
*//*==============================================================================================*/
BOOL DG_CMN_DRV_PSU_write(DG_CMN_DRV_PSU_SLOT_T    slot,
                          DG_CMN_DRV_PSU_CHANNEL_T channel,
                          DG_CMN_DRV_PSU_ADDR_T    addr,
                          DG_CMN_DRV_PSU_DATA_T    data);

/*=============================================================================================*//**
@brief Get the PSU status

@param[in]  slot    - The PSU slot
@param[out] status  - The PSU status (PST & ACOK & DCOK & ALERT)

*//*==============================================================================================*/
BOOL DG_CMN_DRV_PSU_get_status(DG_CMN_DRV_PSU_SLOT_T slot, DG_CMN_DRV_PSU_STATUS_T* status);

/*=============================================================================================*//**
@brief Turn on/off PSU

@param[in] slot - The PSU slot
@param[in] cfg  - The PSU turn on/off configuration

*//*==============================================================================================*/
BOOL DG_CMN_DRV_PSU_set_cfg(DG_CMN_DRV_PSU_SLOT_T slot, DG_CMN_DRV_PSU_CFG_T cfg);

#ifdef __cplusplus
}
#endif

/** @} */
/** @} */

#endif /* _DG_CMN_DRV_PSU_H_  */

