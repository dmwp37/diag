/*==================================================================================================

    Module Name:  dg_cmn_drv_eeprom.c

    General Description: Implements the EEPROM common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <errno.h>
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_eeprom.h"
#include "dg_cmn_drv_i2c.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup EEPROM_driver
@{
implementation of the EEPROM driver
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
extern int  dg_cmn_drv_i2c_connect_device(DG_CMN_DRV_I2C_BUS_T bus, DG_CMN_DRV_I2C_ADDR_T address);
static BOOL dg_cmn_drv_eeprom_wide_read(DG_CMN_DRV_I2C_BUS_T bus, DG_CMN_DRV_I2C_ADDR_T address,
                                        UINT16 offset,  UINT8* read_data);
static BOOL dg_cmn_drv_eeprom_wide_write(DG_CMN_DRV_I2C_BUS_T bus, DG_CMN_DRV_I2C_ADDR_T address,
                                         UINT16 offset,  UINT8 write_data);

static int i2c_write_2b(int fd, UINT8 buf[2]);
static int i2c_write_3b(int fd, UINT8 buf[3]);

/*==================================================================================================
                                         GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/
/** chip bus information */
static DG_CMN_DRV_I2C_BUS_T dg_cmn_drv_eeprom_chip_bus[] =
{
    [DG_CMN_DRV_EEPROM_CB]     = DG_CMN_DRV_I2C_MUX_CPU,
    [DG_CMN_DRV_EEPROM_CB_LOG] = DG_CMN_DRV_I2C_MUX_CPU,
    [DG_CMN_DRV_EEPROM_FEB]    = DG_CMN_DRV_I2C_MUX_FEB,
    [DG_CMN_DRV_EEPROM_WTB0]   = DG_CMN_DRV_I2C_MUX_WTB1,
    [DG_CMN_DRV_EEPROM_WTB1]   = DG_CMN_DRV_I2C_MUX_WTB2,
    [DG_CMN_DRV_EEPROM_BPB]    = DG_CMN_DRV_I2C_MUX_WBP,
    [DG_CMN_DRV_EEPROM_PSU0]   = DG_CMN_DRV_I2C_MUX_PSU0,
    [DG_CMN_DRV_EEPROM_PSU1]   = DG_CMN_DRV_I2C_MUX_PSU1,

};
/** chip address information */
static DG_CMN_DRV_I2C_ADDR_T dg_cmn_drv_eeprom_chip_addr[] =
{
    [DG_CMN_DRV_EEPROM_CB]     = 0x54,
    [DG_CMN_DRV_EEPROM_CB_LOG] = 0x57,
    [DG_CMN_DRV_EEPROM_FEB]    = 0x54,
    [DG_CMN_DRV_EEPROM_WTB0]   = 0x54,
    [DG_CMN_DRV_EEPROM_WTB1]   = 0x54,
    [DG_CMN_DRV_EEPROM_BPB]    = 0x54,
    [DG_CMN_DRV_EEPROM_PSU0]   = 0x50,
    [DG_CMN_DRV_EEPROM_PSU1]   = 0x50,
};

