/*==================================================================================================

    Module Name:  diag_send_main.c

    General Description: The task to send raw test commands to DIAG engine

====================================================================================================

Revision History:
                            Modification     Tracking
Author                          Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
Xudong Huang    - xudongh    2013/12/11     xxxxx-0000   Creation

====================================================================================================
                                            INCLUDE FILES
==================================================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <dg_client_api.h>

/*==================================================================================================
                                           LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                            LOCAL MACROS
==================================================================================================*/
#define DG_SEND_CHECK_ROOT_USER            FALSE
#define DG_SEND_ROOT_UID                   0
#define DG_SEND_MAIN_DEFAULT_RSP_TIMEOUT   0      /* no timeout, let diag engine timeout */
#define DG_SEND_DBG_MAX_DUMP_COLS          16
#define DG_SEND_DBG_DUMP_LINE_HEADER_LEN   6
#define DG_SEND_BUFFER_LEN_MAX             1024   /* Maximum reading buffer length */
#define DG_SEND_DIAG_OPCODE_LEN            4      /* DIAG opcode length */
#define DG_SEND_DIAG_DATA_BYTE_LEN         2      /* string length for each data byte */
#define DG_SEND_CONNECT_MAX_TRY            30     /* Number of times to try to connect to diag engine*/

/*==================================================================================================
                             LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                      LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static BOOL dg_send_main_read_raw_command (char *command, UINT32 str_len, UINT16 *opcode,
                                          unsigned char *data, UINT32 *datalen);
static BOOL dg_send_main_process_raw_command (DG_CLIENT_API_SESSION_T diag_session, char *command);
static void dg_send_dump (const char *title, unsigned char *buf, UINT32 len);

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
int main(int argc, char * argv[])
{
//    int verb=0;
    int filein=0;
    int argnum=1;
    UINT8 try_count=1;
    int i;
    char buffer[DG_SEND_BUFFER_LEN_MAX];
    FILE *fp=NULL;
    FILE *input = stdin;
    DG_CLIENT_API_SESSION_T diag_session;

#if DG_SEND_CHECK_ROOT_USER
    if (getuid() != DG_SEND_ROOT_UID)
    {
        printf ("You must run this tool as ROOT user!\n");
        exit (1);
    }
#endif
    
    printf ("Enter diag send main application\n");

    if (argc < 2)
    {
        printf ("Usage: %s cmd1 ... cmdN | -f [file_name]\n", argv[0]);
        printf ("Where:\n\t cmdN     - AAAA[B...]\n");
        printf ("\t\tAAAA    - opcode has to be 4 hex digits (for instance 0039)\n");
        printf ("\t\t[B...]  - optional data payload of 1 or more hex digits\n");
        printf ("\t\t-f [file_name] - read commands from file (stdin if no name provided)\n");
        exit (0);
    }

    printf ("+++ Reading options...\n");
    while ( *argv[argnum] == '-')
    {
        switch (argv[argnum][1])
        {
            case 'f':
                filein = 1;
                break;
            case 'v':
//                verb = 1;
                break;
            default:
                printf ("-- Unknown option '%s'\n", argv[argnum]);
                exit (1);
        }

        argnum++;
        if (argnum == argc)    break;
    }

    if (filein && (argc > argnum))
    {
       fp = fopen (argv[argnum], "r");
       if (fp == NULL)
       {
        printf ("-- Unable to open file '%s': %s\n", argv[argnum], strerror(errno));
        exit (1);
       }
    }

    printf ("+++ Establishing connection...\n");

    while(DG_CLIENT_API_connect_to_server(1000, &diag_session) != DG_CLIENT_API_STATUS_SUCCESS)
    {
        printf ("Connect to diag engine try %d fails\n", try_count);
        if(try_count == DG_SEND_CONNECT_MAX_TRY)
        {
            printf (" -- Unable to establish connection with DIAG engine after try %d times\n",
                      try_count);
            exit(0);
        }
        else
        {
            /* Try to connect again, wait a bit before retry */
            try_count++;
            sleep(1);
        }
    }

    printf ("+++ Processing commands...\n");
    if (filein)
    {
        if(fp != NULL)
            input = fp;
        while (fgets (buffer, DG_SEND_BUFFER_LEN_MAX, input) != NULL)
        {
            if (buffer [strlen(buffer)-1] == '\n')
            {
                buffer [strlen(buffer)-1] = 0;
            }
            dg_send_main_process_raw_command(diag_session,buffer);
        }
        if(fp != NULL)
            fclose (input);
    }
    else
    {
        for (i=argnum; i < argc; i++)
            dg_send_main_process_raw_command(diag_session, argv[i]);
    }

    printf("+++ Done\n");

    /* Disconnect from DIAGs */
    DG_CLIENT_API_disconnect_from_server(diag_session);
    return (0);
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
static BOOL dg_send_main_read_raw_command (char *command, UINT32 str_len, UINT16 *opcode,
                                          unsigned char *data, UINT32 *datalen)
{
    UINT32 bytecode;
    char  *end;
    char   obyte[5] = {0};
    BOOL   ret = FALSE;

    if(str_len >= DG_SEND_DIAG_OPCODE_LEN) /* string length should at least larger than opcode string length */
    {
        end = command + str_len;
        *datalen = 0;

        /* Retrieving the opcode bytes */
        memcpy (obyte, command, DG_SEND_DIAG_OPCODE_LEN);
        obyte[DG_SEND_DIAG_OPCODE_LEN] = 0;
        command += DG_SEND_DIAG_OPCODE_LEN;
        sscanf (obyte, "%x", &bytecode);
        *opcode = (UINT16)bytecode;

        /* Retrieving the dataload */
        while(command < end)
        {
            memcpy (obyte, command, DG_SEND_DIAG_DATA_BYTE_LEN);
            obyte[DG_SEND_DIAG_DATA_BYTE_LEN] = 0;
            command += DG_SEND_DIAG_DATA_BYTE_LEN;
            sscanf (obyte, "%x", &bytecode);
            *data++ = (unsigned char)bytecode;
            *datalen += 1;
        }
        ret = TRUE;
    }
    else
    {
        printf("Error. Input string length is too short, len is %d\n", str_len);
    }

    return (ret);
}

