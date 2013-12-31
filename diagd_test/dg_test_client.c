/*==================================================================================================

    Module Name:  dg_test_client.c

    General Description: Test client/unit test for the DIAG engine

    IMPORTANT NOTE: This code is not intended to be included in target release.
                    This is only meant to be used to test the DIAG engine

====================================================================================================

Revision History:
                            Modification     Tracking
Author                          Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
Xudong Huang    - xudongh    2013/12/11     xxxxx-0000   Creation
Xudong Huang    - xudongh    2013/12/11     xxxxx-0002   Fix aux test thread return bug
Xudong Huang    - xudongh    2013/12/20     xxxxx-0003   Enable aux engine

====================================================================================================
                                            INCLUDE FILES
==================================================================================================*/
#include <time.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>

#include <dg_client_api.h>

/*==================================================================================================
                                           LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                            LOCAL MACROS
==================================================================================================*/
/* Undefine to enable debug info */

#ifdef DG_DEBUG
    #define TRACE(x ...) \
    do \
    { \
        printf("DG_TEST_CLIENT: "x); \
        printf("\n"); \
    } while (0)
#else
    #define TRACE(x ...)
#endif

/* Mass connection test settings */
#define DG_TEST_CLIENT_MASS_CONNECT_NUM_CLIENTS          10 /* Number of clients/threads to use */
#define DG_TEST_CLIENT_MASS_CONNECT_NUM_CMD_PER_CONNECT  20 /* Number of times a command is sent per
                                                               engine connect */
#define DG_TEST_CLIENT_MASS_CONNECT_NUM_CONNECT_PER_LOOP 50
#define DG_TEST_CLIENT_MASS_CONNECT_LOOP_SLEEP           2
#define DG_TEST_CLIENT_MASS_CONNECT_NUM_LOOPS            4

/*==================================================================================================
                             LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                      LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
int dg_test_client_menu(void);
void dg_test_client_remove_char(UINT8 to_remove, UINT8* string);
void dg_test_client_send_diag_req(void);
void dg_test_client_print_diag_rsp(UINT8* diag_rsp, UINT32 rsp_len);
BOOL dg_test_client_grow_diag_req_test(void);
BOOL dg_test_client_unsol_rsp_test(void);
static UINT8* hexstr_to_hex(int length, UINT8* hexstr);

BOOL dg_test_client_api_timeout_test(void);
void* dg_test_client_mass_connection_test_thread(void* p);

BOOL dg_test_client_mass_connection_test(void);

BOOL dg_test_client_multi_aux_test(void);
void* dg_test_client_multi_aux_test_thread(void*);
UINT8* dg_test_client_create_random_data(UINT32* diag_req_data_len);

/*==================================================================================================
                                          GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/
static DG_CLIENT_API_SESSION_T dg_test_client_server_cs = -1;
static int                     dg_test_client_timestamp = 0;

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

int main(int argc, char** argv)
{
    DG_COMPILE_UNUSED(argc);
    DG_COMPILE_UNUSED(argv);
    srand(time(NULL));
    char junk[2];

    if ((DG_CLIENT_API_launch_server() == DG_CLIENT_API_STATUS_SUCCESS) &&
        (DG_CLIENT_API_connect_to_server(1000, &dg_test_client_server_cs) == DG_CLIENT_API_STATUS_SUCCESS))
    {
        while (1)
        {
            if (dg_test_client_menu() == -1)
            {
                break;
            }
        }
        DG_CLIENT_API_disconnect_from_server(dg_test_client_server_cs);
    }
    else
    {
        printf("can't connect to diag server!\n");
    }

    printf("Press <ENTER> to exit!\n");
    fgets(junk, sizeof(junk), stdin);

    return 0;
}


UINT8* hexstr_to_hex(int length, UINT8* hexstr)
{
    int    j, k;
    UINT8* buff;
    UINT8* new_buff;
    int    bytes_length;

    k            = 0;
    bytes_length = (length % 2) + (length / 2);

    buff = (UINT8*)malloc(length);
    /* printf("hex str %s\n", hexstr); */
    memcpy(buff, hexstr, length);
    /* printf("buff str %s\n", buff); */

    new_buff = (UINT8*)malloc(bytes_length);
    memset(new_buff, 0, bytes_length);

    for (j = length - 1; j >= 0; j--)
    {
        if ((k % 2) == 0)
        {
            if (buff[j] >= '0' && buff[j] <= '9')
            {
                new_buff[j / 2] = ((buff[j] - 48) & 0x0F);
            }
            else if (buff[j] >= 'a' && buff[j] <= 'f')
            {
                new_buff[j / 2] = ((buff[j] - 97 + 10) & 0x0F);
            }
            else if (buff[j] >= 'A' && buff[j] <= 'F')
            {
                new_buff[j / 2] = ((buff[j] - 65 + 10) & 0x0F);
            }
            else
            {
                printf("must be hex decime\n");
                free(buff);
                free(new_buff);
                return NULL;
            }
            if (j > 0)
            {
                if (buff[j - 1] >= '0' && buff[j - 1] <= '9')
                {
                    new_buff[j / 2] |= (((buff[j - 1] - 48) << 4) & 0xF0);
                }
                else if (buff[j - 1] >= 'a' && buff[j - 1] <= 'f')
                {
                    new_buff[j / 2] |= (((buff[j - 1] - 97 + 10) << 4) & 0xF0);
                }
                else if (buff[j - 1] >= 'A' && buff[j - 1] <= 'F')
                {
                    new_buff[j / 2] |= (((buff[j - 1] - 65 + 10) << 4) & 0xF0);
                }
                else
                {
                    printf("must be hex decime\n");
                    free(buff);
                    free(new_buff);
                    return NULL;
                }
            }
        }
        if ((k % 2) == 1)
        {
            k++;
            continue;
        }
        k++;
    }

    /* printf("%s\n", buff); */
    for (k = 0; k < bytes_length; k++)
    {
        /* printf("%x\n", new_buff[k]); */
    }
    return new_buff;
}

