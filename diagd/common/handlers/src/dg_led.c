/*==================================================================================================

    Module Name:  dg_led.c

    General Description: Implements the LED test handler

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_cmn_drv_led.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup LED
@{

@par
<b>LED - 0x0001</b>

@par
This command is responsible for LED test
*/

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/
#define DG_LED_REQ_LEN_MIN 2

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** Actions for LED command */
enum
{
    DG_LED_DISABLE = 0x00,
    DG_LED_ENABLE  = 0x01
};
typedef UINT8 DG_LED_ACTION_T;

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
@brief Handler function for the LED command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_LED_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_LED_ACTION_T             action;
    DG_CMN_DRV_LED_ID_T         led_id;
    DG_CMN_DRV_LED_COLOR_T      led_color;
    DG_DEFS_DIAG_RSP_BUILDER_T* rsp = DG_ENGINE_UTIL_rsp_init();

    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);

    if (DG_ENGINE_UTIL_req_len_check_at_least(req, DG_LED_REQ_LEN_MIN, rsp))
    {
        action = DG_ENGINE_UTIL_req_parse_1_byte_ntoh(req);
        led_id = DG_ENGINE_UTIL_req_parse_1_byte_ntoh(req);

        switch (action)
        {
        case DG_LED_ENABLE:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(led_color), rsp))
            {
                led_color = DG_ENGINE_UTIL_req_parse_1_byte_ntoh(req);

                if (!DG_CMN_DRV_LED_enable(led_id, led_color))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to Enable LED");
                }
            }
            break;

        case DG_LED_DISABLE:
        {
            if (!DG_CMN_DRV_LED_disable(led_id))
            {
                DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                        "Failed to Disable LED");
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

