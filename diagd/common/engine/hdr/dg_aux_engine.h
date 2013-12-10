#ifndef _DG_AUX_ENGINE_H
#define _DG_AUX_ENGINE_H
/*==================================================================================================

    Module Name:  dg_aux_engine.h

    General Description: This file provides an interface to the aux diag engine

====================================================================================================

Revision History:
                            Modification     Tracking
Author                          Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
Xudong Huang    - xudongh    2013/12/11     xxxxx-0000   Creation

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include "dg_defs.h"

#ifdef __cplusplus
extern "C" {
#endif
/*==================================================================================================
                                             CONSTANTS
==================================================================================================*/
/** Defines for aux engine ids. Ids must start at 0 and increment by 1.  There can be no gaps in
    the order */
#define DG_AUX_ENGINE_AUX_ID_AUX1 0
#define DG_AUX_ENGINE_AUX_ID_AUX2 1
#define DG_AUX_ENGINE_AUX_ID_AUX3 2

/** Aux ID used to init/close all aux engines at once */
#define DG_AUX_ENGINE_AUX_ID_ALL  -1
/*==================================================================================================
                                               MACROS
==================================================================================================*/

/*==================================================================================================
                                TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/
/** @addtogroup engine_interface
@{
*/
void DG_AUX_ENGINE_init_available_state(void);
void DG_AUX_ENGINE_init(int aux_id);
void DG_AUX_ENGINE_close(int aux_id);
void DG_AUX_ENGINE_handle_aux_cmd(int aux_id, DG_DEFS_DIAG_REQ_T* diag,
                                  DG_DEFS_DIAG_RSP_BUILDER_T* rsp);
void DG_AUX_ENGINE_handle_aux_cmd_timeout(int                         aux_id,
                                          DG_DEFS_DIAG_REQ_T*         diag,
                                          DG_DEFS_DIAG_RSP_BUILDER_T* rsp,
                                          UINT32                      time_out);
void DG_AUX_ENGINE_handle_aux_cmd_timeout_opt(int aux_id,
                                              DG_DEFS_DIAG_REQ_T* diag,
                                              DG_DEFS_DIAG_RSP_BUILDER_T* rsp,
                                              UINT32 time_out, BOOL* is_enabled);
void DG_AUX_ENGINE_create_and_send_aux_diag(int aux_id, DG_DEFS_OPCODE_T opcode,
                                            UINT32 req_data_len, UINT8* req_data_ptr,
                                            DG_DEFS_DIAG_RSP_BUILDER_T* rsp);
void DG_AUX_ENGINE_create_and_send_aux_diag_opt(int aux_id,
                                                DG_DEFS_OPCODE_T opcode, UINT32 req_data_len,
                                                UINT8* req_data_ptr,
                                                DG_DEFS_DIAG_RSP_BUILDER_T* rsp, BOOL* is_enabled);
BOOL DG_AUX_ENGINE_enable_aux(int aux_id, BOOL set_enable);
BOOL DG_AUX_ENGINE_is_aux_enabled(int aux_id);
BOOL DG_AUX_ENGINE_override_aux(BOOL enable, int aux_id);
int DG_AUX_ENGINE_get_total_enabled_aux(int* first_enabled_aux);

/** @}*/
/*==================================================================================================
                                          GLOBAL VARIABLES
==================================================================================================*/

#ifdef __cplusplus
}
#endif
#endif
