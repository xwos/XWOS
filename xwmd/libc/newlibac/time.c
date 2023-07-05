/**
 * @file
 * @brief newlib适配层：time
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
#include <xwmd/libc/newlibac/linkage.h>
#include <xwmd/libc/newlibac/check.h>
#include <sys/time.h>
#include <sys/times.h>

void newlibac_time_linkage_stub(void)
{
}

// cppcheck-suppress [misra-c2012-8.14]
int _gettimeofday_r(struct _reent * r, struct timeval * restrict tv, void * restrict tz);
clock_t _times_r(struct _reent * r, struct tms * buf);

// cppcheck-suppress [misra-c2012-8.14]
int _gettimeofday_r(struct _reent * r, struct timeval * restrict tv, void * restrict tz)
{
        xwtm_t nowts;

        XWOS_UNUSED(r);
        XWOS_UNUSED(tz);

        errno = 0;
        nowts = xwtm_nowts();
        tv->tv_sec = (time_t)(nowts / XWTM_S(1));
        tv->tv_usec = (suseconds_t)(nowts % XWTM_S(1)) / XWTM_MS(1);
        return 0;
}

clock_t _times_r(struct _reent * r, struct tms * buf)
{
        xwtm_t nowtc;

        XWOS_UNUSED(r);

        nowtc = (clock_t)xwtm_nowtc();
        if (NULL != buf) {
                buf->tms_stime = 0;
                buf->tms_cstime = 0;
                buf->tms_cutime = 0;
                buf->tms_utime = nowtc;
        }
        return nowtc;
}
