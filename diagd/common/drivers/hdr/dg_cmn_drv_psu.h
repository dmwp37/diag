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
/** PSMI address type */
typedef UINT8 DG_CMN_DRV_PSU_PSMI_ADDR_T;

/** PSMI data type */
typedef UINT16 DG_CMN_DRV_PSU_PSMI_DATA_T;

/** PSU information type */
typedef struct
{
    DG_CMN_DRV_PSU_PSMI_DATA_T data[256];
} DG_CMN_DRV_PSU_PSMI_INFO_T;

/** PSU status type */
typedef UINT8 DG_CMN_DRV_PSU_STATUS_T;

/*==================================================================================================
                                   GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*=============================================================================================*//**
@brief Dump the PSU PSMI

@param[in]  slot    - The PSU slot
@param[out] psmi    - The dumped PSMI information

@note
- Fill unavailable data with 0xFF
*//*==============================================================================================*/
BOOL DG_CMN_DRV_PSU_dump_psmi(DG_CMN_DRV_PSU_SLOT_T       slot,
                              DG_CMN_DRV_PSU_PSMI_INFO_T* psmi);

/*=============================================================================================*//**
@brief write PSU PSMI data

@param[in] slot    - The PSU slot
@param[in] addr    - The PSMI address to write
@param[in] data    - The PSMI data to write
*//*==============================================================================================*/
BOOL DG_CMN_DRV_PSU_write_psmi(DG_CMN_DRV_PSU_SLOT_T      slot,
                               DG_CMN_DRV_PSU_PSMI_ADDR_T addr,
                               DG_CMN_DRV_PSU_PSMI_DATA_T data);

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

