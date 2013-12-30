/*==================================================================================================

    Module Name:  dg_drv_util.c

    General Description: Utilities DIAG drivers

====================================================================================================

Revision History:
                            Modification     Tracking
Author                          Date          Number     Description of Changes
-------------------------   ------------    ----------   -------------------------------------------
Xudong Huang    - xudongh    2013/12/11     xxxxx-0000   Creation

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
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
        DG_DBG_ERROR("pthread_once() for error string key failed. errno=%d (%s)",
                     errno, strerror(errno));
    }
}


/*=============================================================================================*//**
@brief Sets an error string for the current driver thread

@param[in]     format     - printf style format string for error message
@param[in]     ...        - Variable argument, used to popluated format string

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

    if (str_len < 0)
    {
        DG_DBG_ERROR("vsnprintf() get string length failed. errno=%d (%s)",
                     errno, strerror(errno));
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

        vsnprintf(p_err_str, str_len + 1, format, args);
        DG_DBG_ERROR("Driver error string set to: %s", p_err_str);

        /* Set new error string */
        if (pthread_setspecific(dg_drv_util_error_string_key, p_err_str) != 0)
        {
            DG_DBG_ERROR("pthread_setspecific() set error string failed. errno=%d (%s)",
                         errno, strerror(errno));
        }
    }

    va_end(args);
}

/*=============================================================================================*//**
@brief Gets an error string for the current driver thread

@return - the current driver thread error string

@note
- reteive error string from thread specific data
- If no error string was set, will return NULL
*//*==============================================================================================*/
char* DG_DRV_UTIL_get_error_string()
{
    return pthread_getspecific(dg_drv_util_error_string_key);
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
        DG_DBG_ERROR("pthread_key_create() for driver error string failed. errno=%d (%s)",
                     errno, strerror(errno));
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

