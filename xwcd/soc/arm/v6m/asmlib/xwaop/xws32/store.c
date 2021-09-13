/**
 * @file
 * @brief 原子操作库: xwaop__xws32__load
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
#include <armv6m_core.h>
#include <xwos/lib/xwaop.h>

__xwlib_code
xws32_t xwaop__xws32__store(xws32_a * a,
                            const enum xwmb_memory_order_em mo,
                            xws32_t v)
{
        switch (mo) {
        case xwmb_modr_relaxed:
                *a = v;
                break;
        case xwmb_modr_consume:
                *a = v;
                break;
        case xwmb_modr_acquire:
                *a = v;
                break;
        case xwmb_modr_release:
                xwmb_mp_mb();
                *a = v;
                break;
        case xwmb_modr_acq_rel:
                xwmb_mp_mb();
                *a = v;
                break;
        case xwmb_modr_seq_cst:
                xwmb_mp_mb();
                *a = v;
                xwmb_mp_mb();
                break;
        }
        return v;
}
