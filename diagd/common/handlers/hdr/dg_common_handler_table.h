#ifndef _DG_COMMON_HANDLER_TABLE_H
#define _DG_COMMON_HANDLER_TABLE_H
/*==================================================================================================

    Module Name:  dg_common_handler_table.h

    General Description:  Header file for all common handler function prototypes

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_defs.h"


#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                              MACROS
==================================================================================================*/

/*==================================================================================================
                                               ENUMS
==================================================================================================*/

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/

/*==================================================================================================
                                   GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/
/* Important Note: Please add all handler functions to this list in alphabetical order.  All
   functions must have the name format/prototype of
   DG_<DIAG NAME>_handler_main(DG_DEFS_DIAG_REQ_T *req) */

void DG_BIOS_handler_main(DG_DEFS_DIAG_REQ_T* req);
void DG_BUTTON_handler_main(DG_DEFS_DIAG_REQ_T* req);
void DG_CLOCK_handler_main(DG_DEFS_DIAG_REQ_T* req);
void DG_DEBUG_LEVEL_handler_main(DG_DEFS_DIAG_REQ_T* req);
void DG_DIMM_handler_main(DG_DEFS_DIAG_REQ_T* req);
void DG_EEPROM_handler_main(DG_DEFS_DIAG_REQ_T* req);
void DG_EXT_LOOP_handler_main(DG_DEFS_DIAG_REQ_T* req);
void DG_FAN_handler_main(DG_DEFS_DIAG_REQ_T* req);
void DG_FPGA_handler_main(DG_DEFS_DIAG_REQ_T* req);
void DG_GPIO_handler_main(DG_DEFS_DIAG_REQ_T* req);
void DG_I2C_handler_main(DG_DEFS_DIAG_REQ_T* req);
void DG_INT_LOOP_handler_main(DG_DEFS_DIAG_REQ_T* req);
void DG_LED_handler_main(DG_DEFS_DIAG_REQ_T* req);
void DG_PCI_handler_main(DG_DEFS_DIAG_REQ_T* req);
void DG_PHY_handler_main(DG_DEFS_DIAG_REQ_T* req);
void DG_PING_handler_main(DG_DEFS_DIAG_REQ_T* req);
void DG_PRBS_handler_main(DG_DEFS_DIAG_REQ_T* req);
void DG_PTTM_handler_main(DG_DEFS_DIAG_REQ_T* req);
void DG_RESET_handler_main(DG_DEFS_DIAG_REQ_T* req);
void DG_RTC_handler_main(DG_DEFS_DIAG_REQ_T* req);
void DG_SSD_handler_main(DG_DEFS_DIAG_REQ_T* req);
void DG_SUSPEND_handler_main(DG_DEFS_DIAG_REQ_T* req);
void DG_TEMP_handler_main(DG_DEFS_DIAG_REQ_T* req);
void DG_TEST_ENGINE_handler_main(DG_DEFS_DIAG_REQ_T* req);
void DG_TPM_handler_main(DG_DEFS_DIAG_REQ_T* req);
void DG_USB_handler_main(DG_DEFS_DIAG_REQ_T* req);
void DG_VERSION_handler_main(DG_DEFS_DIAG_REQ_T* req);
void DG_VOLTAGE_handler_main(DG_DEFS_DIAG_REQ_T* req);

#ifdef __cplusplus
}
#endif

#endif /* _DG_COMMON_HANDLER_TABLE_H */

