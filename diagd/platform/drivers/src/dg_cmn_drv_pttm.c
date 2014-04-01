/*==================================================================================================

    Module Name:  dg_cmn_drv_pttm.c

    General Description: Implements the PTTM common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <errno.h>
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_pttm.h"
#include "dg_cmn_drv_i2c.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup PTTM_driver
@{
implementation of the PTTM driver
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
extern int dg_cmn_drv_i2c_connect_device(DG_CMN_DRV_I2C_BUS_T bus, DG_CMN_DRV_I2C_ADDR_T address);

/*==================================================================================================
                                         GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/
static DG_CMN_DRV_I2C_ADDR_T dg_cmn_drv_pttm_addr[] =
{
    [DG_CMN_DRV_PTTM_AD5247]   = 0x16,
    [DG_CMN_DRV_PTTM_ISL90727] = 0x2e,
    [DG_CMN_DRV_PTTM_ISL90728] = 0x3e,
};

/*==================================================================================================
                                         GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Reads RDAC register from PTTM

@param[in]  chip - PTTM chip selection
@param[out] data - The RDAC register data from PTTM

*//*==============================================================================================*/
BOOL DG_CMN_DRV_PTTM_get(DG_CMN_DRV_PTTM_CHIP_T chip, DG_CMN_DRV_PTTM_DATA_T* data)
{
    BOOL                  ret = FALSE;
    int                   fd;
    DG_CMN_DRV_I2C_ADDR_T dev_addr;

    if (chip > DG_ARRAY_SIZE(dg_cmn_drv_pttm_addr))
    {
        DG_DRV_UTIL_set_error_string("Invalid PTTM chip=%d", chip);
        return FALSE;
    }

    dev_addr = dg_cmn_drv_pttm_addr[chip];

    if ((fd = dg_cmn_drv_i2c_connect_device(DG_CMN_DRV_I2C_PCH_SMB, dev_addr)) > 0)
    {
        int r;
        if (chip == DG_CMN_DRV_PTTM_AD5247)
        {
            if ((r = i2c_smbus_read_byte(fd)) < 0)
            {
                DG_DRV_UTIL_set_error_string("I2C driver failed to read byte from AD5247, "
                                             "errno=%d(%m)", errno);
            }
        }
        else
        {
            if ((r = i2c_smbus_read_byte_data(fd, 0)) < 0)
            {
                DG_DRV_UTIL_set_error_string("PTTM failed to read byte from 0x%02x, errno=%d(%m)",
                                             dev_addr, errno);
            }
        }

        if (r >= 0)
        {
            DG_DBG_TRACE("PTTM successfully read from 0x%02x, value=0x%02x",
                         dev_addr, r);
            *data = r;
            ret   = TRUE;
        }

        close(fd);
    }

    return ret;
}

/*=============================================================================================*//**
@brief Set RDAC register to PTTM

@param[in] chip - PTTM chip selection
@param[in] data - The RDAC register data set to PTTM

*//*==============================================================================================*/
BOOL DG_CMN_DRV_PTTM_set(DG_CMN_DRV_PTTM_CHIP_T chip, DG_CMN_DRV_PTTM_DATA_T data)
{
    BOOL                  ret = FALSE;
    int                   fd;
    DG_CMN_DRV_I2C_ADDR_T dev_addr;

    if (data > DG_CMN_DRV_PTTM_DATE_MAX)
    {
        DG_DRV_UTIL_set_error_string("PTTM data is not valid. val=%d", data);
        return FALSE;
    }

    if (chip > DG_ARRAY_SIZE(dg_cmn_drv_pttm_addr))
    {
        DG_DRV_UTIL_set_error_string("Invalid PTTM chip=%d", chip);
        return FALSE;
    }

    dev_addr = dg_cmn_drv_pttm_addr[chip];

    if ((fd = dg_cmn_drv_i2c_connect_device(DG_CMN_DRV_I2C_PCH_SMB, dev_addr)) > 0)
    {
        int r;
        if (chip == DG_CMN_DRV_PTTM_AD5247)
        {
            if ((r = i2c_smbus_write_byte(fd, data)) < 0)
            {
                DG_DRV_UTIL_set_error_string("I2C driver failed to write byte to AD5247, "
                                             "errno=%d(%m)", errno);
            }
        }
        else
        {
            if ((r = i2c_smbus_write_byte_data(fd, 0, data)) < 0)
            {
                DG_DRV_UTIL_set_error_string("PTTM failed to write byte to 0x%02x, errno=%d(%m)",
                                             dev_addr, errno);
            }
        }

        if (r >= 0)
        {
            DG_DBG_TRACE("PTTM successfully write to 0x%02x, value=0x%02x",
                         dev_addr, data);
            ret = TRUE;
        }

        close(fd);
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

