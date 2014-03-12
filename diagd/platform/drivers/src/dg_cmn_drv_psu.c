/*==================================================================================================

    Module Name:  dg_cmn_drv_psu.c

    General Description: Implements the PSU common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_psu.h"


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
@brief Dump the PSU PSMI

@param[in]  slot    - The PSU slot
@param[out] psmi    - The dumped PSMI information

@note
- Fill unavailable data with 0xFF
*//*==============================================================================================*/
BOOL DG_CMN_DRV_PSU_dump_psmi(DG_CMN_DRV_PSU_SLOT_T       slot,
                              DG_CMN_DRV_PSU_PSMI_INFO_T* psmi)
{
    BOOL ret = FALSE;

    if (slot > DG_CMN_DRV_PSU_SLOT_MAX)
    {
        DG_DRV_UTIL_set_error_string("Invalid PSU slot=%d", slot);
    }
    else
    {
        /* fill all the psmi data with 0xff */
        memset(psmi->data, 0xFF, sizeof(psmi->data));

        psmi->data[0] = 0x00;

        DG_DBG_TRACE("PSU slot %d PSMI dump", slot);
        DG_DBG_DUMP(psmi->data, sizeof(psmi->data));

        ret = TRUE;
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

    if (slot > DG_CMN_DRV_PSU_SLOT_MAX)
    {
        DG_DRV_UTIL_set_error_string("Invalid PSU slot=%d", slot);
    }
    else
    {

        DG_DBG_TRACE("PSU slot %d PSMI write addr=0x%02x, data=0x%04x",
                     slot, addr, data);

        ret = TRUE;
    }

    return ret;
}

/*=============================================================================================*//**
@brief Get the PSU status

@param[in]  slot    - The PSU slot
@param[out] status  - The PSU status (PST & ACOK & DCOK & ALERT)

*//*==============================================================================================*/
BOOL DG_CMN_DRV_PSU_get_status(DG_CMN_DRV_PSU_SLOT_T slot, DG_CMN_DRV_PSU_STATUS_T* status)
{
    BOOL ret = FALSE;

    if (slot > DG_CMN_DRV_PSU_SLOT_MAX)
    {
        DG_DRV_UTIL_set_error_string("Invalid PSU slot=%d", slot);
    }
    else
    {
        *status = 0xaa;

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
    BOOL ret = FALSE;

    if (slot > DG_CMN_DRV_PSU_SLOT_MAX)
    {
        DG_DRV_UTIL_set_error_string("Invalid PSU slot=%d", slot);
    }
    else
    {
        if (cfg == DG_CMN_DRV_PSU_CFG_TURN_ON)
        {
            DG_DBG_TRACE("PSU slot %d configured as Turn ON", slot);

            ret = TRUE;
        }
        else if (cfg == DG_CMN_DRV_PSU_CFG_TURN_OFF)
        {


            DG_DBG_TRACE("PSU slot %d configured as Turn OFF", slot);

            ret = TRUE;
        }
        else
        {
            DG_DRV_UTIL_set_error_string("PSU config is not valid. cfg=%d", cfg);
        }
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

