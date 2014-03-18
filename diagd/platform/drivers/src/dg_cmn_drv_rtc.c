/*==================================================================================================

    Module Name:  dg_cmn_drv_rtc.c

    General Description: Implements the RTC common driver

====================================================================================================

====================================================================================================
                                           INCLUDE FILES
==================================================================================================*/
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/rtc.h>
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
#define DG_CMN_DRV_RTC_DEV "/dev/rtc0"

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

    struct tm rtc_time;
    int       fd;

    if ((fd = open(DG_CMN_DRV_RTC_DEV, O_RDONLY)) < 0)
    {
        DG_DRV_UTIL_set_error_string("can not open %s to get RTC time, errno=%d(%m)",
                                     DG_CMN_DRV_RTC_DEV, errno);
    }
    else if (ioctl(fd, RTC_RD_TIME, &rtc_time) < 0)
    {
        DG_DRV_UTIL_set_error_string("ioctl failed to get RTC time, errno=%d(%m)", errno);
    }
    else
    {
        char time_str[128];
        date->year   = (UINT16)rtc_time.tm_year + 1900; /* tm_year is since 1900 */
        date->month  = (UINT8)rtc_time.tm_mon + 1;      /* tm_mon is since January 0-11 */
        date->day    = (UINT8)rtc_time.tm_mday;
        date->hour   = (UINT8)rtc_time.tm_hour;
        date->minute = (UINT8)rtc_time.tm_min;
        date->second = (UINT8)rtc_time.tm_sec;

        if (strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &rtc_time) == 0)
        {
            DG_DBG_ERROR("can't format local time to string");
        }
        else
        {
            DG_DBG_TRACE("current time is %s", time_str);
        }

        ret = TRUE;
    }

    if (fd > 0)
    {
        close(fd);
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

    struct tm rtc_time;
    int       fd;

    DG_DBG_TRACE("Try to set RTC time: %04d-%02d-%02d %02d:%02d:%02d",
                 date->year, date->month, date->day,
                 date->hour, date->minute, date->second);

    rtc_time.tm_year  = date->year - 1900;  /* tm_year is since 1900 */
    rtc_time.tm_mon   = date->month - 1;
    rtc_time.tm_mday  = date->day;
    rtc_time.tm_hour  = date->hour;
    rtc_time.tm_min   = date->minute;
    rtc_time.tm_sec   = date->second;
    rtc_time.tm_wday  = -1;
    rtc_time.tm_yday  = -1;
    rtc_time.tm_isdst = -1;

    if ((fd = open(DG_CMN_DRV_RTC_DEV, O_RDONLY)) < 0)
    {
        DG_DRV_UTIL_set_error_string("can not open %s to set RTC time, errno=%d(%m)",
                                     DG_CMN_DRV_RTC_DEV, errno);
    }
    else if (ioctl(fd, RTC_SET_TIME, &rtc_time) < 0)
    {
        DG_DRV_UTIL_set_error_string("ioctl failed to set RTC time, errno=%d(%m)", errno);
    }
    else
    {
        DG_DBG_TRACE("Successfully set RTC time");
        ret = TRUE;
    }

    if (fd > 0)
    {
        close(fd);
    }

    return ret;
}

/*==================================================================================================
                                          LOCAL FUNCTIONS
==================================================================================================*/

/** @} */
/** @} */

