#ifndef _DG_CLIENT_COMM_H
#define _DG_CLIENT_COMM_H
/*==================================================================================================

    Module Name:  dg_client_comm.h

    General Description: This file provides an interface to the clients

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
/** Type of DIAG client */
typedef enum
{
    DG_CLIENT_COMM_CLIENT_TYPE_INT, /**< Internal DIAG client */
    DG_CLIENT_COMM_CLIENT_TYPE_EXT  /**< External DIAG client */
} DG_CLIENT_COMM_CLIENT_TYPE_T;

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/
void DG_CLIENT_COMM_shutdown_clients_of_type(DG_CLIENT_COMM_CLIENT_TYPE_T type);
void* DG_CLIENT_COMM_client_connection_listener(void *arg);
void DG_CLIENT_COMM_send_rsp_to_all_clients(DG_DEFS_DIAG_RSP_T *rsp);
void* DG_CLIENT_COMM_client_connection_handler(void* socket_void);
BOOL DG_CLIENT_COMM_add_client_to_list(int socket, DG_CLIENT_COMM_CLIENT_TYPE_T type);
DG_DEFS_STATUS_T DG_CLIENT_COMM_client_write(int fd, DG_DEFS_DIAG_RSP_T* rsp);
void DG_CLIENT_COMM_set_handler_tbl(const DG_DEFS_OPCODE_ENTRY_T *tbl_ptr);
void DG_CLIENT_COMM_set_override_handler_tbl(const DG_DEFS_OPCODE_ENTRY_T *tbl_ptr);

/*==================================================================================================
                                          GLOBAL VARIABLES
==================================================================================================*/

#ifdef __cplusplus  
}
#endif
#endif
