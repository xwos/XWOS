/**
 * @file
 * @brief XWRUST FFI：电源管理
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/osal/pm.h>

void xwrustffi_pm_set_op(xwos_pm_op_f resume_periph,
                         xwos_pm_op_f suspend_periph,
                         xwos_pm_op_f wakeup_cpu,
                         xwos_pm_op_f sleep_cpu,
                         void * arg)
{
        xwos_pm_set_op(resume_periph,
                       suspend_periph,
                       wakeup_cpu,
                       sleep_cpu,
                       arg);
}

void xwrustffi_pm_suspend(void)
{
        xwos_pm_suspend();
}

void xwrustffi_pm_resume(void)
{
        xwos_pm_resume();
}

xwsq_t xwrustffi_pm_get_stage(void)
{
        return xwos_pm_get_stage();
}
