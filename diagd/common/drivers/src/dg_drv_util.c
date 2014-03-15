/*==================================================================================================

    Module Name:  dg_drv_util.c

    General Description: Utilities DIAG drivers

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include <stdarg.h>
#include <errno.h>
#include <pthread.h>
#include "dg_dbg.h"
#include "dg_drv_util.h"


/** @addtogroup drv_util
@{
*/

/** @addtogroup drv_util_error_string
@{
*/

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
void dg_drv_util_create_error_string_key();
void dg_drv_util_free_error_string(void* err_str);

/*==================================================================================================
                                         GLOBAL VARIABLES
==================================================================================================*/

/*==================================================================================================
                                          LOCAL VARIABLES
==================================================================================================*/
static pthread_key_t  dg_drv_util_error_string_key;
static pthread_once_t dg_drv_util_key_once = PTHREAD_ONCE_INIT;

/*==================================================================================================
                                         GLOBAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Initializes error string for the current driver thread

@note
- the init would only happen once for threads, please see the pthread_once manual
*//*==============================================================================================*/
void DG_DRV_UTIL_init_error_string()
{
    int ret = pthread_once(&dg_drv_util_key_once, dg_drv_util_create_error_string_key);

    if (ret != 0)
    {
        DG_DBG_ERROR("pthread_once() for error string key failed. errno=%d(%m)", errno);
    }
}


/*=============================================================================================*//**
@brief Sets an error string for the current driver thread

@param[in]     format     - printf style format string for error message
@param[in]     ...        - Variable argument, used to populated format string

@note
- If err_string already has a string, the old one will be freed
- Calling function responsible for freeing
*//*==============================================================================================*/
void DG_DRV_UTIL_set_error_string(const char* format, ...)
{
    va_list args;       /* Variable arg list */
    char*   p_err_str;  /* pointer to the error string */
    int     str_len;    /* error string length*/

    /* Init variable arg list */
    va_start(args, format);
    str_len = vsnprintf(NULL, 0, format, args);
    va_end(args);

    if (str_len < 0)
    {
        DG_DBG_ERROR("vsnprintf() get string length failed. errno=%d(%m)", errno);
    }
    else if ((p_err_str = (char*)malloc(str_len + 1)) == NULL)
    {
        DG_DBG_ERROR("Failed to create driver error string");
    }
    else
    {
        char* old_err_str = (char*)pthread_getspecific(dg_drv_util_error_string_key);

        /* Free any existing error string */
        if (old_err_str != NULL)
        {
            DG_DBG_TRACE("Overwriting error string: %s", old_err_str);
            free(old_err_str);
        }

        va_start(args, format);
        vsnprintf(p_err_str, str_len + 1, format, args);
        va_end(args);

        DG_DBG_ERROR("Driver error string set to: %s", p_err_str);

        /* Set new error string */
        if (pthread_setspecific(dg_drv_util_error_string_key, p_err_str) != 0)
        {
            DG_DBG_ERROR("pthread_setspecific() set error string failed. errno=%d(%m)", errno);
        }
    }
}

/*=============================================================================================*//**
@brief Gets an error string for the current driver thread

@return - the current driver thread error string

@note
- get error string from thread specific data
- If no error string was set, will return NULL
*//*==============================================================================================*/
char* DG_DRV_UTIL_get_error_string()
{
    return pthread_getspecific(dg_drv_util_error_string_key);
}

/*=============================================================================================*//**
@brief Execute a system command and put the output to a buffer

@param[in]  cmd   - the command string we need to execute
@param[out] p_out - point to the output string buffer

@return - TRUE if the command run successfully

@note
- the p_out point to heap buffer for containing the string output of the command
- when this function returns FALSE, the p_out points to the command failure out put if any
- If there is no output p_out will be set to NULL
- Calling function responsible for freeing p_out buffer
*//*==============================================================================================*/
BOOL DG_DRV_UTIL_system(const char* cmd, char** p_out)
{
    BOOL  ret = FALSE;
    FILE* fp  = NULL;
    int   sys_ret;
    int   exit_val;

    *p_out = NULL;

    DG_DBG_TRACE("Execute command: %s", cmd);
    if ((fp = popen(cmd, "r")) == NULL)
    {
        DG_DRV_UTIL_set_error_string("failed to run: %s, errno=%d(%m)", cmd, errno);
    }
    else
    {
        char*  line = NULL;
        char*  buf  = NULL;
        size_t len;

        int read       = 0;
        int cur_pos    = 0;
        int total_read = 1; /* alloc extra 1 byte for the \0 */

        while ((read = getline(&line, &len, fp)) != -1)
        {
            char* new_buf = NULL;
            /* dump the output */
            DG_DBG_TRACE("--> %s", line);

            total_read += read;

            if ((new_buf = realloc(buf, total_read)) == NULL)
            {
                DG_DBG_ERROR("Can't realloc command output buffer. size=%d", total_read);
                free(buf);
                buf = NULL;
                break;
            }
            else
            {
                buf = new_buf;
            }

            /* append the data to buf */
            memcpy(buf + cur_pos, line, read);
            /* leave the last one byte */
            cur_pos = total_read - 1;
        }

        free(line);
        sys_ret  = pclose(fp);
        exit_val = WEXITSTATUS(sys_ret);
        fp       = NULL;

        if (exit_val != 0)
        {
            DG_DRV_UTIL_set_error_string("%s, sys_ret = %d, exit_val = %d",
                                         cmd, sys_ret, exit_val);
        }
        else
        {
            DG_DBG_TRACE("Successfully Execute command: %s", cmd);
            if (buf != NULL)
            {
                /* NULL terminator */
                buf[total_read - 1] = '\0';

                *p_out = buf;
            }
            ret = TRUE;
        }
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/*=============================================================================================*//**
@brief Create error string key for threads specific data

@note
- this function register a free error string hanldler when thread exit
- you should free the old error string before set a new error string
*//*==============================================================================================*/
void dg_drv_util_create_error_string_key()
{
    int ret = pthread_key_create(&dg_drv_util_error_string_key, dg_drv_util_free_error_string);

    if (ret != 0)
    {
        DG_DBG_ERROR("pthread_key_create() for driver error string failed. errno=%d(%m)", errno);
    }
}

/*=============================================================================================*//**
@brief Initializes error string for the current driver thread

@note
- when thread exit automatically call this function to free the error string
*//*==============================================================================================*/
void dg_drv_util_free_error_string(void* err_str)
{
    DG_DBG_TRACE("Free error string for thread %p", pthread_self());
    free(err_str);
}

/** @} */
/** @} */

