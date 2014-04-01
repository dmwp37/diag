/*==================================================================================================

    Module Name:  dg_loop.c

    General Description: Implements the dg_loop library

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "dg_platform_defs.h"
#include "dg_dbg.h"
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

/*==================================================================================================
                            LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                     LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
extern int  dg_loop_port_to_index(DG_LOOP_PORT_T port);
extern BOOL dg_loop_check_port(DG_LOOP_PORT_T port);

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

@return the file descriptor, -1 if error happened

@note
- if error happened, call DG_DBG_get_err_string() to get the last error
*//*==============================================================================================*/
int DG_LOOP_open(DG_LOOP_PORT_T port)
{
    int fd = -1;

    if (!dg_loop_check_port(port))
    {
        return -1;
    }

    if (dg_loop_port_fd[port] > 0)
    {
        /* already opened */
        return dg_loop_port_fd[port];
    }

    /* open the loop device to simulate the port */
    DG_DBG_TRACE("open loop port 0x%02x", port);

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
    int port = fd;

    if (dg_loop_check_port(port))
    {
        DG_DBG_TRACE("close loop port 0x%02x", port);
        if (dg_loop_port_fd[port] > 0)
        {
            /* close the fd*/
            /* close(dg_loop_port_fd[port]); */
            dg_loop_port_fd[port] = -1;
        }
    }
}

/*=============================================================================================*//**
@brief send data over the loopback file descriptor

@param[in]  fd      - the fd that opened by DG_LOOP_open()
@param[in]  buf     - the buffer contains the data to send
@param[in]  len     - the data length need to send

@return the file descriptor, -1 if error happened

@note
- if error happened, call DG_DBG_get_err_string() to get the last error
*//*==============================================================================================*/
BOOL DG_LOOP_send(int fd, UINT8* buf, UINT32 len)
{
    int port = fd;

    if (!dg_loop_check_port(port))
    {
        return FALSE;
    }

    DG_DBG_TRACE("send to port 0x%02x, buf=%p, len=%d", port, buf, len);

    return TRUE;
}

/*=============================================================================================*//**
@brief receive data from the loopback file descriptor

@param[in]  fd      - the fd that opened by DG_LOOP_open()
@param[out] buf     - the buffer to receive data
@param[in]  len     - the data length need to receive

@return the file descriptor, -1 if error happened

@note
- if error happened, call DG_DBG_get_err_string() to get the last error
*//*==============================================================================================*/
BOOL DG_LOOP_recv(int fd, UINT8* buf, UINT32 len)
{
    int port = fd;

    if (!dg_loop_check_port(port))
    {
        return FALSE;
    }

    DG_DBG_TRACE("recv from port 0x%02x, buf=%p, len=%d", port, buf, len);

    return TRUE;
}

/*=============================================================================================*//**
@brief print the last error string
*//*==============================================================================================*/
void DG_LOOP_print_err_string()
{
    char* err_str = DG_DBG_get_err_string();
    if (err_str != NULL)
    {
        printf("%s\n", err_str);
        free(err_str);
    }
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief check loopback port

@param[in]  port - the loopback port

@return TRUE if success
*//*==============================================================================================*/
BOOL dg_loop_check_port(DG_LOOP_PORT_T port)
{
    if (dg_loop_port_to_index(port) < 0)
    {
        DG_DBG_set_err_string("Invalid Port selection, port=0x%02x", port);
        return FALSE;
    }

    return TRUE;
}

/** @} */

