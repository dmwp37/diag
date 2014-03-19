/*==================================================================================================

    Module Name:  dg_cmn_drv_bios.c

    General Description: Implements the BIOS common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_bios.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup BIOS_driver
@{
implementation of the BIOS driver
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
@brief Reads data from a given BIOS device

@param[in]  bios_id   - The BIOS bios_id to read from
@param[out] bios_info - The BIOS info read from the device

@note
- Read data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_BIOS_read_info(DG_CMN_DRV_BIOS_ID_T bios_id, DG_CMN_DRV_BIOS_INFO_T* bios_info)
{
    BOOL ret = FALSE;

    if ((bios_id == DG_CMN_DRV_BIOS_MASTER) || (bios_id == DG_CMN_DRV_BIOS_SLAVE))
    {
        bios_info->mfg     = 0x12;
        bios_info->dev     = 0x3456;
        bios_info->edi_len = 0x78;
        bios_info->edi_b1  = 0x90;
        DG_DBG_TRACE("BIOS information: BIOS ID:%d, Manufacturer ID:0x%02x, Device ID:0x%02x, "
                     "EDI String length:%d, EDI Byte 1:0x%02x",
                     bios_id, bios_info->mfg, bios_info->dev, bios_info->edi_len, bios_info->edi_b1);
        ret = TRUE;
    }
    else
    {
        DG_DRV_UTIL_set_error_string("Read BIOS information failed, BIOS ID:%d is invalid", bios_id);
    }

    return ret;
}

/*=============================================================================================*//**
@brief Reads status register from a given BIOS device

@param[in]  bios_id   - The BIOS ID to read from
@param[out] val       - The BIOS status value pointer read from the device

@note
- Read data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_BIOS_read_status(DG_CMN_DRV_BIOS_ID_T bios_id, DG_CMN_DRV_BIOS_REG_T* val)
{
    BOOL ret = FALSE;

    if ((bios_id == DG_CMN_DRV_BIOS_MASTER) || (bios_id == DG_CMN_DRV_BIOS_SLAVE))
    {
        *val = 0x1234;
        DG_DBG_TRACE("Read BIOS status register from BIOS ID%d, value:%04x", bios_id, *val);
        ret = TRUE;
    }
    else
    {
        DG_DRV_UTIL_set_error_string("Read BIOS status register from BIOS ID%d failed", bios_id);
    }

    return ret;
}

/*=============================================================================================*//**
@brief Writes status register to a given BIOS device

@param[in]  bios_id   - The BIOS ID to write
@param[out] val       - The BIOS status value to write to the device

@note
- Write data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_BIOS_write_status(DG_CMN_DRV_BIOS_ID_T bios_id, DG_CMN_DRV_BIOS_REG_T val)
{
    BOOL ret = FALSE;

    if ((bios_id == DG_CMN_DRV_BIOS_MASTER) || (bios_id == DG_CMN_DRV_BIOS_SLAVE))
    {
        /* *cpld_reg = val; */
        DG_DBG_TRACE("Write BIOS status register to BIOS ID%d value:%04x", bios_id, val);
        ret = TRUE;
    }
    else
    {
        DG_DRV_UTIL_set_error_string("Write BIOS status register to BIOS ID%d failed", bios_id);
    }

    return ret;
}


/*=============================================================================================*//**
@brief Reads checksumfrom a given BIOS device

@param[in]  bios_id   - The BIOS ID to read from
@param[out] sha1      - The BIOS SHA1 checksum pointer

@note
- Read data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_BIOS_get_checksum(DG_CMN_DRV_BIOS_ID_T bios_id, DG_CMN_DRV_BIOS_SHA1_T* sha1)
{
    BOOL  ret = FALSE;
    UINT8 i;

    if ((bios_id == DG_CMN_DRV_BIOS_MASTER) || (bios_id == DG_CMN_DRV_BIOS_SLAVE))
    {
        for (i = 0; i < sizeof(sha1->value); i++)
        {
            sha1->value[i] = 0x50 + i;
        }
        DG_DBG_TRACE("Get BIOS checksum from BIOS ID%d", bios_id);
        ret = TRUE;
    }
    else
    {
        DG_DRV_UTIL_set_error_string("Read BIOS status register from BIOS ID%d failed", bios_id);
    }

    return ret;
}


/*=============================================================================================*//**
@brief Writes error data to a given BIOS device to make it unbootable

@param[in]  bios_id   - The BIOS ID to write

@note
- Write data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_BIOS_attack(DG_CMN_DRV_BIOS_ID_T bios_id)
{
    BOOL ret = FALSE;

    if ((bios_id == DG_CMN_DRV_BIOS_MASTER) || (bios_id == DG_CMN_DRV_BIOS_SLAVE))
    {
        DG_DBG_TRACE("Attack BIOS ID%d", bios_id);
        ret = TRUE;
    }
    else
    {
        DG_DRV_UTIL_set_error_string("Write BIOS status register to BIOS ID%d failed", bios_id);
    }

    return ret;
}


/*=============================================================================================*//**
@brief Set next boot BIOS souce

@param[in]  bios_id   - The BIOS ID to set

@note
- Set is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_BIOS_set_src(DG_CMN_DRV_BIOS_ID_T bios_id)
{
    BOOL ret = FALSE;

    if ((bios_id == DG_CMN_DRV_BIOS_MASTER) || (bios_id == DG_CMN_DRV_BIOS_SLAVE))
    {
        /* *epld_reg = bios_id; */
        DG_DBG_TRACE("set BIOS boot from BIOS ID:%d when next boot", bios_id);
        ret = TRUE;
    }
    else
    {
        DG_DRV_UTIL_set_error_string("Set BIOS boot source failed");
    }

    return ret;
}

/*=============================================================================================*//**
@brief Get next boot BIOS souce

@param[out]  bios_id   - The BIOS ID to boot when next boot

@note
- Get is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_BIOS_get_src(DG_CMN_DRV_BIOS_ID_T* bios_id)
{
    *bios_id = DG_CMN_DRV_BIOS_SLAVE;
    DG_DBG_TRACE("BIOS boot from: BIOS ID:%d", *bios_id);

    return TRUE;
}


/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

