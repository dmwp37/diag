/*==================================================================================================

    Module Name:  dg_cmn_drv_version.c

    General Description: Implements the VERSION common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_version.h"
#include "diag_version.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup VERSION_driver
@{
implementation of the VERSION driver
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
BOOL dg_cmn_drv_version_copy(char** pp_str, const char* version_str);
BOOL dg_cmn_drv_version_diag(char** pp_str);
BOOL dg_cmn_drv_version_sw(char** pp_str);
BOOL dg_cmn_drv_version_hw(char** pp_str);
BOOL dg_cmn_drv_version_fpga(char** pp_str);

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
@brief Get the version string for different type

@param[in]  type  - what kind of version to get
@param[out] pp_str - the version string pointer

@note the caller of the function is responsible to free the version string
*//*==============================================================================================*/
BOOL DG_CMN_DRV_VERSION_get(DG_CMN_DRV_VERSION_TYPE_T type, char** pp_str)
{
    BOOL ret = FALSE;

    switch (type)
    {
    case DG_CMN_DRV_VERSION_DIAG:
        ret = dg_cmn_drv_version_diag(pp_str);
        break;

    case DG_CMN_DRV_VERSION_SW:
        ret = dg_cmn_drv_version_sw(pp_str);
        break;

    case DG_CMN_DRV_VERSION_HW:
        ret = dg_cmn_drv_version_hw(pp_str);
        break;

    case DG_CMN_DRV_VERSION_FPGA:
        ret = dg_cmn_drv_version_fpga(pp_str);
        break;

    default:
        DG_DRV_UTIL_set_error_string("Invalid version type. type=%d", type);
        break;
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Copy the version string to output buffer

@param[out] pp_str      - the dst version string pointer
@param[in]  version_str - the src version string pointer                                                                                                  *

@note the caller of the function is responsible to free the version string
*//*==============================================================================================*/
BOOL dg_cmn_drv_version_copy(char** pp_str, const char* version_str)
{
    BOOL   ret     = FALSE;
    size_t str_len = strlen(version_str) + 1;
    char*  str_buf = (char*)malloc(str_len);

    DG_DBG_TRACE("version string: %s", version_str);
    if (str_buf == NULL)
    {
        DG_DRV_UTIL_set_error_string("failed to malloc version string");
    }
    else
    {
        strncpy(str_buf, version_str, str_len);
        *pp_str = str_buf;
        ret     = TRUE;
    }
    return ret;
}

/*=============================================================================================*//**
@brief Get the version string for diag

@param[out] pp_str - the version string pointer
*//*==============================================================================================*/
BOOL dg_cmn_drv_version_diag(char** pp_str)
{
    return dg_cmn_drv_version_copy(pp_str, DIAG_VERSION);
}

/*=============================================================================================*//**
@brief Get the version string for SW

@param[out] pp_str - the version string pointer
*//*==============================================================================================*/
BOOL dg_cmn_drv_version_sw(char** pp_str)
{
    /* just print out the content of "uname -srm"*/
    const char* shell_cmd = "uname -srm";

    BOOL  ret = FALSE;
    FILE* f   = NULL;
    int   sys_ret;
    int   exit_val;
    char  buf[512]; /* Arbitrary size, must be large enough for known use cases */

    if ((f = popen(shell_cmd, "r")) == NULL)
    {
        DG_DRV_UTIL_set_error_string("failed to run: %s, error=%s", shell_cmd, strerror(errno));
    }
    else
    {
        /* dump the output */
        fgets(buf, sizeof(buf), f);
        DG_DBG_TRACE("%s", buf);

        sys_ret  = pclose(f);
        exit_val = WEXITSTATUS(sys_ret);
        f        = NULL;

        if (exit_val != 0)
        {
            DG_DRV_UTIL_set_error_string("%s, sys_ret = %d, exit_val = %d",
                                         shell_cmd, sys_ret, exit_val);
        }
        else
        {
            ret = dg_cmn_drv_version_copy(pp_str, buf);
        }
    }

    return ret;
}

/*=============================================================================================*//**
@brief Get the version string for HW

@param[out] pp_str - the version string pointer
*//*==============================================================================================*/
BOOL dg_cmn_drv_version_hw(char** pp_str)
{
#define HW_VERSION "unknow hw"
    return dg_cmn_drv_version_copy(pp_str, HW_VERSION);
}

/*=============================================================================================*//**
@brief Get the version string for fpga

@param[out] pp_str - the version string pointer
*//*==============================================================================================*/
BOOL dg_cmn_drv_version_fpga(char** pp_str)
{
#define FPAG_VERSION "unknow fpga"
    return dg_cmn_drv_version_copy(pp_str, FPAG_VERSION);
}

/** @} */
/** @} */

