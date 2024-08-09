/**
 * @file
 * @brief 原子操作库：xwaop__xwu16_t__tgtlt_then_rsb
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
#include <xwcd/soc/arm/v6m/arch_irq.h>
#include <xwos/lib/xwaop.h>

__xwlib_code
xwer_t xwaop__xwu16_t__tgtlt_then_rsb(atomic_xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;
        xwreg_t flag;

        arch_cpuirq_save_lc(&flag);
        o = *a;
        if ((o > l) && (o < r)) {
                n = v - o;
                rc = XWOK;
                *a = n;
        } else {
                n = o;
                rc = -EACCES;
        }
        arch_cpuirq_restore_lc(flag);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}
