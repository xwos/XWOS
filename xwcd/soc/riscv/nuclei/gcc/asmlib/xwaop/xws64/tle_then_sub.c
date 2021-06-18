/**
 * @file
 * @brief 原子操作库: xwaop__xws64__tle_then_sub
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
#include <riscv_isa.h>
#include <xwos/lib/xwaop.h>

__xwlib_code
xwer_t xwaop__xws64__tle_then_sub(xws64_a * a,
                                  xws64_t t,
                                  xws64_t v,
                                  xws64_t * nv, xws64_t * ov)
{
        xws64_t n;
        xws64_t o;
        xwer_t rc;

        do {
                o = (xws64_t)rv_lrd(a);
                if (o <= t) {
                        n = o - v;
                        xwmb_mp_mb();
                        rc = rv_scd(a, (xwu64_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}
