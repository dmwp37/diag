/*==================================================================================================

    Module Name:  dg_port_led.c

    General Description: Implements the PORT_LED test handler

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_cmn_drv_port_led.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup PORT_LED
@{

@par
<b>PORT_LED - 0x0019</b>

@par
Test all the leds of the interface ports.
*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

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
@brief Handler function for the PORT_LED command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_PORT_LED_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_CMN_DRV_PORT_LED_ACTION_T action;
    DG_CMN_DRV_PORT_LED_ID_T     id;
    DG_CMN_DRV_PORT_LED_PORT_T   port;
    DG_DEFS_DIAG_RSP_BUILDER_T*  rsp = DG_ENGINE_UTIL_rsp_init();

    const UINT32 req_len = sizeof(action) + sizeof(id) + sizeof(port);

    DG_DBG_TRACE("In DG_PORT_LED_handler_main begin to parse Request");
    if (DG_ENGINE_UTIL_req_len_check_equal(req, req_len, rsp))
    {
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, action);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, id);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, port);
        DG_DBG_TRACE("action=0x%02x, id=%d, port=0x%02x", action, id, port);

        if (!DG_CMN_DRV_PORT_LED_operate(action, id, port))
        {
            DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                    "Failed to test port LED");
        }
        else
        {
            DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
        }

        DG_ENGINE_UTIL_rsp_send(rsp, req);
        DG_ENGINE_UTIL_rsp_free(rsp);
    }
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

