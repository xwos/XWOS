/**
 * @file
 * @brief 原子操作库: xwaop__xws32__write
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
#include <armv7m_core.h>
#include <xwos/lib/xwaop32.h>

__xwlib_code
void xwaop__xws32__write(xws32_a * a,
                         xws32_t v,
                         xws32_t * ov)
{
        xws32_t o;

        do {
                o = (xws32_t)ldrex(a);
                xwmb_mp_mb();
        } while (strex(a, (xwu32_t)v));
        if (ov) {
                *ov = o;
        }
}