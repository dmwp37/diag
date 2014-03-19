#ifndef _DG_CMN_DRV_CPLD_H_
#define _DG_CMN_DRV_CPLD_H_
/*==================================================================================================

    Module Name:  dg_cmn_drv_cpld.h

    General Description: This file provides driver interface for CPLD test

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup CPLD_driver
@{

@par
Provide APIs for CPLD
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

enum
{
    DG_CMN_DRV_CPLD_CB  = 0x00,
    DG_CMN_DRV_CPLD_FEB = 0x01,
};
typedef UINT8 DG_CMN_DRV_CPLD_ID_T;

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef UINT16 DG_CMN_DRV_CPLD_OFFSET_T;
typedef UINT8  DG_CMN_DRV_CPLD_VALUE_T;

/*==================================================================================================
                                   GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*=============================================================================================*//**
@brief Get the CPLD register

@param[in]  id     - The CPLD ID
@param[in]  offset - The register offset position in the CPLD
@param[out] val    - The register value

*//*==============================================================================================*/
BOOL DG_CMN_DRV_CPLD_get(DG_CMN_DRV_CPLD_ID_T     id,
                         DG_CMN_DRV_CPLD_OFFSET_T offset,
                         DG_CMN_DRV_CPLD_VALUE_T* val);

/*=============================================================================================*//**
@brief Set the CPLD register

@param[in]  id     - The CPLD ID
@param[in]  offset - The register offset position in the CPLD
@param[out] val    - The register value

*//*==============================================================================================*/
BOOL DG_CMN_DRV_CPLD_set(DG_CMN_DRV_CPLD_ID_T     id,
                         DG_CMN_DRV_CPLD_OFFSET_T offset,
                         DG_CMN_DRV_CPLD_VALUE_T  val);

#ifdef __cplusplus
}
#endif

/** @} */
/** @} */

#endif /* _DG_CMN_DRV_CPLD_H_  */

