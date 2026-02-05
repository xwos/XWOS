/**
 * @file
 * @brief 板级描述层：XWOS适配层：内核：电源管理
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > Licensed under the Apache License, Version 2.0 (the "License");
 * > you may not use this file except in compliance with the License.
 * > You may obtain a copy of the License at
 * >
 * >         http://www.apache.org/licenses/LICENSE-2.0
 * >
 * > Unless required by applicable law or agreed to in writing, software
 * > distributed under the License is distributed on an "AS IS" BASIS,
 * > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * > See the License for the specific language governing permissions and
 * > limitations under the License.
 */

#include "board/std.h"
#include <xwos/osal/pm.h>
#include <xwcd/soc/arm/v7m/armv7m_isa.h>
#include <bm/Stm32Hal/mi.h>

void xwosac_pmcb_resume(void * arg)
{
        xwsq_t ctx;
        xwirq_t irq;

        XWOS_UNUSED(arg);
        xwos_skd_get_context_lc(&ctx, &irq); /* 获取上下文以及中断号，用于调试 */
        stm32hal_resume();
}

void xwosac_pmcb_suspend(void * arg)
{
        xwsq_t ctx;
        xwirq_t irq;

        XWOS_UNUSED(arg);
        xwos_skd_get_context_lc(&ctx, &irq); /* 获取上下文以及中断号，用于调试 */
        stm32hal_suspend();
}

void xwosac_pmcb_wakeup(void * arg)
{
        XWOS_UNUSED(arg);
        stm32hal_wakeup();
}

void xwosac_pmcb_sleep(void * arg)
{
        XWOS_UNUSED(arg);
        stm32hal_sleep();
        armv7m_wfi(); /* 通过 WFI 指令进入STOP模式 */
}

void xwosac_pmcb_init(void)
{
        xwos_pm_set_cb(xwosac_pmcb_resume,
                       xwosac_pmcb_suspend,
                       xwosac_pmcb_wakeup,
                       xwosac_pmcb_sleep,
                       NULL);
}
