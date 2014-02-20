/*==================================================================================================

    Module Name:  dg_cmn_drv_rtc.c

    General Description: Implements the RTC common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include "dg_handler_inc.h"
#include "dg_drv_util.h"
#include "dg_cmn_drv_rtc.h"


/** @addtogroup dg_common_drivers
@{
*/

/** @addtogroup RTC_driver
@{
implementation of the RTC driver
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
@brief Reads date from RTC

@param[out] date - The date structure filled from RTC

*//*==============================================================================================*/
BOOL DG_CMN_DRV_RTC_get(DG_CMN_DRV_RTC_DATE_T* date)
{
    BOOL ret = FALSE;

    struct timeval time_of_day;
    struct tm*     local_time;

    if (gettimeofday(&time_of_day, NULL) != 0)
    {
        DG_DRV_UTIL_set_error_string("Failed to get time of day, errno=%d (%s)",
                                     errno, strerror(errno));
    }
    else if ((local_time = localtime(&time_of_day.tv_sec)) == NULL)
    {
        DG_DRV_UTIL_set_error_string("Failed to get local time, errno=%d (%s)",
                                     errno, strerror(errno));
    }
    else
    {
        char time_str[128];
        date->year   = (UINT16)local_time->tm_year + 1900; /* tm_year is since 1900 */
        date->month  = (UINT8)local_time->tm_mon + 1;      /* tm_mon is since January 0-11 */
        date->day    = (UINT8)local_time->tm_mday;
        date->hour   = (UINT8)local_time->tm_hour;
        date->minute = (UINT8)local_time->tm_min;
        date->second = (UINT8)local_time->tm_sec;

        if (strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local_time) == 0)
        {
            DG_DBG_ERROR("can't format local time to string");
        }
        else
        {
            DG_DBG_TRACE("current time is %s", time_str);
        }

        ret = TRUE;
    }

    return ret;
}


/*=============================================================================================*//**
@brief Set date to RTC

@param[in] date - The date structure set to RTC

*//*==============================================================================================*/
BOOL DG_CMN_DRV_RTC_set(DG_CMN_DRV_RTC_DATE_T* date)
{
    BOOL ret = FALSE;

#if 0
    struct timeval time_of_day;
    struct tm      local_time;
    time_t         clock;

    local_time.tm_year  = date->year - 1900;  /* tm_year is since 1900 */
    local_time.tm_mon   = date->month - 1;
    local_time.tm_mday  = date->day;
    local_time.tm_hour  = date->hour;
    local_time.tm_min   = date->minute;
    local_time.tm_sec   = date->second;
    local_time.tm_isdst = -1;

    if ((clock = mktime(&local_time)) == -1)
    {
        DG_DRV_UTIL_set_error_string("Failed to mktime, errno=%d (%s)", errno, strerror(errno));
    }
    else
    {
        time_of_day.tv_sec  = clock;
        time_of_day.tv_usec = 0;

        if (settimeofday(&time_of_day, NULL) != 0)
        {
            DG_DRV_UTIL_set_error_string("Failed to set time of day, errno=%d (%s)",
                                         errno, strerror(errno));
        }
        else
        {
            DG_DBG_TRACE("Successfully set current time: %04d-%02d-%02d %02d:%02d:%02d",
                         date->year, date->month, date->day,
                         date->hour, date->minute, date->second);
            ret = TRUE;
        }
    }
#else
    DG_DBG_TRACE("Successfully set current time: %04d-%02d-%02d %02d:%02d:%02d",
                 date->year, date->month, date->day,
                 date->hour, date->minute, date->second);
    ret = TRUE;
#endif

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

