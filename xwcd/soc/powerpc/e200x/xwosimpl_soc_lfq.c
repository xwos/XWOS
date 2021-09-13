/**
 * @file
 * @brief 玄武OS移植实现层：SOC无锁队列
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
#include <e200x_core.h>
#include <xwos/lib/lfq.h>

__xwbsp_code
void xwlib_lfq_push(xwlfq_a * h, xwlfq_a * n)
{
        xwlfq_t * next;

        do {
                next = (xwlfq_t *)lwarx(h);
                *n = (xwlfq_t)next;
                xwmb_mp_mb();
        } while (stwcx(h, (xwu32_t)n));
}

__xwbsp_code
xwlfq_t * xwlib_lfq_pop(xwlfq_a * h)
{
        xwlfq_t * top;
        xwlfq_t * next;
        xwer_t rc;

        do {
                top = (xwlfq_t *)lwarx(h);
                if (top) {
                        next = (xwlfq_t *)(*top);
                        xwmb_mp_mb();
                        rc = stwcx(h, (xwu32_t)next);
                } else {
                        break;
                }
        } while (rc);
        return top;
}
