/**
 * @file
 * @brief 原子操作库：xwaop__xws32__tle_then_rsb
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
#include <xwos/lib/xwaop.h>

__xwlib_code
xwer_t xwaop__xws32__tle_then_rsb(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;
        xwreg_t flag;

        arch_cpuirq_save_lc(&flag);
        o = *a;
        if (o <= t) {
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