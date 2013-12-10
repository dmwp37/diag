/*==================================================================================================
 
    Module Name:  dg_ping.c

    General Description: Pings the device
    
====================================================================================================

Revision History:
                            Modification     Tracking
Author                          Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
Xudong Huang    - xudongh    2013/12/11     xxxxx-0000   Creation

====================================================================================================
                                        INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"

/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup PING
@{

@par
<b>PING - 0x0FFF</b>

@par
PING is PING...  This AP version of PING should only be used by products which don't have an aux 
engine
*/

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                            LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                             LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
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
@brief Handler function for the Unique Data Device Backup command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_PING_handler_main(DG_DEFS_DIAG_REQ_T *req)
{  
    DG_DEFS_DIAG_RSP_BUILDER_T *rsp = DG_ENGINE_UTIL_rsp_init(); 

    /* Echo data from request back in DIAG response */
    if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, DG_ENGINE_UTIL_req_get_remain_len(req)))
    {
        DG_ENGINE_UTIL_rsp_append_buf(rsp,
                                      DG_ENGINE_UTIL_req_get_remain_data_ptr(req),
                                      DG_ENGINE_UTIL_req_get_remain_len(req));
        DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
    }
    
    DG_ENGINE_UTIL_rsp_send(rsp, req);
    DG_ENGINE_UTIL_rsp_free(rsp);   
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/


/** @} */
/** @} */
