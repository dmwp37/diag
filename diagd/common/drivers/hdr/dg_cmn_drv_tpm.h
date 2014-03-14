#ifndef _DG_CMN_DRV_TPM_H_
#define _DG_CMN_DRV_TPM_H_
/*==================================================================================================

    Module Name:  dg_cmn_drv_tpm.h

    General Description: This file provides driver interface for TPM test

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup TPM_driver
@{

@par
Provide APIs for TPM
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

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef struct
{
    UINT16 tag;
    UINT32 version;
    UINT16 specLevel;
    UINT8  errataRev;
    UINT32 tpmVendorID;
    UINT16 venorSpecificSize;
    UINT8  buildNumber[13];
} DG_CMN_DRV_TPM_CAPABILITY_T;

typedef UINT8  DG_CMN_DRV_TPM_OFFSET_T;
typedef UINT32 DG_CMN_DRV_TPM_TEST_RESULT_T;
typedef UINT32 DG_CMN_DRV_TPM_PCR_T;
typedef UINT32 DG_CMN_DRV_TPM_COUNTER_T;

/*==================================================================================================
                                   GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*=============================================================================================*//**
@brief Get capability from TPM device

@param[out] tpm - The tpm capability read from the device
*//*==============================================================================================*/
BOOL DG_CMN_DRV_TPM_get_capability(DG_CMN_DRV_TPM_CAPABILITY_T* cap);

/*=============================================================================================*//**
@brief Get self test result from TPM device

@param[out] result - The TPM self test result read from the device
*//*==============================================================================================*/
BOOL DG_CMN_DRV_TPM_get_test_result(DG_CMN_DRV_TPM_TEST_RESULT_T* result);

/*=============================================================================================*//**
@brief Get PCR value from TPM device

@param[in]  offset - PCR offset
@param[out] pcr    - The TPM PCR value read from the device
*//*==============================================================================================*/
BOOL DG_CMN_DRV_TPM_get_pcr(DG_CMN_DRV_TPM_OFFSET_T offset, DG_CMN_DRV_TPM_PCR_T* pcr);

/*=============================================================================================*//**
@brief Get counter from TPM device

@param[in]  offset  - counter offset
@param[out] counter - The TPM counter read from the device
*//*==============================================================================================*/
BOOL DG_CMN_DRV_TPM_get_counter(DG_CMN_DRV_TPM_OFFSET_T offset, DG_CMN_DRV_TPM_COUNTER_T* counter);

#ifdef __cplusplus
}
#endif

/** @} */
/** @} */

#endif /* _DG_CMN_DRV_TPM_H_  */

