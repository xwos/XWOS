/**
 * @file
 * @brief XWOS移植实现层：SOC无锁队列
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
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

#include <xwos/standard.h>
#include <arch_irq.h>

void soc_lfq_push(atomic_xwlfq_t * h, atomic_xwlfq_t * n)
{
        xwreg_t flag;

        arch_cpuirq_save_lc(&flag);
        *n = *h;
        *h = (xwlfq_t)n;
        arch_cpuirq_restore_lc(flag);
}

xwlfq_t * soc_lfq_pop(atomic_xwlfq_t * h)
{
        xwreg_t flag;
        xwlfq_t * top;

        arch_cpuirq_save_lc(&flag);
        top = (xwlfq_t *)(*h);
        if (top) {
                *h = *top;
        }/* else {} */
        arch_cpuirq_restore_lc(flag);
        return top;
}
