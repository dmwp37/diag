/*==================================================================================================

    Module Name:  dg_bios.c

    General Description: Implements the BIOS test handler

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include "dg_handler_inc.h"
#include "dg_cmn_drv_bios.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup BIOS
@{

@par
<b>BIOS - 0x0015</b>

@par
Allows generic access to the BIOS.
*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/


/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** Actions for BIOS command */
enum
{
    DG_BIOS_ACTION_READ_INFO    = 0x00,
    DG_BIOS_ACTION_READ_STATUS  = 0x01,
    DG_BIOS_ACTION_WRITE_STATUS = 0x02,
    DG_BIOS_ACTION_GET_CHECKSUM = 0x03,
    DG_BIOS_ACTION_ATTACK       = 0x04,
    DG_BIOS_ACTION_SET_SRC      = 0x05,
    DG_BIOS_ACTION_GET_SRC      = 0x06
};
typedef UINT8 DG_BIOS_ACTION_T;

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/
static const UINT32 DG_BIOS_INFO_LEN = sizeof(((DG_CMN_DRV_BIOS_INFO_T*)NULL)->mfg) +
                                       sizeof(((DG_CMN_DRV_BIOS_INFO_T*)NULL)->dev) +
                                       sizeof(((DG_CMN_DRV_BIOS_INFO_T*)NULL)->edi_len) +
                                       sizeof(((DG_CMN_DRV_BIOS_INFO_T*)NULL)->edi_b1);

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
@brief Handler function for the BIOS command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_BIOS_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_BIOS_ACTION_T            action;
    DG_CMN_DRV_BIOS_ID_T        bios_id;
    DG_DEFS_DIAG_RSP_BUILDER_T* rsp = DG_ENGINE_UTIL_rsp_init();
    DG_CMN_DRV_BIOS_INFO_T      bios_info;
    DG_CMN_DRV_BIOS_REG_T       bios_reg;
    DG_CMN_DRV_BIOS_SHA1_T      sha1;

    /* Verify action parameter was given */
    DG_DBG_TRACE("In DG_BIOS_handler_main begin to parse Request");
    if (DG_ENGINE_UTIL_req_len_check_at_least(req, sizeof(action), rsp))
    {
        /* Parse and switch on action */
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, action);
        DG_DBG_TRACE("action=0x%02x", action);

        switch (action)
        {
        case DG_BIOS_ACTION_READ_INFO:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(bios_id), rsp))
            {
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, bios_id);
                if (!DG_CMN_DRV_BIOS_read_info(bios_id, &bios_info))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to Read BIOS information");
                }
                else
                {
                    if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, DG_BIOS_INFO_LEN))
                    {
                        DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                        DG_ENGINE_UTIL_rsp_append_data_hton(rsp, bios_info.mfg);
                        DG_ENGINE_UTIL_rsp_append_data_hton(rsp, bios_info.dev);
                        DG_ENGINE_UTIL_rsp_append_data_hton(rsp, bios_info.edi_len);
                        DG_ENGINE_UTIL_rsp_append_data_hton(rsp, bios_info.edi_b1);
                    }
                }
            }
            break;

        case DG_BIOS_ACTION_READ_STATUS:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(bios_id), rsp))
            {
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, bios_id);
                if (!DG_CMN_DRV_BIOS_read_status(bios_id, &bios_reg))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to Read BIOS status register");
                }
                else
                {
                    if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, sizeof(bios_reg)))
                    {
                        DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                        DG_ENGINE_UTIL_rsp_append_data_hton(rsp, bios_reg);
                    }
                }
            }
            break;

        case DG_BIOS_ACTION_WRITE_STATUS:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(bios_id) + sizeof(bios_reg),
                                                          rsp))
            {
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, bios_id);
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, bios_reg);
                if (!DG_CMN_DRV_BIOS_write_status(bios_id, bios_reg))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to Write BIOS status register");
                }
                else
                {
                    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                }
            }
            break;

        case DG_BIOS_ACTION_GET_CHECKSUM:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(bios_id), rsp))
            {
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, bios_id);
                if (!DG_CMN_DRV_BIOS_get_checksum(bios_id, &sha1))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to Get BIOS check sum");
                }
                else
                {
                    if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, sizeof(sha1.value)))
                    {
                        DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                        DG_ENGINE_UTIL_rsp_append_buf(rsp, sha1.value, sizeof(sha1.value));
                    }
                }
            }
            break;

        case DG_BIOS_ACTION_ATTACK:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(bios_id), rsp))
            {
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, bios_id);
                if (!DG_CMN_DRV_BIOS_attack(bios_id))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to Attack BIOS");
                }
                else
                {
                    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                }
            }
            break;

        case DG_BIOS_ACTION_SET_SRC:
            if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, sizeof(bios_id), rsp))
            {
                DG_ENGINE_UTIL_req_parse_data_ntoh(req, bios_id);
                if (!DG_CMN_DRV_BIOS_set_src(bios_id))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to Set BIOS boot source");
                }
                else
                {
                    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                }
            }
            break;

        case DG_BIOS_ACTION_GET_SRC:
            if (!DG_CMN_DRV_BIOS_get_src(&bios_id))
            {
                DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                        "Failed to Get BIOS boot source");
            }
            else
            {
                DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
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

