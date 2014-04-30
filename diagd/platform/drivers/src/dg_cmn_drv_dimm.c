/*==================================================================================================

    Module Name:  dg_cmn_drv_dimm.c

    General Description: Implements the DIMM common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_dimm.h"
#include "dg_cmn_drv_i2c.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup DIMM_driver
@{
implementation of the DIMM driver
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
@brief Reads SPD data from the DIMM on the given slot

@param[in]  slot - The DIMM slot number
@param[out] spd  - The SPD data read from the dimm

*//*==============================================================================================*/
BOOL DG_CMN_DRV_DIMM_get_spd(DG_CMN_DRV_DIMM_SLOT_T slot, DG_CMN_DRV_DIMM_SPD_T* spd)
{
    BOOL ret = FALSE;

    DG_CMN_DRV_I2C_ADDR_T addr;
    DG_CMN_DRV_I2C_SIZE_T size = 128; /* one time read size */

    switch (slot)
    {
    case DG_CMN_DRV_DIMM_SLOT_0:
    case DG_CMN_DRV_DIMM_SLOT_1:
    case DG_CMN_DRV_DIMM_SLOT_2:
    case DG_CMN_DRV_DIMM_SLOT_3:
        addr = 0x50 + slot;
        if (DG_CMN_DRV_I2C_read_bus(DG_CMN_DRV_I2C_PCH_SMB, addr, 0, size, spd->value) &&
            DG_CMN_DRV_I2C_read_bus(DG_CMN_DRV_I2C_PCH_SMB, addr, size,
                                    sizeof(spd->value) - size, spd->value + size))
        {
            DG_DBG_TRACE("Get DIMM SPD from slot %d", slot);
            DG_DBG_DUMP(spd->value, sizeof(spd->value));
            ret = TRUE;
        }
        break;

    default:
        DG_DRV_UTIL_set_error_string("Invalid SPD, slot=0x%02x", slot);
        break;
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

