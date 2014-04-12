/*==================================================================================================

    Module Name:  dg_loop_cfg.c

    General Description: Implements the dg_loop_cfg utility

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include "dg_platform_defs.h"
#include "dg_dbg.h"
#include "dg_loop.h"
#include "dg_loop_priv.h"

/** @addtogroup libdg_loop
@{
*/
/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/
#define DG_LOOP_NODE_NUM 4

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/** node config function type */
typedef BOOL (* DG_LOOP_CONFIG_FUNC_T) (DG_LOOP_PORT_T port, DG_LOOP_NODE_T node, DG_LOOP_CFG_T cfg);

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static BOOL cfg_mgt_mac(DG_LOOP_PORT_T port, DG_LOOP_NODE_T node, DG_LOOP_CFG_T cfg);
static BOOL cfg_mgt_phy(DG_LOOP_PORT_T port, DG_LOOP_NODE_T node, DG_LOOP_CFG_T cfg);
static BOOL cfg_mgt_hdr(DG_LOOP_PORT_T port, DG_LOOP_NODE_T node, DG_LOOP_CFG_T cfg);

/*==================================================================================================
                                         GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/

/** configuration map */
static DG_LOOP_CFG_T dg_loop_port_cfg[DG_LOOP_PORT_NUM][DG_LOOP_NODE_NUM] =
{
    { DG_LOOP_CFG_NORMAL }
};

/** normal node configuration */
static DG_LOOP_CONFIG_FUNC_T dg_loop_cfg_normal_func[DG_LOOP_PORT_NUM][DG_LOOP_NODE_NUM] =
{
    /*===========================================node=============================================
      FPGA,          MAC,           PHY,           HDR                 Port
      ============================================================================================*/
    { NULL,          cfg_mgt_mac,   cfg_mgt_phy,   cfg_mgt_hdr  }, /* DG_LOOP_PORT_MGT    */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_HA     */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_WTB0_1 */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_WTB0_2 */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_WTB1_1 */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_WTB1_2 */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_0   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_1   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_2   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_3   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_4   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_5   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_6   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_7   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_8   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_9   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_10  */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_11  */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_SFP_0  */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_SFP_1  */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_SFP_2  */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_SFP_3  */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_10GE_0 */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_10GE_1 */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_10GE_2 */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_10GE_3 */
};

/** normal node configuration */
static DG_LOOP_CONFIG_FUNC_T dg_loop_cfg_internal_func[DG_LOOP_PORT_NUM][DG_LOOP_NODE_NUM] =
{
    /*===========================================node=============================================
      FPGA,          MAC,           PHY,           HDR                 Port
      ============================================================================================*/
    { NULL,          cfg_mgt_mac,   cfg_mgt_phy,   cfg_mgt_hdr  }, /* DG_LOOP_PORT_MGT    */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_HA     */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_WTB0_1 */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_WTB0_2 */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_WTB1_1 */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_WTB1_2 */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_0   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_1   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_2   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_3   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_4   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_5   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_6   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_7   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_8   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_9   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_10  */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_11  */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_SFP_0  */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_SFP_1  */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_SFP_2  */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_SFP_3  */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_10GE_0 */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_10GE_1 */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_10GE_2 */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_10GE_3 */
};

/** normal node configuration */
static DG_LOOP_CONFIG_FUNC_T dg_loop_cfg_external_func[DG_LOOP_PORT_NUM][DG_LOOP_NODE_NUM] =
{
    /*===========================================node=============================================
      FPGA,          MAC,           PHY,           HDR                 Port
      ============================================================================================*/
    { NULL,          cfg_mgt_mac,   cfg_mgt_phy,   cfg_mgt_hdr  }, /* DG_LOOP_PORT_MGT    */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_HA     */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_WTB0_1 */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_WTB0_2 */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_WTB1_1 */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_WTB1_2 */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_0   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_1   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_2   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_3   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_4   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_5   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_6   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_7   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_8   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_9   */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_10  */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_GE_11  */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_SFP_0  */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_SFP_1  */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_SFP_2  */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_SFP_3  */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_10GE_0 */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_10GE_1 */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_10GE_2 */
    { NULL,          NULL,          NULL,          NULL         }, /* DG_LOOP_PORT_10GE_3 */
};

