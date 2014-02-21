#ifndef _DG_CMN_DRV_USB_H_
#define _DG_CMN_DRV_USB_H_
/*==================================================================================================

    Module Name:  dg_cmn_drv_usb.h

    General Description: This file provides driver interface for USB test

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup USB_driver
@{

@par
Provide APIs for USB
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
    DG_CMN_DRV_USB_PORT_FEB  = 0x00,
    DG_CMN_DRV_USB_PORT_WTB0 = 0x01,
    DG_CMN_DRV_USB_PORT_WTB1 = 0x02,
};
typedef UINT8 DG_CMN_DRV_USB_PORT_T;

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
typedef struct
{
    UINT8  bus;
    UINT8  device;
    UINT16 vendor;
    UINT16 product;
    UINT8  major;
    UINT8  minor;
    UINT16 maxpower;    /* mA */
    UINT16 speed;       /* Mbit/s */
} DG_CMN_DRV_USB_INFO_T;

/*==================================================================================================
                                   GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/*=============================================================================================*//**
@brief Reads data from a given port on the given USB device

@param[in]  port     - The USB port to read from
@param[out] usb_info - The usb info read from the device

@note
- Read data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_USB_read_info(DG_CMN_DRV_USB_PORT_T port, DG_CMN_DRV_USB_INFO_T* usb_info);

#ifdef __cplusplus
}
#endif

/** @} */
/** @} */

#endif /* _DG_CMN_DRV_DBG_LVL_H_  */

