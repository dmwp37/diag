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
#include "dg_cmn_drv_cpld.h"


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
static char* dg_cmn_drv_device[] = { "/dev/sda", "/dev/sdb" };

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
    BOOL                    ret = FALSE;
    DG_CMN_DRV_CPLD_VALUE_T value;

    switch (ssd)
    {
    case DG_CMN_DRV_SSD_OS:
        *status = DG_CMN_DRV_SSD_PRESENCE_MASK;
        DG_DBG_TRACE("Get SSD status 0x%02x", *status);
        ret = TRUE;
        break;

    case DG_CMN_DRV_SSD_LOG:
        if (!DG_CMN_DRV_CPLD_get(DG_CMN_DRV_CPLD_CB, 0x18, &value))
        {
            DG_DBG_ERROR("can't get CPLD SSD status register");
        }
        else
        {
            *status = value & (DG_CMN_DRV_SSD_PRESENCE_MASK | DG_CMN_DRV_SSD_POW_FAIL_MASK);
            DG_DBG_TRACE("Get SSD status 0x%02x", *status);
            ret = TRUE;
        }
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
    BOOL  ret = FALSE;
    char  shell_cmd[128];
    char* p_out = NULL;

    switch (ssd)
    {
    case DG_CMN_DRV_SSD_OS:
    case DG_CMN_DRV_SSD_LOG:
        sprintf(shell_cmd, "hdparm -I %s |grep Model |cut -d: -f2 |sed -e 's/^ *//g' -e's/ *$//g'",
                dg_cmn_drv_device[ssd]);
        if (DG_DRV_UTIL_system(shell_cmd, &p_out))
        {
            if (p_out != NULL)
            {
                /* remove the tail '\n' */
                int last = strlen(p_out) - 1;
                if (p_out[last] == '\n')
                {
                    p_out[last] = '\0';
                }
                DG_DBG_TRACE("Get SSD model %s", p_out);
                ret = TRUE;
            }

            *str = p_out;
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
    BOOL  ret = FALSE;
    char  shell_cmd[128];
    char* p_out = NULL;

    switch (ssd)
    {
    case DG_CMN_DRV_SSD_OS:
    case DG_CMN_DRV_SSD_LOG:
        sprintf(shell_cmd, "hdparm -I %s |grep 'Serial Number' |cut -d: -f2 |sed -e 's/^ *//g'"
                " -e's/ *$//g'", dg_cmn_drv_device[ssd]);
        if (DG_DRV_UTIL_system(shell_cmd, &p_out))
        {
            if (p_out != NULL)
            {
                /* remove the tail '\n' */
                int last = strlen(p_out) - 1;
                if (p_out[last] == '\n')
                {
                    p_out[last] = '\0';
                }
                DG_DBG_TRACE("Get SSD serial %s", p_out);
                ret = TRUE;
            }

            *str = p_out;
        }
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
    BOOL  ret = FALSE;
    char  shell_cmd[128];
    char* p_out = NULL;

    switch (ssd)
    {
    case DG_CMN_DRV_SSD_OS:
    case DG_CMN_DRV_SSD_LOG:
        sprintf(shell_cmd, "hdparm -I %s |grep 'device size with M = 1000' |cut -d: -f2",
                dg_cmn_drv_device[ssd]);
        if (DG_DRV_UTIL_system(shell_cmd, &p_out))
        {
            if (p_out != NULL)
            {
                if (sscanf(p_out, "%d", size) == 1)
                {
                    DG_DBG_TRACE("Get SSD device size %d MBytes", *size);
                    ret = TRUE;
                }
                free(p_out);
            }
        }
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
    BOOL  ret = FALSE;
    char  shell_cmd[128];
    char* p_out = NULL;

    switch (ssd)
    {
    case DG_CMN_DRV_SSD_OS:
    case DG_CMN_DRV_SSD_LOG:
        sprintf(shell_cmd, "hdparm -I %s |grep 'buffer size' |cut -d: -f2",
                dg_cmn_drv_device[ssd]);
        if (DG_DRV_UTIL_system(shell_cmd, &p_out))
        {
            if (p_out != NULL)
            {
                if (sscanf(p_out, "%d", size) == 1)
                {
                    DG_DBG_TRACE("Get SSD cache/buffer size %d", *size);
                }
                else
                {
                    *size = 0;
                    DG_DBG_TRACE("Get SSD cache/buffer size %d", *size);
                }
                ret = TRUE;
                free(p_out);
            }
        }
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

