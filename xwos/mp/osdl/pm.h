/**
 * @file
 * @brief 玄武OS MP内核操作系统接口描述层：电源管理
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_mp_osdl_pm_h__
#define __xwos_mp_osdl_pm_h__

#include <xwos/mp/pm.h>

typedef xwmp_pmdm_cb_f xwosdl_pm_cb_f;

#define XWOSDL_PM_STAGE_SUSPENDED XWMP_PM_STAGE_SUSPENDED
#define XWOSDL_PM_STAGE_SUSPENDING XWMP_PM_STAGE_SUSPENDING
#define XWOSDL_PM_STAGE_RESUMING XWMP_PM_STAGE_RESUMING
#define XWOSDL_PM_STAGE_FREEZING XWMP_PM_STAGE_FREEZING
#define XWOSDL_PM_STAGE_THAWING XWMP_PM_STAGE_THAWING
#define XWOSDL_PM_STAGE_RUNNING XWMP_PM_STAGE_RUNNING

static __xwcc_inline
void xwosdl_pm_set_cb(xwosdl_pm_cb_f resume_cb,
                      xwosdl_pm_cb_f suspend_cb,
                      xwosdl_pm_cb_f wakeup_cb,
                      xwosdl_pm_cb_f sleep_cb,
                      void * arg)
{
        return  xwmp_pmdm_set_cb(resume_cb, suspend_cb, wakeup_cb, sleep_cb, arg);
}

static __xwcc_inline
xwer_t xwosdl_pm_suspend(void)
{
        return xwmp_pmdm_suspend();
}

static __xwcc_inline
xwer_t xwosdl_pm_resume(void)
{
        return xwmp_pmdm_resume();
}

static __xwcc_inline
xwsq_t xwosdl_pm_get_stage(void)
{
        return xwmp_pmdm_get_stage();
}

#endif /* xwos/mp/osdl/pm.h */
