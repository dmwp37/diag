/*==================================================================================================

    Module Name:  dg_cmn_drv_i2c.c

    General Description: Implements the I2C common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <linux/i2c-dev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
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
#define DG_CMN_DRV_I2C_BUS_PATH_SIZE 20
#define DG_CMN_DRV_I2C_BLOCK_SIZE    32

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static int  dg_cmn_drv_i2c_connect_device(DG_CMN_DRV_I2C_BUS_T bus, DG_CMN_DRV_I2C_ADDR_T address);
static BOOL dg_cmn_drv_i2c_read(int fd, DG_CMN_DRV_I2C_OFFSET_T offset,
                                DG_CMN_DRV_I2C_SIZE_T read_len, UINT8* read_data);
static BOOL dg_cmn_drv_i2c_write(int fd, DG_CMN_DRV_I2C_OFFSET_T offset,
                                 DG_CMN_DRV_I2C_SIZE_T write_len, const UINT8* write_data);

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
    BOOL ret    = FALSE;
    int  bus_fd = dg_cmn_drv_i2c_connect_device(bus, address);

    if (bus_fd > 0)
    {
        if (!dg_cmn_drv_i2c_read(bus_fd, offset, read_len, read_data))
        {
            DG_DBG_ERROR("I2C driver failed to read %d bytes from device, "
                         "bus=%d, address=0x%02x, offset=0x%02x, errno=%d (%s)",
                         read_len, bus, address, offset, errno, strerror(errno));
        }
        else
        {
            DG_DBG_TRACE("I2C driver successfully read %d bytes from device, "
                         "bus=%d, address=0x%02x, offset=0x%02x",
                         read_len, bus, address, offset);
            ret = TRUE;
        }
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
    BOOL ret    = FALSE;
    int  bus_fd = dg_cmn_drv_i2c_connect_device(bus, address);

    if (bus_fd > 0)
    {
        if (!dg_cmn_drv_i2c_write(bus_fd, offset, write_len, write_data))
        {
            DG_DBG_ERROR("I2C driver failed to write %d bytes to device, "
                         "bus=%d, address=0x%02x, offset=0x%02x, errno=%d (%s)",
                         write_len, bus, address, offset, errno, strerror(errno));
        }
        else
        {
            DG_DBG_TRACE("I2C driver successfully write %d bytes to device, "
                         "bus=%d, address=0x%02x, offset=0x%02x",
                         write_len, bus, address, offset);
            ret = TRUE;
        }
    }


    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief open connection to specified bus and connect to specified device address

@param[in]     bus      - The I2C bus to open
@param[in]     address  - The device address to connect

@return the file descriptor for the open bus, if < 0 then error has occurred
*//*==============================================================================================*/
int dg_cmn_drv_i2c_connect_device(DG_CMN_DRV_I2C_BUS_T bus, DG_CMN_DRV_I2C_ADDR_T address)
{
    int  bus_fd = -1;
    char bus_name[DG_CMN_DRV_I2C_BUS_PATH_SIZE];

    snprintf(bus_name, DG_CMN_DRV_I2C_BUS_PATH_SIZE, "/dev/i2c/%d", bus);
    if ((bus_fd = open(bus_name, O_RDWR)) < 0)
    {
        DG_DRV_UTIL_set_error_string("I2C driver failed to open bus %s, errno=%d (%s)",
                                     bus_name, errno, strerror(errno));
    }
    else if (ioctl(bus_fd, I2C_SLAVE_FORCE, (int)address) < 0)
    {
        DG_DRV_UTIL_set_error_string("I2C driver failed to ioctl, address=%d, errno=%d (%s)",
                                     address, errno, strerror(errno));
        close(bus_fd);
        bus_fd = -1;
    }

    return bus_fd;
}

/*=============================================================================================*//**
@brief Reads specified number of bytes from the specified offset of the i2c device

@param[in]  fd        - the I2C device fd
@param[in]  offset    - the offset to read
@param[in]  read_len  - The number of bytes to read
@param[out] read_data - Data read

@return TRUE = success, FALSE = failure
*//*==============================================================================================*/
BOOL dg_cmn_drv_i2c_read(int fd, DG_CMN_DRV_I2C_OFFSET_T offset,
                         DG_CMN_DRV_I2C_SIZE_T read_len, UINT8* read_data)
{
    BOOL is_success = TRUE;

    DG_CMN_DRV_I2C_SIZE_T total_bytes_read = 0;

    while (is_success && (total_bytes_read != read_len))
    {
        int bytes_to_read      = read_len - total_bytes_read;
        int current_bytes_read = 0;

        if (bytes_to_read > DG_CMN_DRV_I2C_BLOCK_SIZE)
        {
            bytes_to_read = DG_CMN_DRV_I2C_BLOCK_SIZE;
        }
        current_bytes_read = i2c_smbus_read_i2c_block_data(fd, offset, bytes_to_read, read_data);

        if (current_bytes_read > 0)
        {
            total_bytes_read += current_bytes_read;
            offset           += current_bytes_read;
            read_data        += current_bytes_read;
        }
        else if (current_bytes_read < 0)
        {
            DG_DRV_UTIL_set_error_string(
                "i2c block read failed, current_bytes_read = %d, errno=%d (%s)",
                current_bytes_read, errno, strerror(errno));
            is_success = FALSE;
        }
        else
        {
            if (total_bytes_read != read_len)
            {
                DG_DRV_UTIL_set_error_string(
                    "i2c read exceed, need to read %d bytes, but actual read %d bytes",
                    read_len, total_bytes_read);

                is_success = FALSE;
            }
        }
    }

    return is_success;
}

/*=============================================================================================*//**
@brief Reads specified number of bytes from the specified offset of the i2c device

@param[in]  fd         - the I2C device fd
@param[in]  offset     - the offset to read
@param[in]  write_len  - The number of bytes to read
@param[out] write_data - Data to write

@return TRUE = success, FALSE = failure
*//*==============================================================================================*/
BOOL dg_cmn_drv_i2c_write(int fd, DG_CMN_DRV_I2C_OFFSET_T offset,
                          DG_CMN_DRV_I2C_SIZE_T write_len, const UINT8* write_data)
{
    BOOL is_success = TRUE;

    DG_CMN_DRV_I2C_SIZE_T total_bytes_write = 0;

    while (is_success && (total_bytes_write != write_len))
    {
        int bytes_to_write = write_len - total_bytes_write;
        int write_ret;
        if (bytes_to_write > DG_CMN_DRV_I2C_BLOCK_SIZE)
        {
            bytes_to_write = DG_CMN_DRV_I2C_BLOCK_SIZE;
        }
        write_ret = i2c_smbus_write_i2c_block_data(fd, offset, bytes_to_write, write_data);

        if (write_ret < 0)
        {
            DG_DRV_UTIL_set_error_string("i2c block write failed, errno=%d (%s)",
                                         errno, strerror(errno));
            is_success = FALSE;
        }
        else
        {
            total_bytes_write += bytes_to_write;
            offset            += bytes_to_write;
            write_data        += bytes_to_write;
        }
    }

    return is_success;
}

/** @} */
/** @} */