int dg_test_client_menu(void)
{
    /* int menu_choice; */
    char menu_choice[4];
    int  ret_val = 0;

    printf("\n\nDIAG Test Client\n");
    printf("----------------\n");
    printf("1) Send DIAG Request\n");
    printf("2) Growing DIAG Responses Test\n");
    printf("3) 1000 Unsolicited Responses Test\n");
    printf("4) Client API Timeout Test\n");
    printf("5) Mass Connection Test\n");
    printf("6) Multiple Aux Engine Test\n");
    printf("9) Exit\n");
    printf("\n");
    printf("Enter your choice: ");
    if (fgets(menu_choice, sizeof(menu_choice), stdin) != NULL)
    {
        switch (menu_choice[0])
        {
        case '1':
            dg_test_client_send_diag_req();
            break;

        case '2':
            if (dg_test_client_grow_diag_req_test() == TRUE)
            {
                printf("Growing DIAG Response test passed!!!\n");
            }
            else
            {
                printf("Growing DIAG Response test failed!!!\n");
            }
            break;

        case '3':
            if (dg_test_client_unsol_rsp_test() == TRUE)
            {
                printf("Unsolicited DIAG Response test passed!!!\n");
            }
            else
            {
                printf("Unsolicited DIAG Response test failed!!!\n");
            }
            break;

        case '4':
            if (dg_test_client_api_timeout_test() == TRUE)
            {
                printf("Client API Timeout Test passed!!!\n");
            }
            else
            {
                printf("Client API Timeout Test failed!!!\n");
            }
            break;

        case '5':
            if (dg_test_client_mass_connection_test() == TRUE)
            {
                printf("Mass Connection Test passed!!!\n");
            }
            else
            {
                printf("Mass Connection Test failed!!!\n");
            }
            break;

        case '6':
            if (dg_test_client_multi_aux_test() == TRUE)
            {
                printf("Multiple Aux Engine Test passed!!!\n");
            }
            else
            {
                printf("Multiple Aux Engine Test failed!!!\n");
            }
            break;

        case '9':
            ret_val = -1;
            break;
        }

    }
    else
    {
        printf("\nInput error!\n\n");
    }

    return ret_val;
}

