/*==================================================================================================

    Module Name:  diag_send_main.c

    General Description: The task to send raw test commands to DIAG engine

====================================================================================================

====================================================================================================
                                            INCLUDE FILES
==================================================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "dg_platform_defs.h"
#include "dg_client_api.h"

/*==================================================================================================
                                            LOCAL MACROS
==================================================================================================*/
#if DG_DEBUG != 0
    #define DG_SEND_TRACE(...) do { printf("DIAG_SEND: "__VA_ARGS__); printf("\n"); } while (0)
#else
    #define DG_SEND_TRACE(...)
#endif

#define DG_SEND_ERROR(...) do { printf("DIAG_SEND ERROR: "__VA_ARGS__); printf("\n"); } while (0)
#define DG_SEND_PRINT(...) do { printf(__VA_ARGS__); printf("\n"); } while (0)

/*==================================================================================================
                             LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                           LOCAL CONSTANTS
==================================================================================================*/
#define DG_SEND_CHECK_ROOT_USER          FALSE
#define DG_SEND_ROOT_UID                 0
#define DG_SEND_MAIN_DEFAULT_RSP_TIMEOUT 0      /* no timeout, let diag engine timeout */
#define DG_SEND_DBG_MAX_DUMP_COLS        16
#define DG_SEND_DBG_DUMP_LINE_HEADER_LEN 6
#define DG_SEND_BUFFER_LEN_MAX           1024   /* Maximum reading buffer length */
#define DG_SEND_DIAG_OPCODE_LEN          4      /* DIAG opcode length */
#define DG_SEND_DIAG_DATA_BYTE_LEN       2      /* string length for each data byte */
#define DG_SEND_CONNECT_MAX_TRY          1      /* Number of times try to connect to diag engine*/

