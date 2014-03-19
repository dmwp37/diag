/*==================================================================================================

    Module Name:  dg_cmn_drv_usb.c

    General Description: Implements the USB common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_usb.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup USB_driver
@{
implementation of the USB driver
*/

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/
#define DG_CMN_DRV_USB_PORT_NUM_MAX 3

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
@brief Reads data from a given port on the given USB device

@param[in]  port    - The USB port to read from
@param[out] usb_info - The usb info read from the device

@note
- Read data is only valid when the function returns with a success
*//*==============================================================================================*/
BOOL DG_CMN_DRV_USB_read_info(DG_CMN_DRV_USB_PORT_T port, DG_CMN_DRV_USB_INFO_T* usb_info)
{
    BOOL ret = FALSE;

    if (port < DG_CMN_DRV_USB_PORT_NUM_MAX)
    {
        /* A sample of lvlian USB LAN. ASIX Electronics Corp. AX88772A Fast Ethernet */
        usb_info->bus      = 1;
        usb_info->device   = 4;
        usb_info->vendor   = 0x0b95;
        usb_info->product  = 0x772a;
        usb_info->major    = 2;
        usb_info->minor    = 0;
        usb_info->maxpower = 250;
        usb_info->speed    = 480;
        DG_DBG_TRACE("Read USB information: port=0x%02x, Bus %03d, Device %03d, ID %04x:%04x, "
                     "capabilities: usb-%d.%02d, maxpower=%dmA, speed=%dMbit/s.",
                     port, usb_info->bus, usb_info->device, usb_info->vendor, usb_info->product,
                     usb_info->major, usb_info->minor, usb_info->maxpower, usb_info->speed);
        ret = TRUE;
    }
    else
    {
        DG_DRV_UTIL_set_error_string("Read USB information failed, port=%d should be little than %d",
                                     port, DG_CMN_DRV_USB_PORT_NUM_MAX);
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

