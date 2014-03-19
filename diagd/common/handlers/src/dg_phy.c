/*==================================================================================================

    Module Name:  dg_phy.c

    General Description: Implements the PHY test handler

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_cmn_drv_phy.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup PHY
@{

@par
<b>PHY - 0x000B</b>

@par
Allows generic access to the PHY.
*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** Actions for PHY command */
enum
{
    DG_PHY_ACTION_READ_REG  = 0x00,
    DG_PHY_ACTION_WRITE_REG = 0x01,
};
typedef UINT8 DG_PHY_ACTION_T;

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
@brief Handler function for the PHY command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_PHY_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_PHY_ACTION_T             action;
    DG_CMN_DRV_PHY_PORT_T       port;
    DG_CMN_DRV_PHY_ADDR_T       addr;
    DG_CMN_DRV_PHY_REG_T        val;
    DG_DEFS_DIAG_RSP_BUILDER_T* rsp     = DG_ENGINE_UTIL_rsp_init();
    const UINT32                min_len = sizeof(action) + sizeof(port) + sizeof(addr);

    DG_DBG_TRACE("In DG_PHY_handler_main begin to parse Request");
    if (DG_ENGINE_UTIL_req_len_check_at_least(req, min_len, rsp))
    {
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, action);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, port);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, addr);
        DG_DBG_TRACE("action=0x%02x, port=0x%02x, addr=0x%02x", action, port, addr);

        switch (action)
        {
        case DG_PHY_ACTION_READ_REG:
            if (!DG_CMN_DRV_PHY_read_reg(port, addr, &val))
            {
                DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                        "Failed to Read PHY register");
            }
            else
            {
                if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, sizeof(val)))
                {
                    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                    DG_ENGINE_UTIL_rsp_append_data_hton(rsp, val);
                }
            }
            break;

        case DG_PHY_ACTION_WRITE_REG:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(val), rsp))
            {
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, val);
                if (!DG_CMN_DRV_PHY_write_reg(port, addr, val))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to Write PHY register");
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

