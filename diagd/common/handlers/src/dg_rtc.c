/*==================================================================================================

    Module Name:  dg_rtc.c

    General Description: Implements the RTC test handler

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_cmn_drv_rtc.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup RTC
@{

@par
<b>RTC - 0x000F</b>

@par
Get/Set RTC.
*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** Actions for RTC command */
enum
{
    DG_RTC_ACTION_GET = 0x00,
    DG_RTC_ACTION_SET = 0x01
};
typedef UINT8 DG_RTC_ACTION_T;

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/
/* we can't use sizeof(DG_CMN_DRV_RTC_DATE_T) for padding issue */
static const UINT32 DG_RTC_DATE_SIZE = sizeof(((DG_CMN_DRV_RTC_DATE_T*)NULL)->year) +
                                       sizeof(((DG_CMN_DRV_RTC_DATE_T*)NULL)->month) +
                                       sizeof(((DG_CMN_DRV_RTC_DATE_T*)NULL)->day) +
                                       sizeof(((DG_CMN_DRV_RTC_DATE_T*)NULL)->hour) +
                                       sizeof(((DG_CMN_DRV_RTC_DATE_T*)NULL)->minute) +
                                       sizeof(((DG_CMN_DRV_RTC_DATE_T*)NULL)->second);

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static void dg_rtc_get(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp);
static void dg_rtc_set(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp);

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
@brief Handler function for the RTC command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_RTC_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_RTC_ACTION_T             action;
    DG_DEFS_DIAG_RSP_BUILDER_T* rsp = DG_ENGINE_UTIL_rsp_init();

    /* Verify action parameter was given */
    DG_DBG_TRACE("In DG_RTC_handler_main begin to parse Request");
    if (DG_ENGINE_UTIL_req_len_check_at_least(req, sizeof(action), rsp))
    {
        /* Parse and switch on action */
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, action);

        DG_DBG_TRACE("action=0x%02x", action);

        switch (action)
        {
        case DG_RTC_ACTION_GET:
            dg_rtc_get(req, rsp);
            break;

        case DG_RTC_ACTION_SET:
            dg_rtc_set(req, rsp);
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

/*=============================================================================================*//**
@brief Get date data from RTC

@param[in]     req     - DIAG request
@param[in,out] rsp     - DIAG rsp builder
*//*==============================================================================================*/
void dg_rtc_get(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    DG_CMN_DRV_RTC_DATE_T date;

    if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, 0, rsp))
    {
        if (!DG_CMN_DRV_RTC_get(&date))
        {
            DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                    "Failed to get RTC date");
        }
        else
        {
            /* Allocate memory for date */
            if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, DG_RTC_DATE_SIZE))
            {
                DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                DG_ENGINE_UTIL_rsp_append_data_hton(rsp, date.year);
                DG_ENGINE_UTIL_rsp_append_data_hton(rsp, date.month);
                DG_ENGINE_UTIL_rsp_append_data_hton(rsp, date.day);
                DG_ENGINE_UTIL_rsp_append_data_hton(rsp, date.hour);
                DG_ENGINE_UTIL_rsp_append_data_hton(rsp, date.minute);
                DG_ENGINE_UTIL_rsp_append_data_hton(rsp, date.second);
            }
        }
    }
}

/*=============================================================================================*//**
@brief Set date data to RTC

@param[in]     req     - DIAG request
@param[in,out] rsp     - DIAG rsp builder
*//*==============================================================================================*/
void dg_rtc_set(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    DG_CMN_DRV_RTC_DATE_T date;

    if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, DG_RTC_DATE_SIZE, rsp))
    {
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, date.year);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, date.month);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, date.day);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, date.hour);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, date.minute);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, date.second);

        if (!DG_CMN_DRV_RTC_set(&date))
        {
            DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                    "Failed to set RTC date");
        }
        else
        {
            DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
        }
    }
}

/** @} */
/** @} */

