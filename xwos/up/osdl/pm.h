/**
 * @file
 * @brief XWOS UP内核操作系统接口描述层：电源管理
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_up_osdl_pm_h__
#define __xwos_up_osdl_pm_h__

#include <xwos/up/pm.h>

typedef xwup_skd_pm_cb_f xwosdl_pm_cb_f;

#define XWOSDL_PM_STAGE_SUSPENDED XWUP_PM_STAGE_SUSPENDED
#define XWOSDL_PM_STAGE_SUSPENDING XWUP_PM_STAGE_SUSPENDING
#define XWOSDL_PM_STAGE_RESUMING XWUP_PM_STAGE_RESUMING
#define XWOSDL_PM_STAGE_FREEZING XWUP_PM_STAGE_FREEZING
#define XWOSDL_PM_STAGE_THAWING XWUP_PM_STAGE_THAWING
#define XWOSDL_PM_STAGE_RUNNING XWUP_PM_STAGE_RUNNING

static __xwcc_inline
void xwosdl_pm_set_cb(xwosdl_pm_cb_f resume_cb,
                      xwosdl_pm_cb_f suspend_cb,
                      xwosdl_pm_cb_f wakeup_cb,
                      xwosdl_pm_cb_f sleep_cb,
                      void * arg)
{
        return  xwup_skd_set_pm_cb(resume_cb, suspend_cb, wakeup_cb, sleep_cb, arg);
}

static __xwcc_inline
xwer_t xwosdl_pm_suspend(void)
{
        return xwup_skd_suspend();
}

static __xwcc_inline
xwer_t xwosdl_pm_resume(void)
{
        return xwup_skd_resume();
}

static __xwcc_inline
xwsq_t xwosdl_pm_get_stage(void)
{
        return xwup_skd_get_pm_stage();
}

#endif /* xwos/up/osdl/pm.h */
