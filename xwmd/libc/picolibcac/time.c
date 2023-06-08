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
#include <xwmd/libc/picolibcac/check.h>
#include <sys/time.h>
#include <sys/times.h>

void picolibcac_time_linkage_stub(void)
{
}

int gettimeofday(struct timeval * restrict tv, void * restrict tz)
{
        xwtm_t nowts;

        errno = 0;
        nowts = xwtm_nowts();
        tv->tv_sec = (time_t)(nowts / XWTM_S(1));
        tv->tv_usec = (suseconds_t)(nowts % XWTM_S(1)) / XWTM_MS(1);
        return 0;
}

clock_t times(struct tms * buf)
{
        xwtm_t nowtc;

        nowtc = (clock_t)xwtm_nowtc();
        if (buf) {
                buf->tms_stime = 0;
                buf->tms_cstime = 0;
                buf->tms_cutime = 0;
                buf->tms_utime = nowtc;
        }
        return nowtc;
}
