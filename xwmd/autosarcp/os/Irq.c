/**
 * @file
 * @brief AUTOSAR::CP::Os::Irq
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwmd/autosarcp/os/Irq.h>

void EnableAllInterrupts(void)
{
        xwacos_irq_enable_all_interrupts();
}

void DisableAllInterrupts(void)
{
        xwacos_irq_disable_all_interrupts();
}

void SuspendAllInterrupts(void)
{
        xwacos_irq_suspend_all_interrupts();
}

void ResumeAllInterrupts(void)
{
        xwacos_irq_resume_all_interrupts();
}

void SuspendOSInterrupts(void)
{
        xwacos_irq_suspend_os_interrupts();
}

void ResumeOSInterrupts(void)
{
        xwacos_irq_resume_os_interrupts();
}
