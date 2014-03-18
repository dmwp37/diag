/*==================================================================================================

    Module Name:  dg_cmn_drv_cpld.c

    General Description: Implements the CPLD common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_cpld.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup CPLD_driver
@{
implementation of the CPLD driver
*/

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/
#define DG_CMN_DRV_CPLD_MEM_SIZE 0x10000 /** 64k */
#define DG_CMN_DRV_CPLD_CB_BASE  0x0000  /** CPU CPLD base */
#define DG_CMN_DRV_CPLD_FEB_BASE 0x0400  /** FEB CPLD base */

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static BOOL        dg_cmn_drv_cpld_is_cpld(const char* name);
static const char* dg_cmn_drv_cpld_find();

/*==================================================================================================
                                         GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/
static const int dg_cmn_drv_cpld_base[] = { DG_CMN_DRV_CPLD_CB_BASE,  DG_CMN_DRV_CPLD_FEB_BASE };
static const int dg_cmn_drv_cpld_size[] =
{
    DG_CMN_DRV_CPLD_FEB_BASE,
    DG_CMN_DRV_CPLD_MEM_SIZE - DG_CMN_DRV_CPLD_FEB_BASE
};

/*==================================================================================================
                                         GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Get the CPLD register

@param[in]  id     - The CPLD ID
@param[in]  offset - The register offset position in the CPLD
@param[out] val    - The register value

*//*==============================================================================================*/
BOOL DG_CMN_DRV_CPLD_get(DG_CMN_DRV_CPLD_ID_T     id,
                         DG_CMN_DRV_CPLD_OFFSET_T offset,
                         DG_CMN_DRV_CPLD_VALUE_T* val)
{
    BOOL ret = FALSE;

    const char* cpld_dev;
    int         cpld_fd;
    UINT8*      cpld_base;

    if ((id != DG_CMN_DRV_CPLD_CB) && (id != DG_CMN_DRV_CPLD_FEB))
    {
        DG_DRV_UTIL_set_error_string("Invalid CPLD id=%d", id);
    }
    else if ((cpld_dev = dg_cmn_drv_cpld_find()) == NULL)
    {
        DG_DRV_UTIL_set_error_string("Can't find CPLD device");
    }
    else if ((cpld_fd = open(cpld_dev, O_RDWR | O_SYNC)) < 0)
    {
        DG_DRV_UTIL_set_error_string("Failed to open CPLD device %s. errno=%d(%m)",
                                     cpld_dev, errno);
    }
    else if ((cpld_base = mmap(NULL, DG_CMN_DRV_CPLD_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
                               cpld_fd, 0)) == MAP_FAILED)
    {
        DG_DRV_UTIL_set_error_string("Failed to mmap. errno=%d(%m)", errno);
    }
    else
    {
        UINT8* base = cpld_base + dg_cmn_drv_cpld_base[id];

        if (offset >= dg_cmn_drv_cpld_size[id])
        {
            DG_DRV_UTIL_set_error_string("Invalid CPLD offset. cpld=%d offset=0x%04x",
                                         id, offset);
        }
        else
        {
            *val = *(base + offset);
            DG_DBG_TRACE("CPLD ID %d offset 0x%04x got value: 0x%02x", id, offset, *val);
            ret = TRUE;
        }
    }

    if (cpld_base != MAP_FAILED)
    {
        munmap(cpld_base, DG_CMN_DRV_CPLD_MEM_SIZE);
    }

    if (cpld_fd > 0)
    {
        close(cpld_fd);
    }

    return ret;
}