/*==================================================================================================
                                         GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief loopback node configuration

@param[in]  port    - the path to config on which port
@param[in]  node    - where to loopback packet
@param[in]  cfg     - configuration type

@return TRUE if success

@note
- if the port doesn't contains the node or doesn't support the configuration, FALSE will be returned
- if error happened, call DG_DBG_get_err_string() to get the last error
*//*==============================================================================================*/
BOOL DG_LOOP_config(DG_LOOP_PORT_T port, DG_LOOP_NODE_T node, DG_LOOP_CFG_T cfg)
{
    BOOL ret = FALSE;

    DG_LOOP_CONFIG_FUNC_T cfg_f = NULL;
    /* convert port id to index */
    int index = DG_LOOP_check_port(port);

    if (index < 0)
    {
        DG_DBG_set_err_string("Invalid port selection, port=0x%02x", port);
        return FALSE;
    }

    if (node > DG_LOOP_NODE_HDR)
    {
        DG_DBG_set_err_string("Invalid node selection, node=%d", node);
        return FALSE;
    }

    if (cfg > DG_LOOP_CFG_EXTERNAL)
    {
        DG_DBG_set_err_string("Invalid configuration, cfg=%d", cfg);
        return FALSE;
    }

    DG_DBG_TRACE("DG_LOOP_config() port=0x%02x, node=%d, cfg=%d", port, node, cfg);

    if (cfg == DG_LOOP_CFG_EXTERNAL)
    {
        cfg_f = dg_loop_cfg_external_func[index][node];
    }
    else if (cfg == DG_LOOP_CFG_INTERNAL)
    {
        cfg_f = dg_loop_cfg_internal_func[index][node];
    }
    else
    {
        cfg_f = dg_loop_cfg_normal_func[index][node];
    }

    if (cfg_f == NULL)
    {
        DG_DBG_set_err_string("the config not supported: port=0x%02x, node=%d, cfg=%d",
                              port, node, cfg);

        return FALSE;
    }

    ret = cfg_f(port, node, cfg);

    if (ret)
    {
        /* save the cfg map for reverting */
        dg_loop_port_cfg[index][node] = cfg;
    }

    return ret;
}

/*=============================================================================================*//**
@brief configurate all the port node to normal mode

@return TRUE if success

@note
- if error happened, call DG_DBG_get_err_string() to get the last error
*//*==============================================================================================*/
BOOL DG_LOOP_config_all_normal()
{
    BOOL           ret = TRUE;
    int            index;
    int            node;
    DG_LOOP_PORT_T port;

    for (index = 0; index < DG_LOOP_PORT_NUM; index++)
    {
        for (node = 0; node < DG_LOOP_NODE_NUM; node++)
        {
            if (dg_loop_port_cfg[index][node] != DG_LOOP_CFG_NORMAL)
            {
                port = dg_loop_index_to_port(index);
                /* revert back to normal */
                if (!DG_LOOP_config(port, node, DG_LOOP_CFG_NORMAL))
                {
                    ret = FALSE;
                }
            }
        }
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief MGT MAC config
*//*==============================================================================================*/
BOOL cfg_mgt_mac(DG_LOOP_PORT_T port, DG_LOOP_NODE_T node, DG_LOOP_CFG_T cfg)
{
    BOOL ret = FALSE;

    DG_COMPILE_UNUSED(port);
    DG_COMPILE_UNUSED(node);

    if (cfg == DG_LOOP_CFG_INTERNAL)
    {
        DG_DBG_TRACE("config MGT MAC to internal mode");
        ret = TRUE;
    }
    else if (cfg == DG_LOOP_CFG_EXTERNAL)
    {
        DG_DBG_set_err_string("MGT MAC doesn't support external mode");
    }
    else
    {
        DG_DBG_TRACE("config MGT MAC to normal mode");
        ret = TRUE;
    }

    return ret;
}

/*=============================================================================================*//**
@brief MGT PHY config
*//*==============================================================================================*/
BOOL cfg_mgt_phy(DG_LOOP_PORT_T port, DG_LOOP_NODE_T node, DG_LOOP_CFG_T cfg)
{
    BOOL ret = FALSE;

    DG_COMPILE_UNUSED(port);
    DG_COMPILE_UNUSED(node);

    if (cfg == DG_LOOP_CFG_INTERNAL)
    {
        DG_DBG_TRACE("config MGT PHY to internal mode");
        ret = TRUE;
    }
    else if (cfg == DG_LOOP_CFG_EXTERNAL)
    {
        DG_DBG_TRACE("config MGT PHY to external mode");
        ret = TRUE;
    }
    else
    {
        DG_DBG_TRACE("config MGT PHY to normal mode");
        ret = TRUE;
    }

    return ret;
}

/*=============================================================================================*//**
@brief MGT HDR config
*//*==============================================================================================*/
BOOL cfg_mgt_hdr(DG_LOOP_PORT_T port, DG_LOOP_NODE_T node, DG_LOOP_CFG_T cfg)
{
    BOOL ret = FALSE;

    DG_COMPILE_UNUSED(port);
    DG_COMPILE_UNUSED(node);

    if (cfg == DG_LOOP_CFG_INTERNAL)
    {
        DG_DBG_TRACE("config MGT HDR to internal mode");
        ret = TRUE;
    }
    else if (cfg == DG_LOOP_CFG_EXTERNAL)
    {
        DG_DBG_set_err_string("MGT HDR doesn't support external mode");
    }
    else
    {
        DG_DBG_TRACE("config MGT HDR to normal mode");
        ret = TRUE;
    }

    return ret;
}

/** @} */

