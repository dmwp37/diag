/*==================================================================================================

    Module Name:  dg_loop.c

    General Description: Implements the diag loopback test

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
#define DG_LOOP_DEFAULT_RUN_TIME -1  /* negtive means run the program for ever */
#define DG_LOOP_PORT_PAIR_MAX    100 /* the max port pair */

#define DG_LOOP_CFG_MAX_BUF_SIZE 256

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/* Used by main to communicate with parse_opt. */
typedef struct
{
    char* cfg_file; /* the config file */
    int   time;

    /* pair port args */
    int   tx_port;
    int   rx_port;
    int   size;
    UINT8 pattern;
} DG_LOOP_ARG_T;

typedef struct
{
    DG_LOOP_PORT_T tx_port;
    DG_LOOP_PORT_T rx_port;
    int            size;
    UINT8          pattern;
} DG_LOOP_CONFIG_T;

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static error_t dg_loop_arg_parse(int key, char* arg, struct argp_state* state);
static error_t dg_loop_child_arg_parse(int key, char* arg, struct argp_state* state);
static BOOL    dg_loop_prepare_args(int argc, char** argv, DG_LOOP_ARG_T* args);
static BOOL    dg_loop_get_int_arg(const char* arg, long* value);
static void    dg_loop_print_result(int time);
static void    dg_loop_exit_handler(int sig);
static void    dg_loop_dump_config();

static DG_LOOP_CONFIG_T* dg_loop_read_config(const char* file);
static DG_LOOP_CONFIG_T* dg_loop_detect_config();

/*==================================================================================================
                                         GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/
static BOOL dg_loop_run = TRUE;

static DG_LOOP_CONFIG_T dg_loop_cfg_end = { 0, 0, 0, 0 };

static DG_LOOP_CONFIG_T dg_loop_cfg[DG_LOOP_PORT_PAIR_MAX + 1] =
{
    { DG_LOOP_PORT_mgt,  DG_LOOP_PORT_ha,   1024, 0x5A },
    { DG_LOOP_PORT_ha,   DG_LOOP_PORT_mgt,  1024, 0xA5 },
    { DG_LOOP_PORT_wtb0, DG_LOOP_PORT_ge16, 1024, 0x5A },
    { DG_LOOP_PORT_ge16, DG_LOOP_PORT_wtb0, 1024, 0xA5 },
    { DG_LOOP_PORT_wtb1, DG_LOOP_PORT_ge24, 1024, 0x5A },
    { DG_LOOP_PORT_ge24, DG_LOOP_PORT_wtb1, 1024, 0xA5 },
    { DG_LOOP_PORT_ge0,  DG_LOOP_PORT_ge1,  1024, 0x5A },
    { DG_LOOP_PORT_ge1,  DG_LOOP_PORT_ge0,  1024, 0xA5 },
    { DG_LOOP_PORT_ge2,  DG_LOOP_PORT_ge3,  1024, 0x5A },
    { DG_LOOP_PORT_ge3,  DG_LOOP_PORT_ge2,  1024, 0xA5 },
    { DG_LOOP_PORT_ge4,  DG_LOOP_PORT_ge5,  1024, 0x5A },
    { DG_LOOP_PORT_ge5,  DG_LOOP_PORT_ge4,  1024, 0xA5 },
    { DG_LOOP_PORT_ge6,  DG_LOOP_PORT_ge7,  1024, 0x5A },
    { DG_LOOP_PORT_ge7,  DG_LOOP_PORT_ge6,  1024, 0xA5 },
    { DG_LOOP_PORT_ge8,  DG_LOOP_PORT_ge9,  1024, 0x5A },
    { DG_LOOP_PORT_ge9,  DG_LOOP_PORT_ge8,  1024, 0xA5 },
    { DG_LOOP_PORT_ge10, DG_LOOP_PORT_ge11, 1024, 0x5A },
    { DG_LOOP_PORT_ge11, DG_LOOP_PORT_ge10, 1024, 0xA5 },
    { DG_LOOP_PORT_ge12, DG_LOOP_PORT_ge13, 1024, 0x5A },
    { DG_LOOP_PORT_ge13, DG_LOOP_PORT_ge12, 1024, 0xA5 },
    { DG_LOOP_PORT_ge14, DG_LOOP_PORT_ge15, 1024, 0x5A },
    { DG_LOOP_PORT_ge15, DG_LOOP_PORT_ge14, 1024, 0xA5 },
    { DG_LOOP_PORT_xe0,  DG_LOOP_PORT_xe1,  1024, 0x5A },
    { DG_LOOP_PORT_xe1,  DG_LOOP_PORT_xe0,  1024, 0xA5 },
    { DG_LOOP_PORT_xe2,  DG_LOOP_PORT_xe3,  1024, 0x5A },
    { DG_LOOP_PORT_xe3,  DG_LOOP_PORT_xe2,  1024, 0xA5 },
    { 0, 0, 0, 0 }
};

static DG_LOOP_CONFIG_T* dg_loop_cfg_settings = NULL;
/* test control blocks */
static DG_LOOP_TEST_T dg_loop_test[DG_LOOP_PORT_PAIR_MAX];

