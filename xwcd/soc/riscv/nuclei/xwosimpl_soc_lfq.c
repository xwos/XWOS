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
#include <xwcd/soc/riscv/nuclei/riscv_isa.h>
#include <xwos/lib/lfq.h>

__xwbsp_code
void soc_lfq_push(atomic_xwlfq_t * h, atomic_xwlfq_t * n)
{
        xwlfq_t * next;

        do {
                next = (xwlfq_t *)rv_lrw(h);
                xwmb_mp_mb();
                *n = (xwlfq_t)next;
                xwmb_mp_mb();
        } while (rv_scw(h, (xwu32_t)n));
}

__xwbsp_code
xwlfq_t * soc_lfq_pop(atomic_xwlfq_t * h)
{
        xwlfq_t * top;
        xwlfq_t * next;
        xwer_t rc;

        do {
                top = (xwlfq_t *)rv_lrw(h);
                xwmb_mp_mb();
                if (top) {
                        next = (xwlfq_t *)(*top);
                        xwmb_mp_mb();
                        rc = rv_scw(h, (xwu32_t)next);
                } else {
                        break;
                }
        } while (rc);
        return top;
}
