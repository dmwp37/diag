/*==================================================================================================

    Module Name:  dg_cmn_drv_fpga_flash.c

    General Description: Implements the FPGA_FLASH common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_fpga_flash.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup FPGA_FLASH_driver
@{
implementation of the FPGA_FLASH driver
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
@brief Get FPGA flash information

@param[out] info - The flash info read from the device
*//*==============================================================================================*/
BOOL DG_CMN_DRV_FPGA_FLASH_get_info(DG_CMN_DRV_FLASH_INFO_T* info)
{
    info->mfg_id         = 1;
    info->dev_id1        = 2;
    info->dev_id2        = 3;
    info->dev_id3        = 4;
    info->dev_verify     = 5;
    info->protect_verify = 6;
    DG_DBG_TRACE("Get FPGA flash information");

    return TRUE;
}

/*=============================================================================================*//**
@brief Get FPGA flash CFI

@param[out] cfi - The flash CFI read from the device
*//*==============================================================================================*/
BOOL DG_CMN_DRV_FPGA_FLASH_get_cfi(DG_CMN_DRV_FLASH_CFI_T* cfi)
{
    UINT32 i;

    for (i = 0; i < sizeof(cfi->val); i++)
    {
        cfi->val[i] = i;
    }
    /* support CFI */
    cfi->val[0] = 'Q';
    cfi->val[1] = 'R';
    cfi->val[2] = 'Y';

    DG_DBG_TRACE("Get FPGA flash CFI");
    DG_DBG_DUMP(cfi->val, sizeof(cfi->val));

    return TRUE;
}

/*=============================================================================================*//**
@brief Get FPGA flash memory

@param[in]  addr- The flash memory address read from the device
@param[in]  len - The amount of data to read in bytes
@param[out] buf - The data to read from the device
*//*==============================================================================================*/
BOOL DG_CMN_DRV_FPGA_FLASH_get_mem(DG_CMN_DRV_FLASH_ADDR_T addr,
                                   DG_CMN_DRV_FLASH_LEN_T  len,
                                   UINT8*                  buf)
{
    UINT32 i;

    for (i = 0; i < len; i++)
    {
        *buf++ = (UINT8)(addr + i);
    }
    DG_DBG_TRACE("Get FPGA flash memory address %x, length %d, buffer %p", addr, len, buf);
    DG_DBG_DUMP(buf, len);

    return TRUE;
}

/*=============================================================================================*//**
@brief Set FPGA flash memory

@param[in] addr- The flash memory address read from the device
@param[in] len - The amount of data to write in bytes
@param[in] buf - The data to write to the device
*//*==============================================================================================*/
BOOL DG_CMN_DRV_FPGA_FLASH_set_mem(DG_CMN_DRV_FLASH_ADDR_T addr,
                                   DG_CMN_DRV_FLASH_LEN_T  len,
                                   UINT8*                  buf)
{
    DG_DBG_TRACE("Set FPGA flash memory address %x, length %d, buffer %p", addr, len, buf);
    DG_DBG_DUMP(buf, len);

    return TRUE;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

