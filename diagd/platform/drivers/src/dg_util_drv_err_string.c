/*==================================================================================================
 
    Module Name:  dg_util_drv_err_string.c

    General Description: Test platform utility file for setting driver error strings
    
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

#include "dg_util_drv_err_string.h"
#include "dg_dbg.h"

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
@brief Sets an error string

@param[in,out] err_string - Pointer to pointer to contain the error string
@param[in]     format     - printf style format string for error message
@param[in]     ...        - Variable argument, used to popluated format string

@note
- If err_string already has a string, the old one will be freed
- Calling function responsible for freeing
*//*==============================================================================================*/
void DG_UTIL_DRV_ERR_STRING_set(char **err_string, const char *format, ...)
{
    int max_size = 1000; /* Arbitary max size */
    va_list  args;       /* Variable arg list */

    /* Init variable arg list */
    va_start (args, format);

    /* Free any existing error string */
    if (*err_string != NULL)
    {
        DG_DBG_TRACE("Overwriting error string: %s", *err_string);
        free(*err_string);
        *err_string = NULL;
    }

    if ((*err_string = (char *) malloc(max_size)) == NULL) 
    {
        DG_DBG_ERROR("Failed to create driver error string");
    }
    else
    {
        /* Set new error string */
        memset(*err_string, 0x00, max_size);
        vsnprintf(*err_string, max_size, format, args);
    }

    va_end(args);
}

/*=============================================================================================*//**
@brief Gets an error string

@param[in]     max_length  - Max size of dest_string buffer
@param[out]    dest_string - Buffer to copy error string to
@param[in,out] src_string  - Error string source

@note
- src_string will be freed if its not NULL
- If src_string is NULL, an empty string is placed in dest_string
*//*==============================================================================================*/
DG_CMN_DRV_ERR_T DG_UTIL_DRV_ERR_STRING_get(UINT32 max_length, char *dest_string, char** src_string)
{
    if (*src_string  == NULL)
    {
        dest_string[0] = '\0';
    }
    else
    {
        strncpy(dest_string, *src_string, max_length);
        free(*src_string);
        *src_string = NULL;
    }

    return (DG_CMN_DRV_ERR_NONE);
}


/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/
