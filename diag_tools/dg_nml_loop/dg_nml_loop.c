/*==================================================================================================

    Module Name:  dg_nml_loop.c

    General Description: Implements the diag normal loopback test

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
#define DG_NML_LOOP_DEFAULT_RUN_TIME -1  /* negtive means run the program for ever */
#define DG_NML_LOOP_PORT_PAIR_MAX    13  /* the max port pair */

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/* Used by main to communicate with parse_opt. */
typedef struct
{
    char* cfg_file; /* the config file */
    int   time;
} DG_NML_LOOP_ARG_T;

typedef struct
{
    DG_LOOP_PORT_T port1;
    DG_LOOP_PORT_T port2;
    int            size;
    UINT8          pattern;
} DG_NML_LOOP_CONFIG_T;

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static error_t dg_nml_loop_arg_parse(int key, char* arg, struct argp_state* state);
static BOOL    dg_nml_loop_prepare_args(int argc, char** argv, DG_NML_LOOP_ARG_T* args);
static BOOL    dg_nml_loop_get_int_arg(const char* arg, long* value);
static void    dg_nml_loop_print_err_string();
static void    dg_nml_loop_print_result();
static void    dg_nml_loop_exit_handler(int sig);
static void    dg_nml_loop_dump_config();

static DG_NML_LOOP_CONFIG_T* dg_nml_loop_read_config(const char* file);

/*==================================================================================================
                                         GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/
static BOOL dg_nml_loop_run = TRUE;

static DG_NML_LOOP_CONFIG_T dg_nml_loop_cfg_end = { 0, 0, 0, 0 };

static DG_NML_LOOP_CONFIG_T dg_nml_loop_default_cfg[] =
{
    { DG_LOOP_PORT_MGT,    DG_LOOP_PORT_HA,     1024, 0x5A },
    { DG_LOOP_PORT_WTB0_1, DG_LOOP_PORT_WTB0_2, 1024, 0x5A },
    { DG_LOOP_PORT_WTB1_1, DG_LOOP_PORT_WTB1_2, 1024, 0x5A },
    { DG_LOOP_PORT_GE_0,   DG_LOOP_PORT_GE_1,   1024, 0x5A },
    { DG_LOOP_PORT_GE_2,   DG_LOOP_PORT_GE_3,   1024, 0x5A },
    { DG_LOOP_PORT_GE_4,   DG_LOOP_PORT_GE_5,   1024, 0x5A },
    { DG_LOOP_PORT_GE_6,   DG_LOOP_PORT_GE_7,   1024, 0x5A },
    { DG_LOOP_PORT_GE_8,   DG_LOOP_PORT_GE_9,   1024, 0x5A },
    { DG_LOOP_PORT_GE_10,  DG_LOOP_PORT_GE_11,  1024, 0x5A },
    { DG_LOOP_PORT_SFP_0,  DG_LOOP_PORT_SFP_1,  1024, 0x5A },
    { DG_LOOP_PORT_SFP_2,  DG_LOOP_PORT_SFP_3,  1024, 0x5A },
    { DG_LOOP_PORT_10GE_0, DG_LOOP_PORT_10GE_1, 1024, 0x5A },
    { DG_LOOP_PORT_10GE_2, DG_LOOP_PORT_10GE_3, 1024, 0x5A },
    { 0, 0, 0, 0 }
};

static DG_NML_LOOP_CONFIG_T* dg_nml_loop_cfg_settings = NULL;

static /* test control blocks */
DG_LOOP_TEST_T dg_nml_loop_test[DG_NML_LOOP_PORT_PAIR_MAX][2];

