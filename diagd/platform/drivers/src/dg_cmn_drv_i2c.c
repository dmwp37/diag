/*==================================================================================================

    Module Name:  dg_cmn_drv_i2c.c

    General Description: Implements the I2C common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_i2c.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup I2C_driver
@{
implementation of the I2C driver
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
@brief Reads data from a given address on the given I2C bus

@param[in]  bus       - The I2C bus to read from
@param[in]  address   - The device address to read from
@param[in]  offset    - The device data offset to read from
@param[in]  read_len  - The amount of data to read in bytes
@param[out] read_data - The data read from the device

@note
- Read data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_I2C_read_bus(DG_CMN_DRV_I2C_BUS_T bus, DG_CMN_DRV_I2C_ADDR_T address,
                             DG_CMN_DRV_I2C_OFFSET_T offset, DG_CMN_DRV_I2C_SIZE_T read_len,
                             UINT8* read_data)
{
    BOOL ret = FALSE;

    switch (bus)
    {
    case DG_CMN_DRV_I2C_PCH_SMB:
    case DG_CMN_DRV_I2C_MUX_CPU:
    case DG_CMN_DRV_I2C_MUX_PSU0:
    case DG_CMN_DRV_I2C_MUX_PSU1:
    case DG_CMN_DRV_I2C_MUX_WBP:
    case DG_CMN_DRV_I2C_MUX_WTB1:
    case DG_CMN_DRV_I2C_MUX_WTB2:
    case DG_CMN_DRV_I2C_MUX_FEB:
        memset(read_data, address, read_len);
        DG_DBG_TRACE("Read I2C bus=0x%02x, address=0x%02x, offset=0x%02x, read_len=0x%02x",
                     bus, address, offset, read_len);
        ret = TRUE;
        break;

    default:
        DG_DRV_UTIL_set_error_string("Invalid I2C bus: bus=0x%02x", bus);
        break;

    }

    return ret;
}

/*=============================================================================================*//**
@brief Writes data to a given address on the given I2C bus

@param[in]  bus        - The I2C bus to write to
@param[in]  address    - The device address to write to
@param[in]  offset     - The device data offset to write to
@param[in]  write_len  - The amount of data to write in bytes
@param[in]  write_data - The data to write to the device
*//*==============================================================================================*/
BOOL DG_CMN_DRV_I2C_write_bus(DG_CMN_DRV_I2C_BUS_T bus, DG_CMN_DRV_I2C_ADDR_T address,
                              DG_CMN_DRV_I2C_OFFSET_T offset, DG_CMN_DRV_I2C_SIZE_T write_len,
                              UINT8* write_data)
{
    BOOL ret = FALSE;

    switch (bus)
    {
    case DG_CMN_DRV_I2C_PCH_SMB:
    case DG_CMN_DRV_I2C_MUX_CPU:
    case DG_CMN_DRV_I2C_MUX_PSU0:
    case DG_CMN_DRV_I2C_MUX_PSU1:
    case DG_CMN_DRV_I2C_MUX_WBP:
    case DG_CMN_DRV_I2C_MUX_WTB1:
    case DG_CMN_DRV_I2C_MUX_WTB2:
    case DG_CMN_DRV_I2C_MUX_FEB:
        DG_DBG_TRACE("Write I2C bus=0x%02x, address=0x%02x, offset=0x%02x, write_len=0x%02x",
                     bus, address, offset, write_len);
        DG_DBG_DUMP(write_data, write_len);
        ret = TRUE;
        break;

    default:
        DG_DRV_UTIL_set_error_string("Invalid I2C bus: bus=0x%02x", bus);
        break;

    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