/*==================================================================================================
                                         GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Reads one byte from the EEPROM on the given address

@param[in]  eeprom - The EEPROM to read
@param[in]  addr   - The read address of the EEPROM
@param[out] data   - The read out data

*//*==============================================================================================*/
BOOL DG_CMN_DRV_EEPROM_read(DG_CMN_DRV_EEPROM_ID_T   eeprom,
                            DG_CMN_DRV_EEPROM_ADDR_T addr,
                            UINT8*                   data)
{
    BOOL ret = FALSE;

    switch (eeprom)
    {
    case DG_CMN_DRV_EEPROM_CB:
    case DG_CMN_DRV_EEPROM_FEB:
    case DG_CMN_DRV_EEPROM_WTB0:
    case DG_CMN_DRV_EEPROM_WTB1:
    case DG_CMN_DRV_EEPROM_BPB:
    case DG_CMN_DRV_EEPROM_PSU0:
    case DG_CMN_DRV_EEPROM_PSU1:
        if (DG_CMN_DRV_I2C_read_bus(dg_cmn_drv_eeprom_chip_bus[eeprom],
                                    dg_cmn_drv_eeprom_chip_addr[eeprom],
                                    addr, 1, data))
        {
            DG_DBG_TRACE("Read EEPROM: EEPROM_ID=%d, Offset=0x%04x, Data=0x%02x",
                         eeprom, addr, *data);
            ret = TRUE;
        }
        break;

    case DG_CMN_DRV_EEPROM_CB_LOG:
        if (dg_cmn_drv_eeprom_wide_read(dg_cmn_drv_eeprom_chip_bus[eeprom],
                                        dg_cmn_drv_eeprom_chip_addr[eeprom],
                                        addr, data))
        {
            DG_DBG_TRACE("Read EEPROM: EEPROM_ID=%d, Offset=0x%04x, Data=0x%02x",
                         eeprom, addr, *data);
            ret = TRUE;
        }
        break;

    default:
        DG_DRV_UTIL_set_error_string("Read invalid EEPROM: EEPROM_ID=%d, Offset=0x%04x",
                                     eeprom, addr);
        break;
    }

    return ret;
}

