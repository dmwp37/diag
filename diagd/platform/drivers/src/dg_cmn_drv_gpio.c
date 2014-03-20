/*==================================================================================================

    Module Name:  dg_cmn_drv_gpio.c

    General Description: Implements the GPIO common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_gpio.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup GPIO_driver
@{
implementation of the GPIO driver
*/

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/
#define DG_CMN_DRV_GPIO_PORT_MAX  0x49
#define DG_CMN_DRV_GPIO_PORT_BASE 188

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static BOOL dg_cmn_drv_gpio_export(UINT32 gpio);
static BOOL dg_cmn_drv_gpio_read(UINT32 gpio, const char* file, char** buf);
static BOOL dg_cmn_drv_gpio_write(UINT32 gpio, const char* file, const char* str);

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
@brief Get the GPIO port value

@param[in]  port - The GPIO port
@param[out] val  - The GPIO port value

*//*==============================================================================================*/
BOOL DG_CMN_DRV_GPIO_get(DG_CMN_DRV_GPIO_PORT_T port, DG_CMN_DRV_GPIO_VALUE_T* val)
{
    BOOL   ret  = FALSE;
    UINT32 gpio = DG_CMN_DRV_GPIO_PORT_BASE + port;
    char*  data = NULL;

    if (port > DG_CMN_DRV_GPIO_PORT_MAX)
    {
        DG_DRV_UTIL_set_error_string("Invalid port=%d", port);
    }
    else if (!dg_cmn_drv_gpio_export(gpio))
    {
        DG_DBG_ERROR("Can't export gpio%d", gpio);
    }
    else if (!dg_cmn_drv_gpio_read(gpio, "value", &data))
    {
        DG_DBG_ERROR("Can't read gpio%d/value", gpio);
    }
    else
    {
        if (data[0] != '0')
        {
            *val = DG_CMN_DRV_GPIO_ACTIVE;
        }
        else
        {
            *val = DG_CMN_DRV_GPIO_INACTIVE;
        }

        DG_DBG_TRACE("GPIO Port %d got value: %d", port, *val);
        ret = TRUE;

        free(data);
    }

    return ret;
}


/*=============================================================================================*//**
@brief Set the GPIO port value

@param[in] port - The GPIO port
@param[in] val  - The GPIO port value

*//*==============================================================================================*/
BOOL DG_CMN_DRV_GPIO_set(DG_CMN_DRV_GPIO_PORT_T port, DG_CMN_DRV_GPIO_VALUE_T val)
{
    BOOL   ret  = FALSE;
    UINT32 gpio = DG_CMN_DRV_GPIO_PORT_BASE + port;
    char*  data = (val == DG_CMN_DRV_GPIO_ACTIVE) ? "1" : "0";

    DG_CMN_DRV_GPIO_CFG_T cfg;

    if ((val != DG_CMN_DRV_GPIO_ACTIVE) && (val != DG_CMN_DRV_GPIO_INACTIVE))
    {
        DG_DRV_UTIL_set_error_string("GPIO value is not valid. val=%d", val);
    }
    else if (!DG_CMN_DRV_GPIO_get_cfg(port, &cfg))
    {
        DG_DBG_ERROR("failed to get gpio%d config", gpio);
    }
    else if (cfg != DG_CMN_DRV_GPIO_CFG_OUTPUT)
    {
        DG_DRV_UTIL_set_error_string("can't set gpio%d while configured as input", gpio);
    }
    else if (!dg_cmn_drv_gpio_write(gpio, "value", data))
    {
        DG_DBG_ERROR("Can't write gpio%d/value", gpio);
    }
    else
    {
        DG_DBG_TRACE("GPIO Port %d set value: %d", port, val);

        ret = TRUE;
    }

    return ret;
}

/*=============================================================================================*//**
@brief Configure the GPIO port as input or output

@param[in] port - The GPIO port
@param[in] cfg  - The GPIO port configuration

*//*==============================================================================================*/
BOOL DG_CMN_DRV_GPIO_set_cfg(DG_CMN_DRV_GPIO_PORT_T port, DG_CMN_DRV_GPIO_CFG_T cfg)
{
    BOOL   ret  = FALSE;
    UINT32 gpio = DG_CMN_DRV_GPIO_PORT_BASE + port;
    char*  data = (cfg == DG_CMN_DRV_GPIO_CFG_INPUT) ? "in" : "out";

    if (port > DG_CMN_DRV_GPIO_PORT_MAX)
    {
        DG_DRV_UTIL_set_error_string("Invalid port=%d", port);
    }
    else if ((cfg != DG_CMN_DRV_GPIO_CFG_INPUT) && (cfg != DG_CMN_DRV_GPIO_CFG_OUTPUT))
    {
        DG_DRV_UTIL_set_error_string("GPIO config is not valid. cfg=%d", cfg);
    }
    else if (!dg_cmn_drv_gpio_export(gpio))
    {
        DG_DBG_ERROR("Can't export gpio%d", gpio);
    }
    else if (!dg_cmn_drv_gpio_write(gpio, "direction", data))
    {
        DG_DBG_ERROR("Can't write gpio%d/direction", gpio);
    }
    else
    {
        DG_DBG_TRACE("GPIO Port %d configured as %s", port, data);
        ret = TRUE;
    }

    return ret;
}

