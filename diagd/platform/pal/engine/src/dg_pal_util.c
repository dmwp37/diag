/*==================================================================================================

    Module Name:  dg_pal_util.c

    General Description: Implements the PAL layer utilities for the Test platform

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
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>
#include <signal.h>
#include "dg_defs.h"
#include "dg_dbg.h"
#include "dg_aux_engine.h"
#include "dg_pal_util.h"

/*==================================================================================================
                                           LOCAL CONSTANTS
==================================================================================================*/
#define DG_PAL_UTIL_CLIENT_TCPIP_PORT 11000         /**< DIAG TCP/IP Port Number */

/*==================================================================================================
                                            LOCAL MACROS
==================================================================================================*/
#define DG_PAL_UTIL_ASCII_SLASH       0x002F
#define DG_PAL_UTIL_DEFAULT_PATH      "/data/local/diag/"


/*==================================================================================================
                             LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                      LOCAL FUNCTION PROTOTYPES
==================================================================================================*/
static BOOL  dg_pal_util_lock_process(const char* pid_file);
static int   dg_pal_util_read_pid_file(const char* pid_file);
static BOOL  dg_pal_util_get_ext_ip_address(int* sock, struct sockaddr_in* addr);
static char* dg_pal_util_get_ext_itfc_name(void);

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
@brief Determine if the supplied path + filename uses an absolute path

@param[in] file_name - input file name to be handled

@return to indicate whether the file name is absolute or not

@note
 - 'file_name' must be NULL terminated
*//*==============================================================================================*/
DG_PAL_UTIL_ABSOLUTE_PATH_T DG_PAL_UTIL_check_absolute_path(W_CHAR* file_name)
{
    DG_PAL_UTIL_ABSOLUTE_PATH_T ret       = DG_PAL_UTIL_ABSOLUTE_PATH_ERROR;
    UINT32                      file_size = 0;

    if (file_name != NULL)
    {
        while (file_name[file_size] != 0x0000)
        {
            file_size++;
        }
        DG_DBG_TRACE("file_size =%d", file_size);
        if (file_size >= 1)
        {
            if (*file_name == DG_PAL_UTIL_ASCII_SLASH)
            {
                DG_DBG_TRACE("Filename is Absolute!");
                ret = DG_PAL_UTIL_ABSOLUTE_PATH_YES;
            }
            else
            {
                DG_DBG_TRACE("Filename is Relative!");
                ret = DG_PAL_UTIL_ABSOLUTE_PATH_NO;
            }
        }
    }
    return ret;
}

/*=============================================================================================*//**
@brief Gets the default DIAG 12M directory path for the platform

@param[out] default_path - pointer to contain the platform default file path

@return TRUE indicates success, FALSE indicate failure

@note
 - The calling function must allocate memory the size of #DG_PAL_UTIL_DEFAULT_DIR_MAX_SIZE for
   'default_path'.  The default path for each platform must not exceed that size
 - 'default_path' must be NULL terminated and the last non-NULL character must be the platform's
   directory delimiter, ie. '/' on Linux, '\' on Windows
 - 'default_path' is only valid if the return value indicates success
*//*==============================================================================================*/
BOOL DG_PAL_UTIL_get_default_path(W_CHAR* default_path)
{
    BOOL   ret = FALSE;
    char   default_path_ascii[DG_PAL_UTIL_DEFAULT_DIR_MAX_SIZE];
    UINT16 file_size = 0;
    UINT16 i         = 0;

    if (default_path != NULL)
    {
        file_size = sizeof(DG_PAL_UTIL_DEFAULT_PATH);
        DG_DBG_TRACE("size of default_path_ascii:=%d", file_size);
        DG_DBG_TRACE("name of default_path_ascii:=%s", DG_PAL_UTIL_DEFAULT_PATH);

        if (file_size <= DG_PAL_UTIL_DEFAULT_DIR_MAX_SIZE)
        {
            strcpy(default_path_ascii, DG_PAL_UTIL_DEFAULT_PATH);
            for (i = 0; i < file_size; i++)
            {
                default_path[i] = (W_CHAR)default_path_ascii[i];
            }
            ret = TRUE;
        }
    }
    return ret;
}

