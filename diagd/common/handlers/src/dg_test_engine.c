/*==================================================================================================

    Module Name:  dg_test_engine.c

    General Description: Implements the TEST_ENGINE handler, handler used to test the DIAG engine

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include "dg_handler_inc.h"
#include "dg_client_api.h"
#include "dg_aux_engine.h"


/** @addtogroup common_command_handlers
@{
*/

/** @addtogroup TEST_ENGINE
@{

@par
<b>TEST_ENGINE - 0x0FFE</b>

@par
This command is used to validate/test basic DIAG engine functionality.  The command allows
the following capabilites:
 - Echo back a given request data string
 - Send X number of unsolicited responses
 - Delay x msec before sending a response
*/
/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/
#define DG_TEST_ENGINE_UNSOL_RSP_SIZE 6      /**< Response data size of unsolicited response */
#define DG_TEST_ENGINE_OPCODE_ECHO    0x0FFE /**< Opcode for ECHO command */

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/** TEST_ENGINE action parameters */
enum
{
    DG_TEST_ENGINE_ACTION_ECHO      = 0x00000000, /**< 'Echo' action, echo request data back */
    DG_TEST_ENGINE_ACTION_UNSOL     = 0x00000001, /**< 'Unsol' action, return x number of
                                                        unsolicited responses */
    DG_TEST_ENGINE_ACTION_DELAY     = 0x00000002, /**< 'Delay' action, delays x msec before sending
                                                        a response */
    DG_TEST_ENGINE_ACTION_LOOP      = 0x00000003, /**< 'Cient loop Echo' action, request
                                                        data back */
    DG_TEST_ENGINE_ACTION_FIXED_RSP = 0x00000004, /**< 'Fixed Size' DIAG response */
    DG_TEST_ENGINE_AUX_DISABLE      = 0x00000005  /**< 'Disable Aux' action */
};
typedef UINT32 DG_TEST_ENGINE_ACTION_T;

/** Structure for a ECHO diag request */
typedef struct
{
    UINT32 sub_command;   /**< sub echocommand always 0x00000000      */
    UINT8  echo_data[1];  /**< To be echoed data                      */
} DG_TEST_ENGINE_REQ_ECHO_T;

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static BOOL dg_test_engine_echo_data(UINT8* data, UINT32 size, UINT32 rsp_timeout);

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
@brief Handler function for the TEST_ENGINE DIAG