/*=============================================================================================*//**
@brief Get the GPIO port configuration

@param[in]  port - The GPIO port
@param[out] cfg  - The GPIO port configuration

*//*==============================================================================================*/
BOOL DG_CMN_DRV_GPIO_get_cfg(DG_CMN_DRV_GPIO_PORT_T port, DG_CMN_DRV_GPIO_CFG_T* cfg)
{
    BOOL   ret  = FALSE;
    UINT32 gpio = DG_CMN_DRV_GPIO_PORT_BASE + port;
    char*  data = NULL;

    if (port > DG_CMN_DRV_GPIO_PORT_MAX)
    {
        DG_DRV_UTIL_set_error_string("Invalid port=%d", port);
    }
    else if (!dg_cmn_drv_gpio_export(gpio))
    {
        DG_DBG_ERROR("Can't export gpio%d", gpio);
    }
    else if (!dg_cmn_drv_gpio_read(gpio, "direction", &data))
    {
        DG_DBG_ERROR("Can't read gpio%d/direction", gpio);
    }
    else
    {
        if (strncmp(data, "in", 2) == 0)
        {
            *cfg = DG_CMN_DRV_GPIO_CFG_INPUT;
        }
        else
        {
            *cfg = DG_CMN_DRV_GPIO_CFG_OUTPUT;
        }

        DG_DBG_TRACE("GPIO Port %d got configured as %s", port, data);
        ret = TRUE;

        free(data);
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/
/*=============================================================================================*//**
@brief export gpio to user space

@param[in] gpio - The GPIO port

@return TRUE if success
*//*==============================================================================================*/
BOOL dg_cmn_drv_gpio_export(UINT32 gpio)
{
    BOOL  ret       = FALSE;
    char* gpio_file = NULL;

    if (asprintf(&gpio_file, "/sys/class/gpio/gpio%d/value", gpio) < 0)
    {
        DG_DRV_UTIL_set_error_string("Can't make gpio file name for gpio%d", gpio);
    }
    else if (access(gpio_file, F_OK) == 0)
    {
        DG_DBG_TRACE("gpio%d already exported", gpio);
        ret = TRUE;
    }
    else
    {
        FILE* f = fopen("/sys/class/gpio/export", "w");
        if (f == NULL)
        {
            DG_DRV_UTIL_set_error_string("can't open gpio export file. errno=%d(%m)", errno);
        }
        else
        {
            if (fprintf(f, "%d", gpio) < 0)
            {
                DG_DRV_UTIL_set_error_string("failed to write export file. errno=%d(%m)", errno);
            }
            else
            {
                DG_DBG_TRACE("gpio%d successfully exported", gpio);
                ret = TRUE;
            }
            fclose(f);
        }
    }

    free(gpio_file);

    return ret;
}

/*=============================================================================================*//**
@brief read gpio file from sys

@param[in]  gpio - The GPIO port
@param[in]  file - The file name to read
@param[out] buf  - read out buffer pointer

@return TRUE if success

@note
- the caller must free the buf if success
*//*==============================================================================================*/
BOOL dg_cmn_drv_gpio_read(UINT32 gpio, const char* file, char** buf)
{
    BOOL   ret       = FALSE;
    char*  read_buf  = NULL;
    char*  gpio_file = NULL;
    size_t len;
    FILE*  f;

    if (asprintf(&gpio_file, "/sys/class/gpio/gpio%d/%s", gpio, file) < 0)
    {
        DG_DRV_UTIL_set_error_string("make gpio%d/%s name failed", gpio, file);
    }
    else if ((f = fopen(gpio_file, "r")) == NULL)
    {
        DG_DRV_UTIL_set_error_string("failed to open %s. errno=%d(%m)", gpio_file, errno);
    }
    else if (getline(&read_buf, &len, f) < 0)
    {
        DG_DRV_UTIL_set_error_string("can't getline from %s. errno=%d(%m)", gpio_file, errno);
    }
    else
    {
        DG_DBG_TRACE("successfully read %s: %s", gpio_file, read_buf);
        *buf = read_buf;
        ret  = TRUE;
    }

    free(gpio_file);

    if (f != NULL)
    {
        fclose(f);
    }

    return ret;
}

/*=============================================================================================*//**
@brief write gpio file to sys

@param[in] gpio - The GPIO port
@param[in] file - The file name to write
@param[in] str  - The write string

@return TRUE if success
*//*==============================================================================================*/
BOOL dg_cmn_drv_gpio_write(UINT32 gpio, const char* file, const char* str)
{
    BOOL  ret       = FALSE;
    char* gpio_file = NULL;
    FILE* f;

    if (asprintf(&gpio_file, "/sys/class/gpio/gpio%d/%s", gpio, file) < 0)
    {
        DG_DRV_UTIL_set_error_string("make gpio%d/%s name failed",
                                     gpio, file);
    }
    else if ((f = fopen(gpio_file, "w")) == NULL)
    {
        DG_DRV_UTIL_set_error_string("failed to open %s. errno=%d(%m)",
                                     gpio_file, errno);
    }
    else if (fprintf(f, "%s", str) < 0)
    {
        DG_DRV_UTIL_set_error_string("can't write '%s' to %s. errno=%d(%m)",
                                     str, gpio_file, errno);
    }
    else
    {
        DG_DBG_TRACE("successfully write '%s' to %s", str, gpio_file);
        ret = TRUE;
    }

    free(gpio_file);

    if (f != NULL)
    {
        fclose(f);
    }

    return ret;
}

/** @} */
/** @} */

