/*==================================================================================================

    Module Name:  dg_version.c

    General Description: Implements the VERSION test handler

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include "dg_handler_inc.h"
#include "dg_cmn_drv_version.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup VERSION
@{

@par
<b>VERSION - 0x0000</b>

@par
This command is responsible for VERSION test
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
const UINT32 DG_VERSION_REQ_LEN = sizeof(DG_CMN_DRV_VERSION_TYPE_T);

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
@brief Handler function for the VERSION command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_VERSION_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_CMN_DRV_VERSION_TYPE_T   type;
    DG_DEFS_DIAG_RSP_BUILDER_T* rsp = DG_ENGINE_UTIL_rsp_init();

    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);

    if (DG_ENGINE_UTIL_req_len_check_equal(req, DG_VERSION_REQ_LEN, rsp))
    {
        char* p_version_str = NULL;

        DG_ENGINE_UTIL_req_parse_data_ntoh(req, type);

        if (!DG_CMN_DRV_VERSION_get(type, &p_version_str))
        {
            DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                    "Failed to Get version string");
        }
        else
        {
            UINT32 str_len = (UINT32)strlen(p_version_str) + 1;
            if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, str_len))
            {
                DG_ENGINE_UTIL_rsp_append_buf(rsp, (UINT8*)p_version_str, str_len);
            }
        }

        free(p_version_str);
    }

    DG_ENGINE_UTIL_rsp_send(rsp, req);
    DG_ENGINE_UTIL_rsp_free(rsp);
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

