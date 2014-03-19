/*==================================================================================================

    Module Name:  dg_cmn_drv_int_loop.c

    General Description: Implements the INT_LOOP common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_int_loop.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup INT_LOOP_driver
@{
implementation of the INT_LOOP driver
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
static BOOL dg_cmn_drv_int_loop_check_para(DG_CMN_DRV_INT_LOOP_PORT_T port,
                                           DG_CMN_DRV_INT_LOOP_NODE_T node,
                                           DG_CMN_DRV_INT_LOOP_CFG_T  cfg);

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
@brief Internal loopback test

@param[in] port    - the path to test on which port
@param[in] node    - where to loopback packet
@param[in] cfg     - port configuration
@param[in] size    - the loopback packet size
@param[in] num     - how many loopback packet to send
@param[in] pattern - content of each byte in the packet data field

*//*==============================================================================================*/
BOOL DG_CMN_DRV_INT_LOOP_test(DG_CMN_DRV_INT_LOOP_PORT_T           port,
                              DG_CMN_DRV_INT_LOOP_NODE_T           node,
                              DG_CMN_DRV_INT_LOOP_CFG_T            cfg,
                              DG_CMN_DRV_INT_LOOP_PACKET_SIZE_T    size,
                              DG_CMN_DRV_INT_LOOP_PACKET_NUM_T     num,
                              DG_CMN_DRV_INT_LOOP_PACKET_PATTERN_T pattern)
{
    BOOL ret = FALSE;
    if (dg_cmn_drv_int_loop_check_para(port, node, cfg))
    {
        /* TODO: the function should be implemented in the REAL hardware */
        DG_DBG_TRACE("Successfully receive %d packets, pattern=0x%02x, packet_size=%d",
                     num, pattern, size);
        ret = TRUE;
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

BOOL dg_cmn_drv_int_loop_check_para(DG_CMN_DRV_INT_LOOP_PORT_T port,
                                    DG_CMN_DRV_INT_LOOP_NODE_T node,
                                    DG_CMN_DRV_INT_LOOP_CFG_T  cfg)
{
    switch (port)
    {
    case DG_CMN_DRV_INT_LOOP_PORT_ALL:
    case DG_CMN_DRV_INT_LOOP_PORT_MGT:
    case DG_CMN_DRV_INT_LOOP_PORT_HA:
    case DG_CMN_DRV_INT_LOOP_PORT_WTB0_1:
    case DG_CMN_DRV_INT_LOOP_PORT_WTB0_2:
    case DG_CMN_DRV_INT_LOOP_PORT_WTB1_1:
    case DG_CMN_DRV_INT_LOOP_PORT_WTB1_2:
    case DG_CMN_DRV_INT_LOOP_PORT_GE_0:
    case DG_CMN_DRV_INT_LOOP_PORT_GE_1:
    case DG_CMN_DRV_INT_LOOP_PORT_GE_2:
    case DG_CMN_DRV_INT_LOOP_PORT_GE_3:
    case DG_CMN_DRV_INT_LOOP_PORT_GE_4:
    case DG_CMN_DRV_INT_LOOP_PORT_GE_5:
    case DG_CMN_DRV_INT_LOOP_PORT_GE_6:
    case DG_CMN_DRV_INT_LOOP_PORT_GE_7:
    case DG_CMN_DRV_INT_LOOP_PORT_GE_8:
    case DG_CMN_DRV_INT_LOOP_PORT_GE_9:
    case DG_CMN_DRV_INT_LOOP_PORT_GE_10:
    case DG_CMN_DRV_INT_LOOP_PORT_GE_11:
    case DG_CMN_DRV_INT_LOOP_PORT_SFP_0:
    case DG_CMN_DRV_INT_LOOP_PORT_SFP_1:
    case DG_CMN_DRV_INT_LOOP_PORT_SFP_2:
    case DG_CMN_DRV_INT_LOOP_PORT_SFP_3:
    case DG_CMN_DRV_INT_LOOP_PORT_10GE_0:
    case DG_CMN_DRV_INT_LOOP_PORT_10GE_1:
    case DG_CMN_DRV_INT_LOOP_PORT_10GE_2:
    case DG_CMN_DRV_INT_LOOP_PORT_10GE_3:
        break;

    default:
        DG_DRV_UTIL_set_error_string("Invalid Port selection, port=0x%02x", port);
        return FALSE;
    }

    switch (node)
    {
    case DG_CMN_DRV_INT_LOOP_NODE_FPGA:
        if ((port == DG_CMN_DRV_INT_LOOP_PORT_MGT) ||
            (port == DG_CMN_DRV_INT_LOOP_PORT_HA) ||
            (port == DG_CMN_DRV_INT_LOOP_PORT_WTB0_1) ||
            (port == DG_CMN_DRV_INT_LOOP_PORT_WTB0_2) ||
            (port == DG_CMN_DRV_INT_LOOP_PORT_WTB1_1) ||
            (port == DG_CMN_DRV_INT_LOOP_PORT_WTB1_2))
        {
            DG_DRV_UTIL_set_error_string("FPGA node is not available for port=0x%02x", port);
            return FALSE;
        }
        break;

    case DG_CMN_DRV_INT_LOOP_NODE_MAC:
    case DG_CMN_DRV_INT_LOOP_NODE_PHY:
    case DG_CMN_DRV_INT_LOOP_NODE_PORT:
        break;

    default:
        DG_DRV_UTIL_set_error_string("Invalid Node selection, node=%d", node);
        return FALSE;
    }

    switch (cfg)
    {
    case DG_CMN_DRV_INT_LOOP_CFG_10M:
    case DG_CMN_DRV_INT_LOOP_CFG_100M:
    case DG_CMN_DRV_INT_LOOP_CFG_1000M:
    case DG_CMN_DRV_INT_LOOP_CFG_10G:
        break;

    default:
        DG_DRV_UTIL_set_error_string("Invalid config selection, config=%d", cfg);
        return FALSE;
    }

    return TRUE;
}

/** @} */
/** @} */

