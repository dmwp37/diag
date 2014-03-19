/*==================================================================================================

    Module Name:  dg_ssd.c

    General Description: Implements the SSD test handler

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include "dg_handler_inc.h"
#include "dg_cmn_drv_ssd.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup SSD
@{

@par
<b>SSD - 0x0012</b>

@par
Read SSD status and config information.
*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** Actions for SSD command */
enum
{
    DG_SSD_ACTION_GET_STATUS     = 0x00,
    DG_SSD_ACTION_GET_MODEL      = 0x01,
    DG_SSD_ACTION_GET_SERIAL     = 0x02,
    DG_SSD_ACTION_GET_CAPACITY   = 0x03,
    DG_SSD_ACTION_GET_CACHE_SIZE = 0x04,
};
typedef UINT8 DG_SSD_ACTION_T;

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
@brief Handler function for the SSD command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_SSD_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_SSD_ACTION_T             action;
    DG_CMN_DRV_SSD_ID_T         ssd;
    DG_CMN_DRV_SSD_STATUS_T     status;
    DG_CMN_DRV_SSD_SIZE_T       size;
    char*                       str = NULL;
    UINT32                      len;
    DG_DEFS_DIAG_RSP_BUILDER_T* rsp = DG_ENGINE_UTIL_rsp_init();

    DG_DBG_TRACE("In DG_SSD_handler_main begin to parse Request");
    if (DG_ENGINE_UTIL_req_len_check_equal(req, sizeof(action) + sizeof(ssd), rsp))
    {
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, action);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, ssd);

        DG_DBG_TRACE("action=0x%02x, ssd=0x%02x", action, ssd);

        switch (action)
        {
        case DG_SSD_ACTION_GET_STATUS:
            if (!DG_CMN_DRV_SSD_get_status(ssd, &status))
            {
                DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                        "Failed to Get SSD status");
            }
            else
            {
                if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, sizeof(status)))
                {
                    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                    DG_ENGINE_UTIL_rsp_append_data_hton(rsp, status);
                }
            }
            break;

        case DG_SSD_ACTION_GET_MODEL:
            if (!DG_CMN_DRV_SSD_get_model(ssd, &str))
            {
                DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                        "Failed to Get SSD Model Number");
            }
            else
            {
                len = (UINT32)strlen(str) + 1;
                if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, len))
                {
                    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                    DG_ENGINE_UTIL_rsp_append_buf(rsp, (UINT8*)str, len);
                }
                free(str);
            }
            break;

        case DG_SSD_ACTION_GET_SERIAL:
            if (!DG_CMN_DRV_SSD_get_serial(ssd, &str))
            {
                DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                        "Failed to Get SSD Serial Number");
            }
            else
            {
                len = (UINT32)strlen(str) + 1;
                if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, len))
                {
                    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                    DG_ENGINE_UTIL_rsp_append_buf(rsp, (UINT8*)str, len);
                }
                free(str);
            }
            break;

        case DG_SSD_ACTION_GET_CAPACITY:
            if (!DG_CMN_DRV_SSD_get_capacity(ssd, &size))
            {
                DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                        "Failed to Get SSD size");
            }
            else
            {
                if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, sizeof(size)))
                {
                    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                    DG_ENGINE_UTIL_rsp_append_data_hton(rsp, size);
                }
            }
            break;

        case DG_SSD_ACTION_GET_CACHE_SIZE:
            if (!DG_CMN_DRV_SSD_get_cache_size(ssd, &size))
            {
                DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                        "Failed to Get SSD cache/buffer size");
            }
            else
            {
                if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, sizeof(size)))
                {
                    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                    DG_ENGINE_UTIL_rsp_append_data_hton(rsp, size);
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

