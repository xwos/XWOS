/**
 * @file
 * @brief 原子操作库: xwaop__xws16_t__tle_then_rsb
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
#include <xwcd/soc/arm/v7m/armv7m_isa.h>
#include <xwos/lib/xwaop.h>

__xwlib_code
xwer_t xwaop__xws16_t__tle_then_rsb(atomic_xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov)
{
        xws16_t n;
        xws16_t o;
        xwer_t rc;

        do {
                o = (xws16_t)armv7m_ldrexh(a);
                if (o <= t) {
                        n = v - o;
                        xwmb_mp_mb();
                        rc = armv7m_strexh(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_ddb();
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
