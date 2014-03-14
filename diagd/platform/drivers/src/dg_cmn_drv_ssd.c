/*==================================================================================================

    Module Name:  dg_cmn_drv_ssd.c

    General Description: Implements the SSD common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_ssd.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup SSD_driver
@{
implementation of the SSD driver
*/

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/
#define DG_CMN_DRV_SSD_PRESENCE_MASK 0x01
#define DG_CMN_DRV_SSD_POW_FAIL_MASK 0x02

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
@brief Get SSD status on the given SSD device

@param[in]  ssd    - The SSD ID to read from
@param[out] status - The SSD presence and warning status read from the device by CPLD
*//*==============================================================================================*/
BOOL DG_CMN_DRV_SSD_get_status(DG_CMN_DRV_SSD_ID_T ssd, DG_CMN_DRV_SSD_STATUS_T* status)
{
    BOOL ret = FALSE;

    switch (ssd)
    {
    case DG_CMN_DRV_SSD_OS:
    case DG_CMN_DRV_SSD_LOG:
        *status = DG_CMN_DRV_SSD_PRESENCE_MASK;
        DG_DBG_TRACE("Get SSD status 0x%02x", *status);
        ret = TRUE;
        break;

    default:
        DG_DRV_UTIL_set_error_string("Invalid SSD ID %d", ssd);
    }

    return ret;
}

/*=============================================================================================*//**
@brief Get SSD Model Number on the given SSD device

@param[in]  ssd    - The SSD ID to read from
@param[out] str    - The SSD Model Number read from the device
*//*==============================================================================================*/
BOOL DG_CMN_DRV_SSD_get_model(DG_CMN_DRV_SSD_ID_T ssd, char** str)
{
    BOOL   ret     = FALSE;
    char*  model[] = { "SATA SSD", "ST500DM002-1BD142" };
    UINT32 len;

    switch (ssd)
    {
    case DG_CMN_DRV_SSD_OS:
    case DG_CMN_DRV_SSD_LOG:
        len = strlen(model[ssd]) + 1;
        if ((*str = malloc(len)))
        {
            strncpy(*str, model[ssd], len);
            DG_DBG_TRACE("Get SSD model %s", *str);
            ret = TRUE;
        }
        break;

    default:
        DG_DRV_UTIL_set_error_string("Invalid SSD ID %d", ssd);
    }

    return ret;
}

/*=============================================================================================*//**
@brief Get SSD Serial Number on the given SSD device

@param[in]  ssd    - The SSD ID to read from
@param[out] str    - The SSD Serial Number read from the device
*//*==============================================================================================*/
BOOL DG_CMN_DRV_SSD_get_serial(DG_CMN_DRV_SSD_ID_T ssd, char** str)
{
    BOOL   ret      = FALSE;
    char*  serial[] = { "W3T27KY4", "FED10738028500038456" };
    UINT32 len;

    switch (ssd)
    {
    case DG_CMN_DRV_SSD_OS:
    case DG_CMN_DRV_SSD_LOG:
        len = strlen(serial[ssd]) + 1;
        if ((*str = malloc(len)))
        {
            strncpy(*str, serial[ssd], len);
            DG_DBG_TRACE("Get SSD serial %s", *str);
            ret = TRUE;
        }
        ret = TRUE;
        break;

    default:
        DG_DRV_UTIL_set_error_string("Invalid SSD ID %d", ssd);
    }

    return ret;
}

/*=============================================================================================*//**
@brief Get SSD capacity on the given SSD device

@param[in]  ssd    - The SSD ID to read from
@param[out] size   - The SSD device size wich MBytes
*//*==============================================================================================*/
BOOL DG_CMN_DRV_SSD_get_capacity(DG_CMN_DRV_SSD_ID_T ssd, DG_CMN_DRV_SSD_SIZE_T* size)
{
    BOOL ret         = FALSE;
    int  size_fake[] = { 64000, 128000 };

    switch (ssd)
    {
    case DG_CMN_DRV_SSD_OS:
    case DG_CMN_DRV_SSD_LOG:
        *size = size_fake[ssd];
        DG_DBG_TRACE("Get SSD device size 0x%02x", *size);
        ret = TRUE;
        break;

    default:
        DG_DRV_UTIL_set_error_string("Invalid SSD ID %d", ssd);
    }

    return ret;
}

/*=============================================================================================*//**
@brief Get SSD cache/buffer size on the given SSD device

@param[in]  ssd    - The SSD ID to read from
@param[out] size   - The SSD cache/buffer size wich KBytes
*//*==============================================================================================*/
BOOL DG_CMN_DRV_SSD_get_cache_size(DG_CMN_DRV_SSD_ID_T ssd, DG_CMN_DRV_SSD_SIZE_T* size)
{
    BOOL ret = FALSE;

    switch (ssd)
    {
    case DG_CMN_DRV_SSD_OS:
    case DG_CMN_DRV_SSD_LOG:
        *size = 16384;
        DG_DBG_TRACE("Get SSD cache/buffer size 0x%02x", *size);
        ret = TRUE;
        break;

    default:
        DG_DRV_UTIL_set_error_string("Invalid SSD ID %d", ssd);
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

