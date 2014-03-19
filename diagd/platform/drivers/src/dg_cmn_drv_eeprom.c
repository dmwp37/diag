/*==================================================================================================

    Module Name:  dg_cmn_drv_eeprom.c

    General Description: Implements the EEPROM common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_eeprom.h"


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
    case DG_CMN_DRV_EEPROM_CB_LOG:
    case DG_CMN_DRV_EEPROM_FEB:
    case DG_CMN_DRV_EEPROM_WTB0:
    case DG_CMN_DRV_EEPROM_WTB1:
    case DG_CMN_DRV_EEPROM_BPB:
    case DG_CMN_DRV_EEPROM_PSU0:
    case DG_CMN_DRV_EEPROM_PSU1:

        *data = eeprom;
        DG_DBG_TRACE("Read EEPROM: EEPROM_ID=%d, Offset=0x%04x, Data=0x%02x", eeprom, addr, *data);
        ret = TRUE;
        break;

    default:
        DG_DRV_UTIL_set_error_string("Read EEPROM failed: EEPROM_ID=%d, Offset=0x%04x",
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
    case DG_CMN_DRV_EEPROM_CB_LOG:
    case DG_CMN_DRV_EEPROM_FEB:
    case DG_CMN_DRV_EEPROM_WTB0:
    case DG_CMN_DRV_EEPROM_WTB1:
    case DG_CMN_DRV_EEPROM_BPB:
    case DG_CMN_DRV_EEPROM_PSU0:
    case DG_CMN_DRV_EEPROM_PSU1:

        DG_DBG_TRACE("Write EEPROM: EEPROM_ID=%d, Offset=0x%04x, Data=0x%02x", eeprom, addr, data);
        ret = TRUE;
        break;

    default:
        DG_DRV_UTIL_set_error_string("Write EEPROM failed: EEPROM_ID=%d, Offset=0x%04x",
                                     eeprom, addr);
        break;
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

