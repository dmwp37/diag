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
#define DG_NML_LOOP_PORT_PAIR_MAX    100 /* the max port pair */

#define DG_LOOP_CFG_MAX_BUF_SIZE     256

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
static void    dg_nml_loop_print_result(int time);
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
DG_LOOP_TEST_T dg_nml_loop_test[DG_NML_LOOP_PORT_PAIR_MAX];

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
    int time  = 0;

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
            exit(1);
        }
    }
    else
    {
        dg_nml_loop_cfg_settings = dg_nml_loop_default_cfg;
    }



    /* init the test blocks */
    memset(dg_nml_loop_test, 0, sizeof(dg_nml_loop_test));

    /* prepare connections */
    p_cfg = dg_nml_loop_cfg_settings;
    index = 0;
    while (memcmp(p_cfg, &dg_nml_loop_cfg_end, sizeof(dg_nml_loop_cfg_end)) != 0)
    {
        if (!DG_LOOP_connect(p_cfg->port1, p_cfg->port2))
        {
            printf("failed to connect port1=0x%02x port2=0x%02x: %s\n",
                   p_cfg->port1, p_cfg->port2, DG_DBG_get_err_string());
            exit(1);
        }
        index++;
        p_cfg++;
    }

    /* start all the test thread */
    p_cfg = dg_nml_loop_cfg_settings;
    index = 0;
    while (memcmp(p_cfg, &dg_nml_loop_cfg_end, sizeof(dg_nml_loop_cfg_end)) != 0)
    {
        dg_nml_loop_test[index].tx_port = p_cfg->port1;
        dg_nml_loop_test[index].rx_port = p_cfg->port2;
        dg_nml_loop_test[index].pattern = p_cfg->pattern;
        dg_nml_loop_test[index].size    = p_cfg->size;
        dg_nml_loop_test[index].number  = DG_LOOP_RUN_IFINITE;

        if (!DG_LOOP_start_test(&dg_nml_loop_test[index]))
        {
            printf("failed to start loopback test tx_port=0x%02x rx_port=0x%02x: %s\n",
                   p_cfg->port1, p_cfg->port2, DG_DBG_get_err_string());
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

        printf("time frame: %ds\n", time++);
        sleep(1);
        dg_nml_loop_print_result(0);
    }

    /* stop the thread */
    p_cfg = dg_nml_loop_cfg_settings;
    index = 0;
    while (memcmp(p_cfg, &dg_nml_loop_cfg_end, sizeof(dg_nml_loop_cfg_end)) != 0)
    {
        DG_LOOP_stop_test(&dg_nml_loop_test[index]);
        index++;
        p_cfg++;
    }

    /* clean up thread */
    p_cfg = dg_nml_loop_cfg_settings;
    index = 0;
    while (memcmp(p_cfg, &dg_nml_loop_cfg_end, sizeof(dg_nml_loop_cfg_end)) != 0)
    {
        DG_LOOP_wait_test(&dg_nml_loop_test[index]);
        index++;
        p_cfg++;
    }

    printf("final result:\n");
    dg_nml_loop_print_result(time);

    DG_LOOP_disconnect_all();

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
@param[in] time - 0 for real time statistic of every second, other for final total statistic
*//*==============================================================================================*/
void dg_nml_loop_print_result(int time)
{
    int   index = 0;
    float pps;
    float bps;

    DG_LOOP_TEST_STATISTIC_T* result;
    DG_NML_LOOP_CONFIG_T*     p_cfg = dg_nml_loop_cfg_settings;

    while (memcmp(p_cfg, &dg_nml_loop_cfg_end, sizeof(dg_nml_loop_cfg_end)) != 0)
    {
        result = &dg_nml_loop_test[index].result;
        printf("tx_port=0x%02x, rx_port=0x%02x, ", p_cfg->port1, p_cfg->port2);
        printf("total send=%6d ", result->total_send);
        printf("recv=%6d, ", result->total_recv);

        if (time == 0)
        {
            static int last[DG_NML_LOOP_PORT_PAIR_MAX] = { 0 };

            pps = (float)(result->total_recv - last[index]) / 1000;
            bps = (float)(result->total_recv - last[index]) * dg_nml_loop_test[index].size *
                  8 / 1000000;
            last[index] = result->total_recv;
        }
        else
        {
            pps = (float)result->total_recv / time / 1000;
            bps = (float)result->total_recv / time * dg_nml_loop_test[index].size * 8 / 1000000;
        }
        printf("%5.2f Kpps, %6.2f Mbps. ", pps, bps);

        if (result->fail_send)
        {
            printf("failed_send=%d  ", result->fail_send);
        }
        if (result->fail_recv)
        {
            printf("failed_recv=%d  ", result->fail_recv);
        }
        if (result->wrong_recv)
        {
            printf("wrong_recv=%d  ", result->wrong_recv);
        }
        printf("\n");
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

@return the configuration buffer, NULL if error happened
*//*==============================================================================================*/
DG_NML_LOOP_CONFIG_T* dg_nml_loop_read_config(const char* file)
{
    static DG_NML_LOOP_CONFIG_T dg_nml_loop_default_cfg[DG_NML_LOOP_PORT_PAIR_MAX + 1];

    DG_NML_LOOP_CONFIG_T* ret = dg_nml_loop_default_cfg;

    FILE* fp = NULL;
    char  buf[DG_LOOP_CFG_MAX_BUF_SIZE];
    int   line  = 1;
    int   index = 0;
    int   port1;
    int   port2;
    int   size;
    int   pattern;

    memset(dg_nml_loop_default_cfg, 0, sizeof(dg_nml_loop_default_cfg));

    if (access(file, F_OK) != 0)
    {
        DG_DBG_ERROR("can't access %s, errno=%d(%m)", file, errno);
        return NULL;
    }
    else if ((fp = fopen(file, "r")) == NULL)
    {
        DG_DBG_ERROR("open %s failed, errno=%d(%m)", errno);
        return NULL;
    }

    DG_DBG_TRACE("reading configuration file: %s\n", file);

    while (fgets(buf, DG_LOOP_CFG_MAX_BUF_SIZE, fp) != NULL)
    {
        if (sscanf(buf, "%x%x%d%x", &port1, &port2, &size, &pattern) == 4)
        {
            DG_DBG_TRACE("got %d cfg:   port1=0x%02x port2=0x%02x size=%4d pattern=0x%02x\n",
                         index, port1, port1, size, pattern);

            if (DG_LOOP_port_to_index(port1) < 0)
            {
                printf("line %d: port1 invalid\n", line);
                ret = NULL;
                break;
            }

            if (DG_LOOP_port_to_index(port2) < 0)
            {
                printf("line %d: port2 invalid\n", line);
                ret = NULL;
                break;
            }

            if ((size < DG_LOOP_PACKET_SIZE_MIN) || (size > DG_LOOP_PACKET_SIZE_MAX))
            {
                printf("line %d: out range packet size: %d, min=%d, max=%d\n",
                       line, size, DG_LOOP_PACKET_SIZE_MIN, DG_LOOP_PACKET_SIZE_MAX);
                ret = NULL;
                break;
            }

            dg_nml_loop_default_cfg[index].port1   = (DG_LOOP_PORT_T)port1;
            dg_nml_loop_default_cfg[index].port2   = (DG_LOOP_PORT_T)port2;
            dg_nml_loop_default_cfg[index].size    = size;
            dg_nml_loop_default_cfg[index].pattern = (UINT8)pattern;

            index++;

            if (index >= DG_NML_LOOP_PORT_PAIR_MAX)
            {
                break;
            }
        }
        line++;
    }
    fclose(fp);

    if (index == 0)
    {
        printf("%s contains no valid configuration!\n", file);
        ret = NULL;
    }

    return ret;
}

