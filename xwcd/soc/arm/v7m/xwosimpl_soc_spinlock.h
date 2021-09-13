/**
 * @file
 * @brief 玄武OS移植实现层：SOC自旋锁
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

#ifndef __xwosimpl_soc_spinlock_h__
#define __xwosimpl_soc_spinlock_h__

#ifndef __xwos_ospl_soc_spinlock_h__
  #error "This file should be included from <xwos/ospl/soc/spinlock.h>."
#endif

#include <armv7m_core.h>

#define SOC_SPLK_TICKET_SHIFT          16

struct soc_splk {
        union {
                xwu32_t total;
                struct {
#if defined(ARCHCFG_LITTLE_ENDIAN) && (1 == ARCHCFG_LITTLE_ENDIAN)
                        xwu16_t curr; /**< current owner */
                        xwu16_t next; /**< next owner */
#else /* ARCHCFG_LITTLE_ENDIAN */
                        xwu16_t next; /**< next owner */
                        xwu16_t curr; /**< current owner */
#endif /* !ARCHCFG_LITTLE_ENDIAN */
                } tickets;
        } v;
};

/**
 * @brief Initialize a spinlock
 * @parem socsplk: SOC Spinlock
 */
static __xwbsp_inline
void soc_splk_init(struct soc_splk * socsplk)
{
        socsplk->v.tickets.next = 1;
        socsplk->v.tickets.curr = 1;
}

#if (CPUCFG_CPU_NUM > 1)
static __xwbsp_inline
void soc_splk_lock(struct soc_splk * socsplk)
{
        xwer_t rc;
        xwu32_t newval;
        struct soc_splk tmp;

        cm_prefetch(socsplk);
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
        : [__lock] "r" (socsplk),
          [__ticket] "I" (1 << SOC_SPLK_TICKET_SHIFT)
        : "cc", "memory"
        );

        while (tmp.v.tickets.next != tmp.v.tickets.curr) {
                cm_wfe();
                tmp.v.tickets.curr = xwmb_access(xwu16_t, socsplk->v.tickets.curr);
        }
        xwmb_mp_mb();
}

static __xwbsp_inline
xwer_t soc_splk_trylock(struct soc_splk * socsplk)
{
        xwer_t rc;
        xwu32_t contended;
        struct soc_splk tmp;

        cm_prefetch(socsplk);
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
                : [__lock] "r" (socsplk),
                  [__ticket] "I" (1 << SOC_SPLK_TICKET_SHIFT)
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
void soc_splk_unlock(struct soc_splk * socsplk)
{
        xwmb_mp_mb();
        socsplk->v.tickets.curr++;
        armv7m_dsb();
        cm_sev();
}
#endif /* (CPUCFG_CPU_NUM > 1) */

#endif /* xwosimpl_soc_spinlock.h */
