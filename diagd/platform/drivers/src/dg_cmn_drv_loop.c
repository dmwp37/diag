/*==================================================================================================

    Module Name:  dg_cmn_drv_loop.c

    General Description: Implements the normal loopback common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_loop.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup LOOP_driver
@{
implementation of the normal loopback driver
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
static BOOL dg_cmn_drv_loop_check_port(DG_CMN_DRV_LOOP_PORT_T port);

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
@brief Normal loopback test

@param[in] tx_port - the tx port selection
@param[in] rx_port - the rx port selection
@param[in] size    - the loopback packet size
@param[in] num     - how many loopback packet to send
@param[in] pattern - content of each byte in the packet data field

*//*==============================================================================================*/
BOOL DG_CMN_DRV_LOOP_test(DG_CMN_DRV_LOOP_PORT_T           tx_port,
                          DG_CMN_DRV_LOOP_PORT_T           rx_port,
                          DG_CMN_DRV_LOOP_PACKET_SIZE_T    size,
                          DG_CMN_DRV_LOOP_PACKET_NUM_T     num,
                          DG_CMN_DRV_LOOP_PACKET_PATTERN_T pattern)
{
    BOOL ret = FALSE;
    if (dg_cmn_drv_loop_check_port(tx_port) && dg_cmn_drv_loop_check_port(rx_port))
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

/*=============================================================================================*//**
@brief check Normal loopback port

@param[in] port    - the loopback port

*//*==============================================================================================*/
BOOL dg_cmn_drv_loop_check_port(DG_CMN_DRV_LOOP_PORT_T port)
{
    switch (port)
    {
    case DG_CMN_DRV_LOOP_PORT_MGT:
    case DG_CMN_DRV_LOOP_PORT_HA:
    case DG_CMN_DRV_LOOP_PORT_WTB0_1:
    case DG_CMN_DRV_LOOP_PORT_WTB0_2:
    case DG_CMN_DRV_LOOP_PORT_WTB1_1:
    case DG_CMN_DRV_LOOP_PORT_WTB1_2:
    case DG_CMN_DRV_LOOP_PORT_GE_0:
    case DG_CMN_DRV_LOOP_PORT_GE_1:
    case DG_CMN_DRV_LOOP_PORT_GE_2:
    case DG_CMN_DRV_LOOP_PORT_GE_3:
    case DG_CMN_DRV_LOOP_PORT_GE_4:
    case DG_CMN_DRV_LOOP_PORT_GE_5:
    case DG_CMN_DRV_LOOP_PORT_GE_6:
    case DG_CMN_DRV_LOOP_PORT_GE_7:
    case DG_CMN_DRV_LOOP_PORT_GE_8:
    case DG_CMN_DRV_LOOP_PORT_GE_9:
    case DG_CMN_DRV_LOOP_PORT_GE_10:
    case DG_CMN_DRV_LOOP_PORT_GE_11:
    case DG_CMN_DRV_LOOP_PORT_SFP_0:
    case DG_CMN_DRV_LOOP_PORT_SFP_1:
    case DG_CMN_DRV_LOOP_PORT_SFP_2:
    case DG_CMN_DRV_LOOP_PORT_SFP_3:
    case DG_CMN_DRV_LOOP_PORT_10GE_0:
    case DG_CMN_DRV_LOOP_PORT_10GE_1:
    case DG_CMN_DRV_LOOP_PORT_10GE_2:
    case DG_CMN_DRV_LOOP_PORT_10GE_3:
        break;

    default:
        DG_DRV_UTIL_set_error_string("Invalid Port selection, port=0x%02x", port);
        return FALSE;
    }

    return TRUE;
}
/** @} */
/** @} */

