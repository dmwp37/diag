#ifndef _DG_CMN_DRV_EEPROM_H_
#define _DG_CMN_DRV_EEPROM_H_
/*==================================================================================================

    Module Name:  dg_cmn_drv_eeprom.h

    General Description: This file provides driver interface for EEPROM test

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup EEPROM_driver
@{

@par
Provide APIs for EEPROM
*/

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                              MACROS
==================================================================================================*/

/*==================================================================================================
                                               ENUMS
==================================================================================================*/

/** EEPROM SELECTION ID */
enum
{
    DG_CMN_DRV_EEPROM_CB     = 0x00, /** CPU Board ID EEPROM          */
    DG_CMN_DRV_EEPROM_CB_LOG = 0x01, /** CPU Board log EEPROM         */
    DG_CMN_DRV_EEPROM_FEB    = 0x02, /** Front End Board ID EEPROM    */
    DG_CMN_DRV_EEPROM_WTB0   = 0x03, /** WANPIM0 Test Board ID EEPROM */
    DG_CMN_DRV_EEPROM_WTB1   = 0x04, /** WANPIM1 Test Board ID EEPROM */
    DG_CMN_DRV_EEPROM_BPB    = 0x05, /** Back Plane Board ID EEPROM   */
};
typedef UINT8 DG_CMN_DRV_EEPROM_ID_T;

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/** EEPROM Device Address type */
typedef UINT16 DG_CMN_DRV_EEPROM_ADDR_T;

/*==================================================================================================
                                   GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*=============================================================================================*//**
@brief Reads one byte from the EEPROM on the given address

@param[in]  eeprom - The EEPROM to read
@param[in]  addr   - The read address of the EEPROM
@param[out] data   - The read out data

*//*==============================================================================================*/
BOOL DG_CMN_DRV_EEPROM_read(DG_CMN_DRV_EEPROM_ID_T   eeprom,
                            DG_CMN_DRV_EEPROM_ADDR_T addr,
                            UINT8*                   data);

/*=============================================================================================*//**
@brief Writes one byte to the EEPROM on the given address

@param[in]  eeprom - The EEPROM to write
@param[in]  addr   - The write address of the EEPROM
@param[in]  data   - The byte data to write to

*//*==============================================================================================*/
BOOL DG_CMN_DRV_EEPROM_write(DG_CMN_DRV_EEPROM_ID_T   eeprom,
                             DG_CMN_DRV_EEPROM_ADDR_T addr,
                             UINT8                    data);

#ifdef __cplusplus
}
#endif

/** @} */
/** @} */

#endif /* _DG_CMN_DRV_EEPROM_H_  */