void dg_test_client_send_diag_req(void)
{
    DG_CLIENT_API_STATUS_T status;
    UINT8                  diag_req_data[4000];
    UINT8*                 hex_data = NULL;
    UINT16                 opcode;
    UINT32                 rsp_len;
    UINT8*                 rsp_ptr;

    printf("Enter DIAG Opcode + Req Data: ");
    if (fgets((char*)diag_req_data, sizeof(diag_req_data), stdin) != NULL)
    {
        dg_test_client_remove_char(' ', diag_req_data);
        dg_test_client_remove_char('\n', diag_req_data);

        if (strlen((char*)diag_req_data) % 2)
        {
            printf("Error: Odd number of characters found!\n");
        }
        else if (strlen((char*)diag_req_data) < 4)
        {
            printf("Error: Opcode not found!\n");
        }
        else if ((hex_data = hexstr_to_hex(strlen((char*)diag_req_data), diag_req_data)) == NULL)
        {
            printf("Error: Converting hex string to hex data failed");
        }
        else
        {
            opcode = (hex_data[0] << 8) | hex_data[1];
            if (DG_CLIENT_API_send_diag_req(dg_test_client_server_cs, opcode, dg_test_client_timestamp,
                                            ((strlen((char*)diag_req_data) - 4) / 2),
                                            (hex_data + 2)) != DG_CLIENT_API_STATUS_SUCCESS)
            {
                printf("Error: Failed to send DIAG response\n");
            }
            else
            {
                rsp_ptr = DG_CLIENT_API_rcv_desired_diag_rsp(dg_test_client_server_cs, opcode,
                                                             dg_test_client_timestamp++,
                                                             FALSE, 5000, &rsp_len, &status);
                if (rsp_ptr)
                {
                    dg_test_client_print_diag_rsp(rsp_ptr, rsp_len);
                    free(rsp_ptr);
                }
            }
        }
    }
}

BOOL dg_test_client_grow_diag_req_test(void)
{
    DG_CLIENT_API_STATUS_T status;
    UINT8                  diag_req_data[4000];
    UINT32                 rsp_len;
    UINT8*                 rsp_ptr;
    UINT32                 diag_loop_i;
    UINT32                 byte_loop_i;
    BOOL                   success = TRUE;

    srand((unsigned)time(NULL));
    for (diag_loop_i = 0; diag_loop_i < 2000; diag_loop_i++)
    {
        status = DG_CLIENT_API_STATUS_ERROR;
        memset(diag_req_data, 0, sizeof(diag_req_data));
        for (byte_loop_i = 4; byte_loop_i < (diag_loop_i + 4); byte_loop_i++)
        {
            diag_req_data[byte_loop_i] = (UINT8)rand();
        }

        if (DG_CLIENT_API_send_diag_req(dg_test_client_server_cs, 0x0ffe, dg_test_client_timestamp,
                                        byte_loop_i, diag_req_data) == DG_CLIENT_API_STATUS_SUCCESS)
        {
            rsp_ptr = DG_CLIENT_API_rcv_desired_diag_rsp(dg_test_client_server_cs, 0x0ffe, dg_test_client_timestamp++,
                                                         FALSE, 5000, &rsp_len, &status);
            if (rsp_ptr)
            {
                if ((rsp_len == (byte_loop_i + 12 - 4)) &&
                    (memcmp((rsp_ptr + 12), (diag_req_data + 4), (byte_loop_i - 4)) == 0))
                {
                    printf("DIAG Request/Response #%d matched!\n", diag_loop_i);
                }
                else
                {
                    printf("DIAG Request/Response #%d did NOT match!\n", diag_loop_i);
                    success = FALSE;
                    break;
                }
            }
            free(rsp_ptr);

        }
        else
        {
            success = FALSE;
            printf("Error: Failed sending DIAG request\n");
            break;
        }
    }

    return success;
}

