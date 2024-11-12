/**
 * @file
 * @brief AUTOSAR::CP::Os::Cfg::Hook
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/arcos/hook.h>

void StartupHook(void)
{
}

void ShutdownHook(void)
{
}

void PreTaskHook(void)
{
}

void PostTaskHook(void)
{
}

void ErrorHook(xwer_t Error)
{
        XWOS_UNUSED(Error);
}

struct xwarcos_oshook xwarcos_oshook = {
        .startup_hook = StartupHook,
        .shutdown_hook = ShutdownHook,
        .pre_task_hook = PreTaskHook,
        .post_task_hook = PostTaskHook,
        .error_hook = ErrorHook,
};
