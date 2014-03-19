/*==================================================================================================

    Module Name:  dg_cmn_drv_sfp.c

    General Description: Implements the SFP common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_sfp.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup SFP_driver
@{
implementation of the SFP driver
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
@brief Get the SFP status

@param[in]  sfp    - selected SFP
@param[out] status - The SFP status

*//*==============================================================================================*/
BOOL DG_CMN_DRV_SFP_get_status(DG_CMN_DRV_SFP_ID_T sfp, DG_CMN_DRV_SFP_STATUS_T* status)
{
    BOOL ret = FALSE;

    if (sfp > DG_CMN_DRV_SFP_ID_MAX)
    {
        DG_DRV_UTIL_set_error_string("Invalid sfp=%d", sfp);
    }
    else
    {
        *status = 0;

        DG_DBG_TRACE("SFP ID %d got status: %d", sfp, (UINT8)*status);

        ret = TRUE;
    }

    return ret;
}


/*=============================================================================================*//**
@brief Configure SFP TX

@param[in] sfp - selected SFP
@param[in] cfg - new configuration of the SFP TX

*//*==============================================================================================*/
BOOL DG_CMN_DRV_SFP_tx_config(DG_CMN_DRV_SFP_ID_T sfp, DG_CMN_DRV_SFP_TX_CFG_T cfg)
{
    BOOL ret = FALSE;

    if (sfp > DG_CMN_DRV_SFP_ID_MAX)
    {
        DG_DRV_UTIL_set_error_string("Invalid sfp=%d", sfp);
    }
    else if ((cfg != DG_CMN_DRV_SFP_TX_DISABLE) && (cfg != DG_CMN_DRV_SFP_TX_ENABLE))
    {
        DG_DRV_UTIL_set_error_string("SFP TX config is not valid. cfg=%d", cfg);
    }
    else
    {
        DG_DBG_TRACE("SFP ID %d TX config: %s", sfp,
                     cfg == DG_CMN_DRV_SFP_TX_ENABLE ? "tx enable" : "tx disable");

        ret = TRUE;
    }

    return ret;
}

/*=============================================================================================*//**
@brief Dump the SFP registers

@param[in]  sfp  - selected SFP
@param[in]  reg  - which register set to dump
@param[out] data - the dumped register data

*//*==============================================================================================*/
BOOL DG_CMN_DRV_SFP_dump_register(DG_CMN_DRV_SFP_ID_T        sfp,
                                  DG_CMN_DRV_SFP_REG_T       reg,
                                  DG_CMN_DRV_SFP_REG_DATA_T* data)
{
    BOOL ret = FALSE;

    if (sfp > DG_CMN_DRV_SFP_ID_MAX)
    {
        DG_DRV_UTIL_set_error_string("Invalid sfp=%d", sfp);
    }
    else if ((reg != DG_CMN_DRV_SFP_REG_A0) && (reg != DG_CMN_DRV_SFP_REG_A2))
    {
        DG_DRV_UTIL_set_error_string("SFP register set is not valid. reg=%d", reg);
    }
    else
    {
        memset(data->reg, reg, sizeof(data->reg));

        DG_DBG_TRACE("SFP ID %d dump %s registers", sfp,
                     reg == DG_CMN_DRV_SFP_REG_A0 ? "A0" : "A2");

        DG_DBG_DUMP(data->reg, sizeof(data->reg));

        ret = TRUE;
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

