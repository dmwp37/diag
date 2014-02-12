/*==================================================================================================

    Module Name:  dg_pci.c

    General Description: Implements the PCI test handler

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include "dg_handler_inc.h"
#include "dg_cmn_drv_pci.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup PCI
@{

@par
<b>PCI - 0x0013</b>

@par
Allows generic read access to the PCI bus config and memory space.
*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** Actions for PCI command */
enum
{
    DG_PCI_ACTION_CFG_READ = 0x00,
    DG_PCI_ACTION_MEM_READ = 0x01,
};
typedef UINT8 DG_PCI_ACTION_T;

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/
static const UINT32 DG_PCI_REQ_LEN_MIN = sizeof(DG_CMN_DRV_PCI_DOMAIN_T) +
                                         sizeof(DG_CMN_DRV_PCI_BUS_T) +
                                         sizeof(DG_CMN_DRV_PCI_DEV_T) +
                                         sizeof(DG_CMN_DRV_PCI_FUNC_T) +
                                         sizeof(DG_CMN_DRV_PCI_OFFSET_T) +
                                         sizeof(DG_CMN_DRV_PCI_LEN_T);

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
@brief Handler function for the PCI command

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_PCI_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_PCI_ACTION_T             action;
    DG_CMN_DRV_PCI_DOMAIN_T     domain;
    DG_CMN_DRV_PCI_BUS_T        bus;
    DG_CMN_DRV_PCI_DEV_T        dev;
    DG_CMN_DRV_PCI_FUNC_T       func;
    DG_CMN_DRV_PCI_OFFSET_T     offset;
    DG_CMN_DRV_PCI_LEN_T        len;
    DG_DEFS_DIAG_RSP_BUILDER_T* rsp       = DG_ENGINE_UTIL_rsp_init();
    UINT8*                      read_data = NULL;

    /* Verify action parameter was given */
    DG_DBG_TRACE("In DG_PCI_handler_main begin to parse Request");
    if (DG_ENGINE_UTIL_req_len_check_at_least(req, DG_PCI_REQ_LEN_MIN, rsp))
    {
        /* Parse and switch on action */
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, action);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, domain);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, bus);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, dev);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, func);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, offset);
        DG_ENGINE_UTIL_req_parse_data_ntoh(req, len);

        DG_DBG_TRACE("action=0x%02x, domain=0x%02x, bus=0x%02x, dev=0x%02x,"
                     "offset=0x%08x, len=0x%08x",
                     action, domain, bus, dev, func, offset, len);

        switch (action)
        {
        case DG_PCI_ACTION_CFG_READ:
            read_data = (UINT8*)DG_ENGINE_UTIL_alloc_mem(len, rsp);
            if (read_data != NULL)
            {
                if (!DG_CMN_DRV_PCI_cfg_read(domain, bus, dev, func, offset, len, read_data))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to Read PCI config register");
                }
                else
                {
                    /* Allocate memory for read data */
                    if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, len))
                    {
                        DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                        DG_ENGINE_UTIL_rsp_append_buf(rsp, read_data, len);
                    }
                }

                free(read_data);
            }
            break;

        case DG_PCI_ACTION_MEM_READ:
            read_data = (UINT8*)DG_ENGINE_UTIL_alloc_mem(len, rsp);
            if (read_data != NULL)
            {
                if (!DG_CMN_DRV_PCI_mem_read(domain, bus, dev, func, offset, len, read_data))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string_drv(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                            "Failed to Read PCI memory register");
                }
                else
                {
                    /* Allocate memory for read data */
                    if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, len))
                    {
                        DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                        DG_ENGINE_UTIL_rsp_append_buf(rsp, read_data, len);
                    }
                }

                free(read_data);
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