/* default argument. */
static DG_LOOP_ARG_T dg_loop_args =
{
    .cfg_file = NULL,
    .time     = DG_LOOP_DEFAULT_RUN_TIME,
    .tx_port  = -1,
    .rx_port  = -1,
    .size     = 1024,
    .pattern  = 0x5A,
};

/*==================================================================================================
                                         GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Main function for dg_loop application

@param[in] argc - Number of arguments
@param[in] argv - Array of each argument passed
*//*==============================================================================================*/
int main(int argc, char** argv)
{
    int ret   = 0;
    int index = 0;
    int time  = 0;

    struct sigaction actions;

    DG_LOOP_CONFIG_T* p_cfg;

    /* prepare connections */
    p_cfg = dg_loop_cfg;
    index = 0;
    while (memcmp(p_cfg, &dg_loop_cfg_end, sizeof(dg_loop_cfg_end)) != 0)
    {
        if (!DG_LOOP_connect(p_cfg->tx_port, p_cfg->rx_port))
        {
            printf("failed to connect port1=0x%02x port2=0x%02x: %s\n",
                   p_cfg->tx_port, p_cfg->rx_port, DG_DBG_get_err_string());
            exit(1);
        }
        index++;
        p_cfg++;
    }

    if (!dg_loop_prepare_args(argc, argv, &dg_loop_args))
    {
        return 1;
    }

    /* open all the port first */
    for (index = 0; index < DG_LOOP_PORT_NUM; index++)
    {
        DG_LOOP_open(index);
    }

    /* signal setup */
    memset(&actions, 0, sizeof(actions));
    sigemptyset(&actions.sa_mask);
    actions.sa_flags   = 0;
    actions.sa_handler = dg_loop_exit_handler;
    sigaction(SIGINT, &actions, NULL);
    signal(SIGPIPE, SIG_IGN);

    printf("RUN TIME = %ds\n", dg_loop_args.time);

    if (dg_loop_args.cfg_file != NULL)
    {
        printf("CFG FILE = %s\n\n", dg_loop_args.cfg_file);

        if ((dg_loop_cfg_settings = dg_loop_read_config(dg_loop_args.cfg_file)) == NULL)
        {
            exit(1);
        }
    }
    else if (dg_loop_args.tx_port > 0 || dg_loop_args.rx_port > 0)
    {
        if (dg_loop_args.tx_port < 0)
        {
            printf("you need to specify a tx_port\n");
            exit(1);
        }
        if (dg_loop_args.rx_port < 0)
        {
            printf("you need to specify a rx_port\n");
            exit(1);
        }

        printf("TX_PORT  = %s\n"
               "RX_PORT  = %s\n"
               "PK_SIZE  = %d\n"
               "PATTERN  = 0x%02x\n\n",
               DG_LOOP_port_name(dg_loop_args.tx_port),
               DG_LOOP_port_name(dg_loop_args.rx_port),
               dg_loop_args.size, dg_loop_args.pattern);

        memset(dg_loop_cfg, 0, sizeof(dg_loop_cfg));
        dg_loop_cfg[0].tx_port = dg_loop_args.tx_port;
        dg_loop_cfg[0].rx_port = dg_loop_args.rx_port;
        dg_loop_cfg[0].size    = dg_loop_args.size;
        dg_loop_cfg[0].pattern = dg_loop_args.pattern;

        dg_loop_cfg_settings = dg_loop_cfg;
    }
    else
    {
        printf("Use detected settings\n\n");
        if ((dg_loop_cfg_settings = dg_loop_detect_config()) == NULL)
        {
            exit(1);
        }
    }

    if (dg_loop_args.time == 0)
    {
        exit(0);
    }

    /* init the test blocks */
    memset(dg_loop_test, 0, sizeof(dg_loop_test));

    /* start all the test thread */
    p_cfg = dg_loop_cfg_settings;
    index = 0;
    while (memcmp(p_cfg, &dg_loop_cfg_end, sizeof(dg_loop_cfg_end)) != 0)
    {
        dg_loop_test[index].tx_port = p_cfg->tx_port;
        dg_loop_test[index].rx_port = p_cfg->rx_port;
        dg_loop_test[index].pattern = p_cfg->pattern;
        dg_loop_test[index].size    = p_cfg->size;
        dg_loop_test[index].number  = DG_LOOP_RUN_IFINITE;

        if (!DG_LOOP_start_test(&dg_loop_test[index]))
        {
            printf("failed to start loopback test tx_port=%s rx_port=%s: %s\n",
                   DG_LOOP_port_name(p_cfg->tx_port),
                   DG_LOOP_port_name(p_cfg->rx_port),
                   DG_DBG_get_err_string());
            ret = 1;
        }
        index++;
        p_cfg++;
    }

    /* print out thread statistics */
    while (dg_loop_run)
    {
        if (dg_loop_args.time == 0)
        {
            break;
        }

        if (dg_loop_args.time > 0)
        {
            dg_loop_args.time--;
        }

        printf("\ntime frame: %ds\n", time++);
        sleep(1);
        dg_loop_print_result(0);
    }

    /* stop the thread */
    p_cfg = dg_loop_cfg_settings;
    index = 0;
    while (memcmp(p_cfg, &dg_loop_cfg_end, sizeof(dg_loop_cfg_end)) != 0)
    {
        DG_LOOP_stop_test(&dg_loop_test[index]);
        index++;
        p_cfg++;
    }

    /* clean up thread */
    p_cfg = dg_loop_cfg_settings;
    index = 0;
    while (memcmp(p_cfg, &dg_loop_cfg_end, sizeof(dg_loop_cfg_end)) != 0)
    {
        DG_LOOP_wait_test(&dg_loop_test[index]);
        index++;
        p_cfg++;
    }

    printf("final result:\n");
    dg_loop_print_result(time);

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
@param[out] args - dg_loop own argument

@return TRUE if no error
*//*==============================================================================================*/
BOOL dg_loop_prepare_args(int argc, char** argv, DG_LOOP_ARG_T* args)
{
    /* Program documentation. */
    char tool_doc[] =
        "\nthis tool is used for diag normal loopback test\n"
        "if no config file set, it will use the detected settings\n"
        "if no time argument specified, the program will run forever\n"
        "valid ports: mgt, ha, wtb[0-1], ge_[0-15], ge_16, ge_24, xe_[0-3]";

    /* The options we understand. */
    struct argp_option dg_options[] =
    {
        { "verbose", 'v', 0,      0, "Produce verbose output",                   0 },
        { "quiet",   'q', 0,      0, "Don't produce any output",                 0 },
        { "config",  'f', "FILE", 0, "Set config file for the normal loop test", 0 },
        { "dump",    'd', 0,      0, "Dump the detected settings",               0 },
        { "time",    't', "TIME", 0, "How long the program would run",           0 },
        { NULL,      0,   NULL,   0, NULL,                                       0 }
    };

    struct argp_option dg_child_options[] =
    {
        { "tx",      'x', "PORT",    0, "Set the tx port",         1 },
        { "rx",      'r', "PORT",    0, "Set the rx port",         1 },
        { "size",    's', "SIZE",    0, "Set packet size",         2 },
        { "pattern", 'p', "PATTERN", 0, "Set packet data pattern", 2 },
        { NULL,      0,   NULL,      0, NULL,                      0 }
    };

    struct argp dg_child_argp =
    {
        dg_child_options, dg_loop_child_arg_parse, NULL, NULL, NULL, NULL, NULL
    };

    struct argp_child dg_child[] =
    {
        { &dg_child_argp, 0, "pair port test", 0 },
        { NULL, 0, NULL, 0 }
    };

    struct argp dg_argp =
    {
        dg_options, dg_loop_arg_parse, NULL, tool_doc, dg_child, NULL, NULL
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
error_t dg_loop_arg_parse(int key, char* arg, struct argp_state* state)
{
    /* Get the input argument from argp_parse, which we
       know is a pointer to our arguments structure. */
    DG_LOOP_ARG_T* dg_arg = state->input;

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
        dg_loop_dump_config();
        exit(0);
        break;

    case 't':
        if (!dg_loop_get_int_arg(arg, &value))
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

    case ARGP_KEY_INIT:
        /* init the child input */
        state->child_inputs[0] = dg_arg;
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
@brief argp parser function

*//*==============================================================================================*/
error_t dg_loop_child_arg_parse(int key, char* arg, struct argp_state* state)
{
    DG_LOOP_ARG_T* dg_arg = state->input;
    long           value;

    switch (key)
    {
    case 'x':
        if ((value = DG_LOOP_get_port(arg)) < 0)
        {
            printf("invalid tx_port: %s\n", arg);
            return EINVAL;
        }
        else
        {
            dg_arg->tx_port = (DG_LOOP_PORT_T)value;
        }
        break;

    case 'r':
        if ((value = DG_LOOP_get_port(arg)) < 0)
        {
            printf("invalid rx_port: %s\n", arg);
            return EINVAL;
        }
        else
        {
            dg_arg->rx_port = (DG_LOOP_PORT_T)value;
        }
        break;

    case 's':
        if (!dg_loop_get_int_arg(arg, &value))
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

    case 'p':
        if (!dg_loop_get_int_arg(arg, &value))
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
BOOL dg_loop_get_int_arg(const char* arg, long* value)
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
void dg_loop_print_result(int time)
{
    int   index = 0;
    float pps;
    float bps;

    DG_LOOP_TEST_STATISTIC_T* result;
    DG_LOOP_CONFIG_T*         p_cfg = dg_loop_cfg_settings;

    while (memcmp(p_cfg, &dg_loop_cfg_end, sizeof(dg_loop_cfg_end)) != 0)
    {
        result = &dg_loop_test[index].result;
        printf("tx_port=%-5s  rx_port=%-5s  ",
               DG_LOOP_port_name(p_cfg->tx_port),
               DG_LOOP_port_name(p_cfg->rx_port));
        printf("send=%-10" PRIu64 " ", result->total_send);
        printf("recv=%-10" PRIu64 " ", result->total_recv);

        if (time == 0)
        {
            static int last[DG_LOOP_PORT_PAIR_MAX] = { 0 };

            pps = (float)(result->total_recv - last[index]) / 1000;
            bps = (float)(result->total_recv - last[index]) * dg_loop_test[index].size *
                  8 / 1000000;
            last[index] = result->total_recv;
        }
        else
        {
            pps = (float)result->total_recv / time / 1000;
            bps = (float)result->total_recv / time * dg_loop_test[index].size * 8 / 1000000;
        }
        printf("%5.2f Kpps  %6.2f Mbps ", pps, bps);

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
  - This function is a way to exit the dg_loop
*//*==============================================================================================*/
void dg_loop_exit_handler(int sig)
{
    dg_loop_run = FALSE;
    DG_DBG_TRACE("got signaled: sig = %d", sig);
}

/*=============================================================================================*//**
@brief print out the detected configuration
*//*==============================================================================================*/
void dg_loop_dump_config()
{
    DG_LOOP_CONFIG_T* p_cfg = dg_loop_detect_config();

    if (p_cfg == NULL)
    {
        return;
    }

    printf("#    default normal loop test configuration\n"
           "#===============================================\n"
           "#  tx_port  rx_port  packet_size  pattern\n"
           "#===============================================\n");

    while (memcmp(p_cfg, &dg_loop_cfg_end, sizeof(dg_loop_cfg_end)) != 0)
    {
        printf("   %-5s    %-5s    %4d         0x%02x\n",
               DG_LOOP_port_name(p_cfg->tx_port),
               DG_LOOP_port_name(p_cfg->rx_port),
               p_cfg->size, p_cfg->pattern);

        p_cfg++;
    }

    printf("\n");
}

/*=============================================================================================*//**
@brief read the configuration file

@param[in] file - the configuration file name

@return the configuration buffer, NULL if error happened
*//*==============================================================================================*/
DG_LOOP_CONFIG_T* dg_loop_read_config(const char* file)
{
    DG_LOOP_CONFIG_T* ret = dg_loop_cfg;

    FILE* fp = NULL;
    char  buf[DG_LOOP_CFG_MAX_BUF_SIZE];
    int   line  = 1;
    int   index = 0;
    char  tx_port_name[16];
    char  rx_port_name[16];
    int   tx_port;
    int   rx_port;
    int   size;
    int   pattern;

    memset(dg_loop_cfg, 0, sizeof(dg_loop_cfg));

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
        if (sscanf(buf, "%6s%6s%d%x", tx_port_name, rx_port_name, &size, &pattern) == 4)
        {
            DG_DBG_TRACE("got %d cfg:   port1=%s port2=%s size=%4d pattern=0x%02x\n",
                         index, tx_port_name, rx_port_name, size, pattern);

            if ((tx_port = DG_LOOP_get_port(tx_port_name)) < 0)
            {
                printf("line %d: port1 invalid\n", line);
                ret = NULL;
                break;
            }

            if ((rx_port = DG_LOOP_get_port(rx_port_name)) < 0)
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

            dg_loop_cfg[index].tx_port = (DG_LOOP_PORT_T)tx_port;
            dg_loop_cfg[index].rx_port = (DG_LOOP_PORT_T)rx_port;
            dg_loop_cfg[index].size    = size;
            dg_loop_cfg[index].pattern = (UINT8)pattern;

            index++;

            if (index >= DG_LOOP_PORT_PAIR_MAX)
            {
                break;
            }
        }
        line++;
    }
    fclose(fp);

    if (index == 0)
    {
        printf("%s contains invalid configuration!\n", file);
        ret = NULL;
    }

    return ret;
}

/*=============================================================================================*//**
@brief detect the configuration

@return the configuration buffer, NULL if error happened
*//*==============================================================================================*/
DG_LOOP_CONFIG_T* dg_loop_detect_config()
{
    DG_LOOP_CONFIG_T*    p_cfg = dg_loop_cfg;
    DG_LOOP_PORT_PAIR_T* p_pair;
    int                  index = 0;

    memset(dg_loop_cfg, 0, sizeof(dg_loop_cfg));

    /* detect the connection */
    if ((p_pair = DG_LOOP_auto_detect()) == NULL)
    {
        printf("can't detect connections\n");
        return NULL;
    }

    for (index = 0; index < DG_LOOP_PORT_NUM; index++)
    {
        if (p_pair[index].rx_port != 0xFF)
        {
            p_cfg->tx_port = p_pair[index].tx_port;
            p_cfg->rx_port = p_pair[index].rx_port;
            p_cfg->size    = dg_loop_args.size;
            p_cfg->pattern = dg_loop_args.pattern;

            p_cfg++;
        }
    }

    return dg_loop_cfg;
}

