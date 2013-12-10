/*==================================================================================================

    Module Name:  dg_main_task.c

    General Description: The main task for DIAG application, invokes the common engine

====================================================================================================

Revision History:
                            Modification     Tracking
Author                          Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
Xudong Huang    - xudongh    2013/12/11     xxxxx-0000   Creation

====================================================================================================
                                            INCLUDE FILES
==================================================================================================*/
#include "dg_main.h"
#include "dg_handler_table.h"
#include "dg_dbg.h"

#include <stdio.h>
#include <unistd.h>


/*==================================================================================================
                                           LOCAL CONSTANTS
==================================================================================================*/

/*==================================================================================================
                                            LOCAL MACROS
==================================================================================================*/

/*==================================================================================================
                             LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==================================================================================================*/

/*==================================================================================================
                                      LOCAL FUNCTION PROTOTYPES
==================================================================================================*/

static int dg_main_task_switch_to_user(void);
static int dg_check_for_stale_pid_file(void);

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
@brief Main function for DIAG application

@param[in] argc - Number of arguments
@param[in] argv - Array of each argument passed

@return Success/failure of engine, not used currently
*//*==============================================================================================*/
int main(int argc, char* argv[])
{
    DG_DEFS_STATUS_T status;
    int              rtn_val = 0;

    /* need root priviledge to check the proc file system */
    rtn_val = dg_check_for_stale_pid_file();

    if (rtn_val == 0)
    {
        /* call function to switch diag from root to user */
        if (dg_main_task_switch_to_user() < 0)
        {
            DG_DBG_ERROR("diag.security_critical_error!");
            rtn_val = -1;
        }
        else
        {
            DG_DBG_TRACE("Starting DIAG Task!");
            status = DG_MAIN_start_engine(DG_HANDLER_TABLE_data, argc, argv);

            DG_DBG_TRACE("DIAG Task exit, status = %d", status);
        }
    }

    return rtn_val;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief switches diag task from root mode to user mode

@return int with negative for error
*//*=============================================================================================*/
static int dg_main_task_switch_to_user(void)
{
#if 0
    int status;

    status = prctl(PR_SET_KEEPCAPS, 1, 0, 0, 0);
    if (status >= 0)
    {
        status = setuid(AID_DIAG);
        if (status >= 0)
        {
            struct __user_cap_header_struct header;
            struct __user_cap_data_struct   cap;

            header.version  = _LINUX_CAPABILITY_VERSION;
            header.pid      = 0;

            cap.effective   = cap.permitted = (1 << CAP_SYS_BOOT) |
                                              (1 << CAP_NET_ADMIN) |
                                              (1 << CAP_NET_RAW) |
                                              (1 << CAP_SYS_TIME);
            cap.inheritable = 0;

            status          = capset(&header, &cap);
        }
    }

    return status;
#else
    return 0;
#endif
}

/*=============================================================================================*//**
@brief checks if stale pid file exists and removes it

@return int with negative for error, 0 - success
*//*=============================================================================================*/
static int dg_check_for_stale_pid_file(void)
{
    const char* pid_file           = "/tmp/ap_diag.pid"; /* Storage location of current PID */
    const char* identity_sig       = "/system/bin/diag";
    char        identity_file[256] = { 0 };
    char        cmdline[256];
    FILE*       f;
    int         stored_pid         = 0;

    if ((f = fopen(pid_file, "r")) != NULL)
    {
        fscanf(f, "%d", &stored_pid);
        fclose(f);
    }

    DG_DBG_TRACE("stored_pid is %d", stored_pid);

    /* no file or the same PID then nothing to care about */
    if ((stored_pid != 0) && (stored_pid != getpid()))
    {
        snprintf(identity_file, sizeof(identity_file), "/proc/%d/cmdline", stored_pid);
        /* process with the same pid exists */
        if (!access(identity_file, F_OK))
        {
            FILE* f;
            if ((f = fopen(identity_file, "r")) != NULL)
            {
                fgets(cmdline, sizeof(cmdline), f);
                fclose(f);
                if (!memcmp(identity_sig, cmdline, strlen(identity_sig)))
                {
                    /* second instance of DIAG is being launched */
                    return -1;
                }
            }
        }
        /* pid file exists while process does not - stale pid file */
        remove(pid_file);
        sync();
        sleep(1);
        DG_DBG_TRACE("remove stale pid file with pid %d, launcher is %s", stored_pid, cmdline);
    }

    return 0;
}

