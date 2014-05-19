/*==================================================================================================

    Module Name:  dg_cmn_drv_temp.c

    General Description: Implements the Temperature common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include <linux/i2c-dev.h>
#include <unistd.h>
#include <errno.h>
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_temp.h"
#include "dg_cmn_drv_i2c.h"

/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup TEMP_driver
@{
implementation of the Temperature driver
*/

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/
/* for chip TMP435 */
#define TMP435_LOCAL_TEMP_HIGH_READ   0x00
#define TMP435_REMOTE_TEMP_HIGH_READ  0x01
#define TMP435_CONFIG_1_READ          0x03
#define TMP435_EXTENDED_RANGE_ENABLED (1 << 2)
#define TMP435_LOCAL_TEMP_LOW_READ    0x15
#define TMP435_REMOTE_TEMP_LOW_READ   0x10

/* for PSU temperature */
#define PSMI_TEMPERATURE_REG 0x8e

/* for CPU temperature */
#define GET_CPU_CMD "cat /sys/module/coretemp/drivers/platform:coretemp/coretemp.0/temp1_input"

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
typedef BOOL (* DG_CMN_DRV_TEMP_GET_T) (DG_CMN_DRV_I2C_BUS_T  bus,
                                        DG_CMN_DRV_I2C_ADDR_T dev_addr,
                                        float*                temp);

typedef struct
{
    DG_CMN_DRV_I2C_BUS_T  bus;
    DG_CMN_DRV_I2C_ADDR_T addr;
    DG_CMN_DRV_TEMP_GET_T func;
} DG_CMN_DRV_TEMP_INFO_T;

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
extern int dg_cmn_drv_i2c_connect_device(DG_CMN_DRV_I2C_BUS_T bus, DG_CMN_DRV_I2C_ADDR_T address);

static BOOL dg_cmn_drv_temp_psu(DG_CMN_DRV_I2C_BUS_T  bus,
                                DG_CMN_DRV_I2C_ADDR_T dev_addr,
                                float*                temp);

static BOOL dg_cmn_drv_temp_tmp435(DG_CMN_DRV_I2C_BUS_T  bus,
                                   DG_CMN_DRV_I2C_ADDR_T dev_addr,
                                   float*                temp);

static BOOL dg_cmn_drv_temp_tmp435_local(DG_CMN_DRV_I2C_BUS_T  bus,
                                         DG_CMN_DRV_I2C_ADDR_T dev_addr,
                                         float*                temp);

static BOOL dg_cmn_drv_temp_cpu(DG_CMN_DRV_I2C_BUS_T  bus,
                                DG_CMN_DRV_I2C_ADDR_T dev_addr,
                                float*                temp);