/*==================================================================================================
                                         GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Main function for dg_nml_loop application

@param[in] argc - Number of arguments
@param[in] argv - Array of each argument passed
*//*==============================================================================================*/
int main(int argc, char** argv)
{
    int ret   = 0;
    int index = 0;

    /* default argument. */
    DG_NML_LOOP_ARG_T args =
    {
        .cfg_file = NULL,
        .time     = DG_NML_LOOP_DEFAULT_RUN_TIME
    };

    struct sigaction actions;

    DG_NML_LOOP_CONFIG_T* p_cfg;

    if (!dg_nml_loop_prepare_args(argc, argv, &args))
    {
        return 1;
    }

    /* signal setup */
    memset(&actions, 0, sizeof(actions));
    sigemptyset(&actions.sa_mask);
    actions.sa_flags   = 0;
    actions.sa_handler = dg_nml_loop_exit_handler;
    sigaction(SIGINT, &actions, NULL);
    signal(SIGPIPE, SIG_IGN);

    printf("RUN TIME = %ds\n"
           "CFG FILE = %s\n",
           args.time, args.cfg_file);

    if (args.cfg_file != NULL)
    {
        if ((dg_nml_loop_cfg_settings = dg_nml_loop_read_config(args.cfg_file)) == NULL)
        {
            exit(0);
        }
    }
    else
    {
        dg_nml_loop_cfg_settings = dg_nml_loop_default_cfg;
    }

    /* init the test blocks */
    memset(dg_nml_loop_test, 0, sizeof(dg_nml_loop_test));

    /* start all the test thread */
    p_cfg = dg_nml_loop_cfg_settings;
    index = 0;
    while (memcmp(p_cfg, &dg_nml_loop_cfg_end, sizeof(dg_nml_loop_cfg_end)) != 0)
    {
        dg_nml_loop_test[index][0].tx_port = p_cfg->port1;
        dg_nml_loop_test[index][0].rx_port = p_cfg->port2;
        dg_nml_loop_test[index][0].pattern = p_cfg->pattern;
        dg_nml_loop_test[index][0].size    = p_cfg->size;
        dg_nml_loop_test[index][0].number  = DG_LOOP_RUN_IFINITE;

        dg_nml_loop_test[index][1].tx_port = p_cfg->port2;
        dg_nml_loop_test[index][1].rx_port = p_cfg->port1;
        dg_nml_loop_test[index][1].pattern = p_cfg->pattern;
        dg_nml_loop_test[index][1].size    = p_cfg->size;
        dg_nml_loop_test[index][1].number  = DG_LOOP_RUN_IFINITE;

        if (!DG_LOOP_start_test(&dg_nml_loop_test[index][0]))
        {
            printf("failed to start loopback test tx_port=0x%02x rx_port=0x%02x: ",
                   p_cfg->port1, p_cfg->port2);
            dg_nml_loop_print_err_string();
            ret = 1;
        }

        if (!DG_LOOP_start_test(&dg_nml_loop_test[index][1]))
        {
            printf("failed to start loopback test tx_port=0x%02x rx_port=0x%02x: ",
                   p_cfg->port2, p_cfg->port1);
            dg_nml_loop_print_err_string();
            ret = 1;
        }

        index++;
        p_cfg++;
    }

    /* print out thread statistics */
    while (dg_nml_loop_run)
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
        dg_nml_loop_print_result();
    }

    /* stop the thread */
    p_cfg = dg_nml_loop_cfg_settings;
    index = 0;
    while (memcmp(p_cfg, &dg_nml_loop_cfg_end, sizeof(dg_nml_loop_cfg_end)) != 0)
    {
        DG_LOOP_stop_test(&dg_nml_loop_test[index][0]);
        DG_LOOP_stop_test(&dg_nml_loop_test[index][1]);
        index++;
        p_cfg++;
    }

    /* clean up thread */
    p_cfg = dg_nml_loop_cfg_settings;
    index = 0;
    while (memcmp(p_cfg, &dg_nml_loop_cfg_end, sizeof(dg_nml_loop_cfg_end)) != 0)
    {
        DG_LOOP_wait_test(&dg_nml_loop_test[index][0]);
        DG_LOOP_wait_test(&dg_nml_loop_test[index][1]);
        index++;
        p_cfg++;
    }

    dg_nml_loop_print_result();

    printf("normal loop test finished\n");

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Do argument check

@param[in]  argc - Number of arguments
@param[in]  argv - Array of each argument passed
@param[out] args - dg_nml_loop own argument

