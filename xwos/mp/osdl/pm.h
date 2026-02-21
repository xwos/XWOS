/**
 * @file
 * @brief XWOS MP内核操作系统接口描述层：电源管理
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_mp_osdl_pm_h__
#define __xwos_mp_osdl_pm_h__

#include <xwos/mp/pm.h>

typedef xwmp_skd_pm_cb_f xwosdl_pm_cb_f;

#define XWOSDL_PM_STAGE_SUSPENDED XWMP_PM_STAGE_SUSPENDED
#define XWOSDL_PM_STAGE_SUSPENDING XWMP_PM_STAGE_SUSPENDING
#define XWOSDL_PM_STAGE_RESUMING XWMP_PM_STAGE_RESUMING
#define XWOSDL_PM_STAGE_ALLFRZ XWMP_PM_STAGE_ALLFRZ
#define XWOSDL_PM_STAGE_FREEZING XWMP_PM_STAGE_FREEZING
#define XWOSDL_PM_STAGE_THAWING XWMP_PM_STAGE_THAWING
#define XWOSDL_PM_STAGE_RUNNING XWMP_PM_STAGE_RUNNING

static __xwcc_inline
void xwosdl_pm_set_cb(xwosdl_pm_cb_f resume,
                      xwosdl_pm_cb_f suspend,
                      xwosdl_pm_cb_f wakeup,
                      xwosdl_pm_cb_f sleep,
                      void * arg)
{
        return  xwmp_pm_set_cb(resume, suspend, wakeup, sleep, arg);
}

static __xwcc_inline
void xwosdl_pm_suspend(void)
{
        xwmp_pm_suspend();
}

static __xwcc_inline
void xwosdl_pm_resume(void)
{
        xwmp_pm_resume();
}

static __xwcc_inline
xwsq_t xwosdl_pm_get_stage(void)
{
        return xwmp_pm_get_stage();
}

#endif /* xwos/mp/osdl/pm.h */
