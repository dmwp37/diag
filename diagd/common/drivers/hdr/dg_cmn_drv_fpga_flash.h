#ifndef _DG_CMN_DRV_FPGA_FLASH_H_
#define _DG_CMN_DRV_FPGA_FLASH_H_
/*==================================================================================================

    Module Name:  dg_cmn_drv_fpga_flash.h

    General Description: This file provides driver interface for FPGA_FLASH test

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup FPGA_FLASH_driver
@{

@par
Provide APIs for FPGA_FLASH
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

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef struct
{
    UINT16 mfg_id;
    UINT16 dev_id1;
    UINT16 dev_id2;
    UINT16 dev_id3;
    UINT16 dev_verify;
    UINT16 protect_verify;
} DG_CMN_DRV_FLASH_INFO_T;

typedef struct
{
    UINT8 val[65]; /* address from 0x10 to 0x50, for X16 width chip only low 8bit is valid */
} DG_CMN_DRV_FLASH_CFI_T;

typedef UINT32 DG_CMN_DRV_FLASH_ADDR_T;
typedef UINT8  DG_CMN_DRV_FLASH_LEN_T;

/*==================================================================================================
                                   GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*=============================================================================================*//**
@brief Get FPGA flash information

@param[out] info - The flash info read from the device
*//*==============================================================================================*/
BOOL DG_CMN_DRV_FPGA_FLASH_get_info(DG_CMN_DRV_FLASH_INFO_T* info);

/*=============================================================================================*//**
@brief Get FPGA flash CFI

@param[out] cfi - The flash CFI read from the device
*//*==============================================================================================*/
BOOL DG_CMN_DRV_FPGA_FLASH_get_cfi(DG_CMN_DRV_FLASH_CFI_T* cfi);

/*=============================================================================================*//**
@brief Get FPGA flash memory

@param[in]  addr- The flash memory address read from the device
@param[in]  len - The amount of data to read in bytes
@param[out] buf - The data to read from the device
*//*==============================================================================================*/
BOOL DG_CMN_DRV_FPGA_FLASH_get_mem(DG_CMN_DRV_FLASH_ADDR_T addr,
                                   DG_CMN_DRV_FLASH_LEN_T  len,
                                   UINT8*                  buf);

/*=============================================================================================*//**
@brief Set FPGA flash memory

@param[in] addr- The flash memory address read from the device
@param[in] len - The amount of data to write in bytes
@param[in] buf - The data to write to the device
*//*==============================================================================================*/
BOOL DG_CMN_DRV_FPGA_FLASH_set_mem(DG_CMN_DRV_FLASH_ADDR_T addr,
                                   DG_CMN_DRV_FLASH_LEN_T  len,
                                   UINT8*                  buf);

#ifdef __cplusplus
}
#endif

/** @} */
/** @} */

#endif /* _DG_CMN_DRV_FPGA_FLASH_H_  */