BOOL dg_test_client_unsol_rsp_test(void)
{
    DG_CLIENT_API_STATUS_T status;
    UINT8                  diag_req_data[6];
    UINT32                 rsp_len;
    UINT8*                 rsp_ptr;
    UINT8*                 rsp_payload;
    UINT32                 i;
    BOOL                   success = FALSE;

    UINT32 action = 0x00000001;
    UINT16 num    = 1000;

    action = htonl(action);
    num    = htons(num);
    memcpy(diag_req_data, &action, sizeof(action));
    memcpy(diag_req_data + 4, &num, sizeof(num));
/*
    diag_req_data[0] = 0x00;
    diag_req_data[1] = 0x00;
    diag_req_data[2] = 0x00;
    diag_req_data[3] = 0x01;
    diag_req_data[4] = 0x03;
    diag_req_data[5] = 0xe8;
*/
    if (DG_CLIENT_API_send_diag_req(dg_test_client_server_cs, 0x0ffe, dg_test_client_timestamp,
                                    6, diag_req_data) == DG_CLIENT_API_STATUS_SUCCESS)
    {
        rsp_ptr = DG_CLIENT_API_rcv_desired_diag_rsp(dg_test_client_server_cs, 0x0ffe, dg_test_client_timestamp++,
                                                     FALSE, 5000, &rsp_len, &status);

        if (rsp_ptr)
        {
            dg_test_client_print_diag_rsp(rsp_ptr, rsp_len);
            free(rsp_ptr);

            success = TRUE;
            for (i = 0; i < 1000; i++)
            {
                rsp_ptr = DG_CLIENT_API_rcv_desired_diag_rsp(dg_test_client_server_cs, 0x0ffe, 0,
                                                             TRUE, 5000, &rsp_len, &status);
                if (rsp_ptr)
                {
                    dg_test_client_print_diag_rsp(rsp_ptr, rsp_len);
                    rsp_payload = rsp_ptr + 12;
                    if ((rsp_payload[0] != ((i & 0xFF00) >> 8)) ||
                        (rsp_payload[1] != ((i & 0x00FF))) ||
                        (rsp_payload[2] != 0xDE) ||
                        (rsp_payload[3] != 0xAD) ||
                        (rsp_payload[4] != 0xBE) ||
                        (rsp_payload[5] != 0xEF))
                    {
                        success = FALSE;
                        free(rsp_ptr);
                        break;
                    }
                    free(rsp_ptr);
                }
                else
                {
                    success = FALSE;
                    break;
                }
            }
        }

    }
    else
    {
        printf("Error: Failed sending DIAG request\n");
    }

    return success;
}

void dg_test_client_remove_char(UINT8 to_remove, UINT8* string)
{
    UINT8* src  = string;
    UINT8* dest = string;

    TRACE("Remove char \'%c\' from string %s", to_remove, string);
    while (*src != '\0')
    {
        if (*src == to_remove)
        {
            src++;
            if (*src == '\0')
            {
                break;
            }
        }
        else
        {
            *dest = *src;
            dest++;
            src++;
        }
    }
    *dest = '\0';

    TRACE("String after remove: %s", string);
}

void dg_test_client_print_diag_rsp(UINT8* diag_rsp, UINT32 rsp_len)
{
    BOOL   diag_fail;
    BOOL   unsol_rsp;
    UINT16 timestamp;
    UINT16 opcode;
    UINT8  rsp_code;
    UINT32 data_offset_val;
    UINT32 data_len;
    UINT32 i;

    if (DG_CLIENT_API_parse_diag_rsp(diag_rsp, rsp_len, &diag_fail, &unsol_rsp,
                                     &timestamp, &opcode, &rsp_code, &data_offset_val,
                                     &data_len) == DG_CLIENT_API_STATUS_SUCCESS)
    {
        printf("Opcode = 0x%04x, Response Code = 0x%02x, Timestamp = 0x%02x, F = %d, U = %d\n",
               opcode, rsp_code, timestamp, diag_fail, unsol_rsp);
        if (data_len)
        {
            printf("%d bytes of response data:\n", data_len);
            /* If response code is an ASCII error string, print out he ascii response */
            if (rsp_code >= 0x80)
            {
                for (i = 0; i < data_len; i++)
                {
                    printf("%c", *(diag_rsp + data_offset_val + i));
                }
            }
            else
            {
                for (i = 0; i < data_len; i++)
                {
                    printf("%02x", *(diag_rsp + data_offset_val + i));
                    if (((i % 32) == 0) && (i != 0))
                    {
                        printf("\n");
                    }
                }
            }
            printf("\n");
        }
    }

}

