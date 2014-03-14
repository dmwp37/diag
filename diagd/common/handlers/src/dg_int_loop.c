/*==================================================================================================

    Module Name:  dg_int_loop.c

    General Description: Implements the INT_LOOP test handler

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_cmn_drv_int_loop.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup INT_LOOP
@{

@par
<b>INT_LOOP - 0x0021</b>

@par
Internal Loopback test.
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
@brief Handler function for the INT_LOOP command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_INT_LOOP_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_CMN_DRV_INT_LOOP_PORT_T           port;
    DG_CMN_DRV_INT_LOOP_NODE_T           node;
    DG_CMN_DRV_INT_LOOP_CFG_T            cfg;
    DG_CMN_DRV_INT_LOOP_PACKET_SIZE_T    size;
    DG_CMN_DRV_INT_LOOP_PACKET_NUM_T     num;
    DG_CMN_DRV_INT_LOOP_PACKET_PATTERN_T pattern;
    DG_DEFS_DIAG_RSP_BUILDER_T*          rsp = DG_ENGINE_UTIL_rsp_init();

    const UINT32 req_len = sizeof(port) + sizeof(node) + sizeof(cfg) +
                           sizeof(size) + sizeof(pattern) + sizeof(num);

    DG_DBG_TRACE("In DG_INT_LOOP_handler_main begin to parse Request");
    if (DG_ENGINE_UTIL_req_len_check_equal(req, req_len, rsp))
    {
        /* Parse and switch on action */
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, port);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, node);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, cfg);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, size);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, pattern);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, num);

        DG_DBG_TRACE("port=0x%02x, node=%d, cfg=%d, size=%d, num=%d, pattern=0x%02x",
                     port, node, cfg, size, num, pattern);

        if (!DG_CMN_DRV_INT_LOOP_test(port, node, cfg, size, num, pattern))
        {
            DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                    "Failed to Test internal loopback");
        }
        else
        {
            if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, sizeof(num)))
            {
                DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                DG_ENGINE_UTIL_rsp_append_data_hton(rsp, num);
            }
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

