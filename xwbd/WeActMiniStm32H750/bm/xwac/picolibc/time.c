/**
 * @file
 * @brief 板级描述层：XWOS适配层：定义libc的时钟
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > Licensed under the Apache License, Version 2.0 (the "License");
 * > you may not use this file except in compliance with the License.
 * > You may obtain a copy of the License at
 * >
 * >         http://www.apache.org/licenses/LICENSE-2.0
 * >
 * > Unless required by applicable law or agreed to in writing, software
 * > distributed under the License is distributed on an "AS IS" BASIS,
 * > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * > See the License for the specific language governing permissions and
 * > limitations under the License.
 */

#include "board/std.h"
#include <time.h>
#include "bm/stm32cube/Core/Inc/rtc.h"

xwer_t picolibcac_rtc_get_datetime(struct tm * tm, suseconds_t * ms)
{
        RTC_DateTypeDef rtcdate;
        RTC_TimeTypeDef rtctime;

        HAL_RTC_GetDate(&hrtc, &rtcdate, RTC_FORMAT_BIN);
        HAL_RTC_GetTime(&hrtc, &rtctime, RTC_FORMAT_BIN);
        tm->tm_sec = rtctime.Seconds;
        tm->tm_min = rtctime.Minutes;
        tm->tm_hour = rtctime.Hours + (rtctime.TimeFormat * 12);
        tm->tm_mday = rtcdate.Date;
        tm->tm_mon = rtcdate.Month - 1;
        tm->tm_year = rtcdate.Year + 2000 - 1900;
        tm->tm_wday = (rtcdate.WeekDay == RTC_WEEKDAY_SUNDAY) ? 0 : rtcdate.WeekDay;
        tm->tm_yday = 0;
        tm->tm_isdst = 0;
        *ms = (((rtctime.SecondFraction - rtctime.SubSeconds) * 1000) /
               (rtctime.SecondFraction + 1));
        return XWOK;
}
