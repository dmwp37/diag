/*==================================================================================================

    Module Name:  dg_tpm.c

    General Description: Implements the TPM test handler

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_cmn_drv_tpm.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup TPM
@{

@par
<b>TPM - 0x0014</b>

@par
Test the TPM (Trusted Platform Module) chip ,including capability,PCR and monotonic counter.
*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** Actions for TPM command */
enum
{
    DG_TPM_ACTION_GET_CAPABILITY = 0x00,
    DG_TPM_ACTION_GET_TESTRESULT = 0x01,
    DG_TPM_ACTION_GET_PCR        = 0x02,
    DG_TPM_ACTION_GET_COUNTER    = 0x03,
};
typedef UINT8 DG_TPM_ACTION_T;

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
@brief Handler function for the TPM command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_TPM_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_TPM_ACTION_T              action;
    DG_CMN_DRV_TPM_OFFSET_T      offset;
    DG_CMN_DRV_TPM_CAPABILITY_T  cap;
    DG_CMN_DRV_TPM_TEST_RESULT_T result;
    DG_CMN_DRV_TPM_PCR_T         pcr;
    DG_CMN_DRV_TPM_COUNTER_T     counter;
    DG_DEFS_DIAG_RSP_BUILDER_T*  rsp = DG_ENGINE_UTIL_rsp_init();

    DG_DBG_TRACE("In DG_TPM_handler_main begin to parse Request");
    if (DG_ENGINE_UTIL_req_len_check_at_least(req, sizeof(action), rsp))
    {
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, action);

        DG_DBG_TRACE("action=0x%02x", action);

        switch (action)
        {
        case DG_TPM_ACTION_GET_CAPABILITY:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, 0, rsp))
            {
                if (!DG_CMN_DRV_TPM_get_capability(&cap))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to Get TPM capability");
                }
                else
                {
                    /* Allocate enough memory for read data */
                    if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, sizeof(cap)))
                    {
                        DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                        DG_ENGINE_UTIL_rsp_append_data_hton(rsp, cap.tag);
                        DG_ENGINE_UTIL_rsp_append_data_hton(rsp, cap.version);
                        DG_ENGINE_UTIL_rsp_append_data_hton(rsp, cap.specLevel);
                        DG_ENGINE_UTIL_rsp_append_data_hton(rsp, cap.errataRev);
                        DG_ENGINE_UTIL_rsp_append_data_hton(rsp, cap.tpmVendorID);
                        DG_ENGINE_UTIL_rsp_append_data_hton(rsp, cap.venorSpecificSize);
                        DG_ENGINE_UTIL_rsp_append_buf(rsp, cap.buildNumber, sizeof(cap.buildNumber));
                    }
                }
            }
            break;

        case DG_TPM_ACTION_GET_TESTRESULT:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, 0, rsp))
            {
                if (!DG_CMN_DRV_TPM_get_test_result(&result))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to Get TPM selftest result");
                }
                else
                {
                    if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, sizeof(cap)))
                    {
                        DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                        DG_ENGINE_UTIL_rsp_append_data_hton(rsp, result);
                    }
                }
            }
            break;

        case DG_TPM_ACTION_GET_PCR:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(offset), rsp))
            {
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, offset);
                if (!DG_CMN_DRV_TPM_get_pcr(offset, &pcr))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to Get TPM PCR");
                }
                else
                {
                    if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, sizeof(pcr)))
                    {
                        DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                        DG_ENGINE_UTIL_rsp_append_data_hton(rsp, pcr);
                    }
                }
            }
            break;

        case DG_TPM_ACTION_GET_COUNTER:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(offset), rsp))
            {
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, offset);
                if (!DG_CMN_DRV_TPM_get_counter(offset, &counter))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to Get TPM counter");
                }
                else
                {
                    if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, sizeof(counter)))
                    {
                        DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                        DG_ENGINE_UTIL_rsp_append_data_hton(rsp, counter);
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

