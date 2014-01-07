/*==================================================================================================

    Module Name:  dg_main.c

    General Description: Implements start-up/shutdown of DIAG engine

====================================================================================================

Revision History:
                            Modification     Tracking
Author                          Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
Xudong Huang    - xudongh    2013/12/11     xxxxx-0000   Creation

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <stdarg.h>
#include "dg_defs.h"
#include "dg_dbg.h"
#include "dg_aux_engine.h"
#include "dg_main.h"
#include "dg_client_comm.h"
#include "dg_pal_util.h"

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
static void dg_main_exit_handler(int sig);
static void dg_main_cleanup_engine(void);
static void dg_main_log_exit_reason(const char* format, ...);

/*==================================================================================================
                                          GLOBAL VARIABLES
==================================================================================================*/
BOOL DG_MAIN_engine_exit_flag = FALSE;

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/
static pthread_t dg_main_working_thread;

/*==================================================================================================
                                          GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Main function for DIAG engine

@param[in] handler_tbl - Pointer to the handler table
@param[in] argc        - Number of arguments
@param[in] argv        - Array of each argument passed

@return Success/failure of function

@todo The following code was removed from the LJ implementation, need to determine how it affects
      Symbian, if at all:
      - forking the main process
      - setsid();
      - Setting these siganls to SIG_IGN: SIGINT, SIGHUP, SIGCHLD, SIGTSTP, SIGTTOU, SIGTTIN
      - Power-up in suspend mode
*//*==============================================================================================*/
DG_DEFS_STATUS_T DG_MAIN_start_engine(const DG_DEFS_OPCODE_ENTRY_T* handler_tbl, int argc,
                                      char* argv[])
{
    DG_DEFS_STATUS_T status = DG_DEFS_STATUS_GEN_ERROR;

    DG_COMPILE_UNUSED(argc);
    DG_COMPILE_UNUSED(argv);
    /* Check to see if DIAG process is already running */
    if (DG_PAL_UTIL_is_diag_process_exist() == TRUE)
    {
        DG_DBG_ERROR("DIAG Process already running, exiting DIAG engine!");
        dg_main_log_exit_reason("DIAG Process already running");
    }
    else
    {
        struct sigaction actions;

        memset(&actions, 0, sizeof(actions));
        sigemptyset(&actions.sa_mask);
        actions.sa_flags   = 0;
        actions.sa_handler = dg_main_exit_handler;
        sigaction(SIGINT, &actions, NULL);

        /* Get default debug level gate */
        dg_dbg_level         = DG_PAL_DBG_load_dbg_lvl();
        dg_dbg_autolog_level = DG_PAL_DBG_load_autolog_lvl();

        DG_AUX_ENGINE_init_available_state();

        DG_CLIENT_COMM_set_handler_tbl(handler_tbl);

        if (!pthread_create(&dg_main_working_thread, NULL,
                            DG_CLIENT_COMM_client_connection_listener, (void*)NULL))
        {
            DG_DBG_TRACE("Created connection handler thread");
            status = DG_DEFS_STATUS_SUCCESS;

            /* Need to stop running without exiting the main thread and without causing
               target to wakeup from deep sleep mode.  This is done by waiting for the
               child thread to return                                                   */

            pthread_join(dg_main_working_thread, NULL);
        }
        else
        {
            DG_DBG_ERROR("could not create thread!");
        }
    }

    dg_main_cleanup_engine();

    return status;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief This function handle the SIGINT signal

@param[in] sig - The signal

@note
  - This function is a way to exit the diag daemon
*//*==============================================================================================*/
void dg_main_exit_handler(int sig)
{
    DG_MAIN_engine_exit_flag = TRUE;
    DG_DBG_TRACE("Diag daemon got signaled: sig = %d", sig);

    /*
     * here we need to tell the socket listener thread to stop listening
     * thus the diag main thread can have a chance to do the clean up work
     */
    pthread_kill(dg_main_working_thread, SIGUSR1);
}

/*=============================================================================================*//**
@brief Cleans up the DIAG engine for shutdown
*//*==============================================================================================*/
void dg_main_cleanup_engine(void)
{
    remove(DG_CFG_PID_FILE);
    remove(DG_CFG_INT_SOCKET);
    sync();
    DG_DBG_TRACE("DIAG Engine cleanup complete");
}

/*=============================================================================================*//**
@brief Logs the exit reason to a file if the DIAG app exit()s

@param[in] format - printf style exit reason
@param[in] ...    - variable arguments for exit reason
*//*==============================================================================================*/
void dg_main_log_exit_reason(const char* format, ...)
{
    /** @todo Need to determine if this function is still desired.  If so, need to determine
        where to store the file */
    DG_COMPILE_UNUSED(format);
#if 0
    FILE*   log_file;
    char*   exit_msg = NULL;
    va_list args;

    va_start(args, format);

    log_file = fopen("/ezx_user/12m/diag_exit.log", "w+");
    if (log_file != NULL)
    {
        fprintf(log_file, "DIAG Process called exit() @ %d msec\n", DG_TIMER_get_current_time());
        fprintf(log_file, "Exit reason: ");
        vfprintf(log_file, format, args);
        fprintf(log_file, "\n");
        fflush(log_file);
        fclose(log_file);
    }

    va_end(args);
#endif
}

