/*==================================================================================================

    Module Name:  dg_cmn_drv_pci.c

    General Description: Implements the PCI common driver

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
#include "dg_cmn_drv_pci.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup PCI_driver
@{
implementation of the PCI driver
*/

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/
#define DG_CMN_DRV_PCI_REG_MAX         64
#define DG_CMN_DRV_PCI_PARSE_BYTES_NUM 3  /* sizeof " %02x" */

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
@brief Reads data from a given address on the given PCI config space

@param[in]  domain    - The PCI domain ID to read from
@param[in]  bus       - The PCI bus ID to read from
@param[in]  dev       - The PCI device ID to read from
@param[in]  func      - The PCI function ID to read from
@param[in]  offset    - The device register offset to read from
@param[in]  len       - The amount of data to read in bytes
@param[out] read_data - The data read from the device

@note
- Read data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_PCI_cfg_read(DG_CMN_DRV_PCI_DOMAIN_T domain,
                             DG_CMN_DRV_PCI_BUS_T    bus,
                             DG_CMN_DRV_PCI_DEV_T    dev,
                             DG_CMN_DRV_PCI_FUNC_T   func,
                             DG_CMN_DRV_PCI_OFFSET_T offset,
                             DG_CMN_DRV_PCI_LEN_T    len,
                             UINT8*                  read_data)
{
    BOOL  ret = FALSE;
    char  shell_cmd[256];
    FILE* f = NULL;
    int   sys_ret;
    int   exit_val;
    UINT8 i;
    char  buf[DG_CMN_DRV_PCI_REG_MAX * DG_CMN_DRV_PCI_PARSE_BYTES_NUM + 1];
    int   read_reg;

    if ((offset > (DG_CMN_DRV_PCI_REG_MAX - 1))
        || (len <= 0)
        || (len > DG_CMN_DRV_PCI_REG_MAX)
        || ((offset + len) > DG_CMN_DRV_PCI_REG_MAX))
    {
        DG_DRV_UTIL_set_error_string("Invalid offset 0x%x or len 0x%x, beyond Max value %d.",
                                     offset, len, DG_CMN_DRV_PCI_REG_MAX);
        return ret;
    }

    snprintf(shell_cmd, sizeof(shell_cmd),
             "lspci -xs %x:%x:%x.%x |grep '0: ' |cut -d: -f2 | tr -d '\\n' | cut -c %d-%d",
             domain, bus, dev, func, (offset * DG_CMN_DRV_PCI_PARSE_BYTES_NUM) + 1,
             (offset + len) * DG_CMN_DRV_PCI_PARSE_BYTES_NUM);

    if ((f = popen(shell_cmd, "r")) == NULL)
    {
        DG_DRV_UTIL_set_error_string("failed to run: %s, error=%s", shell_cmd, strerror(errno));
    }
    else
    {
        /* dump the output */
        fgets(buf, sizeof(buf), f);
        if (strlen(buf) < len * DG_CMN_DRV_PCI_PARSE_BYTES_NUM)
        {
            DG_DRV_UTIL_set_error_string("failed to get result from command: %s", shell_cmd);
        }
        else
        {
            ret = TRUE;
            for (i = 0; i < len; i++)
            {
                if (sscanf(buf + i * DG_CMN_DRV_PCI_PARSE_BYTES_NUM, " %02x", &read_reg) == 1)
                {
                    read_data[i] = (UINT8)read_reg;
                }
                else
                {
                    DG_DRV_UTIL_set_error_string("failed to parse string: %s", buf + i *
                                                 DG_CMN_DRV_PCI_PARSE_BYTES_NUM);
                    ret = FALSE;
                    break;
                }
            }
        }

        sys_ret  = pclose(f);
        exit_val = WEXITSTATUS(sys_ret);
        f        = NULL;

        if (exit_val != 0)
        {
            DG_DBG_ERROR("%s, sys_ret = %d, exit_val = %d", shell_cmd, sys_ret, exit_val);
        }
    }

    return ret;
}


/*=============================================================================================*//**
@brief Writes data to a given address on the given PCI memory space

@param[in]  domain    - The PCI domain ID to read from
@param[in]  bus       - The PCI bus ID to read from
@param[in]  dev       - The PCI device ID to read from
@param[in]  func      - The PCI function ID to read from
@param[in]  offset    - The device register offset to read from
@param[in]  len       - The amount of data to read in bytes
@param[out] read_data - The data read from the device
*//*==============================================================================================*/
BOOL DG_CMN_DRV_PCI_mem_read(DG_CMN_DRV_PCI_DOMAIN_T domain,
                             DG_CMN_DRV_PCI_BUS_T    bus,
                             DG_CMN_DRV_PCI_DEV_T    dev,
                             DG_CMN_DRV_PCI_FUNC_T   func,
                             DG_CMN_DRV_PCI_OFFSET_T offset,
                             DG_CMN_DRV_PCI_LEN_T    len,
                             UINT8*                  read_data)
{
    BOOL ret = FALSE;


    DG_DRV_UTIL_set_error_string("Read PCI memory space failed: domain=0x%02x, bus=0x%02x, "
                                 "dev=0x%02x, func=0x%02x, offset=0x%08x, len=0x%08x"
                                 "(Unsupport memory space read)",
                                 domain, bus, dev, func, offset, len);

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