/*=============================================================================================*//**
@brief Set the CPLD register

@param[in]  id     - The CPLD ID
@param[in]  offset - The register offset position in the CPLD
@param[out] val    - The register value

*//*==============================================================================================*/
BOOL DG_CMN_DRV_CPLD_set(DG_CMN_DRV_CPLD_ID_T     id,
                         DG_CMN_DRV_CPLD_OFFSET_T offset,
                         DG_CMN_DRV_CPLD_VALUE_T  val)
{
    BOOL ret = FALSE;

    const char* cpld_dev;
    int         cpld_fd;
    UINT8*      cpld_base;

    if ((id != DG_CMN_DRV_CPLD_CB) && (id != DG_CMN_DRV_CPLD_FEB))
    {
        DG_DRV_UTIL_set_error_string("Invalid CPLD id=%d", id);
    }
    else if ((cpld_dev = dg_cmn_drv_cpld_find()) == NULL)
    {
        DG_DRV_UTIL_set_error_string("Can't find CPLD device");
    }
    else if ((cpld_fd = open(cpld_dev, O_RDWR | O_SYNC)) < 0)
    {
        DG_DRV_UTIL_set_error_string("Failed to open CPLD device %s. errno=%d(%m)",
                                     cpld_dev, errno);
    }
    else if ((cpld_base = mmap(NULL, DG_CMN_DRV_CPLD_MEM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
                               cpld_fd, 0)) == MAP_FAILED)
    {
        DG_DRV_UTIL_set_error_string("Failed to mmap. errno=%d(%m)", errno);
    }
    else
    {
        UINT8* base = cpld_base + dg_cmn_drv_cpld_base[id];

        if (offset >= dg_cmn_drv_cpld_size[id])
        {
            DG_DRV_UTIL_set_error_string("Invalid CPLD offset. cpld=%d offset=0x%04x",
                                         id, offset);
        }
        else
        {
            *(base + offset) = val;
            DG_DBG_TRACE("CPLD ID %d offset 0x%04x set value: %d", id, offset, val);
            ret = TRUE;
        }
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/
/*=============================================================================================*//**
@brief find the CPLD UIO driver device

@return the name of the CPLD device, NULL if failed

*//*==============================================================================================*/
const char* dg_cmn_drv_cpld_find()
{
    static char        cpld_path[128];
    static const char* cpld_dev = NULL;

    if (cpld_dev == NULL)
    {
        struct dirent** namelist = NULL;
        int             n;

        if ((n = scandir("/sys/class/uio", &namelist, NULL,  alphasort)) < 0)
        {
            DG_DBG_ERROR("can't scan /sys/class/uio. errno=%d(%m)", errno);
        }
        else
        {
            int i;
            for (i = 0; i < n; i++)
            {
                if ((cpld_dev != NULL) ||
                    (strcmp(namelist[i]->d_name, ".") == 0) ||
                    (strcmp(namelist[i]->d_name, "..") == 0))
                {
                    /* do nothing */
                }
                else if (dg_cmn_drv_cpld_is_cpld(namelist[i]->d_name))
                {
                    snprintf(cpld_path, sizeof(cpld_path), "/dev/%s", namelist[i]->d_name);
                    cpld_dev = cpld_path;
                    DG_DBG_TRACE("cpld_dev: %s", cpld_dev);
                }

                free(namelist[i]);
            }
            free(namelist);
        }

    }

    return cpld_dev;
}

/*=============================================================================================*//**
@brief judge if the uio is CPLD driver

@return TRUE if the uio is for CPLD, otherwise return FALSE

*//*==============================================================================================*/
BOOL dg_cmn_drv_cpld_is_cpld(const char* name)
{
    BOOL  ret = FALSE;
    char  filename[128];
    FILE* fp = NULL;

    snprintf(filename, sizeof(filename), "/sys/class/uio/%s/name", name);

    if ((fp = fopen(filename, "r")) == NULL)
    {
        DG_DBG_TRACE("can not open %s. errno=%d(%m)", filename, errno);
    }
    else
    {
        char*  line = NULL;
        size_t len;

        if (getline(&line, &len, fp) != -1)
        {
            /* see if the name contains our driver name */
            if (strstr(line, "uio_cpld") != NULL)
            {
                DG_DBG_TRACE("/sys/class/uio/%s is for CPLD", name);
                ret = TRUE;
            }
        }

        free(line);
        fclose(fp);
    }

    return ret;
}

/** @} */
/** @} */

