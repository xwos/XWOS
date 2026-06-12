/**
 * @file
 * @brief XWOS移植实现层：ARCH无锁队列
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
#include <xwcd/soc/arm/v7m/arch_irq.h>
#include <xwcd/soc/arm/v7m/m7/fc7300/soc_mb.h>
#include <xwos/ospl/lfq.h>

__xwbsp_code
void xwospl_lfq_push(atomic_xwlfq_t * h, atomic_xwlfq_t * n)
{
        xwu32_t mblkcode;
        xwlfq_t top;

        do {
                mblkcode = soc_mb_lock(SOC_MB_CH_XWAOP);
                if (mblkcode > 0) {
                        do {
                                top = (xwlfq_t)armv7m_ldrex(h);
                                xwmb_mp_mb();
                                *n = top;
                                xwmb_mp_mb();
                        } while (armv7m_strex(h, (xwu32_t)n));
                        soc_mb_unlock(SOC_MB_CH_XWAOP, mblkcode);
                        break;
                }
        } while (true);
}

__xwbsp_code
xwlfq_t * xwospl_lfq_pop(atomic_xwlfq_t * h)
{
        xwer_t rc;
        xwu32_t mblkcode;
        xwlfq_t top;
        xwlfq_t next;

        do {
                mblkcode = soc_mb_lock(SOC_MB_CH_XWAOP);
                if (mblkcode > 0) {
                        do {
                                top = armv7m_ldrex(h);
                                xwmb_mp_mb();
                                if (top) {
                                        next = (*(xwlfq_t *)top);
                                        xwmb_mp_mb();
                                        rc = armv7m_strex(h, (xwu32_t)next);
                                } else {
                                        break;
                                }
                        } while (rc);
                        soc_mb_unlock(SOC_MB_CH_XWAOP, mblkcode);
                        break;
                }
        } while (true);
        return (xwlfq_t *)top;
}
