/**
 * @file
 * @brief XWOS移植实现层：ARCH自旋锁
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

#ifndef __xwosimpl_arch_spinlock_h__
#define __xwosimpl_arch_spinlock_h__

#ifndef __xwos_ospl_spinlock_h__
#  error "This file should be included from <xwos/ospl/spinlock.h>."
#endif

#include <xwcd/soc/arm/v7m/armv7m_isa.h>

#define XWOSPL_SPLK_TICKET_SHIFT          16

struct xwospl_splk {
        union {
                xwu32_t total;
                struct {
#if defined(ARCHCFG_LITTLE_ENDIAN) && (1 == ARCHCFG_LITTLE_ENDIAN)
                        xwu16_t curr; /**< current owner */
                        xwu16_t next; /**< next owner */
#else
                        xwu16_t next; /**< next owner */
                        xwu16_t curr; /**< current owner */
#endif
                } tickets;
        } v;
};

#define XWOSPL_SPLK_INITIALIZER { .v.tickets.curr = 0, .v.tickets.next = 0, }

static __xwbsp_inline
void xwospl_splk_init(struct xwospl_splk * osplsplk)
{
        osplsplk->v.tickets.next = 0;
        osplsplk->v.tickets.curr = 0;
}

#if (CPUCFG_CPU_NUM > 1)
static __xwbsp_inline
void xwospl_splk_lock(struct xwospl_splk * osplsplk)
{
        xwer_t rc;
        xwu32_t newval;
        struct xwospl_splk tmp;

        armv7m_prefetch(osplsplk);
        __asm__ volatile(
        "1:\n"
        "       ldrex   %[__tmp], [%[__lock]]\n"
        "       add     %[__newval], %[__tmp], %[__ticket]\n"
        "       strex   %[__rc], %[__newval], [%[__lock]]\n"
        "       teq     %[__rc], #0\n"
        "       bne     1b\n"
        : [__tmp] "=&r" (tmp.v.total),
          [__newval] "=&r" (newval),
          [__rc] "=&r" (rc)
        : [__lock] "r" (osplsplk),
          [__ticket] "I" (1 << XWOSPL_SPLK_TICKET_SHIFT)
        : "cc", "memory"
        );

        while (tmp.v.tickets.next != tmp.v.tickets.curr) {
                armv7m_wfe();
                tmp.v.tickets.curr = xwmb_access(xwu16_t, &osplsplk->v.tickets.curr);
        }
        xwmb_mp_mb();
}

static __xwbsp_inline
xwer_t xwospl_splk_trylock(struct xwospl_splk * osplsplk)
{
        xwer_t rc;
        xwu32_t contended;
        struct xwospl_splk tmp;

        armv7m_prefetch(osplsplk);
        do {
                __asm__ volatile(
                "       ldrex   %[__tmp], [%[__lock]]\n"
                "       mov     %[__rc], #0\n"
                "       ror     %[__contended], %[__tmp], #16\n"
                "       subs    %[__contended], %[__tmp]\n"
                "       itt     eq\n"
                "       addeq   %[__tmp], %[__tmp], %[__ticket]\n"
                "       strexeq %[__rc], %[__tmp], [%[__lock]]\n"
                : [__tmp] "=&r" (tmp.v.total),
                  [__contended] "=&r" (contended),
                  [__rc] "=&r" (rc)
                : [__lock] "r" (osplsplk),
                  [__ticket] "I" (1 << XWOSPL_SPLK_TICKET_SHIFT)
                : "cc"
                );
        } while (rc);

        if (0 == contended) {
                xwmb_mp_mb();
                return XWOK;
        } else {
                return -EAGAIN;
        }
}

static __xwbsp_inline
void xwospl_splk_unlock(struct xwospl_splk * osplsplk)
{
        xwmb_mp_mb();
        osplsplk->v.tickets.curr++;
        armv7m_dsb();
        armv7m_sev();
}
#endif

#endif /* xwcd/soc/arm/v7m/xwosimpl_arch_spinlock.h */
