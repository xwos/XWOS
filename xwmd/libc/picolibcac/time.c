/**
 * @file
 * @brief picolibc适配层：time
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/lib/errno.h>
#include <xwos/osal/time.h>
#include <xwmd/libc/picolibcac/linkage.h>
#include <xwmd/libc/picolibcac/check.h>
#include <sys/time.h>
#include <sys/times.h>

void picolibcac_time_linkage_stub(void)
{
}

#if defined(XWMDCFG_libc_picolibcac_TIME) && (1 == XWMDCFG_libc_picolibcac_TIME)
extern
xwer_t picolibcac_rtc_set_datetime(struct tm * tm, suseconds_t ms);

extern
xwer_t picolibcac_rtc_get_datetime(struct tm * tm, suseconds_t * ms);

// cppcheck-suppress [misra-c2012-8.14]
int settimeofday(const struct timeval * tv, const struct timezone * tz);

// cppcheck-suppress [misra-c2012-8.14]
int gettimeofday(struct timeval * restrict tv, void * restrict tz);
clock_t times(struct tms * buf);


// cppcheck-suppress [misra-c2012-8.14]
int settimeofday(const struct timeval * tv, const struct timezone * tz)
{
        struct tm tm;
        xwer_t rc;
        int ret;

        XWOS_UNUSED(tz);

        if (NULL == tv) {
                errno = EFAULT;
                ret = -1;
        } else {
                gmtime_r(&tv->tv_sec, &tm);
                rc = picolibcac_rtc_set_datetime(&tm, tv->tv_usec);
                if (XWOK == rc) {
                        errno = 0;
                        ret = 0;
                } else {
                        errno = -rc;
                        ret = -1;
                }
        }
        return ret;
}

// cppcheck-suppress [misra-c2012-8.14]
int gettimeofday(struct timeval * restrict tv, void * restrict tz)
{
        struct tm tm;
        suseconds_t ms;
        xwer_t rc;
        int ret;

        XWOS_UNUSED(tz);

        if (NULL == tv) {
                errno = EFAULT;
                ret = -1;
        } else {
                rc = picolibcac_rtc_get_datetime(&tm, &ms);
                if (XWOK == rc) {
                        errno = 0;
                        ret = 0;
                        tv->tv_sec = mktime(&tm);
                        tv->tv_usec = ms;
                } else {
                        errno = -rc;
                        ret = -1;
                }
        }
        return ret;
}
#else
// cppcheck-suppress [misra-c2012-8.14]
int gettimeofday(struct timeval * restrict tv, void * restrict tz);
clock_t times(struct tms * buf);

// cppcheck-suppress [misra-c2012-8.14]
int gettimeofday(struct timeval * restrict tv, void * restrict tz)
{
        xwtm_t nowts;

        XWOS_UNUSED(tv);
        XWOS_UNUSED(tz);

        errno = 0;
        nowts = xwtm_nowts();
        // cppcheck-suppress [misra-c2012-10.7] 64bit to 32bit
        tv->tv_sec = (time_t)(nowts / XWTM_MS(1000));
        // cppcheck-suppress [misra-c2012-10.7] 64bit to 32bit
        tv->tv_usec = (suseconds_t)((nowts % XWTM_MS(1000)) / XWTM_US(1000));
        return 0;
}
#endif

clock_t times(struct tms * buf)
{
        xwtm_t nowtc;

        nowtc = (clock_t)xwtm_nowtc();
        if (NULL != buf) {
                buf->tms_stime = 0;
                buf->tms_cstime = 0;
                buf->tms_cutime = 0;
                buf->tms_utime = nowtc;
        }
        return nowtc;
}
