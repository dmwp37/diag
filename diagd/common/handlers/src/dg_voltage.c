/*==================================================================================================

    Module Name:  dg_voltage.c

    General Description: Implements the Voltage test handler

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_cmn_drv_voltage.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup VOLTAGE
@{

@par
<b>VOLTAGE - 0x0005</b>

@par
VOLTAGE test handler.
*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** Actions for VOLTAGE command */
enum
{
    DG_VOLTAGE_ACTION_GET = 0x00,
    DG_VOLTAGE_ACTION_SET = 0x01
};
typedef UINT8 DG_VOLTAGE_ACTION_T;

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
@brief Handler function for the VOLTAGE command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_VOLTAGE_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_VOLTAGE_ACTION_T          action;
    DG_CMN_DRV_VOLTAGE_CHIP_T    chip;
    DG_CMN_DRV_VOLTAGE_CHANNEL_T channel;
    DG_CMN_DRV_VOLTAGE_DATA_T    data;
    DG_CMN_DRV_VOLTAGE_LEVEL_T   level;
    DG_DEFS_DIAG_RSP_BUILDER_T*  rsp = DG_ENGINE_UTIL_rsp_init();

    DG_DBG_TRACE("In DG_VOLTAGE_handler_main begin to parse Request");
    if (DG_ENGINE_UTIL_req_len_check_at_least(req, sizeof(action), rsp))
    {
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, action);

        DG_DBG_TRACE("action=0x%02x", action);

        switch (action)
        {
        case DG_VOLTAGE_ACTION_GET:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(chip) + sizeof(channel), rsp))
            {
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, chip);
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, channel);

                if (!DG_CMN_DRV_VOLTAGE_get(chip, channel, &data))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to get VOLTAGE value");
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

        case DG_VOLTAGE_ACTION_SET:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(level), rsp))
            {
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, level);

                if (!DG_CMN_DRV_VOLTAGE_set(level))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to set VOLTAGE level");
                }
                else
                {
                    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
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