@param[in] req - DIAG request
*//*==============================================================================================*/
void DG_TEST_ENGINE_handler_main(DG_DEFS_DIAG_REQ_T* req)
{
    DG_TEST_ENGINE_ACTION_T     action;
    DG_DEFS_DIAG_RSP_BUILDER_T* rsp;

    /* Init response builder */
    rsp = DG_ENGINE_UTIL_rsp_init();

    /* Verify action parameter was given */
    if (!DG_ENGINE_UTIL_req_len_check_at_least(req, sizeof(action), rsp))
    {
        DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_ERR_LENGTH,
                                            "Invalid Data Length, expected at least %d bytes",
                                            sizeof(action));
    }
    else
    {
        /* Parse and switch on action */
        action = DG_ENGINE_UTIL_req_parse_4_bytes_ntoh(req);
        DG_DBG_TRACE("Action = %d", action);
        switch (action)
        {
        case DG_TEST_ENGINE_ACTION_ECHO:
        {
            /* Echo data from request (minus the action field, back in DIAG response */
            if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, DG_ENGINE_UTIL_req_get_remain_len(req)))
            {
                DG_ENGINE_UTIL_rsp_append_buf(rsp,
                                              DG_ENGINE_UTIL_req_get_remain_data_ptr(req),
                                              DG_ENGINE_UTIL_req_get_remain_len(req));
                DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
            }
        }
        break;

        case DG_TEST_ENGINE_ACTION_UNSOL:
        {
            /* Send X number of unsolicited responses */
            UINT16 num_unsol = 0;
            UINT16 rsp_i;

            /* Verify command request length and allocate response data */
            if (DG_ENGINE_UTIL_req_remain_len_check_at_least(req, sizeof(num_unsol), rsp) &&
                DG_ENGINE_UTIL_rsp_data_alloc(rsp, DG_TEST_ENGINE_UNSOL_RSP_SIZE))
            {
                /* Send the first response to indicate we are about to start */
                DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                DG_ENGINE_UTIL_rsp_send(rsp, req);

                /* Get the number of responses to be sent */
                num_unsol = DG_ENGINE_UTIL_req_parse_2_bytes_ntoh(req);

                /* For each response, have the unsolicited response number +
                   a preset data string 0xDEADBEEF */
                DG_ENGINE_UTIL_rsp_append_2_bytes_hton(rsp, 0x0000);
                DG_ENGINE_UTIL_rsp_append_4_bytes_hton(rsp, 0xDEADBEEF);
                DG_ENGINE_UTIL_rsp_set_flag(rsp, DG_DEFS_RSP_FLAG_UNSOL);

                DG_DBG_TRACE("Sending %d unsolicited responses", num_unsol);
                for (rsp_i = 0; rsp_i < num_unsol; rsp_i++)
                {
                    DG_ENGINE_UTIL_rsp_replace_2_bytes_hton(rsp, 0, rsp_i);
                    /* Don't send out the last response here, let the main rsp_send take
                       care of it */
                    if (rsp_i < (num_unsol - 1))
                    {
                        DG_ENGINE_UTIL_rsp_send(rsp, req);
                    }
                }
            }
        }
        break;

        case DG_TEST_ENGINE_ACTION_DELAY:
        {
            UINT32 msec_to_wait = 0;

            if (DG_ENGINE_UTIL_req_remain_len_check_at_least(req, sizeof(msec_to_wait), rsp))
            {
                msec_to_wait = DG_ENGINE_UTIL_req_parse_4_bytes_ntoh(req);
                usleep(1000 * msec_to_wait);
                DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
            }
        }
        break;

        case DG_TEST_ENGINE_ACTION_LOOP:
        {
            UINT8* data = DG_ENGINE_UTIL_req_get_remain_data_ptr(req);
            UINT32 size = DG_ENGINE_UTIL_req_get_remain_len(req);

            if (!dg_test_engine_echo_data(data, size, 0))
            {
                DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_RSP_GEN_FAIL,
                                                    "client loop echo test failed");
            }
            else if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, size))
            {
                DG_ENGINE_UTIL_rsp_append_buf(rsp, data, size);
                DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
            }
        }
        break;

        case DG_TEST_ENGINE_ACTION_FIXED_RSP:
        {
            /* Verify command request length and allocate response data */
            if (DG_ENGINE_UTIL_req_remain_len_check_at_least(req, 4, rsp))
            {
                UINT32 rsp_size = DG_ENGINE_UTIL_req_parse_4_bytes_ntoh(req);
                if (DG_ENGINE_UTIL_rsp_data_alloc(rsp, rsp_size))
                {
                    UINT8  data = 0;
                    UINT32 index;
                    DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);

                    for (index = 0; index < rsp_size; index++)
                    {
                        DG_ENGINE_UTIL_rsp_append_1_byte_hton(rsp, data);
                        data++;
                    }
                }
            }
        }
        break;

        case DG_TEST_ENGINE_AUX_DISABLE:
        {
            /* Verify command request length and allocate response data */
            if (DG_ENGINE_UTIL_req_remain_len_check_at_least(req, 2, rsp))
            {
                BOOL  set_enable;
                UINT8 aux_id     = DG_ENGINE_UTIL_req_parse_1_byte_ntoh(req);
                UINT8 enable_val = DG_ENGINE_UTIL_req_parse_1_byte_ntoh(req);

                if ((enable_val != 0x00) && (enable_val != 0x01))
                {
                    DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_ERR_PARM,
                                                        "Invalid enable parameter %d", enable_val);
                }
                else
                {
                    set_enable = (BOOL)enable_val;
                    if (DG_AUX_ENGINE_enable_aux(aux_id, set_enable))
                    {
                        DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GENERIC);
                    }
                    else
                    {
                        DG_ENGINE_UTIL_rsp_set_code(rsp, DG_RSP_CODE_CMD_RSP_GEN_FAIL);
                    }
                }
            }
        }
        break;

        default:
            DG_ENGINE_UTIL_rsp_set_error_string(rsp, DG_RSP_CODE_ASCII_ERR_PARM,
                                                "Invalid parameter: 0x%08x", action);
            break;
        }
    }

    DG_ENGINE_UTIL_rsp_send(rsp, req);

    /* Free response builder */
    DG_ENGINE_UTIL_rsp_free(rsp);
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Echo data

