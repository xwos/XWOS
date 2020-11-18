/**
 * @file
 * @brief 汇编库：无锁队列
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
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
#include <asmlib/lfq.h>

void arch_lfq_push(__xwcc_atomic xwlfq_t * h, __xwcc_atomic xwlfq_t * n)
{
        xwreg_t flag;

        arch_cpuirq_save_lc(&flag);
        *n = *h;
        *h = (xwlfq_t)n;
        arch_cpuirq_restore_lc(flag);
}

xwlfq_t * arch_lfq_pop(__xwcc_atomic xwlfq_t * h)
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
