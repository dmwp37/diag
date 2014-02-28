/*==================================================================================================

    Module Name:  dg_dimm.c

    General Description: Implements the DIMM test handler

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include "dg_handler_inc.h"
#include "dg_cmn_drv_dimm.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup DIMM
@{

@par
<b>DIMM - 0x0008</b>

@par
Read DIMM SPD content.
*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** Actions for DIMM command */
enum
{
    DG_DIMM_ACTION_GET_SPD = 0x00,
};
typedef UINT8 DG_DIMM_ACTION_T;

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
@brief Handler function for the DIMM command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_DIMM_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_DIMM_ACTION_T            action;
    DG_CMN_DRV_DIMM_SLOT_T      slot;
    DG_DEFS_DIAG_RSP_BUILDER_T* rsp = DG_ENGINE_UTIL_rsp_init();

    /* Verify action parameter was given */
    DG_DBG_TRACE("In DG_DIMM_handler_main begin to parse Request");
    if (DG_ENGINE_UTIL_req_len_check_at_least(req, sizeof(action), rsp))
    {
        /* Parse and switch on action */
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, action);

        DG_DBG_TRACE("action=0x%02x", action);

        switch (action)
        {
        case DG_DIMM_ACTION_GET_SPD:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(slot), rsp))
            {
                DG_CMN_DRV_DIMM_SPD_T spd;

                DG_ENGINE_UTIL_req_parse_data_ntoh(req, slot);

                if (!DG_CMN_DRV_DIMM_get_spd(slot, &spd))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to Get DIMM SPD");
                }
                else
                {
                    /* Allocate memory for spd data */
                    if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, sizeof(spd.value)))
                    {
                        DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                        DG_ENGINE_UTIL_rsp_append_buf(rsp, spd.value, sizeof(spd.value));
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

