/*==================================================================================================

    Module Name:  dg_loop.c

    General Description: Implements the dg_loop library

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include "dg_platform_defs.h"
#include "dg_loop.h"


/** @addtogroup libdg_loop
@{
*/
/*==================================================================================================
                                          LOCAL CONSTANTS
==================================================================================================*/
#define DG_LOOP_PORT_MAX (DG_LOOP_PORT_10GE_3 + 1)

/*==================================================================================================
                                           LOCAL MACROS
==================================================================================================*/
#ifdef DG_DEBUG
    #define DG_LOOP_TRACE(x ...) do { printf("DG_LOOP: "x); printf("\n"); } while (0)
#else
    #define DG_LOOP_TRACE(x ...)
#endif

#define DG_LOOP_ERROR(x ...) do { printf("DG_LOOP ERROR: "x); printf("\n"); } while (0)

#define DG_LOOP_SET_ERROR(err_str, x ...) \
    do \
    { \
        asprintf(err_str, x); \
        DG_LOOP_ERROR("%s", *(err_str)); \
    } while (0)


/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static BOOL dg_loop_check_port(DG_LOOP_PORT_T port, char** err_str);

/*==================================================================================================
                                         GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/
/** internal real file descriptor array for each ports */
static int dg_loop_port_fd[DG_LOOP_PORT_MAX] = { -1 };

/*==================================================================================================
                                         GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief open port for send/recv data

@param[in]  port    - the path to open on which port
@param[out] err_str - error string if any

@return the file descriptor, -1 if error happened

@note
- caller must free the *err_str
*//*==============================================================================================*/
int DG_LOOP_open(DG_LOOP_PORT_T port, char** err_str)
{
    int fd = -1;

    if (!dg_loop_check_port(port, err_str))
    {
        return -1;
    }

    if (dg_loop_port_fd[port] > 0)
    {
        /* already opened */
        return dg_loop_port_fd[port];
    }

    /* open the loop device to simulate the port */
    DG_LOOP_TRACE("open loop port 0x%02x", port);

    fd = port;

    dg_loop_port_fd[port] = fd;

    return fd;
}

/*=============================================================================================*//**
@brief close port

@param[in] fd - the fd that opened by DG_LOOP_open()
*//*==============================================================================================*/
void DG_LOOP_close(int fd)
{
    int   port    = fd;
    char* err_str = NULL;
    if (dg_loop_check_port(port, &err_str))
    {
        DG_LOOP_TRACE("close loop port 0x%02x", port);
        if (dg_loop_port_fd[port] > 0)
        {
            /* close the fd*/
            /* close(dg_loop_port_fd[port]); */
            dg_loop_port_fd[port] = -1;
        }
    }

    free(err_str);
}

/*=============================================================================================*//**
@brief send data over the loopback file descriptor

@param[in]  fd      - the fd that opened by DG_LOOP_open()
@param[in]  buf     - the buffer contains the data to send
@param[in]  len     - the data length need to send
@param[out] err_str - error string if any

@return the file descriptor, -1 if error happened

@note
- caller must free the *err_str
*//*==============================================================================================*/
BOOL DG_LOOP_send(int fd, UINT8* buf, UINT32 len, char** err_str)
{
    int port = fd;

    if (dg_loop_check_port(port, err_str))
    {
        return FALSE;
    }

    DG_LOOP_TRACE("send to port 0x%02x, buf=%p, len=%d", port, buf, len);

    return TRUE;
}

/*=============================================================================================*//**
@brief receive data from the loopback file descriptor

@param[in]  fd      - the fd that opened by DG_LOOP_open()
@param[out] buf     - the buffer to receive data
@param[in]  len     - the data length need to receive
@param[out] err_str - error string if any

@return the file descriptor, -1 if error happened

@note
- caller must free the *err_str
*//*==============================================================================================*/
BOOL DG_LOOP_recv(int fd, UINT8* buf, UINT32 len, char** err_str)
{
    int port = fd;

    if (dg_loop_check_port(port, err_str))
    {
        return FALSE;
    }

    DG_LOOP_TRACE("recv from port 0x%02x, buf=%p, len=%d", port, buf, len);

    return TRUE;
}

/*=============================================================================================*//**
@brief loopback node configuration

@param[in]  port    - the path to config on which port
@param[in]  node    - where to loopback packet
@param[in]  cfg     - configuration type
@param[out] err_str - error string if any

@return TRUE if success

@note
- if the port doesn't contains the node or doesn't support the configuration, FALSE will be returned
- caller must free the *err_str
*//*==============================================================================================*/
BOOL DG_LOOP_config(DG_LOOP_PORT_T port, DG_LOOP_NODE_T node, DG_LOOP_CFG_T cfg, char** err_str)
{
    if (dg_loop_check_port(port, err_str))
    {
        return FALSE;
    }

    if (node > DG_LOOP_NODE_PORT)
    {
        DG_LOOP_SET_ERROR(err_str, "Invalid node selection, node=%d", node);
        return FALSE;
    }

    if (cfg > DG_LOOP_CFG_EXTERNAL)
    {
        DG_LOOP_SET_ERROR(err_str, "Invalid configuration, cfg=%d", cfg);
        return FALSE;
    }

    DG_LOOP_TRACE("DG_LOOP_config() port=0x%02x, node=%d, cfg=%d", port, node, cfg);

    return TRUE;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/
/*=============================================================================================*//**
@brief check loopback port

@param[in]  port    - the loopback port
@param[out] err_str - error string

@return TRUE if success
*//*==============================================================================================*/
BOOL dg_loop_check_port(DG_LOOP_PORT_T port, char** err_str)
{
    switch (port)
    {
    case DG_LOOP_PORT_MGT:
    case DG_LOOP_PORT_HA:
    case DG_LOOP_PORT_WTB0_1:
    case DG_LOOP_PORT_WTB0_2:
    case DG_LOOP_PORT_WTB1_1:
    case DG_LOOP_PORT_WTB1_2:
    case DG_LOOP_PORT_GE_0:
    case DG_LOOP_PORT_GE_1:
    case DG_LOOP_PORT_GE_2:
    case DG_LOOP_PORT_GE_3:
    case DG_LOOP_PORT_GE_4:
    case DG_LOOP_PORT_GE_5:
    case DG_LOOP_PORT_GE_6:
    case DG_LOOP_PORT_GE_7:
    case DG_LOOP_PORT_GE_8:
    case DG_LOOP_PORT_GE_9:
    case DG_LOOP_PORT_GE_10:
    case DG_LOOP_PORT_GE_11:
    case DG_LOOP_PORT_SFP_0:
    case DG_LOOP_PORT_SFP_1:
    case DG_LOOP_PORT_SFP_2:
    case DG_LOOP_PORT_SFP_3:
    case DG_LOOP_PORT_10GE_0:
    case DG_LOOP_PORT_10GE_1:
    case DG_LOOP_PORT_10GE_2:
    case DG_LOOP_PORT_10GE_3:
        break;

    default:
        DG_LOOP_SET_ERROR(err_str, "Invalid Port selection, port=0x%02x", port);
        return FALSE;
    }

    return TRUE;
}

/** @} */