@return TRUE if no error
*//*==============================================================================================*/
BOOL dg_nml_loop_prepare_args(int argc, char** argv, DG_NML_LOOP_ARG_T* args)
{
    /* Program documentation. */
    char tool_doc[] =
        "\nthis tool is used for diag normal loopback test\n"
        "if no config file set, it will use internal default settings\n"
        "if not time argument specified, the program will run for ever";

    /* The options we understand. */
    struct argp_option dg_options[] =
    {
        { "verbose", 'v', 0,      0, "Produce verbose output",                   0 },
        { "quiet",   'q', 0,      0, "Don't produce any output",                 0 },
        { "config",  'f', "FILE", 0, "Set config file for the normal loop test", 0 },
        { "dump",    'd', 0,      0, "Dump the default settings",                0 },
        { "time",    't', "TIME", 0, "How long the program would run",           0 },
        { NULL,      0,   NULL,   0, NULL,                                       0 }
    };

    struct argp dg_argp =
    {
        dg_options, dg_nml_loop_arg_parse, NULL, tool_doc, NULL, NULL, NULL
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
error_t dg_nml_loop_arg_parse(int key, char* arg, struct argp_state* state)
{
    /* Get the input argument from argp_parse, which we
       know is a pointer to our arguments structure. */
    DG_NML_LOOP_ARG_T* dg_arg = state->input;

    long value;

    switch (key)
    {
    case 'q':
        DG_DBG_set_dbg_level(DG_DBG_LVL_DISABLE);
        break;

    case 'v':
        DG_DBG_set_dbg_level(DG_DBG_LVL_VERBOSE);
        break;

    case 'f':
        dg_arg->cfg_file = arg;
        break;

    case 'd':
        dg_nml_loop_dump_config();
        exit(0);
        break;

    case 't':
        if (!dg_nml_loop_get_int_arg(arg, &value))
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
        printf("Too many arguments\n");
        argp_usage(state);
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
BOOL dg_nml_loop_get_int_arg(const char* arg, long* value)
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
void dg_nml_loop_print_result()
{
    int index = 0;

    DG_NML_LOOP_CONFIG_T* p_cfg = dg_nml_loop_cfg_settings;

    while (memcmp(p_cfg, &dg_nml_loop_cfg_end, sizeof(dg_nml_loop_cfg_end)) != 0)
    {
        printf("tx_port=0x%02x, rx_port=0x%02x\n", p_cfg->port1, p_cfg->port2);
        printf("total_send=%d  ", dg_nml_loop_test[index][0].result.total_send);
        printf("failed_send=%d  ", dg_nml_loop_test[index][0].result.fail_send);
        printf("total_recv=%d  ", dg_nml_loop_test[index][0].result.total_recv);
        printf("failed_recv=%d  ", dg_nml_loop_test[index][0].result.fail_recv);
        printf("wrong_recv=%d\n\n", dg_nml_loop_test[index][0].result.wrong_recv);

        printf("tx_port=0x%02x, rx_port=0x%02x\n", p_cfg->port2, p_cfg->port1);
        printf("total_send=%d  ", dg_nml_loop_test[index][1].result.total_send);
        printf("failed_send=%d  ", dg_nml_loop_test[index][1].result.fail_send);
        printf("total_recv=%d  ", dg_nml_loop_test[index][1].result.total_recv);
        printf("failed_recv=%d  ", dg_nml_loop_test[index][1].result.fail_recv);
        printf("wrong_recv=%d\n\n", dg_nml_loop_test[index][1].result.wrong_recv);

        index++;
        p_cfg++;
    }
}

/*=============================================================================================*//**
@brief This function handle the SIGINT signal

@param[in] sig - The signal

@note
  - This function is a way to exit the dg_nml_loop
*//*==============================================================================================*/
void dg_nml_loop_exit_handler(int sig)
{
    dg_nml_loop_run = FALSE;
    DG_DBG_TRACE("got signaled: sig = %d", sig);
}

/*=============================================================================================*//**
@brief print out the default configuration
*//*==============================================================================================*/
void dg_nml_loop_dump_config()
{
    DG_NML_LOOP_CONFIG_T* p_cfg = dg_nml_loop_default_cfg;

    printf("#    default normal loop test configuration\n"
           "# for the port definition please ref diag loop spec\n"
           "#===============================================\n"
           "#  port1    port2    packet_size    pattern\n"
           "#===============================================\n");

    while (memcmp(p_cfg, &dg_nml_loop_cfg_end, sizeof(dg_nml_loop_cfg_end)) != 0)
    {
        printf("   0x%02x     0x%02x     %4d           0x%02x\n",
               p_cfg->port1, p_cfg->port2, p_cfg->size, p_cfg->pattern);

        p_cfg++;
    }

    printf("\n");
}

/*=============================================================================================*//**
@brief read the configuration file

@param[in] file - the configuration file name

@return the configuration buffer
*//*==============================================================================================*/
DG_NML_LOOP_CONFIG_T* dg_nml_loop_read_config(const char* file)
{
    static DG_NML_LOOP_CONFIG_T dg_nml_loop_default_cfg[DG_NML_LOOP_PORT_PAIR_MAX + 1];

    DG_NML_LOOP_CONFIG_T* ret = dg_nml_loop_default_cfg;

    memset(dg_nml_loop_default_cfg, 0, sizeof(dg_nml_loop_default_cfg));
    DG_DBG_TRACE("reading configuration file: %s\n", file);
    return ret;
}

/*=============================================================================================*//**
@brief print the last error string
*//*==============================================================================================*/
void dg_nml_loop_print_err_string()
{
    char* err_str = DG_DBG_get_err_string();
    if (err_str != NULL)
    {
        printf("%s\n", err_str);
        free(err_str);
    }
}

