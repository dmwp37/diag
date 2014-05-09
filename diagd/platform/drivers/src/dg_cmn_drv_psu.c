/*==================================================================================================

    Module Name:  dg_cmn_drv_psu.c

    General Description: Implements the PSU common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <errno.h>
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_psu.h"
#include "dg_cmn_drv_cpld.h"
#include "dg_cmn_drv_i2c.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup PSU_driver
@{
implementation of the PSU driver
*/

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/
#define CPLD_PSU_PWR_CTL_EN     0x0D
#define CPLD_PSU_PWR_EN_MASK    0x0C
#define CPLD_PSU_PWR_CTL        0x14
#define CPLD_PSU_STS            0x13

#define DG_CMN_DRV_PSU_DEV_ADDR 0x58

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
@brief Dump the PSU PSMI

@param[in]  slot    - The PSU slot
@param[out] psmi    - The dumped PSMI information

@note
- Fill unavailable data with 0xFF
*//*==============================================================================================*/
BOOL DG_CMN_DRV_PSU_dump_psmi(DG_CMN_DRV_PSU_SLOT_T       slot,
                              DG_CMN_DRV_PSU_PSMI_INFO_T* psmi)
{
    BOOL ret = TRUE;
    int  fd;
    int  i;
    int  r;

    DG_CMN_DRV_I2C_BUS_T bus = slot ? DG_CMN_DRV_I2C_MUX_PSU1 : DG_CMN_DRV_I2C_MUX_PSU0;

    if (slot > DG_CMN_DRV_PSU_SLOT_MAX)
    {
        DG_DRV_UTIL_set_error_string("Invalid PSU slot=%d", slot);
        return FALSE;
    }

    if ((fd = dg_cmn_drv_i2c_connect_device(bus, DG_CMN_DRV_PSU_DEV_ADDR)) < 0)
    {
        return FALSE;
    }

    for (i = 0; i < (int)DG_ARRAY_SIZE(psmi->data); i++)
    {
        if ((r = i2c_smbus_read_word_data(fd, (UINT8)i)) < 0)
        {
            DG_DRV_UTIL_set_error_string("PSMI failed to read from slot=%d, addr=%d, errno=%d(%m)",
                                         slot, i, errno);
            ret = FALSE;
            break;
        }
        else
        {
            psmi->data[i] = r;
        }
    }

    close(fd);

    if (ret)
    {
        DG_DBG_TRACE("PSU slot %d PSMI dump", slot);
        DG_DBG_DUMP(psmi->data, sizeof(psmi->data));
    }

    return ret;
}

/*=============================================================================================*//**
@brief write PSU PSMI data

@param[in] slot    - The PSU slot
@param[in] addr    - The PSMI address to write
@param[in] data    - The PSMI data to write
*//*==============================================================================================*/
BOOL DG_CMN_DRV_PSU_write_psmi(DG_CMN_DRV_PSU_SLOT_T      slot,
                               DG_CMN_DRV_PSU_PSMI_ADDR_T addr,
                               DG_CMN_DRV_PSU_PSMI_DATA_T data)
{
    BOOL ret = FALSE;
    int  fd;

    DG_CMN_DRV_I2C_BUS_T bus = slot ? DG_CMN_DRV_I2C_MUX_PSU1 : DG_CMN_DRV_I2C_MUX_PSU0;

    if (slot > DG_CMN_DRV_PSU_SLOT_MAX)
    {
        DG_DRV_UTIL_set_error_string("Invalid PSU slot=%d", slot);
        return FALSE;
    }

    if ((fd = dg_cmn_drv_i2c_connect_device(bus, DG_CMN_DRV_PSU_DEV_ADDR)) < 0)
    {
        return FALSE;
    }

    if (i2c_smbus_write_word_data(fd, addr, data) < 0)
    {
        DG_DRV_UTIL_set_error_string("PSMI failed to write slot=%d, addr=%d, errno=%d(%m)",
                                     slot, addr, errno);
    }
    else
    {
        DG_DBG_TRACE("PSMI write slot=%d, addr=%d, data=0x%04x", slot, addr, data);
        ret = TRUE;
    }

    close(fd);

    return ret;
}

/*=============================================================================================*//**
@brief Get the PSU status

@param[in]  slot    - The PSU slot
@param[out] status  - The PSU status (PST & ACOK & DCOK & ALERT)

*//*==============================================================================================*/
BOOL DG_CMN_DRV_PSU_get_status(DG_CMN_DRV_PSU_SLOT_T slot, DG_CMN_DRV_PSU_STATUS_T* status)
{
    BOOL                    ret = FALSE;
    DG_CMN_DRV_CPLD_VALUE_T value;

    if (slot > DG_CMN_DRV_PSU_SLOT_MAX)
    {
        DG_DRV_UTIL_set_error_string("Invalid PSU slot=%d", slot);
    }
    else if (!DG_CMN_DRV_CPLD_get(DG_CMN_DRV_CPLD_CB, CPLD_PSU_STS, &value))
    {
        DG_DBG_ERROR("can't get PSU status register");
    }
    else
    {
        *status = value;
        DG_DBG_TRACE("PSU slot %d got status=0x%02x", slot, *status);
        ret = TRUE;
    }

    return ret;
}

/*=============================================================================================*//**
@brief Turn on/off PSU

@param[in] slot - The PSU slot
@param[in] cfg  - The PSU turn on/off configuration

*//*==============================================================================================*/
BOOL DG_CMN_DRV_PSU_set_cfg(DG_CMN_DRV_PSU_SLOT_T slot, DG_CMN_DRV_PSU_CFG_T cfg)
{
    BOOL                    ret = FALSE;
    DG_CMN_DRV_CPLD_VALUE_T value;

    if (slot > DG_CMN_DRV_PSU_SLOT_MAX)
    {
        DG_DRV_UTIL_set_error_string("Invalid PSU slot=%d", slot);
    }
    else if ((cfg != DG_CMN_DRV_PSU_CFG_TURN_OFF) && (cfg != DG_CMN_DRV_PSU_CFG_TURN_ON))
    {
        DG_DRV_UTIL_set_error_string("Invalid PSU config=%d", cfg);
    }
    else if (!DG_CMN_DRV_CPLD_get(DG_CMN_DRV_CPLD_CB, CPLD_PSU_PWR_CTL_EN, &value))
    {
        DG_DBG_ERROR("can't get PSU enable register");
    }
    else if (!DG_CMN_DRV_CPLD_set(DG_CMN_DRV_CPLD_CB,
                                  CPLD_PSU_PWR_CTL_EN,
                                  value | (~CPLD_PSU_PWR_EN_MASK)))
    {
        DG_DBG_ERROR("can't set PSU enable register");
    }
    else if (!DG_CMN_DRV_CPLD_get(DG_CMN_DRV_CPLD_CB, CPLD_PSU_PWR_CTL, &value))
    {
        DG_DBG_ERROR("can't get PSU power register");
    }
    else
    {
        UINT8 mask = 0x01 << slot;

        if (cfg == DG_CMN_DRV_PSU_CFG_TURN_OFF)
        {
            value &= ~mask;
        }
        else
        {
            value |= mask;
        }

        if (!DG_CMN_DRV_CPLD_set(DG_CMN_DRV_CPLD_CB, CPLD_PSU_PWR_CTL, value))
        {
            DG_DBG_ERROR("can't set PSU power register");
        }
        else
        {
            DG_DBG_TRACE("PSU slot %d configured as Power %s", slot, cfg ? "ON" : "OFF");
            ret = TRUE;
        }
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

