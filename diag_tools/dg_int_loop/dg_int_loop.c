/*==================================================================================================

    Module Name:  dg_int_loop.c

    General Description: Implements the diag internal loopback test

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include "dg_platform_defs.h"
#include "dg_loop.h"
#include "dg_dbg.h"
#include "diag_version.h"

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/
const char* argp_program_version     = DIAG_VERSION;
const char* argp_program_bug_address = "<SSD-SBU-JDiagDev@juniper.net>";

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/
#define DG_INT_LOOP_PORT_MAX            DG_LOOP_PORT_10GE_3
#define DG_INT_LOOP_NODE_MAX            DG_LOOP_NODE_HDR
#define DG_INT_LOOP_DEFAULT_PORT        0   /* 0 means test for all data ports */
#define DG_INT_LOOP_DEFAULT_RUN_TIME    -1  /* negtive means run the program for ever */
#define DG_INT_LOOP_DEFAULT_PACKET_SIZE 1024
#define DG_INT_LOOP_DEFAULT_PATTERN     0x5A


/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/* Used by main to communicate with parse_opt. */
typedef struct
{
    UINT8 pattern;  /* data pattern from argument*/
    UINT8 node;
    UINT8 port;
    int   size;
    int   time;
} DG_INT_LOOP_ARG_T;

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static error_t dg_int_loop_arg_parse(int key, char* arg, struct argp_state* state);
static BOOL    dg_int_loop_prepare_args(int argc, char** argv, DG_INT_LOOP_ARG_T* args);
static BOOL    dg_int_loop_get_int_arg(const char* arg, long* value);
static void    dg_int_loop_print_err_string();
static void    dg_int_loop_print_result(DG_LOOP_TEST_STATISTIC_T* result);
static void    dg_int_loop_exit_handler(int sig);

