/*==================================================================================================

    Module Name:  dg_cmn_drv_temp.c

    General Description: Implements the Temperature common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
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
#define TMP435_LOCAL_TEMP_HIGH_READ  0x00
#define TMP435_REMOTE_TEMP_HIGH_READ 0x01
#define TMP435_CONFIG_1_READ         0x03
#define EXTENDED_RANGE_ENABLED       (1 << 2)
#define TMP435_LOCAL_TEMP_LOW_READ   0x15
#define TMP435_REMOTE_TEMP_LOW_READ  0x10


/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

static BOOL dg_cmn_drv_tmp435_get(DG_CMN_DRV_I2C_BUS_T  bus,
                                  DG_CMN_DRV_I2C_ADDR_T dev_addr,
                                  BOOL                  local,
                                  float*                temp);
static BOOL dg_cmn_drv_temp_cpu_get(float* temp);

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
@brief Get temperature from specified temp sensor

@param[in]  sensor      - temperature sensor selection
@param[out] temperature - The temperature from the sensor (convert from float)

@note
- the temp is a UINT32 pointer, but it must contains the exact data as a float
- *temp = *(UINT32*)&(float_temperature)
*//*==============================================================================================*/
BOOL DG_CMN_DRV_TEMP_get(DG_CMN_DRV_TEMP_SENSOR_T sensor, DG_CMN_DRV_TEMP_DATA_T* temperature)
{
    DG_CMN_DRV_I2C_BUS_T  bus;
    DG_CMN_DRV_I2C_ADDR_T dev_addr;
    BOOL                  local  = TRUE;
    BOOL                  ret    = FALSE;
    float                 f_temp = 23.4;

    switch (sensor)
    {
    case DG_CMN_DRV_TEMP_CB_0:
        /* intake temperature*/
        bus      = DG_CMN_DRV_I2C_MUX_CPU;
        dev_addr = 0x48;
        break;

    case DG_CMN_DRV_TEMP_CB_1:
        /* exhaust temperature*/
        bus      = DG_CMN_DRV_I2C_MUX_CPU;
        dev_addr = 0x49;
        break;

    case DG_CMN_DRV_TEMP_FEB_0:
        bus      = DG_CMN_DRV_I2C_MUX_FEB;
        dev_addr = 0x4c;
        break;

    case DG_CMN_DRV_TEMP_FEB_1:
        bus      = DG_CMN_DRV_I2C_MUX_FEB;
        dev_addr = 0x4d;
        break;

    case DG_CMN_DRV_TEMP_FEB_2:
        bus      = DG_CMN_DRV_I2C_MUX_FEB;
        dev_addr = 0x4e;
        break;

    case DG_CMN_DRV_TEMP_PHY_0:
        bus      = DG_CMN_DRV_I2C_MUX_FEB;
        dev_addr = 0x4c;
        local    = FALSE;
        break;

    case DG_CMN_DRV_TEMP_PHY_1:
        bus      = DG_CMN_DRV_I2C_MUX_FEB;
        dev_addr = 0x4d;
        local    = FALSE;
        break;

    case DG_CMN_DRV_TEMP_FPGA:
        bus      = DG_CMN_DRV_I2C_MUX_FEB;
        dev_addr = 0x4e;
        local    = FALSE;
        break;

    case DG_CMN_DRV_TEMP_WTB_1:
        bus      = DG_CMN_DRV_I2C_MUX_WTB1;
        dev_addr = 0x4c;
        break;

    case DG_CMN_DRV_TEMP_WTB_2:
        bus      = DG_CMN_DRV_I2C_MUX_WTB2;
        dev_addr = 0x4c;
        break;

    case DG_CMN_DRV_TEMP_CPU:
        break;

    case DG_CMN_DRV_TEMP_PCH:
    case DG_CMN_DRV_TEMP_PSU_0:
    case DG_CMN_DRV_TEMP_PSU_1:
        DG_DRV_UTIL_set_error_string("no such temperature sensor=%d", sensor);
        return FALSE;

    default:
        DG_DRV_UTIL_set_error_string("Invalid temperature sensor=%d", sensor);
        return FALSE;
    }

    if (sensor != DG_CMN_DRV_TEMP_CPU)
    {
        ret = dg_cmn_drv_tmp435_get(bus, dev_addr, local, &f_temp);
    }
    else
    {
        ret = dg_cmn_drv_temp_cpu_get(&f_temp);
    }
    if (ret == TRUE)
    {
        DG_DBG_TRACE("TEMP sensor %d got temperature: %+.2f C", sensor, f_temp);
        *temperature = *(UINT32*)&f_temp;
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/
/*=============================================================================================*//**
@brief Get temperature from TMP435 chip

@param[in]  bus      - chip bus number
@param[in]  dev_addr - chip device address
@param[in]  local    - if TMP435 local sensor then true, else if remote then false.
@param[out] temp     - The temperature from the sensor
*//*==============================================================================================*/
BOOL dg_cmn_drv_tmp435_get(DG_CMN_DRV_I2C_BUS_T  bus,
                           DG_CMN_DRV_I2C_ADDR_T dev_addr,
                           BOOL                  local,
                           float*                temp)
{
    UINT8 config1, high, low, reg_low, reg_high;
    float t;

    if (local == TRUE)
    {
        reg_high = TMP435_LOCAL_TEMP_HIGH_READ;
        reg_low  = TMP435_LOCAL_TEMP_LOW_READ;
    }
    else
    {
        reg_high = TMP435_REMOTE_TEMP_HIGH_READ;
        reg_low  = TMP435_REMOTE_TEMP_LOW_READ;
    }

    if (DG_CMN_DRV_I2C_read_bus(bus, dev_addr, TMP435_CONFIG_1_READ, 1, &config1) &&
        DG_CMN_DRV_I2C_read_bus(bus, dev_addr, reg_high, 1, &high) &&
        DG_CMN_DRV_I2C_read_bus(bus, dev_addr, reg_low, 1, &low))
    {

        t  = low >> 4;
        t *= 0.0625;
        t += high;

        if (config1 & EXTENDED_RANGE_ENABLED)
        {
            t -= 64;
        }
        *temp = t;
        return TRUE;
    }

    return FALSE;
}

/*=============================================================================================*//**
@brief Get temperature from CPU

@param[out] temp     - The temperature from the sensor
@note
- The command depends on the kernel module coretemp. So please confirm with "lsmod |grep coretemp".
*//*==============================================================================================*/
BOOL dg_cmn_drv_temp_cpu_get(float* temp)
{
    BOOL        ret       = FALSE;
    const char* shell_cmd = "cat /sys/module/coretemp/drivers/platform:coretemp/coretemp.0/temp1_input";
    char*       p_out     = NULL;
    int         temp1;

    if (DG_DRV_UTIL_system(shell_cmd, &p_out))
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

