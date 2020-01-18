/**
 * @file
 * @brief 汇编库：无锁队列
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <armv7m_core.h>
#include <asmlib/lfq.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwbsp_code
void arch_lfq_push(__atomic xwlfq_t * h, __atomic xwlfq_t * n)
{
        xwlfq_t * next;

        do {
                next = (xwlfq_t *)ldrex(h);
                xwmb_smp_ddb();
                *n = (xwlfq_t)next;
                xwmb_smp_mb();
        } while (strex(h, (xwu32_t)n));
}

__xwbsp_code
xwlfq_t * arch_lfq_pop(__atomic xwlfq_t * h)
{
        xwlfq_t * top;
        xwlfq_t * next;
        xwer_t rc;

        do {
                top = (xwlfq_t *)ldrex(h);
                xwmb_smp_ddb();
                if (top) {
                        next = (xwlfq_t *)(*top);
                        xwmb_smp_mb();
                        rc = strex(h, (xwu32_t)next);
                } else {
                        break;
                }
        } while (rc);
        return top;
}
