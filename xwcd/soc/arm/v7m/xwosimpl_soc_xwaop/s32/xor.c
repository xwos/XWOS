/**
 * @file
 * @brief 原子操作库: xwaop__xws32_t__xor
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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
void xwaop__xws32_t__xor(atomic_xws32_t * a,
                         xws32_t v,
                         xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;

        do {
                o = (xws32_t)armv7m_ldrex(a);
                n = o ^ v;
                xwmb_mp_mb();
        } while (armv7m_strex(a, (xwu32_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}