/*=============================================================================================*//**
@brief Writes one byte to the EEPROM on the given address

@param[in]  eeprom - The EEPROM to write
@param[in]  addr   - The write address of the EEPROM
@param[in]  data   - The byte data to write to

*//*==============================================================================================*/
BOOL DG_CMN_DRV_EEPROM_write(DG_CMN_DRV_EEPROM_ID_T   eeprom,
                             DG_CMN_DRV_EEPROM_ADDR_T addr,
                             UINT8                    data)
{
    BOOL ret = FALSE;

    switch (eeprom)
    {
    case DG_CMN_DRV_EEPROM_CB:
    case DG_CMN_DRV_EEPROM_FEB:
    case DG_CMN_DRV_EEPROM_WTB0:
    case DG_CMN_DRV_EEPROM_WTB1:
    case DG_CMN_DRV_EEPROM_BPB:
    case DG_CMN_DRV_EEPROM_PSU0:
    case DG_CMN_DRV_EEPROM_PSU1:
        if (DG_CMN_DRV_I2C_write_bus(dg_cmn_drv_eeprom_chip_bus[eeprom],
                                     dg_cmn_drv_eeprom_chip_addr[eeprom],
                                     addr, 1, &data))
        {
            DG_DBG_TRACE("Write EEPROM: EEPROM_ID=%d, Offset=0x%04x, Data=0x%02x",
                         eeprom, addr, data);
            ret = TRUE;
        }
        break;

    case DG_CMN_DRV_EEPROM_CB_LOG:
        if (dg_cmn_drv_eeprom_wide_write(dg_cmn_drv_eeprom_chip_bus[eeprom],
                                         dg_cmn_drv_eeprom_chip_addr[eeprom],
                                         addr, data))
        {
            DG_DBG_TRACE("Write EEPROM: EEPROM_ID=%d, Offset=0x%04x, Data=0x%02x",
                         eeprom, addr, data);
            ret = TRUE;
        }
        break;

    default:
        DG_DRV_UTIL_set_error_string("Write invalid EEPROM: EEPROM_ID=%d, Offset=0x%04x",
                                     eeprom, addr);
        break;
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Read data from 16bits address eeprom

@param[in]  bus       - The I2C bus to read from
@param[in]  address   - The device address to read from
@param[in]  offset    - The device data offset to read from
@param[out] read_data - The data read from the device

@return TURE if success
*//*==============================================================================================*/
BOOL dg_cmn_drv_eeprom_wide_read(DG_CMN_DRV_I2C_BUS_T bus, DG_CMN_DRV_I2C_ADDR_T address,
                                 UINT16 offset,  UINT8* read_data)
{
    BOOL  ret    = FALSE;
    int   bus_fd = dg_cmn_drv_i2c_connect_device(bus, address);
    UINT8 buf[2] = { (offset >> 8) & 0x0ff, offset & 0x0ff };
    int   r;

    if (bus_fd > 0)
    {
        if (i2c_write_2b(bus_fd, buf) < 0)
        {
            DG_DBG_ERROR("I2C driver failed to write offset to device, "
                         "bus=%d, address=0x%02x, offset=0x%04x, errno=%d(%m)",
                         bus, address, offset, errno);
        }
        else if ((r = i2c_smbus_read_byte(bus_fd)) < 0)
        {
            DG_DBG_ERROR("I2C driver failed to read byte from device, "
                         "bus=%d, address=0x%02x, offset=0x%04x, errno=%d(%m)",
                         bus, address, offset, errno);
        }
        else
        {
            DG_DBG_TRACE("I2C driver successfully read one bytes from device, "
                         "bus=%d, address=0x%02x, offset=0x%04x, value=0x%02x",
                         bus, address, offset, r);
            *read_data = r;

            ret = TRUE;
        }

        close(bus_fd);
    }

    return ret;
}

/*=============================================================================================*//**
@brief Write data to 16bits address eeprom

@param[in]  bus        - The I2C bus to write to
@param[in]  address    - The device address to write to
@param[in]  offset     - The device data offset to write to
@param[out] write_data - The data write to the device

@return TURE if success
*//*==============================================================================================*/
BOOL dg_cmn_drv_eeprom_wide_write(DG_CMN_DRV_I2C_BUS_T bus, DG_CMN_DRV_I2C_ADDR_T address,
                                  UINT16 offset,  UINT8 write_data)
{
    BOOL  ret    = FALSE;
    int   bus_fd = dg_cmn_drv_i2c_connect_device(bus, address);
    UINT8 buf[3] = { (offset >> 8) & 0x00ff, offset & 0x00ff, write_data };

    if (bus_fd > 0)
    {
        if (i2c_write_3b(bus_fd, buf) < 0)
        {
            DG_DBG_ERROR("I2C driver failed to write to device, "
                         "bus=%d, address=0x%02x, offset=0x%04x, errno=%d(%m)",
                         bus, address, offset, errno);
        }
        else
        {
            DG_DBG_TRACE("I2C driver successfully write one byte to device, "
                         "bus=%d, address=0x%02x, offset=0x%04x, value=0x%02x",
                         bus, address, offset, write_data);
            ret = TRUE;
        }

        close(bus_fd);
    }

    return ret;
}

/*=============================================================================================*//**
@brief write 2 bytes to i2c bus

@param[in]  fd   - the I2C device fd
@param[in]  buf  - the raw data to write

@return
- -1 on error
*//*==============================================================================================*/
int i2c_write_2b(int fd, UINT8 buf[2])
{
    int r;
    int retry = 0;
    /* we must simulate a plain I2C byte write with SMBus functions */
retry:
    r = i2c_smbus_write_byte_data(fd, buf[0], buf[1]);
    if (r < 0)
    {
        if ((errno == ENXIO) && (retry++ < 500))
        {
            /* write busy, retry */
            usleep(10);
            goto retry;
        }
    }
    return r;
}

/*=============================================================================================*//**
@brief write 3 bytes to i2c bus

@param[in]  fd   - the I2C device fd
@param[in]  buf  - the raw data to write

@return
- -1 on error
*//*==============================================================================================*/
int i2c_write_3b(int fd, UINT8 buf[3])
{
    int r;
    int retry = 0;
    /* we must simulate a plain I2C byte write with SMBus functions */
    /* the __u16 data field will be byte swapped by the SMBus protocol */
retry:
    r = i2c_smbus_write_word_data(fd, buf[0], buf[2] << 8 | buf[1]);
    if (r < 0)
    {
        if ((errno == ENXIO) && (retry++ < 500))
        {
            /* write busy, retry */
            usleep(10);
            goto retry;
        }
    }
    return r;
}

/** @} */
/** @} */