/*==================================================================================================
                                         GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/
static DG_CMN_DRV_TEMP_INFO_T dg_cmn_drv_temp_info[] =
{
    [DG_CMN_DRV_TEMP_CB_0]  = { DG_CMN_DRV_I2C_MUX_CPU,  0x48, dg_cmn_drv_temp_tmp435_local },
    [DG_CMN_DRV_TEMP_CB_1]  = { DG_CMN_DRV_I2C_MUX_CPU,  0x4e, dg_cmn_drv_temp_tmp435_local },
    [DG_CMN_DRV_TEMP_FEB_0] = { DG_CMN_DRV_I2C_MUX_FEB,  0x4c, dg_cmn_drv_temp_tmp435_local },
    [DG_CMN_DRV_TEMP_FEB_1] = { DG_CMN_DRV_I2C_MUX_FEB,  0x4e, dg_cmn_drv_temp_tmp435_local },
    [DG_CMN_DRV_TEMP_FEB_2] = { DG_CMN_DRV_I2C_MUX_FEB,  0x4d, dg_cmn_drv_temp_tmp435_local },
    [DG_CMN_DRV_TEMP_CPU]   = { DG_CMN_DRV_I2C_MUX_CPU,  0xff, dg_cmn_drv_temp_cpu },
    [DG_CMN_DRV_TEMP_PCH]   = { DG_CMN_DRV_I2C_MUX_CPU,  0xff, NULL },
    [DG_CMN_DRV_TEMP_PSU_0] = { DG_CMN_DRV_I2C_MUX_PSU0, 0x58, dg_cmn_drv_temp_psu },
    [DG_CMN_DRV_TEMP_PSU_1] = { DG_CMN_DRV_I2C_MUX_PSU1, 0x58, dg_cmn_drv_temp_psu },
    [DG_CMN_DRV_TEMP_WTB_1] = { DG_CMN_DRV_I2C_MUX_WTB1, 0x4c, dg_cmn_drv_temp_tmp435_local },
    [DG_CMN_DRV_TEMP_WTB_2] = { DG_CMN_DRV_I2C_MUX_WTB2, 0x4c, dg_cmn_drv_temp_tmp435_local },
    [DG_CMN_DRV_TEMP_PHY_0] = { DG_CMN_DRV_I2C_MUX_FEB,  0x4c, dg_cmn_drv_temp_tmp435 },
    [DG_CMN_DRV_TEMP_PHY_1] = { DG_CMN_DRV_I2C_MUX_FEB,  0x4e, dg_cmn_drv_temp_tmp435 },
    [DG_CMN_DRV_TEMP_FPGA]  = { DG_CMN_DRV_I2C_MUX_FEB,  0x4d, dg_cmn_drv_temp_tmp435 },
};

/*==================================================================================================
                                         GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Get temperature from specified temp sensor

@param[in]  sensor      - temperature sensor selection
@param[out] temperature - The temperature from the sensor (convert from float)

@note
- the temp is a UINT32 pointer, but it must contains the exact data as a float
- *temp = *(UINT32*)&(float_temperature)
*//*==============================================================================================*/
BOOL DG_CMN_DRV_TEMP_get(DG_CMN_DRV_TEMP_SENSOR_T sensor, DG_CMN_DRV_TEMP_DATA_T* temperature)
{
    BOOL  ret    = FALSE;
    float f_temp = 23.4;

    if (sensor > DG_ARRAY_SIZE(dg_cmn_drv_temp_info))
    {
        DG_DRV_UTIL_set_error_string("Invalid temperature sensor=%d", sensor);
        return FALSE;
    }

    if (dg_cmn_drv_temp_info[sensor].func == NULL)
    {
        DG_DRV_UTIL_set_error_string("Unsupported temperature sensor=%d", sensor);
        return FALSE;
    }

    if (dg_cmn_drv_temp_info[sensor].func(dg_cmn_drv_temp_info[sensor].bus,
                                          dg_cmn_drv_temp_info[sensor].addr,
                                          &f_temp))
    {
        DG_DBG_TRACE("TEMP sensor %d got temperature: %+.2f C", sensor, f_temp);
        *temperature = *(UINT32*)&f_temp;
        ret          = TRUE;
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Get temperature from PSU PSMI

@param[in]  bus      - chip bus number
@param[in]  dev_addr - chip device address
@param[out] temp     - The temperature from the sensor
*//*==============================================================================================*/
BOOL dg_cmn_drv_temp_psu(DG_CMN_DRV_I2C_BUS_T  bus,
                         DG_CMN_DRV_I2C_ADDR_T dev_addr,
                         float*                temp)
{
    BOOL ret = FALSE;
    int  fd;
    int  r;

    if ((fd = dg_cmn_drv_i2c_connect_device(bus, dev_addr)) < 0)
    {
        return FALSE;
    }

    if ((r = i2c_smbus_read_word_data(fd, PSMI_TEMPERATURE_REG)) < 0)
    {
        DG_DRV_UTIL_set_error_string("PSMI failed to read temperature, bus=%d, errno=%d(%m)",
                                     bus, errno);
    }
    else
    {
        __s16 exponent;
        __s32 mantissa;
        long  val;

        exponent = ((__s16)r) >> 11;
        mantissa = ((__s16)((r & 0x7ff) << 5)) >> 5;

        val = mantissa;

        if (exponent >= 0)
        {
            val <<= exponent;
        }
        else
        {
            val >>= -exponent;
        }

        *temp = val;
        ret   = TRUE;
    }

    close(fd);

    return ret;
}

/*=============================================================================================*//**
@brief Get temperature from TMP435 chip

@param[in]  bus      - chip bus number
@param[in]  dev_addr - chip device address
@param[out] temp     - The temperature from the sensor
*//*==============================================================================================*/
BOOL dg_cmn_drv_temp_tmp435(DG_CMN_DRV_I2C_BUS_T  bus,
                            DG_CMN_DRV_I2C_ADDR_T dev_addr,
                            float*                temp)
{
    BOOL  ret = FALSE;
    UINT8 config1;
    UINT8 high;
    UINT8 low;
    float t;

    if (DG_CMN_DRV_I2C_read_bus(bus, dev_addr, TMP435_CONFIG_1_READ, 1, &config1) &&
        DG_CMN_DRV_I2C_read_bus(bus, dev_addr, TMP435_REMOTE_TEMP_HIGH_READ, 1, &high) &&
        DG_CMN_DRV_I2C_read_bus(bus, dev_addr, TMP435_REMOTE_TEMP_LOW_READ, 1, &low))
    {
        t  = low >> 4;
        t *= 0.0625;
        t += high;

        if (config1 & TMP435_EXTENDED_RANGE_ENABLED)
        {
            t -= 64;
        }
        *temp = t;
        ret   = TRUE;
    }

    return ret;
}

/*=============================================================================================*//**
@brief Get local temperature from TMP435 chip

@param[in]  bus      - chip bus number
@param[in]  dev_addr - chip device address
@param[out] temp     - The temperature from the sensor
*//*==============================================================================================*/
BOOL dg_cmn_drv_temp_tmp435_local(DG_CMN_DRV_I2C_BUS_T  bus,
                                  DG_CMN_DRV_I2C_ADDR_T dev_addr,
                                  float*                temp)
{
    BOOL  ret = FALSE;
    UINT8 config1;
    UINT8 high;
    UINT8 low;
    float t;

    if (DG_CMN_DRV_I2C_read_bus(bus, dev_addr, TMP435_CONFIG_1_READ, 1, &config1) &&
        DG_CMN_DRV_I2C_read_bus(bus, dev_addr, TMP435_LOCAL_TEMP_HIGH_READ, 1, &high) &&
        DG_CMN_DRV_I2C_read_bus(bus, dev_addr, TMP435_LOCAL_TEMP_LOW_READ, 1, &low))
    {
        t  = low >> 4;
        t *= 0.0625;
        t += high;

        if (config1 & TMP435_EXTENDED_RANGE_ENABLED)
        {
            t -= 64;
        }
        *temp = t;
        ret   = TRUE;
    }

    return ret;
}
/*=============================================================================================*//**
@brief Get temperature from CPU

@param[in]  bus      - NOT USED
@param[in]  dev_addr - NOT USED
@param[out] temp     - The temperature from the sensor
@note
- The command depends on the kernel module coretemp. So please confirm with "lsmod |grep coretemp".
*//*==============================================================================================*/
BOOL dg_cmn_drv_temp_cpu(DG_CMN_DRV_I2C_BUS_T  bus,
                         DG_CMN_DRV_I2C_ADDR_T dev_addr,
                         float*                temp)
{
    BOOL  ret   = FALSE;
    char* p_out = NULL;
    int   temp1;

    DG_COMPILE_UNUSED(bus);
    DG_COMPILE_UNUSED(dev_addr);

    if (DG_DRV_UTIL_system(GET_CPU_CMD, &p_out))
    {
        if (p_out != NULL)
        {
            if (sscanf(p_out, "%d", &temp1) == 1)
            {
                *temp = temp1 / 1000.0;
                ret   = TRUE;
            }
            free(p_out);
        }
    }

    return ret;
}

/** @} */
/** @} */

