#ifndef _DG_CMN_DRV_SSD_H_
#define _DG_CMN_DRV_SSD_H_
/*==================================================================================================

    Module Name:  dg_cmn_drv_ssd.h

    General Description: This file provides driver interface for SSD test

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup SSD_driver
@{

@par
Provide APIs for SSD
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
    DG_CMN_DRV_SSD_OS  = 0x00,
    DG_CMN_DRV_SSD_LOG = 0x01,
};
typedef UINT8 DG_CMN_DRV_SSD_ID_T;

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef UINT8  DG_CMN_DRV_SSD_STATUS_T;
typedef UINT32 DG_CMN_DRV_SSD_SIZE_T;

/*==================================================================================================
                                   GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*=============================================================================================*//**
@brief Get SSD status on the given SSD device

@param[in]  ssd    - The SSD ID to read from
@param[out] status - The SSD presence and warning status read from the device by CPLD
*//*==============================================================================================*/
BOOL DG_CMN_DRV_SSD_get_status(DG_CMN_DRV_SSD_ID_T ssd, DG_CMN_DRV_SSD_STATUS_T* status);

/*=============================================================================================*//**
@brief Get SSD Model Number on the given SSD device

@param[in]  ssd    - The SSD ID to read from
@param[out] str    - The SSD Model Number read from the device
*//*==============================================================================================*/
BOOL DG_CMN_DRV_SSD_get_model(DG_CMN_DRV_SSD_ID_T ssd, char** str);

/*=============================================================================================*//**
@brief Get SSD Serial Number on the given SSD device

@param[in]  ssd    - The SSD ID to read from
@param[out] str    - The SSD Serial Number read from the device
*//*==============================================================================================*/
BOOL DG_CMN_DRV_SSD_get_serial(DG_CMN_DRV_SSD_ID_T ssd, char** str);

/*=============================================================================================*//**
@brief Get SSD capacity on the given SSD device

@param[in]  ssd    - The SSD ID to read from
@param[out] size   - The SSD device size wich MBytes
*//*==============================================================================================*/
BOOL DG_CMN_DRV_SSD_get_capacity(DG_CMN_DRV_SSD_ID_T ssd, DG_CMN_DRV_SSD_SIZE_T* size);

/*=============================================================================================*//**
@brief Get SSD cache/buffer size on the given SSD device

@param[in]  ssd    - The SSD ID to read from
@param[out] size   - The SSD cache/buffer size wich KBytes
*//*==============================================================================================*/
BOOL DG_CMN_DRV_SSD_get_cache_size(DG_CMN_DRV_SSD_ID_T ssd, DG_CMN_DRV_SSD_SIZE_T* size);

#ifdef __cplusplus
}
#endif

/** @} */
/** @} */

#endif /* _DG_CMN_DRV_DBG_LVL_H_  */

