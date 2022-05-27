/**
 * @file
 * @brief XWRUST FFI：电源管理
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/osal/pm.h>

xwer_t xwrustffi_pm_suspend(void)
{
        return xwos_pm_suspend();
}

xwer_t xwrustffi_pm_resume(void)
{
        return xwos_pm_resume();
}
