/*==================================================================================================

    Module Name:  dg_port_cfg.c

    General Description: Implements the diag port config tool

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <argp.h>
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
#define DG_PORT_CFG_DEFAULT_PORT 0xff  /* all the data port */

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/* Used by main to communicate with parse_opt. */
typedef struct
{
    DG_LOOP_PORT_T port;
    DG_LOOP_NODE_T node;
    DG_LOOP_CFG_T  mode;
} DG_PORT_CFG_ARG_T;

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static error_t dg_port_cfg_arg_parse(int key, char* arg, struct argp_state* state);
static BOOL    dg_port_cfg_prepare_args(int argc, char** argv, DG_PORT_CFG_ARG_T* args);
static BOOL    dg_port_cfg_get_int_arg(const char* arg, long* value);
static void    dg_port_cfg_dump();
static int     dg_port_cfg_str_to_index(const char** str_table, int size, const char* str);
static BOOL    dg_port_cfg_config(DG_LOOP_PORT_T port, DG_LOOP_NODE_T node, DG_LOOP_CFG_T mode);

/*==================================================================================================
                                         GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/
static const char* dg_port_cfg_node_name[] =
{
    [DG_LOOP_NODE_FPGA] = "FPGA",
    [DG_LOOP_NODE_MAC]  = "MAC",
    [DG_LOOP_NODE_PHY]  = "PHY",
    [DG_LOOP_NODE_HDR]  = "HDR"
};

static const char* dg_port_cfg_mode_name[] =
{
    [DG_LOOP_CFG_NORMAL]   = "normal",
    [DG_LOOP_CFG_INTERNAL] = "internal",
    [DG_LOOP_CFG_EXTERNAL] = "external"
};

static DG_LOOP_PORT_T dg_port_cfg_data_ports[] =
{
    DG_LOOP_PORT_GE_0,
    DG_LOOP_PORT_GE_1,
    DG_LOOP_PORT_GE_2,
    DG_LOOP_PORT_GE_3,
    DG_LOOP_PORT_GE_4,
    DG_LOOP_PORT_GE_5,
    DG_LOOP_PORT_GE_6,
    DG_LOOP_PORT_GE_7,
    DG_LOOP_PORT_GE_8,
    DG_LOOP_PORT_GE_9,
    DG_LOOP_PORT_GE_10,
    DG_LOOP_PORT_GE_11,
    DG_LOOP_PORT_SFP_0,
    DG_LOOP_PORT_SFP_1,
    DG_LOOP_PORT_SFP_2,
    DG_LOOP_PORT_SFP_3,
    DG_LOOP_PORT_10GE_0,
    DG_LOOP_PORT_10GE_1,
    DG_LOOP_PORT_10GE_2,
    DG_LOOP_PORT_10GE_3
};

/*==================================================================================================
                                         GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Main function for dg_port_cfg application

@param[in] argc - Number of arguments
@param[in] argv - Array of each argument passed
*//*==============================================================================================*/
int main(int argc, char** argv)
{
    int ret = 0;

    DG_PORT_CFG_ARG_T args =
    {               /* Default values. */
        .port = DG_PORT_CFG_DEFAULT_PORT,
        .node = DG_LOOP_NODE_PHY,
        .mode = DG_LOOP_CFG_NORMAL,
    };

    if (!dg_port_cfg_prepare_args(argc, argv, &args))
    {
        exit(1);
    }

    if (args.port == DG_PORT_CFG_DEFAULT_PORT)
    {
        UINT8 i;
        for (i = 0; i < DG_ARRAY_SIZE(dg_port_cfg_data_ports); i++)
        {
            if (!dg_port_cfg_config(dg_port_cfg_data_ports[i], args.node, args.mode))
            {
                ret = 1;
            }
        }
    }
    else
    {
        if (!dg_port_cfg_config(args.port, args.node, args.mode))
        {
            ret = 1;
        }
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
@param[out] args - dg_port_cfg own argument

@return TRUE if no error
*//*==============================================================================================*/
BOOL dg_port_cfg_prepare_args(int argc, char** argv, DG_PORT_CFG_ARG_T* args)
{
    /* Program documentation. */
    char tool_doc[] =
        "\nthis tool is used for diag port configuration\n"
        "by default it will config all the data path port\n"
        "if no node specified, PHY is used as default\n"
        "valid MODE are: normal/internal/external\n"
        "valid NODE are: fpga/mac/phy/header";

    /* A description of the arguments we accept. */
    char args_doc[] = "MODE";

    /* The options we understand. */
    struct argp_option dg_options[] =
    {
        { "dump",    'd', 0,      0, "Dump the current configuration",        0 },
        { "port",    'p', "PORT", 0, "Select the port to config",             0 },
        { "node",    'n', "NODE", 0, "Select the node to config",             1 },
        { "verbose", 'v', 0,      0, "Produce verbose output",               -2 },
        { "quiet",   'q', 0,      0, "Don't produce any output",             -2 },
        { NULL,      0,   NULL,   0, NULL,                                    0 }
    };

    struct argp dg_argp =
    {
        dg_options, dg_port_cfg_arg_parse, args_doc, tool_doc, NULL, NULL, NULL
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
error_t dg_port_cfg_arg_parse(int key, char* arg, struct argp_state* state)
{
    /* Get the input argument from argp_parse, which we
       know is a pointer to our arguments structure. */
    DG_PORT_CFG_ARG_T* dg_arg = state->input;

    long value;

    switch (key)
    {
    case 'q':
        DG_DBG_set_dbg_level(DG_DBG_LVL_DISABLE);
        break;

    case 'v':
        DG_DBG_set_dbg_level(DG_DBG_LVL_VERBOSE);
        break;

    case 'd':
        dg_port_cfg_dump();
        exit(0);
        break;

    case 'p':
        if (!dg_port_cfg_get_int_arg(arg, &value))
        {
            return EINVAL;
        }
        else if (DG_LOOP_port_to_index((DG_LOOP_PORT_T)value) < 0)
        {
            printf("invalid port: %s\n", arg);
            return EINVAL;
        }
        else
        {
            dg_arg->port = (UINT8)value;
        }
        break;

    case 'n':
        value = dg_port_cfg_str_to_index(dg_port_cfg_node_name,
                                         DG_ARRAY_SIZE(dg_port_cfg_node_name),
                                         arg);
        if (value < 0)
        {
            printf("invalid node: %s\n", arg);
            return EINVAL;
        }
        else
        {
            dg_arg->node = value;
        }
        break;

    case ARGP_KEY_ARG:
        if (state->arg_num >= 1)
        {
            printf("Too many arguments\n");
            argp_usage(state);
        }

        value = dg_port_cfg_str_to_index(dg_port_cfg_mode_name,
                                         DG_ARRAY_SIZE(dg_port_cfg_mode_name),
                                         arg);
        if (value < 0)
        {
            printf("invalid mode: %s\n", arg);
            return EINVAL;
        }
        else
        {
            dg_arg->mode = value;
        }
        break;

    case ARGP_KEY_NO_ARGS:
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
BOOL dg_port_cfg_get_int_arg(const char* arg, long* value)
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
@brief print out the current configuration
*//*==============================================================================================*/
void dg_port_cfg_dump()
{
    printf("#    current port configuration\n"
           "# for the port definition please ref diag loop spec\n"
           "#===============================================\n"
           "#  port     node     mode\n"
           "#===============================================\n"
           "   0x01     N/A     normal\n"
           "   0x02     N/A     normal\n"
           "   0x03     FPGA    external\n"
           "   0x04     HDR     internal\n"
           "   0x10     MAC     internal\n"
           "   0x13     PHY     external\n");

    printf("\n");
}

/*=============================================================================================*//**
@brief convert string to the index in the lookup table

@param[in] str_table - the string lookup table
@param[in] size      - size of the string table
@param[in] str       - the string to lookup

@return the index of the string in the table, -1 if failed to match
*//*==============================================================================================*/
int dg_port_cfg_str_to_index(const char** str_table, int size, const char* str)
{
    int index = -1;
    int i;

    for (i = 0; i < size; i++)
    {
        if (strcasecmp(str, str_table[i]) == 0)
        {
            index = i;
            break;
        }
    }

    return index;
}

/*=============================================================================================*//**
@brief port configuration

@param[in]  port - the path to config on which port
@param[in]  node - where to loopback packet
@param[in]  mode - configuration mode

@return TRUE if success
*//*==============================================================================================*/
BOOL dg_port_cfg_config(DG_LOOP_PORT_T port, DG_LOOP_NODE_T node, DG_LOOP_CFG_T mode)
{
    BOOL ret = TRUE;
    if (!DG_LOOP_config(port, node, mode))
    {
        ret = FALSE;
        printf("config port=0x%02x  node=%s  mode=%s failed!\n",
               port, dg_port_cfg_node_name[node], dg_port_cfg_mode_name[mode]);

    }
    else
    {
        printf("config port=0x%02x  node=%s  mode=%s\n",
               port, dg_port_cfg_node_name[node], dg_port_cfg_mode_name[mode]);
    }

    return ret;
}

