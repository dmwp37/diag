/*==================================================================================================

    Module Name:  dg_eeprom.c

    General Description: Implements the EEPROM test handler

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_cmn_drv_eeprom.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup EEPROM
@{

@par
<b>EEPROM - 0x0004</b>

@par
EEPROM READ/WRITE access.
*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** Actions for EEPROM command */
enum
{
    DG_EEPROM_ACTION_READ  = 0x00,
    DG_EEPROM_ACTION_WRITE = 0x01,
};
typedef UINT8 DG_EEPROM_ACTION_T;

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static void dg_eeprom_read(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp);
static void dg_eeprom_write(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp);

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
@brief Handler function for the EEPROM command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_EEPROM_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_EEPROM_ACTION_T          action;
    DG_DEFS_DIAG_RSP_BUILDER_T* rsp = DG_ENGINE_UTIL_rsp_init();

    DG_DBG_TRACE("In DG_EEPROM_handler_main begin to parse Request");
    if (DG_ENGINE_UTIL_req_len_check_at_least(req, sizeof(action), rsp))
    {
        /* Parse and switch on action */
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, action);

        DG_DBG_TRACE("action=0x%02x", action);

        switch (action)
        {
        case DG_EEPROM_ACTION_READ:
            dg_eeprom_read(req, rsp);
            break;

        case DG_EEPROM_ACTION_WRITE:
            dg_eeprom_write(req, rsp);
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
@brief EEPROM read

@param[in]     req     - DIAG request
@param[in,out] rsp     - DIAG rsp builder

*//*==============================================================================================*/
void dg_eeprom_read(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    DG_CMN_DRV_EEPROM_ID_T   eeprom;
    DG_CMN_DRV_EEPROM_ADDR_T addr;
    UINT8                    data;

    const UINT32 req_size = sizeof(eeprom) + sizeof(addr);
    if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, req_size, rsp))
    {
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, eeprom);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, addr);

        if (!DG_CMN_DRV_EEPROM_read(eeprom, addr, &data))
        {
            DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                    "Failed to Read EEPROM");
        }
        else
        {
            /* Allocate memory for read data */
            if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, sizeof(data)))
            {
                DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                DG_ENGINE_UTIL_rsp_append_data_hton(rsp, data);
            }
        }
    }
}

/*=============================================================================================*//**
@brief EEPROM write

@param[in]     req     - DIAG request
@param[in,out] rsp     - DIAG rsp builder

*//*==============================================================================================*/
void dg_eeprom_write(DG_DEFS_DIAG_REQ_T* req, DG_DEFS_DIAG_RSP_BUILDER_T* rsp)
{
    DG_CMN_DRV_EEPROM_ID_T   eeprom;
    DG_CMN_DRV_EEPROM_ADDR_T addr;
    UINT8                    data;

    const UINT32 req_size = sizeof(eeprom) + sizeof(addr) + sizeof(data);
    if (DG_ENGINE_UTIL_req_remain_len_check_equal(req, req_size, rsp))
    {
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, eeprom);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, addr);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, data);

        if (!DG_CMN_DRV_EEPROM_write(eeprom, addr, data))
        {
            DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                    "Failed to Write EEPROM");
        }
        else
        {
            DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
        }
    }
}

/** @} */
/** @} */