BOOL dg_test_client_api_timeout_test(void)
{
    DG_CLIENT_API_STATUS_T status;
    UINT8                  diag_req_data[8]; /* = {0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x0F, 0xA0};*/ /* 4000msec wait */
    UINT32                 rsp_len;
    UINT8*                 rsp_ptr    = NULL;
    BOOL                   is_success = FALSE;

    UINT32 action   = 0x00000002;
    UINT32 time_out = 4000;

    action   = htonl(action);
    time_out = htonl(time_out);
    memcpy(diag_req_data, &action, sizeof(action));
    memcpy(diag_req_data + 4, &time_out, sizeof(time_out));

    /* Send 4000 msec delay command */
    if (DG_CLIENT_API_send_diag_req(dg_test_client_server_cs, 0x0ffe, dg_test_client_timestamp,
                                    sizeof(diag_req_data), diag_req_data) == DG_CLIENT_API_STATUS_SUCCESS)
    {
        /* Wait only 1 second for response */
        rsp_ptr = DG_CLIENT_API_rcv_desired_diag_rsp(dg_test_client_server_cs, 0x0ffe,
                                                     dg_test_client_timestamp,
                                                     FALSE, 1000, &rsp_len, &status);
        if (rsp_ptr)
        {
            printf("Got response, shouldn't have!!\n");
            free(rsp_ptr);
        }
        else
        {
            printf("Did not get response, status = %d\n", status);
            /* Wait 4 second for response */
            rsp_ptr = DG_CLIENT_API_rcv_desired_diag_rsp(dg_test_client_server_cs, 0x0ffe,
                                                         dg_test_client_timestamp++,
                                                         FALSE, 4000, &rsp_len, &status);
            if (rsp_ptr)
            {
                printf("Got response!\n");
                dg_test_client_print_diag_rsp(rsp_ptr, rsp_len);
                free(rsp_ptr);
                is_success = TRUE;
            }
        }
    }
    else
    {
        printf("Error: Failed sending DIAG request\n");
    }

    return is_success;
}

BOOL dg_test_client_mass_connection_test(void)
{
    BOOL      is_success = TRUE;
    UINT8     index      = 0;
    pthread_t thread_id[DG_TEST_CLIENT_MASS_CONNECT_NUM_CLIENTS];
    int       thread_status;
    int*      thread_ret_value = 0;

    for (index = 0; index < DG_TEST_CLIENT_MASS_CONNECT_NUM_CLIENTS; index++)
    {
        if ((thread_status = pthread_create(&thread_id[index], NULL,
                                            dg_test_client_mass_connection_test_thread, NULL)) != 0)
        {
            printf("Error: Error creating client thread #%d\n", index);
            is_success = FALSE;
            break;
        }
    }

    if (is_success == TRUE)
    {
        for (index = 0; index < DG_TEST_CLIENT_MASS_CONNECT_NUM_CLIENTS; index++)
        {
            thread_ret_value = NULL;
            if ((thread_status = pthread_join(thread_id[index], (void**)&thread_ret_value)) != 0)
            {
                printf("Error: Joining client thread #%d\n", index);
                is_success = FALSE;
            }
            else if (thread_ret_value != 0)
            {
                printf("Error: Client thread #%d returned failure status of %d\n", index, *thread_ret_value);
                is_success = FALSE;
            }
        }
    }

    return is_success;

}

