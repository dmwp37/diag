/*==================================================================================================

    Module Name:  dg_gpio.c

    General Description: Implements the GPIO test handler

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_cmn_drv_gpio.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup GPIO
@{

@par
<b>GPIO - 0x0017</b>

@par
GPIO test handler.
*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** Actions for GPIO command */
enum
{
    DG_GPIO_ACTION_GET     = 0x00,
    DG_GPIO_ACTION_SET     = 0x01,
    DG_GPIO_ACTION_SET_CFG = 0x02,
    DG_GPIO_ACTION_GET_CFG = 0x03,
};
typedef UINT8 DG_GPIO_ACTION_T;

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
@brief Handler function for the GPIO command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_GPIO_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_GPIO_ACTION_T            action;
    DG_CMN_DRV_GPIO_PORT_T      port;
    DG_CMN_DRV_GPIO_VALUE_T     data;
    DG_CMN_DRV_GPIO_CFG_T       cfg;
    DG_DEFS_DIAG_RSP_BUILDER_T* rsp = DG_ENGINE_UTIL_rsp_init();

    const UINT32 min_len = sizeof(action) + sizeof(port);

    DG_DBG_TRACE("In DG_GPIO_handler_main begin to parse Request");
    if (DG_ENGINE_UTIL_req_len_check_at_least(req, min_len, rsp))
    {
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, action);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, port);

        DG_DBG_TRACE("action=0x%02x, port=0x%02x", action, port);

        switch (action)
        {
        case DG_GPIO_ACTION_GET:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, 0, rsp))
            {
                if (!DG_CMN_DRV_GPIO_get(port, &data))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to get GPIO value");
                }
                else
                {
                    if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, sizeof(data)))
                    {
                        DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                        DG_ENGINE_UTIL_rsp_append_data_hton(rsp, data);
                    }
                }
            }
            break;

        case DG_GPIO_ACTION_SET:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(data), rsp))
            {
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, data);
                if (!DG_CMN_DRV_GPIO_set(port, data))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to set GPIO value");
                }
                else
                {
                    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                }
            }
            break;

        case DG_GPIO_ACTION_SET_CFG:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(cfg), rsp))
            {
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, cfg);
                if (!DG_CMN_DRV_GPIO_set_cfg(port, cfg))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to config GPIO port");
                }
                else
                {
                    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                }
            }
            break;

        case DG_GPIO_ACTION_GET_CFG:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, 0, rsp))
            {
                if (!DG_CMN_DRV_GPIO_get_cfg(port, &cfg))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to get GPIO config");
                }
                else
                {
                    if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, sizeof(cfg)))
                    {
                        DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                        DG_ENGINE_UTIL_rsp_append_data_hton(rsp, cfg);
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

