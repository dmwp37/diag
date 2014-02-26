#ifndef _DG_CMN_DRV_BIOS_H_
#define _DG_CMN_DRV_BIOS_H_
/*==================================================================================================

    Module Name:  dg_cmn_drv_bios.h

    General Description: This file provides driver interface for BIOS test

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup BIOS_driver
@{

@par
Provide APIs for BIOS
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
    DG_CMN_DRV_BIOS_MASTER = 0x00,
    DG_CMN_DRV_BIOS_SLAVE  = 0x01,
};
typedef UINT8 DG_CMN_DRV_BIOS_ID_T;

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef struct
{
    UINT8  mfg;
    UINT16 dev;
    UINT8  edi_len;
    UINT8  edi_b1;
} DG_CMN_DRV_BIOS_INFO_T;

typedef UINT16 DG_CMN_DRV_BIOS_REG_T;

typedef struct
{
    UINT8 value[20];
} DG_CMN_DRV_BIOS_SHA1_T;


/*==================================================================================================
                                   GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*=============================================================================================*//**
@brief Reads information from a given BIOS device

@param[in]  bios_id   - The BIOS port to read from
@param[out] bios_info - The BIOS info read from the device

@note
- Read data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_BIOS_read_info(DG_CMN_DRV_BIOS_ID_T bios_id, DG_CMN_DRV_BIOS_INFO_T* bios_info);

/*=============================================================================================*//**
@brief Reads status register from a given BIOS device

@param[in]  bios_id   - The BIOS ID to read from
@param[out] val       - The BIOS status value pointer read from the device

@note
- Read data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_BIOS_read_status(DG_CMN_DRV_BIOS_ID_T bios_id, DG_CMN_DRV_BIOS_REG_T* val);

/*=============================================================================================*//**
@brief Writes status register to a given BIOS device

@param[in]  bios_id   - The BIOS ID to write
@param[out] val       - The BIOS status value to write to the device

@note
- Write data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_BIOS_write_status(DG_CMN_DRV_BIOS_ID_T bios_id, DG_CMN_DRV_BIOS_REG_T val);

/*=============================================================================================*//**
@brief Reads checksumfrom a given BIOS device

@param[in]  bios_id   - The BIOS ID to read from
@param[out] sha1      - The BIOS SHA1 checksum pointer

@note
- Read data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_BIOS_get_checksum(DG_CMN_DRV_BIOS_ID_T bios_id, DG_CMN_DRV_BIOS_SHA1_T* sha1);

/*=============================================================================================*//**
@brief Writes error data to a given BIOS device to make it unbootable

@param[in]  bios_id   - The BIOS ID to write

@note
- Write data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_BIOS_attack(DG_CMN_DRV_BIOS_ID_T bios_id);

/*=============================================================================================*//**
@brief Set next boot BIOS souce

@param[in]  bios_id   - The BIOS ID to set

@note
- Set is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_BIOS_set_src(DG_CMN_DRV_BIOS_ID_T bios_id);

/*=============================================================================================*//**
@brief Get next boot BIOS souce

@param[out]  bios_id   - The BIOS ID to boot when next boot

@note
- Get is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_BIOS_get_src(DG_CMN_DRV_BIOS_ID_T* bios_id);

#ifdef __cplusplus
}
#endif

/** @} */
/** @} */

#endif /* _DG_CMN_DRV_DBG_LVL_H_  */

