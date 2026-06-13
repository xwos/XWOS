/**
 * @file
 * @brief 原子操作库: xwaop__xwu16_t__write
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
#include <xwos/ospl/xwaop.h>

#if defined(SOCCFG_LIB_XWAOP_HOOK) && (1 == SOCCFG_LIB_XWAOP_HOOK)
#  include <soc_xwaop_hook.h>
#endif
#ifndef SOC_XWAOP_BEGIN
#  define SOC_XWAOP_BEGIN
#endif
#ifndef SOC_XWAOP_END
#  define SOC_XWAOP_END
#endif

__xwlib_code
void xwaop__xwu16_t__write(atomic_xwu16_t * a,
                           xwu16_t v,
                           xwu16_t * ov)
{
        xwu16_t o;

        SOC_XWAOP_BEGIN
        do {
                o = (xwu16_t)armv7m_ldrexh(a);
                xwmb_mp_mb();
        } while (armv7m_strexh(a, (xwu16_t)v));
        SOC_XWAOP_END
        if (ov) {
                *ov = o;
        }
}