void* dg_test_client_mass_connection_test_thread(void* p)
{
    DG_COMPILE_UNUSED(p);
    int                     thread_ret = 0;
    UINT8                   timestamp  = 0;
    DG_CLIENT_API_SESSION_T diag_cs;
    UINT8                   diag_req_data[] = { 0x00, 0x00, 0x00, 0x00, 0xDE, 0xAD, 0xBE, 0xEF };
    UINT8*                  rsp_ptr;
    UINT32                  rsp_len;
    DG_CLIENT_API_STATUS_T  status;
    int                     num_commands = 0;
    int                     num_connects = 0;
    int                     num_loops    = 0;

    printf("Created client thread %p, doing %d loop(s) of %d connect(s), each connect sending %d diag(s)\n",
           (void*)pthread_self(), DG_TEST_CLIENT_MASS_CONNECT_NUM_LOOPS, DG_TEST_CLIENT_MASS_CONNECT_NUM_CONNECT_PER_LOOP,
           DG_TEST_CLIENT_MASS_CONNECT_NUM_CMD_PER_CONNECT);

    for (num_loops = 0; ((num_loops < DG_TEST_CLIENT_MASS_CONNECT_NUM_LOOPS) &&
                         (thread_ret == 0)); num_loops++)
    {
        /* Connect number of times per loop */
        for (num_connects = 0; ((num_connects < DG_TEST_CLIENT_MASS_CONNECT_NUM_CONNECT_PER_LOOP) && (thread_ret == 0));
             num_connects++)
        {
            timestamp = 0;
            if (DG_CLIENT_API_connect_to_server(20000, &diag_cs) != DG_CLIENT_API_STATUS_SUCCESS)
            {
                printf("Error: Failed to send DIAG request on client thread %p, diag_cs = %d\n",
                       (void*)pthread_self(), diag_cs);
                thread_ret = 1;
            }
            else
            {
                /* Send the number of commands for this loop */
                for (num_commands = 0;
                     ((num_commands < DG_TEST_CLIENT_MASS_CONNECT_NUM_CMD_PER_CONNECT) && (thread_ret == 0));
                     num_commands++)
                {
                    if (DG_CLIENT_API_send_diag_req(diag_cs, 0x0ffe, timestamp,
                                                    sizeof(diag_req_data),
                                                    diag_req_data) != DG_CLIENT_API_STATUS_SUCCESS)
                    {
                        printf("Error: Failed to send DIAG request on client thread %p, diag_cs = %d\n",
                               (void*)pthread_self(), diag_cs);
                        thread_ret = 1;
                    }
                    else
                    {
                        rsp_ptr = DG_CLIENT_API_rcv_desired_diag_rsp(diag_cs, 0x0ffe,
                                                                     timestamp,
                                                                     FALSE, 5000, &rsp_len, &status);
                        if (!rsp_ptr)
                        {
                            printf("Error: Failed to receive DIAG response on client thread %p, diag_cs = %d, status = %d\n",
                                   (void*)pthread_self(), diag_cs, status);
                            thread_ret = 1;
                        }
                        else
                        {
                            timestamp++;
                            free(rsp_ptr);
                        }
                    }
                } /* End sending commands per connect */
                DG_CLIENT_API_disconnect_from_server(diag_cs);
            }
        } /* End number of commands per loop */

        /* If we are not done with the test loops, wait a bit.  Done to allow sockets to free up */
        if ((num_loops != (DG_TEST_CLIENT_MASS_CONNECT_NUM_LOOPS - 1)) &&
            (thread_ret == 0))
        {
            printf("Client thread %p completed loop #%d of %d, sleeping %d seconds before next loop\n",
                   (void*)pthread_self(), (num_loops + 1),  DG_TEST_CLIENT_MASS_CONNECT_NUM_LOOPS,
                   DG_TEST_CLIENT_MASS_CONNECT_LOOP_SLEEP);
            sleep(DG_TEST_CLIENT_MASS_CONNECT_LOOP_SLEEP);
        }
    }
    return (int*)(intptr_t)thread_ret;
}

#define DG_TEST_AUX_THREAD_NUM 2
BOOL dg_test_client_multi_aux_test(void)
{
    BOOL      is_success = TRUE;
    UINT8     index      = 0;
    pthread_t thread_id[DG_TEST_AUX_THREAD_NUM];
    int       thread_status;
    int       thread_ret_value = 0;
    UINT16    opcode           = 0xbeef;

    for (index = 0; index < DG_TEST_AUX_THREAD_NUM; index++)
    {
        if ((thread_status = pthread_create(&thread_id[index], NULL,
                                            dg_test_client_multi_aux_test_thread,  (void*)(long)opcode)) != 0)
        {
            printf("Error: Error creating client thread #%d\n", index);
            is_success = FALSE;
            break;
        }

        if (index == 0)
        {
            opcode = 0xdead;
        }
    }

    if (is_success == TRUE)
    {
        for (index = 0; index < DG_TEST_AUX_THREAD_NUM; index++)
        {
            if ((thread_status = pthread_join(thread_id[index], (void**)&thread_ret_value)) != 0)
            {
                printf("Error: Joining client thread #%d\n", index);
                is_success = FALSE;
            }
            else if (thread_ret_value != 0)
            {
                printf("Error: Client thread #%d returned failure status of %d\n", index, thread_ret_value);
                is_success = FALSE;
            }
        }
    }

    return is_success;
}

