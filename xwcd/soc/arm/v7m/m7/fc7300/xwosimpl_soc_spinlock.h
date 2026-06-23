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

#ifndef __xwos_ospl_spinlock_h__
#  error "This file should be included from <xwos/ospl/spinlock.h>."
#endif

#include <xwos/ospl/irq.h>
#include <xwcd/soc/arm/v7m/m7/fc7300/soc_mb.h>

#define XWOSPL_SPLK_TICKET_SHIFT          16

struct xwospl_splk {
        union {
                volatile xwu32_t raw;
                struct {
#if defined(ARCHCFG_LITTLE_ENDIAN) && (1 == ARCHCFG_LITTLE_ENDIAN)
                        volatile xwu16_t curr; /**< current owner */
                        volatile xwu16_t next; /**< next owner */
#else
                        volatile xwu16_t next; /**< next owner */
                        volatile xwu16_t curr; /**< current owner */
#endif
                } ticket;
        } v;
};

#define XWOSPL_SPLK_INITIALIZER { .v.ticket.curr = 0, .v.ticket.next = 0, }

static __xwbsp_inline
void xwospl_splk_init(struct xwospl_splk * osplsplk)
{
        osplsplk->v.ticket.next = 0;
        osplsplk->v.ticket.curr = 0;
}

#if (CPUCFG_CPU_NUM > 1U)
static __xwbsp_inline
void xwospl_splk_lock(struct xwospl_splk * osplsplk)
{
        struct xwospl_splk lock;
        xwu32_t mblkcode;
        xwreg_t cpuirq;
        xwu16_t ticket;

        do {
                xwospl_cpuirq_save_lc(&cpuirq);
                mblkcode = soc_mb_lock(SOC_MB_CH_XWOS_SPINLOCK);
                if (mblkcode > 0) {
                        xwmb_mp_load_acquire(xwu32_t, lock.v.raw, &osplsplk->v.raw);
                        ticket = lock.v.ticket.next;
                        lock.v.ticket.next += 1U;
                        xwmb_mp_store_release(xwu16_t,
                                              &osplsplk->v.ticket.next,
                                              lock.v.ticket.next);
                        soc_mb_unlock(SOC_MB_CH_XWOS_SPINLOCK, mblkcode);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        break;
                } else {
                        xwospl_cpuirq_restore_lc(cpuirq);
                }
        } while (true);

        while (ticket != lock.v.ticket.curr) {
                xwmb_mp_load_acquire(xwu32_t, lock.v.raw, &osplsplk->v.raw);
        }
}

static __xwbsp_inline
xwer_t xwospl_splk_trylock(struct xwospl_splk * osplsplk)
{
        struct xwospl_splk lock;
        xwu32_t mblkcode;
        xwreg_t cpuirq;
        xwer_t rc;

        do {
                xwospl_cpuirq_save_lc(&cpuirq);
                mblkcode = soc_mb_lock(SOC_MB_CH_XWOS_SPINLOCK);
                if (mblkcode > 0) {
                        xwmb_mp_load_acquire(xwu32_t, lock.v.raw, &osplsplk->v.raw);
                        if (lock.v.ticket.curr != lock.v.ticket.next) {
                                rc = -EAGAIN;
                        } else {
                                lock.v.ticket.next += 1U;
                                xwmb_mp_store_release(xwu16_t,
                                                      &osplsplk->v.ticket.next,
                                                      lock.v.ticket.next);
                                rc = XWOK;
                        }
                        soc_mb_unlock(SOC_MB_CH_XWOS_SPINLOCK, mblkcode);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        break;
                } else {
                        xwospl_cpuirq_restore_lc(cpuirq);
                }
        } while (true);
        return rc;
}

static __xwbsp_inline
void xwospl_splk_unlock(struct xwospl_splk * osplsplk)
{
        struct xwospl_splk lock;

        xwmb_mp_load_acquire(xwu32_t, lock.v.raw, &osplsplk->v.raw);
        lock.v.ticket.curr += 1U;
        xwmb_mp_store_release(xwu16_t, &osplsplk->v.ticket.curr, lock.v.ticket.curr);
}
#endif

#endif /* xwcd/soc/arm/v7m/m7/fc7300/xwosimpl_soc_spinlock.h */
