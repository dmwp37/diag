/*==================================================================================================

    Module Name:  dg_clock.c

    General Description: Implements the Clock test handler

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_cmn_drv_clock.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup CLOCK
@{

@par
<b>CLOCK - 0x0007</b>

@par
Get/Set clock register.
*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** Actions for CLOCK command */
enum
{
    DG_CLOCK_ACTION_GET = 0x00,
    DG_CLOCK_ACTION_SET = 0x01
};
typedef UINT8 DG_CLOCK_ACTION_T;

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
@brief Handler function for the CLOCK command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_CLOCK_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_CLOCK_ACTION_T           action;
    DG_CMN_DRV_CLOCK_REG_T      reg;
    DG_CMN_DRV_CLOCK_DATA_T     data;
    DG_DEFS_DIAG_RSP_BUILDER_T* rsp = DG_ENGINE_UTIL_rsp_init();

    const UINT32 min_len = sizeof(action) + sizeof(reg);

    DG_DBG_TRACE("In DG_CLOCK_handler_main begin to parse Request");
    if (DG_ENGINE_UTIL_req_len_check_at_least(req, min_len, rsp))
    {
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, action);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, reg);

        DG_DBG_TRACE("action=0x%02x, reg=%d", action, reg);

        switch (action)
        {
        case DG_CLOCK_ACTION_GET:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, 0, rsp))
            {
                if (!DG_CMN_DRV_CLOCK_get(reg, &data))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to get CLOCK register value");
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

        case DG_CLOCK_ACTION_SET:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(data), rsp))
            {
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, data);
                if (!DG_CMN_DRV_CLOCK_set(reg, data))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to set CLOCK register value");
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

