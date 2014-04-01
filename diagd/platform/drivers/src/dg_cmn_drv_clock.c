/*==================================================================================================

    Module Name:  dg_cmn_drv_clock.c

    General Description: Implements the CLOCK common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <errno.h>
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_clock.h"
#include "dg_cmn_drv_i2c.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup CLOCK_driver
@{
implementation of the CLOCK driver
*/

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/
#define DG_CMN_DRV_CLOCK_REG_MAX  9
#define DG_CMN_DRV_CLOCK_DEV_ADDR 0x69

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

/*==================================================================================================
                                         GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Read clock register

@param[in]  reg  - clock register
@param[out] data - The clock register data

*//*==============================================================================================*/
BOOL DG_CMN_DRV_CLOCK_get(DG_CMN_DRV_CLOCK_REG_T reg, DG_CMN_DRV_CLOCK_DATA_T* data)
{
    BOOL ret = FALSE;
    int  fd;

    if (reg > DG_CMN_DRV_CLOCK_REG_MAX)
    {
        DG_DRV_UTIL_set_error_string("Invalid clock register=%d", reg);
        return FALSE;
    }

    if ((fd = dg_cmn_drv_i2c_connect_device(DG_CMN_DRV_I2C_PCH_SMB, DG_CMN_DRV_CLOCK_DEV_ADDR)) > 0)
    {
        UINT8 block_data[288];
        int   r;

        if ((r = i2c_smbus_read_block_data(fd, reg, block_data)) < 0)
        {
            DG_DRV_UTIL_set_error_string("Clock failed to read from 932SQ420D, "
                                         "errno=%d(%m)", errno);
        }
        else if (r == 0)
        {
            DG_DRV_UTIL_set_error_string("Clock failed to read from 932SQ420D, "
                                         "no data read out");
        }
        else
        {
            DG_DBG_TRACE("PTTM successfully read from reg=%d, value=0x%02x",
                         reg, block_data[0]);
            *data = block_data[0];
            ret   = TRUE;
        }

        close(fd);
    }

    return ret;
}

/*=============================================================================================*//**
@brief Set clock register

@param[in] reg  - clock register
@param[in] data - The clock register data

*//*==============================================================================================*/
BOOL DG_CMN_DRV_CLOCK_set(DG_CMN_DRV_CLOCK_REG_T reg, DG_CMN_DRV_CLOCK_DATA_T data)
{
    BOOL ret = FALSE;
    int  fd;

    if (reg > DG_CMN_DRV_CLOCK_REG_MAX)
    {
        DG_DRV_UTIL_set_error_string("Invalid clock register=%d", reg);
        return FALSE;
    }

    if ((fd = dg_cmn_drv_i2c_connect_device(DG_CMN_DRV_I2C_PCH_SMB, DG_CMN_DRV_CLOCK_DEV_ADDR)) > 0)
    {
        int r;

        if ((r = i2c_smbus_write_block_data(fd, reg, 1, &data)) < 0)
        {
            DG_DRV_UTIL_set_error_string("Clock failed to write to 932SQ420D reg=%d, "
                                         "errno=%d(%m)", reg, errno);
        }
        else
        {
            DG_DBG_TRACE("Clock successfully write to 932SQ420D, reg=%d, value=0x%02x",
                         reg, data);
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

