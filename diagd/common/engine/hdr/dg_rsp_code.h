#ifndef _DG_RSP_CODE_H
#define _DG_RSP_CODE_H
/*==================================================================================================
                                                                               
    Module Name:          dg_rsp_code.h

    General Description:  Header file for DIAG response code defines.
                          
====================================================================================================

Revision History:
                            Modification     Tracking
Author                          Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
Xudong Huang    - xudongh    2013/12/11     xxxxx-0000   Creation
Xudong Huang    - xudongh    2013/12/19     xxxxx-0001   Update diag rsp protocol

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/

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
/** Diag response codes */
typedef enum
{
    DG_RSP_CODE_CMD_RSP_GENERIC      = 0x00,     /**< Generic Response           */
    DG_RSP_CODE_CMD_RSP_GEN_FAIL     = 0x01,     /**< General failure            */
    DG_RSP_CODE_PAR_ERR_PROT         = 0x02,     /**< Parser protocol error      */
    DG_RSP_CODE_PAR_ERR_MODE         = 0x03,     /**< Parser mode error          */
    DG_RSP_CODE_PAR_ERR_OPCODE       = 0x04,     /**< Parser opcode error        */
    DG_RSP_CODE_PAR_ERR_PARM         = 0x05,     /**< Parser parameter error     */
    DG_RSP_CODE_PAR_ERR_LENGTH       = 0x06,     /**< Parser length error        */
    DG_RSP_CODE_PAR_ERR_SECUR        = 0x07,     /**< Parser security error      */
    DG_RSP_CODE_CMD_MALLOC_FAIL      = 0x0A,     /**< Error allocating memory    */
    DG_RSP_CODE_CMD_INTL_ERR         = 0x0B,     /**< Tcmd internal error        */
    DG_RSP_CODE_CMD_RSP_TIMEOUT      = 0x0C,     /**< Timeout error              */
    DG_RSP_CODE_CMD_PAR_ERR_SUBMODE  = 0x0D,     /**< Parser submode error       */
    DG_RSP_CODE_CMD_TRANS_LEN_ERR    = 0x0E,     /**< Transport length error     */
    DG_RSP_CODE_CMD_RSP_IRRE_ERR     = 0x0F,     /**< Irrecoverable error        */
    DG_RSP_CODE_CMD_RSP_MUX_ERR      = 0x11,     /**< Open mux channel error     */

    DG_RSP_CODE_ASCII_ERR_LENGTH     = 0x80,     /**< ASCII length error         */
    DG_RSP_CODE_ASCII_ERR_MODE       = 0x83,     /**< ASCII mode error           */
    DG_RSP_CODE_ASCII_ERR_OPCODE     = 0x84,     /**< ASCII opcode error         */
    DG_RSP_CODE_ASCII_ERR_PARM       = 0x85,     /**< ASCII parameter error      */
    DG_RSP_CODE_ASCII_RSP_GEN_FAIL   = 0x87,     /**< ASCII Generic Failure      */
    DG_RSP_CODE_ASCII_MALLOC_FAIL    = 0x8A,     /**< ASCII allocating memory error */
    DG_RSP_CODE_ASCII_RSP_TIMEOUT    = 0x8C,     /**< ASCII timeout error        */
    DG_RSP_CODE_ASCII_RSP_MUX_ERR    = 0x91,     /**< ASCII mux channel error    */

    /** Add all new standard response codes before DG_RSP_CODE_NOT_SET */
    DG_RSP_CODE_NOT_SET
} DG_RSP_CODE_T;

/*==================================================================================================
                                    GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/

/*==================================================================================================
                                        FUNCTION PROTOTYPES
==================================================================================================*/

#ifdef __cplusplus  
}       
#endif

#endif