/*==================================================================================================
                                      LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static BOOL dg_send_read_raw_command(char* command, UINT32 str_len, UINT16* opcode,
                                     UINT8* data, UINT32* datalen);
static BOOL dg_send_process_raw_command(int diag_session, char* command);
static void dg_send_dump(UINT8* buf, UINT32 len);
static void dg_send_print_output(UINT16 opcode, UINT8* buf, UINT32 len);

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
@brief Main function for diag send application

@param[in] argc - Number of arguments
@param[in] argv - Array of each argument passed
*//*==============================================================================================*/
int main(int argc, char* argv[])
{
    int   ret       = 0;
    int   filein    = 0;
    int   argnum    = 1;
    UINT8 try_count = 1;
    int   i;
    char  buffer[DG_SEND_BUFFER_LEN_MAX];
    FILE* fp    = NULL;
    FILE* input = stdin;
    int   diag_session;
    char* server_name = NULL;

#if DG_SEND_CHECK_ROOT_USER
    if (getuid() != DG_SEND_ROOT_UID)
    {
        DG_SEND_PRINT("You must run this tool as ROOT user!");
        exit(1);
    }
#endif

    DG_SEND_TRACE("Enter diag send main application");

    if (argc < 2)
    {
        DG_SEND_PRINT("Usage: %s [-l<server>] cmd1 ... cmdN | -f [file_name]", argv[0]);
        DG_SEND_PRINT("Where:\n\t-l<server> - specify the diag server address");
        DG_SEND_PRINT("Where:\n\t cmdN     - AAAA[B...]");
        DG_SEND_PRINT("\t\tAAAA    - opcode has to be 4 hex digits (for instance 0039)");
        DG_SEND_PRINT("\t\t[B...]  - optional data payload of 1 or more hex digits");
        DG_SEND_PRINT("\t\t-f [file_name] - read commands from file (stdin if no name provided)");
        exit(1);
    }

    DG_SEND_TRACE("+++ Reading options...");
    while (*argv[argnum] == '-')
    {
        switch (argv[argnum][1])
        {
        case 'l':
            server_name = &argv[argnum][2];
            break;

        case 'f':
            filein = 1;
            break;

        case 'v':
            break;

        default:
            DG_SEND_PRINT("-- Unknown option '%s'", argv[argnum]);
            exit(1);
        }

        argnum++;
        if (argnum == argc)
        {
            break;
        }
    }

    if (filein && (argc > argnum))
    {
        fp = fopen(argv[argnum], "r");
        if (fp == NULL)
        {
            DG_SEND_PRINT("-- Unable to open file '%s': errno=%d (%s)",
                          argv[argnum], errno, strerror(errno));
            exit(1);
        }
    }

    DG_SEND_TRACE("+++ Establishing connection...");

    while ((diag_session = DG_CLIENT_API_connect_to_server(server_name)) < 0)
    {
        DG_SEND_ERROR("Connect to diag engine try %d fails", try_count);
        if (try_count == DG_SEND_CONNECT_MAX_TRY)
        {
            DG_SEND_PRINT(" -- Unable to establish connection with DIAG engine after try %d times",
                          try_count);
            exit(1);
        }
        else
        {
            /* Try to connect again, wait a bit before retry */
            try_count++;
            Sleep(1000);
        }
    }

    DG_SEND_TRACE("+++ Processing commands...");
    if (filein)
    {
        if (fp != NULL)
        {
            input = fp;
        }
        while (fgets(buffer, DG_SEND_BUFFER_LEN_MAX, input) != NULL)
        {
            if (buffer[strlen(buffer) - 1] == '\n')
            {
                buffer[strlen(buffer) - 1] = 0;
            }
            if (!dg_send_process_raw_command(diag_session, buffer))
            {
                DG_SEND_ERROR("dg_send_process_raw_command failed");
                ret = -1;
                break;
            }
        }
        if (fp != NULL)
        {
            fclose(input);
        }
    }
    else
    {
        for (i = argnum; i < argc; i++)
        {
            if (!dg_send_process_raw_command(diag_session, argv[i]))
            {
                DG_SEND_ERROR("dg_send_process_raw_command failed");
                ret = -1;
                break;
            }
        }
    }

    DG_SEND_TRACE("+++ Done");

    /* Disconnect from DIAGs */
    DG_CLIENT_API_disconnect_from_server(diag_session);
    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/
/*=============================================================================================*//**
@brief Read diag raw command from input char string

@param[in]   command  - The raw command string pointer
@param[in]   str_len  - The length of the command string
@param[out]  opcode   - diag opcode parsed
@param[out]  data     - diag data pointer
@param[out]  datalen  - diag data length
*//*==============================================================================================*/
BOOL dg_send_read_raw_command(char* command, UINT32 str_len, UINT16* opcode,
                              UINT8* data, UINT32* datalen)
{
    UINT32 bytecode;
    char*  end;
    char   obyte[5] = { 0 };
    BOOL   ret      = FALSE;

    /* string length should at least larger than opcode string length */
    if (str_len >= DG_SEND_DIAG_OPCODE_LEN)
    {
        end      = command + str_len;
        *datalen = 0;

        /* Retrieving the opcode bytes */
        memcpy(obyte, command, DG_SEND_DIAG_OPCODE_LEN);
        obyte[DG_SEND_DIAG_OPCODE_LEN] = 0;
        command                       += DG_SEND_DIAG_OPCODE_LEN;
        sscanf(obyte, "%x", &bytecode);
        *opcode = (UINT16)bytecode;

        /* Retrieving the dataload */
        while (command < end)
        {
            memcpy(obyte, command, DG_SEND_DIAG_DATA_BYTE_LEN);
            obyte[DG_SEND_DIAG_DATA_BYTE_LEN] = 0;
            command += DG_SEND_DIAG_DATA_BYTE_LEN;
            sscanf(obyte, "%x", &bytecode);
            *data++   = (UINT8)bytecode;
            *datalen += 1;
        }
        ret = TRUE;
    }
    else
    {
        DG_SEND_PRINT("Error. Input string length is too short, len is %d", str_len);
    }

    return ret;
}

/*=============================================================================================*//**
@brief Process diag raw command

@param[in]   diag_session  - DIAG client connection handle
@param[in]   command       - The raw command string pointer
*//*==============================================================================================*/
BOOL dg_send_process_raw_command(int diag_session, char* command)
{
    static UINT8 timestamp = 0;
    UINT16       opcode;
    UINT32       datalen;
    UINT8        data[DG_SEND_BUFFER_LEN_MAX];
    BOOL         is_success = FALSE;

    DG_CLIENT_API_REQ_T  diag_req;
    DG_CLIENT_API_RSP_T* diag_rsp;

    if (dg_send_read_raw_command(command, strlen(command), &opcode, data, &datalen))
    {
        DG_SEND_PRINT("Sending DIAG opcode 0x%04x, time stamp = %d", opcode, timestamp);
        DG_SEND_PRINT("<- Data sent");
        dg_send_dump(data, datalen);

        diag_req.opcode    = opcode;
        diag_req.timestamp = timestamp;
        diag_req.data_len  = datalen;
        diag_req.data_ptr  = data;

        if (!DG_CLIENT_API_send_diag_req(diag_session, &diag_req))
        {
            DG_SEND_PRINT("Sending DIAG 0x%04x failed", opcode);
        }
        else
        {
            /* Wait for a response */
            diag_rsp = DG_CLIENT_API_recv_diag_rsp(diag_session, &diag_req, FALSE,
                                                   DG_SEND_MAIN_DEFAULT_RSP_TIMEOUT);
            if (diag_rsp == NULL)
            {
                DG_SEND_PRINT("Can't get DIAG response for opcode 0x%04x", opcode);
            }
            /* Determine if the response indicates a failure */
            else if (diag_rsp->is_fail)
            {
                char* err_str = NULL;

                DG_SEND_ERROR("DIAG response for opcode 0x%04x indicates failure, rsp_len=%d",
                              opcode, diag_rsp->data_len);
                DG_SEND_PRINT("-> Failure data received");
                /* If response code is an ASCII error string, print out the ascii response */
                if ((diag_rsp->rsp_code >= 0x80) &&
                    ((err_str = (char*)(diag_rsp->data_ptr)) != NULL))
                {
                    DG_SEND_PRINT("%s", err_str);
                }
                else
                {
                    dg_send_dump(diag_rsp->data_ptr, diag_rsp->data_len);
                }
            }
            else
            {
                DG_SEND_TRACE("DIAG 0x%04x success!", opcode);
                dg_send_print_output(opcode, diag_rsp->data_ptr, diag_rsp->data_len);
                is_success = TRUE;
            }

            if (diag_rsp != NULL)
            {
                DG_CLIENT_API_diag_rsp_free(diag_rsp);
            }
        }
        timestamp++;
    }
    else
    {
        DG_SEND_PRINT("dg_send_read_raw_command failed");
    }

    return is_success;
}

/*=============================================================================================*//**
@brief Dumps the contents of a buffer to the debug port

@param[in] buf      - The buffer to dump
@param[in] len      - The length of buf in bytes

*//*==============================================================================================*/
void dg_send_dump(UINT8* buf, UINT32 len)
{
    UINT8* ptr;
    UINT32 i, j, cur_max_col;

    /* Each row includes a line header 6 characters, and MAX_DUMP_COLS plus NULL */
    /* Each column takes up 3 characters */
    UINT8 buffer[DG_SEND_DBG_DUMP_LINE_HEADER_LEN + (DG_SEND_DBG_MAX_DUMP_COLS * 3) + 1];

    for (i = 0; i <= len / DG_SEND_DBG_MAX_DUMP_COLS; i++)
    {
        ptr = buffer;

        /* Reset string buffer for each new row */
        memset(buffer, 0, sizeof(buffer));

        /* For all rows, the number of columns is the max number, except for the last row */
        cur_max_col = (i == len / DG_SEND_DBG_MAX_DUMP_COLS) ?
                      (len % DG_SEND_DBG_MAX_DUMP_COLS) : DG_SEND_DBG_MAX_DUMP_COLS;

        if (cur_max_col > 0)
        {
            /* Write the line header info */
            sprintf((char*)ptr, "%04x: ", (i * DG_SEND_DBG_MAX_DUMP_COLS));
            for (j = 0; j < cur_max_col; j++)
            {
                sprintf((char*)ptr, "%s%02x ", buffer, *(buf + i * DG_SEND_DBG_MAX_DUMP_COLS + j));
            }
            DG_SEND_PRINT("%s", buffer);
        }
    }
}

/*=============================================================================================*//**
@brief Print out the output according to the opcode, please ref diag spec for output format

@param[in] opcode   - the opcode of the diag response
@param[in] buf      - The output data buffer
@param[in] len      - The length of buf in bytes

*//*==============================================================================================*/
void dg_send_print_output(UINT16 opcode, UINT8* buf, UINT32 len)
{
    DG_SEND_PRINT("-> Data success received");
    switch (opcode)
    {
    case 0x0000: /* version */
        DG_SEND_PRINT("%s", (const char*)buf);
        break;

    default:
        dg_send_dump(buf, len);
        break;
    }
}