/*=============================================================================================*//**
@brief Process diag raw command

@param[in]   diag_session  - DIAG client connection handle
@param[in]   command       - The raw command string pointer
*//*==============================================================================================*/
static BOOL dg_send_main_process_raw_command (DG_CLIENT_API_SESSION_T diag_session, char *command)
{
    static UINT8 timestamp = 0;
    UINT16 opcode;
    UINT32 datalen;
    unsigned char data[DG_SEND_BUFFER_LEN_MAX];
    DG_CLIENT_API_STATUS_T status;
    UINT8 *rsp = NULL;
    UINT32 rsp_len = 0;
    BOOL is_success = FALSE;

    if(dg_send_main_read_raw_command(command, strlen (command), &opcode, data, &datalen))
    {
        printf("Sending DIAG opcode 0x%04x, time stamp = %d\n", opcode, timestamp);
        dg_send_dump("-> Data sent\n", data, datalen);

        status = DG_CLIENT_API_send_diag_req(diag_session, opcode, timestamp, datalen, data);
        if (status != DG_CLIENT_API_STATUS_SUCCESS)
        {
            printf("Sending DIAG 0x%04x failed, status = %d\n", opcode, status);
        }
        else
        {
            /* Wait for a response */
            rsp = DG_CLIENT_API_rcv_desired_diag_rsp(diag_session, opcode, timestamp, FALSE, DG_SEND_MAIN_DEFAULT_RSP_TIMEOUT,
                                                     &rsp_len, &status);
            if ( (rsp == NULL) || (status != DG_CLIENT_API_STATUS_SUCCESS) )
            {
                printf("Error getting DIAG response for opcode 0x%04x, status = %d\n", opcode, status);
            }
            /* Determine if the response indicates a failure */
            else if (DG_CLIENT_API_check_rsp_for_fail(rsp, rsp_len) == TRUE)
            {
                UINT32 data_offset;
                UINT8  rsp_code;
                char* err_str = NULL;

                printf("DIAG response for opcode 0x%04x indicates failure, rsp length %d\n", opcode, rsp_len);
                if(DG_CLIENT_API_parse_diag_rsp(rsp, rsp_len, NULL, NULL, NULL, NULL, &rsp_code,
                                                &data_offset, NULL) == DG_CLIENT_API_STATUS_SUCCESS)
                {
                    /* If response code is an ASCII error string, print out the ascii response */
                    if ((rsp_code >= 0x80)&&
                        ((err_str = (char*)(rsp + data_offset)) != NULL))
                    {
                        printf("TC ASCII Error: %s\n", err_str);
                    }
                    else
                    {
                        dg_send_dump("-> Failure data received\n", rsp, rsp_len);
                    }
                }
            }
            else
            {
                printf("DIAG 0x%04x success!\n", opcode);
                dg_send_dump("-> Data received\n", rsp, rsp_len);
                is_success = TRUE;
            }

            if (rsp != NULL)
            {
                DG_CLIENT_API_diag_rsp_free(rsp);
            }
        }
        timestamp++;
    }
    else
    {
        printf("Error. dg_send_main_read_raw_command fails \n");
    }

    return(is_success);
}

/*=============================================================================================*//**
@brief Dumps the contents of a buffer to the debug port

@param[in] title    - title of the dump action
@param[in] buf      - The buffer to dump
@param[in] len      - The length of buf in bytes

*//*==============================================================================================*/
static void dg_send_dump (const char *title, unsigned char *buf, UINT32 len)
{
    unsigned char *ptr;
    UINT32 i, j, cur_max_col;

    /* Each row includes a line header 6 characters, and MAX_DUMP_COLS plus NULL */
    /* Each column takes up 3 characters */
    unsigned char buffer[DG_SEND_DBG_DUMP_LINE_HEADER_LEN + (DG_SEND_DBG_MAX_DUMP_COLS * 3) + 1];

    /* print the dump title */
    printf("%s hex dump from addr %08x, len %d\n", title, (UINT32)buf, len);

    for(i = 0; i <= len/DG_SEND_DBG_MAX_DUMP_COLS; i++)
    {
        ptr = buffer;

        /* Reset string buffer for each new row */
        memset(buffer, 0, sizeof(buffer));

        /* Write the line header info */
        sprintf((char*)ptr, "%04x: ", (i * DG_SEND_DBG_MAX_DUMP_COLS));

        /* For all rows, the number of columns is the max number, except for the last row */
        cur_max_col = (i == len/DG_SEND_DBG_MAX_DUMP_COLS) ? len%DG_SEND_DBG_MAX_DUMP_COLS : DG_SEND_DBG_MAX_DUMP_COLS;
        for(j = 0; j < cur_max_col; j++)
        {
            sprintf((char*)ptr, "%s%02x ", buffer, *(buf + i * DG_SEND_DBG_MAX_DUMP_COLS + j));
        }
        printf("%s\n", buffer);
    }
}