void* dg_test_client_multi_aux_test_thread(void* data)
{
    int                     thread_ret = 0;
    UINT8                   timestamp  = 0;
    DG_CLIENT_API_SESSION_T diag_cs;
    UINT8*                  diag_req_data;
    UINT32                  diag_req_data_len = 0;
    UINT8*                  rsp_ptr;
    UINT32                  rsp_len;
    DG_CLIENT_API_STATUS_T  status;
    int                     num_commands = 0;
    UINT16                  opcode       = *(UINT16*)(&data);

    printf("Created client thread %p, doing %d loop(s) of %d connect(s), each connect sending %d diag(s)\n",
           (void*)pthread_self(), DG_TEST_CLIENT_MASS_CONNECT_NUM_LOOPS, DG_TEST_CLIENT_MASS_CONNECT_NUM_CONNECT_PER_LOOP,
           DG_TEST_CLIENT_MASS_CONNECT_NUM_CMD_PER_CONNECT);

    timestamp = 0;
    if (DG_CLIENT_API_connect_to_server(20000, &diag_cs) != DG_CLIENT_API_STATUS_SUCCESS)
    {
        printf("Error: Failed to send DIAG request on client thread %p, diag_cs = %d\n",
               (void*)pthread_self(), diag_cs);
        thread_ret = 1;
    }
    else
    {
        /* Send the number of commands for this loop */
        for (num_commands = 0; ((num_commands < DG_TEST_CLIENT_MASS_CONNECT_NUM_CMD_PER_CONNECT) && (thread_ret == 0));
             num_commands++)
        {
            if ((diag_req_data = dg_test_client_create_random_data(&diag_req_data_len)) == NULL)
            {
                printf("Error: Failed to send create random DIAG request on client thread %p, diag_cs = %d\n",
                       (void*)pthread_self(), diag_cs);
                thread_ret = 1;
            }
            else if (DG_CLIENT_API_send_diag_req(diag_cs, opcode, timestamp,
                                                 diag_req_data_len, diag_req_data) != DG_CLIENT_API_STATUS_SUCCESS)
            {
                printf("Error: Failed to send DIAG request on client thread %p, diag_cs = %d\n",
                       (void*)pthread_self(), diag_cs);
                thread_ret = 1;
            }
            else
            {
                rsp_ptr = DG_CLIENT_API_rcv_desired_diag_rsp(diag_cs, opcode,
                                                             timestamp,
                                                             FALSE, 5000, &rsp_len, &status);
                if (!rsp_ptr)
                {
                    printf("Error: Failed to receive DIAG response on client thread %p, diag_cs = %d, status = %d\n",
                           (void*)pthread_self(), diag_cs, status);
                    thread_ret = 1;
                }
                else
                {
                    /* +12 for header, +1 for aux id in simulated aux response */
                    if (rsp_len != (diag_req_data_len + 12 + 1))
                    {
                        printf("Error: DIAG response length is wrong, length = %d", rsp_len);
                        thread_ret = 1;
                    }
                    else if (memcmp(rsp_ptr + 12 + 1, diag_req_data, diag_req_data_len) != 0)
                    {
                        printf("Error: DIAG response data is wrong");
                        thread_ret = 1;
                    }
                    timestamp++;
                    free(rsp_ptr);
                }
            }
        } /* End sending commands per connect */
        DG_CLIENT_API_disconnect_from_server(diag_cs);
    }
    return (int*)(intptr_t)thread_ret;
}

UINT8* dg_test_client_create_random_data(UINT32* diag_req_data_len)
{
    UINT32 index;
    UINT8* req_data = NULL;

    *diag_req_data_len = rand() % 2000;

    req_data = (UINT8*)malloc(*diag_req_data_len);
    if (req_data != NULL)
    {
        for (index = 0; index < *diag_req_data_len; index++)
        {
            req_data[index] = rand() % 255;
        }
    }

    return req_data;
}

