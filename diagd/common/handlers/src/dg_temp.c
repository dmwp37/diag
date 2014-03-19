/*==================================================================================================

    Module Name:  dg_temp.c

    General Description: Implements the Temperature test handler

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_cmn_drv_temp.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup TEMP
@{

@par
<b>TEMP - 0x0003</b>

@par
Temperature test handler.
*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** Actions for TEMP command */
enum
{
    DG_TEMP_ACTION_GET = 0x00,
};
typedef UINT8 DG_TEMP_ACTION_T;

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
@brief Handler function for the TEMP command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_TEMP_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_TEMP_ACTION_T            action;
    DG_CMN_DRV_TEMP_SENSOR_T    sensor;
    DG_CMN_DRV_TEMP_DATA_T      temperature;
    DG_DEFS_DIAG_RSP_BUILDER_T* rsp = DG_ENGINE_UTIL_rsp_init();

    const UINT32 req_len = sizeof(action) + sizeof(sensor);

    DG_DBG_TRACE("In DG_TEMP_handler_main begin to parse Request");
    if (DG_ENGINE_UTIL_req_len_check_equal(req, req_len, rsp))
    {
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, action);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, sensor);

        DG_DBG_TRACE("action=0x%02x, sensor=%d", action, sensor);

        switch (action)
        {
        case DG_TEMP_ACTION_GET:
            if (!DG_CMN_DRV_TEMP_get(sensor, &temperature))
            {
                DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                        "Failed to get temperature");
            }
            else
            {
                if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, sizeof(temperature)))
                {
                    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                    DG_ENGINE_UTIL_rsp_append_data_hton(rsp, temperature);
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