/*=============================================================================================*//**
@brief Determines if another DIAG process (besides the process invoking this function) exists

@return TRUE = another process exists, FALSE = no other process exists

@note
 - On error, FALSE should be returned
*//*==============================================================================================*/
BOOL DG_PAL_UTIL_is_diag_process_exist(void)
{
    const char* pid_file     = DG_CFG_PID_FILE; /* Storage location of current PID */
    BOOL        is_server_up = TRUE;
    int         stored_pid   = dg_pal_util_read_pid_file(pid_file);

    if ((stored_pid == 0) || (stored_pid == getpid()))
    {
        /* There was no stored PID, or, the active process is the locked process */
        is_server_up = FALSE;
    }
    else
    {
        /* The stored process is not the active process. Verify if the stored process
           is actually running, or if the stored PID was invalid. Do this by sending a kill
           with a signal of 0 (do nothing, just do error checking).  If errno is set to
           ESRCH, then the process is not active */
        if ((kill(stored_pid, 0)) && (errno == ESRCH))
        {
            is_server_up = FALSE;
        }
    }

    /* If the server is not up yet, we are the first instance. Lock the process */
    if (is_server_up == FALSE)
    {
        dg_pal_util_lock_process(pid_file);
    }
    return is_server_up;
}

/*=============================================================================================*//**
@brief Panics the DIAG process with the given reason

@param[in] reason - Printf-style panic reason format string
@param[in] ...    - Variable argument list

@note
 - This function is expected to not return
*//*==============================================================================================*/
void DG_PAL_UTIL_panic(const char* reason, ...)
{
    DG_COMPILE_UNUSED(reason);
    /** @todo Need to implement eventually... probably */
}

/*=============================================================================================*//**
@brief Determines if the specified socket is allowed to connect to DIAGs

@param[in] socket - The socket to verify

@return TRUE = socket is allowed, FALSE = socket is not allowed/failure

@note
 - This function shall return FALSE on any failures
*//*==============================================================================================*/
BOOL DG_PAL_UTIL_is_socket_allowed(int socket)
{
    struct sockaddr_storage sock_addr;
    socklen_t               sock_addr_len = sizeof(sock_addr);
    BOOL                    is_allowed    = FALSE;
    int                     i;
    int                     is_iface_match;
    int                     num_iface; /* Number of interfaces */

    struct sockaddr_storage peer_addr;
    struct sockaddr_in*     in_peer_addr  = (struct sockaddr_in*)&peer_addr;
    socklen_t               peer_addr_len = sizeof(peer_addr);

    /* Fixed size of 32 is taken arbitrary but checked with TAPI team and 32 should be more than
       enough for DIAG*/
    unsigned char buff[sizeof(struct ifreq) * 32];

    struct ifconf       ifc;
    struct sockaddr_in* iface_addr;

    /* Get local address of socket */
    if ((getsockname(socket, (struct sockaddr*)&sock_addr, &sock_addr_len)) != 0)
    {
        DG_DBG_ERROR("failed to getsockname");
    }
    else if (sock_addr.ss_family == AF_UNIX)
    {
        struct sockaddr_un* unix_sock = (struct sockaddr_un*)&sock_addr;

        DG_DBG_TRACE("Socket family is AF_UNIX, socket path is %s", unix_sock->sun_path);

        if (strcmp(unix_sock->sun_path, DG_CFG_INT_SOCKET) == 0)
        {
            is_allowed = TRUE;
        }
    }
    else
    {
        char*               iface_name;
        struct sockaddr_in* serv_addr_iface = (struct sockaddr_in*)&sock_addr;

        DG_DBG_TRACE("Socket name %s", inet_ntoa(serv_addr_iface->sin_addr));

        ifc.ifc_buf = (char*)buff;
        ifc.ifc_len = sizeof(buff);

        /* Get list of interfaces from the OS */
        if ((ioctl(socket, SIOCGIFCONF, (caddr_t)&ifc) != 0))
        {
            DG_DBG_ERROR("failed to SIOCGIFCONF");
        }
        else if ((iface_name = dg_pal_util_get_ext_itfc_name()) == NULL)
        {
            DG_DBG_ERROR("Failed to get interface name while checking allowed socket");
        }
        else
        {
            /* Number of interfaces returned by the OS */

            num_iface = ifc.ifc_len / sizeof(struct ifreq);
            DG_DBG_TRACE("Looking through %d interfaces (ifc_len=%d)...", num_iface, ifc.ifc_len);
            for (i = 0; i < num_iface; i++)
            {
                iface_addr = (struct sockaddr_in*)&ifc.ifc_req[i].ifr_addr;

                /* Does local address of socket match this interface? */
                is_iface_match = (iface_addr->sin_addr.s_addr == serv_addr_iface->sin_addr.s_addr);
                DG_DBG_TRACE("...Interface %s at %s%s", ifc.ifc_req[i].ifr_name, inet_ntoa(iface_addr->sin_addr),
                             is_iface_match ? " (match)" : "(no match found)");

                /* exit if one match is found */
                if (is_iface_match == 1)
                {
                    /* Ensure connection is on the USB interface */
                    if (strcmp(ifc.ifc_req[i].ifr_name, iface_name) != 0)
                    {
                        DG_DBG_TRACE("Interface %s not allowed", ifc.ifc_req[i].ifr_name);
                    }
                    /* Get IP address of peer, ensure peer & host addresses are not the same */
                    else if (getpeername(socket, (struct sockaddr*)&peer_addr, &peer_addr_len) != 0)
                    {
                        DG_DBG_ERROR("getpeername failed, errno = %d (%s)", errno, strerror(errno));
                    }
                    else if (in_peer_addr->sin_addr.s_addr == serv_addr_iface->sin_addr.s_addr)
                    {
                        DG_DBG_TRACE("Host address (%s) and peer address (%s) the same, not allowed",
                                     inet_ntoa(serv_addr_iface->sin_addr),
                                     inet_ntoa(in_peer_addr->sin_addr));
                    }
                    else
                    {
                        is_allowed = TRUE;
                    }
                    break;
                }
            }

            free(iface_name);
        }
    }

    DG_DBG_TRACE("is_allowed = %d", is_allowed);
    return is_allowed;
}

