/*==================================================================================================

    Module Name:  dg_prbs.c

    General Description: Implements the PRBS test handler

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_cmn_drv_prbs.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup PRBS
@{

@par
<b>PRBS - 0x0020</b>

@par
PRBS test commands to verify the signal interity of all high speed signals,like QSGMII between
SWITCH and PHY,HiGig between FPGA and SWITCH.
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
@brief Handler function for the PRBS command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_PRBS_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_CMN_DRV_PRBS_BUS_T       bus;
    DG_CMN_DRV_PRBS_ORDER_T     order;
    DG_CMN_DRV_PRBS_COUNT_T     count;
    DG_DEFS_DIAG_RSP_BUILDER_T* rsp     = DG_ENGINE_UTIL_rsp_init();
    const UINT32                req_len = sizeof(bus) + sizeof(order);

    DG_DBG_TRACE("In DG_PRBS_handler_main begin to parse Request");
    if (DG_ENGINE_UTIL_req_len_check_equal(req, req_len, rsp))
    {
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, bus);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, order);
        DG_DBG_TRACE("bus=0x%02x, order=0x%02x", bus, order);

        if (!DG_CMN_DRV_PRBS_operate(bus, order, &count))
        {
            DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                    "Failed to PRBS test");
        }
        else
        {
            if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, sizeof(count)))
            {
                DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                DG_ENGINE_UTIL_rsp_append_data_hton(rsp, count);
            }
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

