/**
 * @file
 * @brief xwos::autosar::cp::os::irq
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/arcos/irq.h>
#include <xwos/osal/irq.h>

void xwacos_irq_disable_all_interrupts(void)
{
        xwos_cpuirq_disable_lc();
}

void xwacos_irq_enable_all_interrupts(void)
{
        xwos_cpuirq_enable_lc();
}

void xwacos_irq_suspend_all_interrupts(void)
{
        xwos_cpuirq_suspend_lc();
}

void xwacos_irq_resume_all_interrupts(void)
{
        xwos_cpuirq_resume_lc();
}

void xwacos_irq_suspend_os_interrupts(void)
{
        /* FIXME: ISO:17356-3 中定义的 `SuspendOSInterrupts()` 只禁止进入
           Category 2 ISR 。Category 2 ISR 类似于XWOS的中断底半部，
           但 Category 2 ISR 与XWOS的中断底半部有所不同：
           + XWOS的中断底半部只有一个优先级。
           + Category 2 ISR 可以有不同的优先级。
           目前先将 `xwacos_irq_suspend_os_interrupts()` 实现为
           `xwacos_irq_suspend_all_interrupts()` ，待重构XWOS的中断底半部后，
           再重新实现此函数。
        */
        xwos_cpuirq_suspend_lc();
}

void xwacos_irq_resume_os_interrupts(void)
{
        /* FIXME: 同 `xwacos_irq_suspend_os_interrupts()` */
        xwos_cpuirq_resume_lc();
}
