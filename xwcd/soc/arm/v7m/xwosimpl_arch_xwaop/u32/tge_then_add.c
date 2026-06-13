/**
 * @file
 * @brief 原子操作库: xwaop__xwu32_t__tge_then_add
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
xwer_t xwaop__xwu32_t__tge_then_add(atomic_xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        SOC_XWAOP_BEGIN
        do {
                o = (xwu32_t)armv7m_ldrex(a);
                if (o >= t) {
                        n = o + v;
                        xwmb_mp_mb();
                        rc = armv7m_strex(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_ddb();
                        break;
                }
        } while (rc);
        SOC_XWAOP_END
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}
