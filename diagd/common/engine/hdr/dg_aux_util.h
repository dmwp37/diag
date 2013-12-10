#ifndef _DG_AUX_UTIL_H
#define _DG_AUX_UTIL_H
/*==================================================================================================

    Module Name:  dg_aux_util.h

    General Description: This file provides an interface to the aux utility functions

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

/*=============================================================================================*//**
@brief Set a BP request into the list to indicate this request needs a response from BP

@param[in] aux_id  - The aux engine for the req
@param[in] opcode  - req opcode
@param[in] seq_tag - req seq_tag

@note
  - the work thread use this API when the BP request need a solicited response
  - the function add one bp request into the list
*//*==============================================================================================*/
void DG_AUX_UTIL_set_bp_req(UINT8 aux_id, DG_DEFS_OPCODE_T opcode, UINT8 seq_tag);

/*=============================================================================================*//**
@brief Get the solicited bp response for the expected request

@param[in] aux_id   - The aux engine for the req
@param[in] opcode   - req opcode
@param[in] seq_tag  - req seq_tag
@param[in] time_out - max timeout value to block

@return the expected solicited response or NULL if error

@note
  - the work thread use this API when the BP request need a solicited response
  - the function seeks the request from the BP req list
  - the function will block until the response comes back or timeout happened
*//*==============================================================================================*/
DG_DEFS_DIAG_RSP_T* DG_AUX_UTIL_get_bp_rsp(UINT8 aux_id, DG_DEFS_OPCODE_T opcode, UINT8 seq_tag,
                                           UINT32 time_out);

/*=============================================================================================*//**
@brief Set the solicited response for the expected request

@param[in] aux_id  - The aux engine for the req
@param[in] opcode  - req opcode
@param[in] seq_tag - req seq_tag
@param[in] rsp     - the solicited bp response

@return TRUE if success

@note
  - the BP listener thread use this API when a solicited response comes
  - this function will tell the work thread that the expected response is ready and let it go
*//*==============================================================================================*/
BOOL DG_AUX_UTIL_set_bp_rsp(UINT8 aux_id, DG_DEFS_OPCODE_T opcode, UINT8 seq_tag,
                            DG_DEFS_DIAG_RSP_T* rsp);

/*=============================================================================================*//**
@brief Cancel the BP request for specified thread

@param[in] pid  - the specified thread ID

@note
  - the monitor thread use this API to cancel it's bp req
  - when timeout happens the engine should call this function
  - this function will tell the work thread that the expected response is ready and let it go
*//*==============================================================================================*/
void DG_AUX_UTIL_cancel_thread_bp_req(pthread_t pid);

/** @}*/
/*==================================================================================================
                                          GLOBAL VARIABLES
==================================================================================================*/

#ifdef __cplusplus
}
#endif
#endif
