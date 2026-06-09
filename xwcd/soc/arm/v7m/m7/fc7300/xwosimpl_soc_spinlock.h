/**
 * @file
 * @brief XWOS移植实现层：SOC自旋锁
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

#ifndef __xwosimpl_soc_spinlock_h__
#define __xwosimpl_soc_spinlock_h__

#ifndef __xwos_ospl_soc_spinlock_h__
#  error "This file should be included from <xwos/ospl/soc/spinlock.h>."
#endif

#include <xwcd/soc/arm/v7m/m7/fc7300/soc_mb.h>

#define SOC_MB_CH_SPINLOCK              8

struct soc_splk {
        volatile xwu32_t lock;
};

#define SOC_SPLK_INITIALIZER { .lock = 0U, }

static __xwbsp_inline
void soc_splk_init(struct soc_splk * socsplk)
{
        socsplk->lock = 0;
}

#if (CPUCFG_CPU_NUM > 1)
static __xwbsp_inline
void soc_splk_lock(struct soc_splk * socsplk)
{
        xwu32_t locked;

        do {
                while (socsplk->lock > 0U) {
                }
                locked = soc_mb_lock(SOC_MB_CH_SPINLOCK);
                if (locked > 0) {
                        xwmb_mp_load_acquire(xwu32_t, locked, socsplk);
                        if (0U == locked) {
                                xwmb_mp_store_release(xwu32_t, socsplk, 1U);
                                soc_mb_unlock(SOC_MB_CH_SPINLOCK);
                                break;
                        }
                        soc_mb_unlock(SOC_MB_CH_SPINLOCK);
                }
        } while (true);
}

static __xwbsp_inline
xwer_t soc_splk_trylock(struct soc_splk * socsplk)
{
        xwu32_t locked;
        xwer_t rc;

        do {
                locked = soc_mb_lock(SOC_MB_CH_SPINLOCK);
                if (locked > 0) {
                        xwmb_mp_load_acquire(xwu32_t, locked, socsplk);
                        if (0U == locked) {
                                xwmb_mp_store_release(xwu32_t, socsplk, 1U);
                                rc = XWOK;
                        } else {
                                rc = -EAGAIN;
                        }
                        soc_mb_unlock(SOC_MB_CH_SPINLOCK);
                        break;
                }
        } while (true);
        return rc;
}

static __xwbsp_inline
void soc_splk_unlock(struct soc_splk * socsplk)
{
        xwmb_mp_store_release(xwu32_t, socsplk, 0U);
}
#endif

#endif /* xwcd/soc/arm/v7m/m7/fc7300/xwosimpl_soc_spinlock.h */