/*==================================================================================================
                                         GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/
static BOOL dg_int_loop_run = TRUE;

/*==================================================================================================
                                         GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Main function for dg_int_loop application

@param[in] argc - Number of arguments
@param[in] argv - Array of each argument passed
*//*==============================================================================================*/
int main(int argc, char** argv)
{
    DG_INT_LOOP_ARG_T args =
    {              /* Default values. */
        .pattern = DG_INT_LOOP_DEFAULT_PATTERN,
        .port    = 1,
        .node    = DG_LOOP_NODE_HDR,
        .size    = DG_INT_LOOP_DEFAULT_PACKET_SIZE,
        .time    = DG_INT_LOOP_DEFAULT_RUN_TIME
    };

    int            ret = 0;
    DG_LOOP_TEST_T test;

    struct sigaction actions;

    if (!dg_int_loop_prepare_args(argc, argv, &args))
    {
        return 1;
    }

    /* signal setup */
    memset(&actions, 0, sizeof(actions));
    sigemptyset(&actions.sa_mask);
    actions.sa_flags   = 0;
    actions.sa_handler = dg_int_loop_exit_handler;
    sigaction(SIGINT, &actions, NULL);
    signal(SIGPIPE, SIG_IGN);

    printf("PATTERN     = 0x%02x\n"
           "RUN TIME    = %ds\n"
           "PACKET SIZE = %d bytes\n"
           "PORT        = 0x%02x\n"
           "NODE        = %d\n",
           args.pattern, args.time, args.size, args.port, args.node);

    /* init the test struct */
    memset(&test, 0, sizeof(test));
    test.tx_port = args.port;
    test.rx_port = args.port;
    test.pattern = args.pattern;
    test.size    = args.size;
    test.number  = DG_LOOP_RUN_IFINITE;

    if (args.port != 0)
    {
        DG_LOOP_TEST_STATISTIC_T* result = &test.result;

        if (!DG_LOOP_config(args.port, args.node, DG_LOOP_CFG_INTERNAL))
        {
            printf("failed to config internal loop back, port=0x%02x, node=%d: ",
                   args.port, args.node);
            dg_int_loop_print_err_string();
            ret = 1;
        }
        else if (!DG_LOOP_start_test(&test))
        {
            printf("failed to start loopback test: ");
            dg_int_loop_print_err_string();
            ret = 1;
        }
        else
        {
            while (dg_int_loop_run)
            {
                if (args.time == 0)
                {
                    break;
                }

                if (args.time > 0)
                {
                    args.time--;
                }

                sleep(1);
                dg_int_loop_print_result(result);

                if (!DG_LOOP_query_test(&test))
                {
                    DG_DBG_TRACE("test has been finished!");
                    break;
                }
            }

            DG_DBG_TRACE("test finished");
            DG_LOOP_wait_test(&test);
            dg_int_loop_print_result(result);
        }
        /* revert all the configuration */
        DG_LOOP_config_all_normal();
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Do argument check

@param[in]  argc - Number of arguments
@param[in]  argv - Array of each argument passed
@param[out] args - dg_int_loop own argument

@return TRUE if no error
*//*==============================================================================================*/
BOOL dg_int_loop_prepare_args(int argc, char** argv, DG_INT_LOOP_ARG_T* args)
{
    /* Program documentation. */
    char tool_doc[] =
        "\nthis tool is used for diag internal loopback test\n"
        "by default it will test all the data port, with packtet size of 1024 bytes\n"
        "if not time argument specified, the program will run for ever";

    /* A description of the arguments we accept. */
    char args_doc[] = "[PATTERN] (in form of hex byte, default is 0x5A)";

    /* The options we understand. */
    struct argp_option dg_options[] =
    {
        { "verbose", 'v', 0,      0, "Produce verbose output",                        0 },
        { "quiet",   'q', 0,      0, "Don't produce any output",                      0 },
        { "port",    'p', "PORT", 0, "Select on which port to do internal loop test", 0 },
        { "node",    'n', "NODE", 0, "Select on which node to config internal loop",  0 },
        { "size",    's', "SIZE", 0, "Set the packet size for each frame",            0 },
        { "time",    't', "TIME", 0, "How long the program would run",                0 },
        { NULL,      0,   NULL,   0, NULL,                                            0 }
    };

    struct argp dg_argp =
    {
        dg_options, dg_int_loop_arg_parse, args_doc, tool_doc, 0, 0, 0
    };

    if (argp_parse(&dg_argp, argc, argv, 0, 0, args) != 0)
    {
        printf("invalid argument detected!\n");
        return FALSE;
    }

    return TRUE;
}

/*=============================================================================================*//**
@brief argp parser function

*//*==============================================================================================*/
error_t dg_int_loop_arg_parse(int key, char* arg, struct argp_state* state)
{
    /* Get the input argument from argp_parse, which we
       know is a pointer to our arguments structure. */
    DG_INT_LOOP_ARG_T* dg_arg = state->input;

    long value;

    switch (key)
    {
    case 'q':
        DG_DBG_set_dbg_level(DG_DBG_LVL_DISABLE);
        break;

    case 'v':
        DG_DBG_set_dbg_level(DG_DBG_LVL_VERBOSE);
        break;

    case 'p':
        if (!dg_int_loop_get_int_arg(arg, &value))
        {
            return EINVAL;
        }
        else if ((value < 0) || (value > DG_INT_LOOP_PORT_MAX))
        {
            printf("out range port: %s, max=0x%02x\n", arg, DG_INT_LOOP_PORT_MAX);
            return EINVAL;
        }
        else
        {
            dg_arg->port = (UINT8)value;
        }
        break;

    case 'n':
        if (!dg_int_loop_get_int_arg(arg, &value))
        {
            return EINVAL;
        }
        else if ((value < 0) || (value > DG_INT_LOOP_NODE_MAX))
        {
            printf("out range node: %s, max=%d\n", arg, DG_INT_LOOP_NODE_MAX);
            return EINVAL;
        }
        else
        {
            dg_arg->node = (UINT8)value;
        }
        break;

    case 's':
        if (!dg_int_loop_get_int_arg(arg, &value))
        {
            return EINVAL;
        }
        else if ((value < DG_LOOP_PACKET_SIZE_MIN) || (value > DG_LOOP_PACKET_SIZE_MAX))
        {
            printf("out range packet size: %s, min=%d, max=%d\n",
                   arg, DG_LOOP_PACKET_SIZE_MIN, DG_LOOP_PACKET_SIZE_MAX);
            return EINVAL;
        }
        else
        {
            dg_arg->size = value;
        }
        break;

    case 't':
        if (!dg_int_loop_get_int_arg(arg, &value))
        {
            return EINVAL;
        }
        else if (value < 0)
        {
            printf("invalid time: %s\n", arg);
            return EINVAL;
        }
        else
        {
            dg_arg->time = value;
        }
        break;

    case ARGP_KEY_ARG:
        if (state->arg_num >= 1)
        {
            printf("Too many arguments\n");
            argp_usage(state);
        }

        if (!dg_int_loop_get_int_arg(arg, &value))
        {
            return EINVAL;
        }
        else if ((value < 0) || (value > 0xFF))
        {
            printf("invalid pattern: %s\n", arg);
            return EINVAL;
        }
        else
        {
            dg_arg->pattern = (UINT8)value;
        }
        break;

    default:
        return ARGP_ERR_UNKNOWN;
    }

    return 0;
}

/*=============================================================================================*//**
@brief convert string arg to int

@param[in]  arg   - the string arg
@param[out] value - the convert value

@return TRUE if success
*//*==============================================================================================*/
BOOL dg_int_loop_get_int_arg(const char* arg, long* value)
{
    BOOL  ret = FALSE;
    char* p_ch;
    long  i;

    errno = 0;

    i = strtol(arg, &p_ch, 0);
    if (errno != 0)
    {
        printf("can't convert to int: %s, errno=%d(%m)\n", arg, errno);
    }
    else if ((i == 0) && (p_ch == arg))
    {
        printf("invalid value: %s\n", arg);
    }
    else
    {
        *value = i;
        ret    = TRUE;
    }

    return ret;
}
/*=============================================================================================*//**
@brief pint out the statistic result

@param[in] result - loop test result
*//*==============================================================================================*/
void dg_int_loop_print_result(DG_LOOP_TEST_STATISTIC_T* result)
{
    printf("total_send=%d  ", result->total_send);
    printf("failed_send=%d\n", result->fail_send);
    printf("total_recv=%d  ", result->total_recv);
    printf("failed_recv=%d  ", result->fail_recv);
    printf("wrong_recv=%d\n\n", result->wrong_recv);
}

/*=============================================================================================*//**
@brief This function handle the SIGINT signal

@param[in] sig - The signal

@note
  - This function is a way to exit the dg_int_loop
*//*==============================================================================================*/
void dg_int_loop_exit_handler(int sig)
{
    dg_int_loop_run = FALSE;
    DG_DBG_TRACE("got signaled: sig = %d", sig);
}

/*=============================================================================================*//**
@brief print the last error string
*//*==============================================================================================*/
void dg_int_loop_print_err_string()
{
    char* err_str = DG_DBG_get_err_string();
    if (err_str != NULL)
    {
        printf("%s\n", err_str);
        free(err_str);
    }
}

