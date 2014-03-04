/*==================================================================================================

    Module Name:  dg_usb.c

    General Description: Implements the USB test handler

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include "dg_handler_inc.h"
#include "dg_cmn_drv_usb.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup USB
@{

@par
<b>USB - 0x0011</b>

@par
Allows generic read access to the USB bus config and memory space.
*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** Actions for USB command */
enum
{
    DG_USB_ACTION_READ_INFO = 0x00
};
typedef UINT8 DG_USB_ACTION_T;

/*==================================================================================================
                                          LOCAL CONSTANTS
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
@brief Handler function for the USB command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_USB_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_USB_ACTION_T             action;
    DG_CMN_DRV_USB_PORT_T       port;
    DG_DEFS_DIAG_RSP_BUILDER_T* rsp = DG_ENGINE_UTIL_rsp_init();
    DG_CMN_DRV_USB_INFO_T       usb_info;

    UINT32 req_len = sizeof(action) + sizeof(port);

    /* Verify action parameter was given */
    DG_DBG_TRACE("In DG_USB_handler_main begin to parse Request");
    if (DG_ENGINE_UTIL_req_len_check_equal(req, req_len, rsp))
    {
        /* Parse and switch on action */
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, action);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, port);

        DG_DBG_TRACE("action=0x%02x, port=0x%02x", action, port);

        switch (action)
        {
        case DG_USB_ACTION_READ_INFO:
        {
            if (!DG_CMN_DRV_USB_read_info(port, &usb_info))
            {
                DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                        "Failed to Read USB information");
            }
            else
            {
                /* Allocate enough memory for read data */
                if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, sizeof(usb_info)))
                {
                    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                    DG_ENGINE_UTIL_rsp_append_data_hton(rsp, usb_info.bus);
                    DG_ENGINE_UTIL_rsp_append_data_hton(rsp, usb_info.device);
                    DG_ENGINE_UTIL_rsp_append_data_hton(rsp, usb_info.vendor);
                    DG_ENGINE_UTIL_rsp_append_data_hton(rsp, usb_info.product);
                    DG_ENGINE_UTIL_rsp_append_data_hton(rsp, usb_info.major);
                    DG_ENGINE_UTIL_rsp_append_data_hton(rsp, usb_info.minor);
                    DG_ENGINE_UTIL_rsp_append_data_hton(rsp, usb_info.maxpower);
                    DG_ENGINE_UTIL_rsp_append_data_hton(rsp, usb_info.speed);
                }
            }
        }
        break;

        default:
            DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_ERR_PARM,
                                                "Invalid action 0x%02x", action);
            break;
        }
    }

    DG_ENGINE_UTIL_rsp_send(rsp, req);
    DG_ENGINE_UTIL_rsp_free(rsp);
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

