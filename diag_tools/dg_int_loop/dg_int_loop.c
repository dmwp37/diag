/*==================================================================================================

    Module Name:  dg_int_loop.c

    General Description: Implements the diag internal loopback test

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <errno.h>
#include "dg_platform_defs.h"
#include "dg_loop.h"

/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/
const char* argp_program_version     = "dg_int_loop v1.0";
const char* argp_program_bug_address = "<SSD-SBU-JDiagDev@juniper.net>";

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/
#define DG_INT_LOOP_PORT_MAX            DG_LOOP_PORT_10GE_3
#define DG_INT_LOOP_DEFAULT_PORT        0   /* 0 means test for all data ports */
#define DG_INT_LOOP_DEFAULT_RUN_TIME    0   /* 0 means run the program for ever */
#define DG_INT_LOOP_DEFAULT_PACKET_SIZE 1024
#define DG_INT_LOOP_DEFAULT_PATTERN     0x5A


/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/
/* Used by main to communicate with parse_opt. */
typedef struct
{
    UINT8 pattern;  /* data pattern from argument*/
    UINT8 port;
    int   size;
    int   time;
} DG_INT_LOOP_ARG_T;

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static error_t dg_int_loop_arg_parse(int key, char* arg, struct argp_state* state);
static BOOL    dg_int_loop_prepare_args(int argc, char** argv, DG_INT_LOOP_ARG_T* args);

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
@brief Main function for dg_int_loop application

@param[in] argc - Number of arguments
@param[in] argv - Array of each argument passed
*//*==============================================================================================*/
int main(int argc, char** argv)
{
    DG_INT_LOOP_ARG_T args =
    {              /* Default values. */
        .pattern = DG_INT_LOOP_DEFAULT_PATTERN,
        .port    = DG_INT_LOOP_DEFAULT_PORT,
        .size    = DG_INT_LOOP_DEFAULT_PACKET_SIZE,
        .time    = DG_INT_LOOP_DEFAULT_RUN_TIME
    };

    if (!dg_int_loop_prepare_args(argc, argv, &args))
    {
        return 1;
    }

    printf("PATTERN     = 0x%02x\n"
           "RUN TIME    = %ds\n"
           "PACKET SIZE = %d bytes\n"
           "PORT        = 0x%02x\n",
           args.pattern, args.time, args.size, args.port);

    exit(0);
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Do argument check

@param[in] argc - Number of arguments
@param[in] argv - Array of each argument passed
@param[in] args - dg_int_loop argument struct

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
        { "port",   'p', "PORT", 0, "Select on which port to do the internal loopback test", 0 },
        { "size",   's', "SIZE", 0, "Set the packet size for each frame",                    0 },
        { "time",   't', "TIME", 0, "How long the program would run",                        0 },
        { 0 }
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

    long  value;
    char* p_ch;

    switch (key)
    {
    case 'p':
        errno = 0;
        value = (UINT8)strtol(arg, &p_ch, 0);
        if (errno != 0)
        {
            printf("can't recognize port: %s, errno=%d(%m)\n", arg, errno);
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

    case 's':
        errno = 0;
        value = strtol(arg, NULL, 0);
        if (errno != 0)
        {
            printf("can't recognize size: %s, errno=%d(%m)\n", arg, errno);
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
        errno = 0;
        value = strtol(arg, NULL, 0);
        if (errno != 0)
        {
            printf("can't recognize time: %s, errno=%d(%m)\n", arg, errno);
            return EINVAL;
        }
        else if (value == 0)
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

        errno = 0;
        value = (UINT8)strtol(arg, NULL, 16);
        if (errno != 0)
        {
            printf("can't recognize pattern: %s, errno=%d(%m)\n", arg, errno);
            return EINVAL;
        }
        else if ((value < 0) || (value > 0XFF))
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

