/**
 * @file
 * @brief 原子操作库: xwaop__xwu16__and
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
#include <armv7m_isa.h>
#include <xwos/lib/xwaop.h>

__xwlib_code
void xwaop__xwu16__and(xwu16_a * a,
                       xwu16_t v,
                       xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;

        do {
                o = (xwu16_t)cm_ldrexh(a);
                n = o & v;
                xwmb_mp_mb();
        } while (cm_strexh(a, (xwu16_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}
