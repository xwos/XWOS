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
#include <arch_spinlock.h>

/**
 * @brief XWOS的自旋锁
 */
struct xwmp_splk {
        struct arch_splk aspl; /**< 架构描述层中的自旋锁结构体 */
};

struct xwmp_irq_resource;

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
                          const struct xwmp_irq_resource * irqs, xwsz_t num);
xwer_t xwmp_rawly_trylock_irqs(struct xwmp_splk * spl,
                               const struct xwmp_irq_resource * irqs, xwsz_t num);
void xwmp_rawly_unlock_irqs(struct xwmp_splk * spl,
                            const struct xwmp_irq_resource * irqs, xwsz_t num);
void xwmp_rawly_lock_irqssv(struct xwmp_splk * spl,
                            const struct xwmp_irq_resource * irqs,
                            xwreg_t flags[], xwsz_t num);
xwer_t xwmp_rawly_trylock_irqssv(struct xwmp_splk * spl,
                                 const struct xwmp_irq_resource * irqs,
                                 xwreg_t flags[], xwsz_t num);
void xwmp_rawly_unlock_irqsrs(struct xwmp_splk * spl,
                              const struct xwmp_irq_resource * irqs,
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
                         const struct xwmp_irq_resource * irqs,
                         xwsz_t num);
xwer_t xwmp_splk_trylock_irqs(struct xwmp_splk * spl,
                              const struct xwmp_irq_resource * irqs,
                              xwsz_t num);
void xwmp_splk_unlock_irqs(struct xwmp_splk * spl,
                           const struct xwmp_irq_resource * irqs,
                           xwsz_t num);
void xwmp_splk_lock_irqssv(struct xwmp_splk * spl,
                           const struct xwmp_irq_resource * irqs,
                           xwreg_t flags[], xwsz_t num);
xwer_t xwmp_splk_trylock_irqssv(struct xwmp_splk * spl,
                                const struct xwmp_irq_resource * irqs,
                                xwreg_t flags[], xwsz_t num);
void xwmp_splk_unlock_irqsrs(struct xwmp_splk * spl,
                             const struct xwmp_irq_resource * irqs,
                             xwreg_t flags[], xwsz_t num);
void xwmp_splk_lock_bh(struct xwmp_splk * spl);
xwer_t xwmp_splk_trylock_bh(struct xwmp_splk * spl);
void xwmp_splk_unlock_bh(struct xwmp_splk * spl);

void xwmp_bitsplk_init(xwptr_t * ptr);
void xwmp_bitsplk_lock(xwptr_t * ptr);
xwer_t xwmp_bitsplk_trylock(xwptr_t * ptr);
void xwmp_bitsplk_unlock(xwptr_t * ptr);
void xwmp_bitsplk_lock_cpuirq(xwptr_t * ptr);
xwer_t xwmp_bitsplk_trylock_cpuirq(xwptr_t * ptr);
void xwmp_bitsplk_unlock_cpuirq(xwptr_t * ptr);
void xwmp_bitsplk_lock_cpuirqsv(xwptr_t * ptr, xwreg_t * cpuirq);
xwer_t xwmp_bitsplk_trylock_cpuirqsv(xwptr_t * ptr, xwreg_t * cpuirq);
void xwmp_bitsplk_unlock_cpuirqrs(xwptr_t * ptr, xwreg_t cpuirq);
void xwmp_bitsplk_lock_irqs(xwptr_t * ptr, const struct xwmp_irq_resource * irqs,
                            xwsz_t num);
xwer_t xwmp_bitsplk_trylock_irqs(xwptr_t * ptr,
                                 const struct xwmp_irq_resource * irqs,
                                 xwsz_t num);
void xwmp_bitsplk_unlock_irqs(xwptr_t * ptr, const struct xwmp_irq_resource * irqs,
                              xwsz_t num);
void xwmp_bitsplk_lock_irqssv(xwptr_t * ptr,
                              const struct xwmp_irq_resource * irqs,
                              xwreg_t flags[], xwsz_t num);
xwer_t xwmp_bitsplk_trylock_irqssv(xwptr_t * ptr,
                                   const struct xwmp_irq_resource * irqs,
                                   xwreg_t flags[], xwsz_t num);
void xwmp_bitsplk_unlock_irqsrs(xwptr_t * ptr,
                                const struct xwmp_irq_resource * irqs,
                                xwreg_t flags[], xwsz_t num);
void xwmp_bitsplk_lock_bh(xwptr_t * ptr);
xwer_t xwmp_bitsplk_trylock_bh(xwptr_t * ptr);
void xwmp_bitsplk_unlock_bh(xwptr_t * ptr);

#endif /* xwos/mp/lock/spinlock.h */