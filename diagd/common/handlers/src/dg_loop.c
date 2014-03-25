/*==================================================================================================

    Module Name:  dg_loop.c

    General Description: Implements the normal loopback test handler

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_cmn_drv_loop.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup LOOP
@{

@par
<b>LOOP - 0x0023</b>

@par
Normal Loopback test.
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
@brief Handler function for the LOOP command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_LOOP_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_CMN_DRV_LOOP_PORT_T           tx_port;
    DG_CMN_DRV_LOOP_PORT_T           rx_port;
    DG_CMN_DRV_LOOP_PACKET_SIZE_T    size;
    DG_CMN_DRV_LOOP_PACKET_NUM_T     num;
    DG_CMN_DRV_LOOP_PACKET_PATTERN_T pattern;
    DG_DEFS_DIAG_RSP_BUILDER_T*      rsp = DG_ENGINE_UTIL_rsp_init();

    const UINT32 req_len = sizeof(tx_port) + sizeof(rx_port) +
                           sizeof(size) + sizeof(pattern) + sizeof(num);

    DG_DBG_TRACE("In DG_LOOP_handler_main begin to parse Request");
    if (DG_ENGINE_UTIL_req_len_check_equal(req, req_len, rsp))
    {
        /* Parse and switch on action */
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, tx_port);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, rx_port);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, size);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, pattern);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, num);

        DG_DBG_TRACE("tx_port=0x%02x, rx_port=0x%02x, size=%d, num=%d, pattern=0x%02x",
                     tx_port, rx_port, size, num, pattern);

        if (!DG_CMN_DRV_LOOP_test(tx_port, rx_port, size, num, pattern))
        {
            DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                    "Failed to Test normal loopback");
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

