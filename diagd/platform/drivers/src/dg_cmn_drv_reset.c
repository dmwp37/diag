/*==================================================================================================

    Module Name:  dg_cmn_drv_reset.c

    General Description: Implements the RESET common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_reset.h"
#include "dg_cmn_drv_cpld.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup RESET_driver
@{
implementation of the RESET driver
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
@brief Reads data from a given address on the given RESET bus

@param[in]  comp       - Which component to reset
@param[in]  is_reset   - TRUE to reset and FALSE to recover

@note
- Read data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_RESET_reset(DG_CMN_DRV_RESET_COMP_T comp, BOOL is_reset)
{
    BOOL                     ret = FALSE;
    DG_CMN_DRV_CPLD_ID_T     chip;
    DG_CMN_DRV_CPLD_OFFSET_T reset_reg;
    DG_CMN_DRV_CPLD_OFFSET_T reset_en_reg;
    DG_CMN_DRV_CPLD_VALUE_T  value;
    UINT8                    mask;
    char*                    p_out = NULL;

    switch (comp)
    {
    case DG_CMN_DRV_RESET_CPU:
    case DG_CMN_DRV_RESET_FEB:
    case DG_CMN_DRV_RESET_CB_RENESAS:
    case DG_CMN_DRV_RESET_I2C:
        chip         = DG_CMN_DRV_CPLD_CB;
        reset_reg    = 0x03;
        reset_en_reg = 0x0c;
        break;

    case DG_CMN_DRV_RESET_PHY0:
    case DG_CMN_DRV_RESET_PHY1:
    case DG_CMN_DRV_RESET_SWITCH:
    case DG_CMN_DRV_RESET_MGT_PHY:
    case DG_CMN_DRV_RESET_FPGA:
    case DG_CMN_DRV_RESET_FPGA_PCIE:
    case DG_CMN_DRV_RESET_FEB_RENESAS:
    case DG_CMN_DRV_RESET_FPGA_FLASH:
        chip         = DG_CMN_DRV_CPLD_FEB;
        reset_reg    = 0x05;
        reset_en_reg = 0x07;
        break;

    case DG_CMN_DRV_RESET_SWITCH_PERST0:
    case DG_CMN_DRV_RESET_SWITCH_PERST1:
    case DG_CMN_DRV_RESET_I2C_MUX:
    case DG_CMN_DRV_RESET_WANPIM1:
    case DG_CMN_DRV_RESET_WANPIM2:
        chip         = DG_CMN_DRV_CPLD_FEB;
        reset_reg    = 0x06;
        reset_en_reg = 0x08;
        break;

    case DG_CMN_DRV_RESET_SYS:
        DG_DRV_UTIL_system("reboot", &p_out);
        free(p_out);
        DG_DBG_TRACE("System Reboot!");
        return TRUE;
        break;

    default:
        DG_DRV_UTIL_set_error_string("Unsupported reset chip component! Component=%d", comp);
        break;
    }

    mask = 0x01 << (comp & 0x0F);

    /* first enable all the reset */
    if (!DG_CMN_DRV_CPLD_set(chip, reset_en_reg, 0xff))
    {
        DG_DBG_ERROR("can't enable reset register, comp=0x%02x", comp);
    }
    else if (!DG_CMN_DRV_CPLD_get(chip, reset_reg, &value))
    {
        DG_DBG_ERROR("can't get CPLD reset register, comp=0x%02x", comp);
    }
    else
    {
        if (is_reset)
        {
            value |= mask;
        }
        else
        {
            value &= ~mask;
        }

        if (!DG_CMN_DRV_CPLD_set(chip, reset_reg, value))
        {
            DG_DBG_ERROR("can't set reset register, comp=0x%02x, reset=%d", comp, is_reset);
        }
        else
        {
            DG_DBG_TRACE("Reset Component=%d, Reset=%d", comp, is_reset);
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