/*=============================================================================================*//**
@brief Create a socket for listening for internal diag clients

@param[out] sock - The created socket, set to -1 on failure

@return TRUE = if an internal DIAG socket is to be used, FALSE = feature not used
*//*==============================================================================================*/
BOOL DG_PAL_UTIL_create_int_diag_listen_sock(int* sock)
{
    struct sockaddr_un server;

    if ((*sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0)
    {
        DG_DBG_ERROR("Failed to create socket, errno = %d (%s)", errno, strerror(errno));
    }
    else
    {
        server.sun_family = AF_UNIX;
        strcpy(server.sun_path, DG_CFG_INT_SOCKET);
        unlink(server.sun_path);

        if (bind(*sock, (const struct sockaddr*)&server,
                 (sizeof(server.sun_family) + strlen(server.sun_path) + 1)) < 0)
        {
            DG_DBG_ERROR("Error binding to DIAG socket, errno = %d (%s)", errno, strerror(errno));
            close(*sock);
            *sock = -1;
        }
        else
        {
            DG_DBG_TRACE("Successfully setup internal DIAG socket %d", *sock);
        }
    }

    return TRUE;
}

/*=============================================================================================*//**
@brief Create a socket for listening for external diag clients

@param[out] sock - The created socket, set to -1 on failure

@return TRUE = if an external DIAG socket is to be used, FALSE = feature not used

@note
- In most cases, this should create a TCP/IP socket on port 11000 on the USBLan network interface
*//*==============================================================================================*/
BOOL DG_PAL_UTIL_create_ext_diag_listen_sock(int* sock)
{
    struct sockaddr_in serv_addr;
    int                sock_options;
    struct ifreq       ifr;
    struct sockaddr_in iface;
    char*              iface_name;

    /* Create the socket */
    *sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*sock == -1)
    {
        DG_DBG_ERROR("Error creating DIAG socket!");
    }
    /* Get the ip address to use */
    else if (dg_pal_util_get_ext_ip_address(sock, &iface) == FALSE)
    {
        close(*sock);
        *sock = -1;
    }
    else if ((iface_name = dg_pal_util_get_ext_itfc_name()) == NULL)
    {
        DG_DBG_ERROR("Failed to get interface name while creating ext socket");
    }
    else
    {
        /* Setup the socket for IPv4, bind to usblan address */
        serv_addr.sin_family = AF_INET;
        memcpy(&serv_addr.sin_addr, &iface.sin_addr, sizeof(serv_addr.sin_addr));
        serv_addr.sin_port = htons(DG_PAL_UTIL_CLIENT_TCPIP_PORT);

        memset(&ifr, 0, sizeof(ifr));
        snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", iface_name);

        /* Set value for socket options and set the options.
           SO_REUSEADDR = Allow re-use of local addresses
           TCP_NODELAY  = Disable Nagle algorithm, sends segments ASAP and prevents buffering
           SO_BINDTODEVICE = Bind to a specific device */
        sock_options = 1;
        if ((setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR,
                        &sock_options, sizeof(sock_options)) == -1) ||
            (setsockopt(*sock, IPPROTO_TCP, TCP_NODELAY,
                        &sock_options, sizeof(sock_options)) == -1))
        {
            DG_DBG_ERROR("Failed on setsockopt() on sock: %d, errno = %d (%s)",
                         *sock, errno, strerror(errno));
            close(*sock);
            *sock = -1;
        }
        else if (bind(*sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
        {
            DG_DBG_ERROR("Failed to bind to socket, errno = %d (%s)",
                         errno, strerror(errno));
            close(*sock);
            *sock = -1;
        }
        else
        {
            DG_DBG_TRACE("Succesfully created external listener DIAG socket: %d", *sock);
        }
    }

    return TRUE;
}

/*=============================================================================================*//**
@brief Create a socket listening for network interface updates

@param[out] sock - The created socket, set to -1 on failure

@return TRUE = if an update socket is to be used, FALSE = feature not used
*//*==============================================================================================*/
BOOL DG_PAL_UTIL_create_update_sock(int* sock)
{
#if 0
    int                fd = -1;
    struct sockaddr_nl sa_nl;
    int                on = 1;

    memset(&sa_nl, 0, sizeof(sa_nl));

    sa_nl.nl_family = AF_NETLINK;
    sa_nl.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR;

    *sock = socket(PF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (*sock > -1)
    {
        /* Enable passing credentials to prevent message spoofing */
        if (setsockopt(*sock, SOL_SOCKET, SO_PASSCRED, &on, sizeof(on)) == -1)
        {
            DG_DBG_ERROR("Failed to enable SO_PASSCRED, errno = %d (%s)", errno, strerror(errno));
            close(*sock);
            *sock = -1;
        }
        else if (bind(*sock, (struct sockaddr*)&sa_nl, sizeof(sa_nl)) == -1)
        {
            DG_DBG_ERROR("Failed to bind to netlink socket, errno = %d (%s)",
                         errno, strerror(errno));
            close(*sock);
            *sock = -1;
        }
    }

    return TRUE;
#else
    DG_COMPILE_UNUSED(sock);
    return FALSE;
#endif
}

/*=============================================================================================*//**
@brief Handles network interface update events

@param[in] fd - Socket which is listening for the events

@return The network event which occurred, if the feature is not supported or there was an error,
#DG_PAL_UTIL_SOCKET_UPDATE_NOOP should be returned.
*//*==============================================================================================*/
DG_PAL_UTIL_SOCKET_UPDATE_T DG_PAL_UTIL_handle_update_sock_event(int fd)
{
    DG_PAL_UTIL_SOCKET_UPDATE_T status = DG_PAL_UTIL_SOCKET_UPDATE_NOOP;

#if 0
    struct sockaddr_nl sock_nl;
    char               nl_buf[8192]; /* arbitary size */
    struct iovec       iov = { nl_buf, sizeof(nl_buf) };
    char               cred_msg[CMSG_SPACE(sizeof(struct ucred))];
    struct nlmsghdr*   nlmsg;
    struct msghdr      hdr = { &sock_nl, sizeof(sock_nl), &iov, 1, cred_msg, sizeof(cred_msg), 0 };
    ssize_t            bytes_read;

    /* Read the netlink socket, do not block */
    bytes_read = recvmsg(fd, &hdr, MSG_DONTWAIT);
    if (bytes_read > 0)
    {
        struct cmsghdr* cmsg;
        struct ucred*   cred;

        DG_DBG_TRACE("NL - Read %ld bytes from update socket.", bytes_read);
        /* Verify the message is legit, check the socket groups & pid */
        if (((sock_nl.nl_groups != RTMGRP_LINK) &&
             (sock_nl.nl_groups != RTMGRP_IPV4_IFADDR)) ||
            (sock_nl.nl_pid != 0))
        {
            DG_DBG_ERROR("Invalid socket message, groups = %d, pid = %d",
                         sock_nl.nl_groups, sock_nl.nl_pid);
        }
        /* Make sure the control message has the credentials */
        else if (((cmsg = CMSG_FIRSTHDR(&hdr)) == NULL) ||
                 (cmsg->cmsg_type != SCM_CREDENTIALS))
        {
            DG_DBG_ERROR("Missing or invalid control message");
        }
        else
        {
            /* Make sure the UID is root */
            cred = (struct ucred*)CMSG_DATA(cmsg);
            if (cred->uid != 0)
            {
                DG_DBG_ERROR("Invalid UID of %d", cred->uid);
            }
            else
            {
                /* Parse through all of the messages */
                for (nlmsg = (struct nlmsghdr*)nl_buf;
                     NLMSG_OK(nlmsg, (size_t)bytes_read);
                     nlmsg = NLMSG_NEXT(nlmsg, bytes_read))
                {
                    /* The end of multipart message. */
                    if (nlmsg->nlmsg_type == NLMSG_DONE)
                    {
                        DG_DBG_TRACE("NL - Hit end of multipart message");
                        break;
                    }

                    if (nlmsg->nlmsg_type == NLMSG_ERROR)
                    {
                        DG_DBG_ERROR("NL - Message error");
                    }
                    else
                    {
                        /* Parse any RTM new link messages */
                        status = dg_pal_util_parse_rtm_msg(nlmsg, (nlmsg->nlmsg_len - sizeof(*nlmsg)));

                        /* Intentionally do not break here.  Want to continue for loop in case there was
                           another message in queue */
                    }
                }
            }
        }
    }
#else
    DG_COMPILE_UNUSED(fd);
#endif
    return status;
}

/*=============================================================================================*//**
@brief Notifies glue layer of a client being added/removed

@param[in] is_add  - If a client is being added, TRUE = added, FALSE = removed
@param[in] num_int - New number of internal clients
@param[in] num_ext - New number of external clients
*//*==============================================================================================*/
void DG_PAL_UTIL_notify_client_update(BOOL is_add, int num_int, int num_ext)
{
    DG_DBG_TRACE("Client added = %d, num internal = %d, num_external = %d",
                 is_add, num_int, num_ext);

    /* when there is no client connected, close all the aux engine */
    if ((num_int == 0) && (num_ext == 0))
    {
        DG_AUX_ENGINE_close(DG_AUX_ENGINE_AUX_ID_ALL);
    }
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Reads a PID value out of the specified file

@param[in] pid_file - Filename to read the PID from

@return The PID value, 0 on error
*//*==============================================================================================*/
int dg_pal_util_read_pid_file(const char* pid_file)
{
    FILE* f;
    int   pid = 0;

    if ((f = fopen(pid_file, "r")) != NULL)
    {
        fscanf(f, "%d", &pid);
        fclose(f);
    }
    return pid;
}

/*=============================================================================================*//**
@brief Flags that the DIAG engine is active by storing the engine's PID value to a file

@param[in] pid_file - Filename to store the PID to

@return TRUE = success, FALSE = failure
*//*==============================================================================================*/
BOOL dg_pal_util_lock_process(const char* pid_file)
{
    BOOL  success = FALSE;
    FILE* fp      = NULL;
    int   fd      = -1;
    int   pid     = 0;

    /* Write PID to file, set PID file to read/write for owner, read by others */
    if ((fd = open(pid_file, (O_RDWR | O_CREAT), 0644)) != -1)
    {
        if ((fp = fdopen(fd, "r+")) != NULL)
        {
            if (flock(fd, (LOCK_EX | LOCK_NB)) < 0)
            {
                fscanf(fp, "%d", &pid);
                DG_DBG_TRACE("Can't lock process, lock held by pid %d", pid);
            }
            else
            {
                pid = getpid();
                if ((fprintf(fp, "%d", pid)) &&
                    (!fflush(fp)) &&
                    (!flock(fd, LOCK_UN)))
                {
                    success = TRUE;
                }
            }
            fclose(fp);
        }
        close(fd);
    }
    return success;
}

/*=============================================================================================*//**
@brief Parse RTM messages

@param[in] nlmsg - Netlink message to parse
@param[in] len   - Length of message payload

@return The network event which occurred, if the feature is not supported or there was an error,
#DG_PAL_UTIL_SOCKET_UPDATE_NOOP should be returned.
*//*==============================================================================================*/
#if 0
DG_PAL_UTIL_SOCKET_UPDATE_T dg_pal_util_parse_rtm_msg(struct nlmsghdr* nlmsg, int len)
{
    DG_PAL_UTIL_SOCKET_UPDATE_T status     = DG_PAL_UTIL_SOCKET_UPDATE_NOOP;
    int                         attrib_len = 0;
    struct rtattr*              attrib     = NULL;
    char*                       iface_name;

    if ((iface_name = dg_pal_util_get_ext_itfc_name()) == NULL)
    {
        DG_DBG_ERROR("Failed to get interface name while parsing rtm msg");
    }
    else
    {
        switch (nlmsg->nlmsg_type)
        {
        case RTM_NEWLINK:
            DG_DBG_TRACE("NL - message type is RTM_NEWLINK");
            /* Ensure the interface info is present */
            if ((size_t)len >= sizeof(struct ifinfomsg))
            {
                struct ifinfomsg* ifi = NLMSG_DATA(nlmsg);

                /* Get the pointer to the start of the attributes */
                attrib = IFLA_RTA(ifi);

                /* Get the length of all of the attributes */
                attrib_len = IFLA_PAYLOAD(nlmsg);

                /* Loop through all of the attributes, looking for the one we care about */
                while (RTA_OK(attrib, attrib_len))
                {
                    /* Check for the interface name */
                    if (attrib->rta_type == IFLA_IFNAME)
                    {
                        /* Ensure it is usblan interface */
                        if (strcmp(RTA_DATA(attrib), iface_name) == 0)
                        {
                            if ((ifi->ifi_flags & (IFF_UP | IFF_RUNNING)) &&
                                (ifi->ifi_change & (IFF_UP | IFF_RUNNING)))
                            {
                                DG_DBG_TRACE("TCP Available!");
                                status = DG_PAL_UTIL_SOCKET_UPDATE_EXT_ADD;
                                break;
                            }
                            else if (((ifi->ifi_flags & (IFF_UP | IFF_RUNNING)) == 0) &&
                                     (ifi->ifi_change & (IFF_UP | IFF_RUNNING)))
                            {
                                DG_DBG_TRACE("TCP Removed!");
                                status = DG_PAL_UTIL_SOCKET_UPDATE_EXT_REMOVE;
                                break;
                            }
                        }
                    }
                    attrib = RTA_NEXT(attrib, attrib_len);
                }
            }
            break;

        case RTM_NEWADDR:
            DG_DBG_TRACE("NL - message type is RTM_NEWADDR");
            /* Ensure the interface address is present */
            if ((size_t)len >= sizeof(struct ifaddrmsg))
            {
                struct ifaddrmsg* ifa = NLMSG_DATA(nlmsg);

                /* Get the pointer to the start of the attributes */
                attrib = IFA_RTA(ifa);

                /* Get the length of all of the attributes */
                attrib_len = IFA_PAYLOAD(nlmsg);

                /* Loop through all of the attributes, looking for the one we care about */
                while (RTA_OK(attrib, attrib_len))
                {
                    /* Check for the interface name */
                    if (attrib->rta_type == IFA_LOCAL)
                    {
                        /* Ensure it is usblan interface */
                        uint32_t ipaddr = htonl(*((uint32_t*)RTA_DATA(attrib)));
                        char     name[IFNAMSIZ];

                        if_indextoname(ifa->ifa_index, name);

                        if (strcmp(name, iface_name) == 0)
                        {
                            DG_DBG_TRACE("TCP Updated!");
                            DG_DBG_TRACE("%s is now %d.%d.%d.%d\n",
                                         name,
                                         (ipaddr >> 24) & 0xff,
                                         (ipaddr >> 16) & 0xff,
                                         (ipaddr >> 8) & 0xff,
                                         ipaddr & 0xff);
                            status = DG_PAL_UTIL_SOCKET_UPDATE_EXT_UPDATE;
                            break;
                        }
                    }
                    attrib = RTA_NEXT(attrib, attrib_len);
                }
            }

            break;

        default:
            DG_DBG_TRACE("NL - ignore NL message, type = %d", nlmsg->nlmsg_type);
            /* just ignore other RTM msg */
            break;
        }

        free(iface_name);
    }
    return status;
}
#endif

/*=============================================================================================*//**
@brief Finds the IP address to use for external clients

@param[in]  sock - Socket to use to get interface list
@param[out] addr - Address info interface to use for external clients, only valid if success
                   returned

@return TRUE = Success
*//*==============================================================================================*/
BOOL dg_pal_util_get_ext_ip_address(int* sock, struct sockaddr_in* addr)
{

    unsigned char       buff[sizeof(struct ifreq) * 32];   /* Fixed size of 32 is arbitrary */
    struct ifconf       ifc;
    struct sockaddr_in* iface_addr = NULL;
    int                 num_iface;
    int                 i;
    BOOL                is_success = FALSE;
    char*               iface_name;

    if (*sock == -1)
    {
        DG_DBG_ERROR("Invalid socket");
    }
    else
    {
        ifc.ifc_buf = (char*)buff;
        ifc.ifc_len = sizeof(buff);

        /* Get list of interfaces from the OS */
        if ((ioctl(*sock, SIOCGIFCONF, &ifc) != 0))
        {
            DG_DBG_ERROR("Failed to SIOCGIFCONF,  errno = %d (%s)", errno, strerror(errno));
        }
        else if ((iface_name = dg_pal_util_get_ext_itfc_name()) == NULL)
        {
            DG_DBG_ERROR("Failed to get interface name while getting ext IP address");
        }
        else
        {
            num_iface = ifc.ifc_len / sizeof(struct ifreq);
            DG_DBG_TRACE("Looking through %d interfaces (ifc_len=%d)...", num_iface, ifc.ifc_len);
            for (i = 0; i < num_iface; i++)
            {
                iface_addr = (struct sockaddr_in*)&ifc.ifc_req[i].ifr_addr;
                DG_DBG_TRACE("Enumerate interface %s, address = %s",  ifc.ifc_req[i].ifr_name,
                             inet_ntoa(iface_addr->sin_addr));
                /* Find the USB interface */
                if ((ifc.ifc_req[i].ifr_addr.sa_family == AF_INET) &&
                    (strcmp(ifc.ifc_req[i].ifr_name, iface_name) == 0))
                {
                    memcpy(addr, &ifc.ifc_req[i].ifr_addr, sizeof(struct sockaddr_in));
                    DG_DBG_TRACE("Found interface %s, address = %s",  ifc.ifc_req[i].ifr_name,
                                 inet_ntoa(iface_addr->sin_addr));
                    is_success = TRUE;
                    break;
                }
            }
        }
    }

    return is_success;
}

/*=============================================================================================*//**
@brief Gets the external client network interface name

@return Pointer to the interface name, NULL on error.  Calling function must free

@note
 - Calling function must free return pointer if non-null
*//*==============================================================================================*/
char* dg_pal_util_get_ext_itfc_name(void)
{
    char* name = "{A4856C8D-6E55-4FA7-ABE9-4981835C7CA3}";

    return name;
}

/** @} */
/** @} */

