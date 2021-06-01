/**
 * @file
 * @brief 玄武OS MP内核锁机制：自旋锁
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_mp_lock_spinlock_h__
#define __xwos_mp_lock_spinlock_h__

#include <xwos/standard.h>
#include <xwos/ospl/soc/spinlock.h>

/**
 * @brief XWOS的自旋锁
 */
struct xwmp_splk {
        struct soc_splk socsplk; /**< 架构描述层中的自旋锁结构体 */
};

void xwmp_rawly_lock(struct xwmp_splk * spl);
xwer_t xwmp_rawly_trylock(struct xwmp_splk * spl);
void xwmp_rawly_unlock(struct xwmp_splk * spl);
void xwmp_rawly_lock_cpuirq(struct xwmp_splk * spl);
xwer_t xwmp_rawly_trylock_cpuirq(struct xwmp_splk * spl);
void xwmp_rawly_unlock_cpuirq(struct xwmp_splk * spl);
void xwmp_rawly_lock_cpuirqsv(struct xwmp_splk * spl, xwreg_t * cpuirq);
xwer_t xwmp_rawly_trylock_cpuirqsv(struct xwmp_splk * spl, xwreg_t * cpuirq);
void xwmp_rawly_unlock_cpuirqrs(struct xwmp_splk * spl, xwreg_t cpuirq);
void xwmp_rawly_lock_irqs(struct xwmp_splk * spl,
                          const struct xwos_irq_resource * irqs, xwsz_t num);
xwer_t xwmp_rawly_trylock_irqs(struct xwmp_splk * spl,
                               const struct xwos_irq_resource * irqs, xwsz_t num);
void xwmp_rawly_unlock_irqs(struct xwmp_splk * spl,
                            const struct xwos_irq_resource * irqs, xwsz_t num);
void xwmp_rawly_lock_irqssv(struct xwmp_splk * spl,
                            const struct xwos_irq_resource * irqs,
                            xwreg_t flags[], xwsz_t num);
xwer_t xwmp_rawly_trylock_irqssv(struct xwmp_splk * spl,
                                 const struct xwos_irq_resource * irqs,
                                 xwreg_t flags[], xwsz_t num);
void xwmp_rawly_unlock_irqsrs(struct xwmp_splk * spl,
                              const struct xwos_irq_resource * irqs,
                              xwreg_t flags[], xwsz_t num);

void xwmp_splk_init(struct xwmp_splk * spl);
void xwmp_splk_lock(struct xwmp_splk * spl);
xwer_t xwmp_splk_trylock(struct xwmp_splk * spl);
void xwmp_splk_unlock(struct xwmp_splk * spl);
void xwmp_splk_lock_cpuirq(struct xwmp_splk * spl);
xwer_t xwmp_splk_trylock_cpuirq(struct xwmp_splk * spl);
void xwmp_splk_unlock_cpuirq(struct xwmp_splk * spl);
void xwmp_splk_lock_cpuirqsv(struct xwmp_splk * spl, xwreg_t * cpuirq);
xwer_t xwmp_splk_trylock_cpuirqsv(struct xwmp_splk * spl, xwreg_t * cpuirq);
void xwmp_splk_unlock_cpuirqrs(struct xwmp_splk * spl, xwreg_t cpuirq);
void xwmp_splk_lock_irqs(struct xwmp_splk * spl,
                         const struct xwos_irq_resource * irqs,
                         xwsz_t num);
xwer_t xwmp_splk_trylock_irqs(struct xwmp_splk * spl,
                              const struct xwos_irq_resource * irqs,
                              xwsz_t num);
void xwmp_splk_unlock_irqs(struct xwmp_splk * spl,
                           const struct xwos_irq_resource * irqs,
                           xwsz_t num);
void xwmp_splk_lock_irqssv(struct xwmp_splk * spl,
                           const struct xwos_irq_resource * irqs,
                           xwreg_t flags[], xwsz_t num);
xwer_t xwmp_splk_trylock_irqssv(struct xwmp_splk * spl,
                                const struct xwos_irq_resource * irqs,
                                xwreg_t flags[], xwsz_t num);
void xwmp_splk_unlock_irqsrs(struct xwmp_splk * spl,
                             const struct xwos_irq_resource * irqs,
                             xwreg_t flags[], xwsz_t num);
void xwmp_splk_lock_bh(struct xwmp_splk * spl);
xwer_t xwmp_splk_trylock_bh(struct xwmp_splk * spl);
void xwmp_splk_unlock_bh(struct xwmp_splk * spl);

#endif /* xwos/mp/lock/spinlock.h */
