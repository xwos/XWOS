/**
 * @file
 * @brief 原子操作库: xwaop__xws8__add
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
void xwaop__xws8_t__add(atomic_xws8_t * a,
                        xws8_t v,
                        xws8_t * nv, xws8_t * ov)
{
        xws8_t n;
        xws8_t o;

        do {
                o = (xws8_t)armv7m_ldrexb(a);
                n = o + v;
                xwmb_mp_mb();
        } while (armv7m_strexb(a, (xwu8_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}
