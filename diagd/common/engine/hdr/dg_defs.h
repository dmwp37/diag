#ifndef _DG_DEFS_H
#define _DG_DEFS_H
/*==================================================================================================

    Module Name:          dg_defs.h

    General Description:  Header file for miscellaneous diag commands

====================================================================================================

Revision History:
                            Modification     Tracking
Author                          Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
Xudong Huang    - xudongh    2013/12/11     xxxxx-0000   Creation
Xudong Huang    - xudongh    2013/12/19     xxxxx-0001   Update diag rsp protocol
Xudong Huang    - xudongh    2013/12/20     xxxxx-0002   Update diag req protocol

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
/* All platform dependent basic type defines go here */
#include <dg_pal_client_platform_inc.h>
#include <pthread.h>
#include "dg_rsp_code.h"

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                               MACROS
==================================================================================================*/
/** @addtogroup engine_interface
@{
*/

/* DIAG header values */
#define DG_DEFS_HDR_FLAG_RESPONSE_EXPECTED     0 /**< Header indicates a response is expected */
#define DG_DEFS_HDR_FLAG_RESPONSE_NOT_EXPECTED 1 /**< Header indicates a response is not expected */

#define DG_DEFS_HDR_FLAG_RESPONSE_SOLICITED    0 /**< Header indicates a solicited response */
#define DG_DEFS_HDR_FLAG_RESPONSE_UNSOLICITED  1 /**< Header indicates a unsolicited response */

#define DG_DEFS_HDR_DIAG_VERSION_VALUE         0 /**< Current DIAG header version */

/** Opcode value for last element in handler table */
#define DG_DEFS_HANDLER_TABLE_OPCODE_END       0xDEAD
/*==================================================================================================
                                               ENUMS
==================================================================================================*/

/*==================================================================================================
                                   STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
/** DIAG Opcode */
typedef UINT16 DG_DEFS_OPCODE_T;

/** Diag command run mode bitmask */
typedef enum
{
    DG_DEFS_MODE_NORMAL = 0x01, /**< Normal mode         */
    DG_DEFS_MODE_TEST   = 0x02, /**< Test Mode           */
    DG_DEFS_MODE_ALL    = 0x08
} DG_DEFS_MODE_T;

/** Target processor type */
typedef enum
{
    DG_DEFS_SEC_IC_STATE_INVALID     = 0x0000, /**< Invalid target type      */
    DG_DEFS_SEC_IC_STATE_ENGINEERING = 0x0001, /**< Engineering target       */
    DG_DEFS_SEC_IC_STATE_PRODUCT     = 0x0002  /**< Product target           */
} DG_DEFS_SEC_IC_STATE_T;

/** Response flags bitmask, used to describe a DIAG response */
typedef enum
{
    DG_DEFS_RSP_FLAG_NONE  = 0x00, /**< No special attribute for the response */
    DG_DEFS_RSP_FLAG_UNSOL = 0x01  /**< Unsolicited Response */
} DG_DEFS_RSP_FLAG_T;

/** The Diag Protocol Header (Bulk Endpoint/12 byte) - Request Header */
typedef struct
{
    DG_DEFS_OPCODE_T opcode;           /**< Opcode */
    UINT16           diag_version;     /**< Diag Version Number */
    UINT8            reserved;         /**< reserved */
    UINT8            no_rsp_reqd_flag; /**< No Response Required Flag */
    UINT16           seq_tag;          /**< Sequence Tag */
    UINT32           length;           /**< Data Length of Request */
} DG_DEFS_DIAG_REQ_HDR_T;

/** The Diag Protocol Header (Bulk Endpoint/12 byte) - Response Header */
typedef struct
{
    UINT8  rsp_code;       /**< Response Code */
    UINT8  unsol_rsp_flag; /**< Unsolicited Response Flag */
    UINT16 diag_version;   /**< Diag Version Number */
    UINT16 seq_tag;        /**< Sequence Tag */
    UINT16 opcode;         /**< Opcode */
    UINT32 length;         /**< Data Length of Response */
} DG_DEFS_DIAG_RSP_HDR_T;


/** Diag Request Structure */
typedef struct
{
    DG_DEFS_DIAG_REQ_HDR_T header;        /**< DIAG command header */
    int                    sender_id;     /**< DIAG cmd sender - file descriptor */
    UINT8*                 data_ptr;      /**< DIAG cmd data */
    UINT32                 data_offset;   /**< Offset into data_ptr */
    BOOL                   is_handled;    /**< Indicates if the handler thread has handled the req */
    pthread_cond_t         handled_cond;  /**< Cond. variable for 'is_handled' */
    pthread_mutex_t        handled_mutex; /**< Mutex for 'is_handled' */
    BOOL                   can_delete;    /**< Indicates if the handler thread can delete the req */
    pthread_cond_t         delete_cond;   /**< Cond. variable for 'can_delete' */
    pthread_mutex_t        delete_mutex;  /**< Mutex of 'can_delete' */
} DG_DEFS_DIAG_REQ_T;

/** Diag Response Structure */
typedef struct
{
    DG_DEFS_DIAG_RSP_HDR_T header;   /**< DIAG response header */
    UINT8*                 data_ptr; /**< Pointer to response data field */
} DG_DEFS_DIAG_RSP_T;

/** Structure used for building DIAG responses.  For use by DIAG handlers. */
typedef UINT8 DG_DEFS_DIAG_RSP_BUILDER_T;

/** Opcode handler table entry structure */
typedef void (* DG_DEFS_DIAG_HANDLER_T)(DG_DEFS_DIAG_REQ_T*);
typedef struct
{
    DG_DEFS_OPCODE_T       opcode;       /**< Diag Opcode associated with handler */
    DG_DEFS_MODE_T         mode;         /**< Diag mode required for handler */
    DG_DEFS_DIAG_HANDLER_T fptr;         /**< Function pointer for opcode */
    UINT32                 timeout_msec; /**< Timeout time in msec */
} DG_DEFS_OPCODE_ENTRY_T;


/*==================================================================================================
                                    GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

/** @} */
#ifdef __cplusplus
}
#endif

#endif

