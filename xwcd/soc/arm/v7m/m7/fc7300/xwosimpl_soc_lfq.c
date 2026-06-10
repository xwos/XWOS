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
        xwreg_t cpuirq;
        xwu32_t mblkcode;
        xwlfq_t top;

        arch_cpuirq_save_lc(&cpuirq);
        do {
                mblkcode = soc_mb_lock(SOC_MB_CH_SPINLOCK);
                if (mblkcode > 0) {
                        xwmb_mp_load_acquire(xwlfq_t, top, h);
                        *n = top;
                        xwmb_mp_store_release(xwlfq_t, h, (xwlfq_t)n);
                        soc_mb_unlock(SOC_MB_CH_SPINLOCK, mblkcode);
                        break;
                }
        } while (true);
        arch_cpuirq_restore_lc(cpuirq);
}

__xwbsp_code
xwlfq_t * xwospl_lfq_pop(atomic_xwlfq_t * h)
{
        xwreg_t cpuirq;
        xwu32_t mblkcode;
        xwlfq_t top;
        xwlfq_t next;

        arch_cpuirq_save_lc(&cpuirq);
        do {
                mblkcode = soc_mb_lock(SOC_MB_CH_SPINLOCK);
                if (mblkcode > 0) {
                        xwmb_mp_load_acquire(xwlfq_t, top, h);
                        if (top) {
                                next = *((xwlfq_t *)top);
                                xwmb_mp_store_release(xwlfq_t, h, next);
                        }
                        soc_mb_unlock(SOC_MB_CH_SPINLOCK, mblkcode);
                        break;
                }
        } while (true);
        arch_cpuirq_restore_lc(cpuirq);
        return (xwlfq_t *)top;
}