@param[in/out]  data        - data buffer to echo, when returned will fill responsed echo data
@param[in]      size        - the size of the data buffer
@param[in]  rsp_timeout     - Amount of time to wait for DIAG response (in msec, 0 means wait
                              forever)

@return TRUE for success

@note
- Calling function is responsible for allocating (and freeing)
*//*==============================================================================================*/
BOOL dg_test_engine_echo_data(UINT8* data, UINT32 size, UINT32 rsp_timeout)
{
    DG_TEST_ENGINE_REQ_ECHO_T* echo_req;
    BOOL                       status = FALSE;
    int                        diag_session;
    UINT32                     cmd_len = 0;

    DG_DBG_TRACE("Requested echo data size = %d", size);

    cmd_len = offsetof(DG_TEST_ENGINE_REQ_ECHO_T, echo_data) + size;

    if (data == NULL)
    {
        DG_DBG_ERROR("Invalid pass in parameter!");
    }
    else if ((diag_session = DG_CLIENT_API_connect_to_server(NULL)) == -1)
    {
        DG_DBG_ERROR("Connect to server failed!");
    }
    else if ((echo_req = (DG_TEST_ENGINE_REQ_ECHO_T*)malloc(cmd_len)) == NULL)
    {
        DG_DBG_ERROR("Malloc ECHO req failed! length = %d", cmd_len);
        DG_CLIENT_API_disconnect_from_server(diag_session);
    }
    else
    {
        DG_CLIENT_API_REQ_T diag_req;

        echo_req->sub_command = htonl(0x00000000); /* for echo request */
        memcpy((UINT8*)(echo_req->echo_data), data, size);

        diag_req.opcode    = DG_TEST_ENGINE_OPCODE_ECHO;
        diag_req.timestamp = 0;
        diag_req.data_len  = cmd_len;
        diag_req.data_ptr  = (UINT8*)echo_req;

        /* Send echo request */
        status = DG_CLIENT_API_send_diag_req(diag_session, &diag_req);

        /* after send the request we release the resource */
        free(echo_req);

        if (status != TRUE)
        {
            DG_DBG_ERROR("Send DIAG ECHO request failed!");
            status = FALSE;
        }
        else
        {
            DG_CLIENT_API_RSP_T* diag_rsp = NULL;
            /* Get the ECHO response */
            diag_rsp = DG_CLIENT_API_recv_diag_rsp(diag_session, &diag_req, FALSE, rsp_timeout);
            if (diag_rsp == NULL)
            {
                DG_DBG_ERROR("Receive DIAG ECHO response failed!");
                status = FALSE;
            }
            else
            {
                if (diag_rsp->is_fail == TRUE)
                {
                    DG_DBG_ERROR("ECHO data failed!");
                    status = FALSE;
                }
                else if (diag_rsp->data_len != size)
                {
                    DG_DBG_ERROR("ECHO data size mismatch! origin: %d, receive: %d",
                                 size, diag_rsp->data_len);
                    status = FALSE;
                }
                else
                {
                    memcpy(data, diag_rsp->data_ptr, size);
                }

                DG_CLIENT_API_diag_rsp_free(diag_rsp);
            }
        }

        DG_CLIENT_API_disconnect_from_server(diag_session);
    }

    /* Return status code */
    DG_DBG_TRACE("Return status = %d", status);
    return status;
}

/** @} */
/** @} */

