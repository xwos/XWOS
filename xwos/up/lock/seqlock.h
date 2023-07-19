/**
 * @file
 * @brief XWOS UP内核锁机制：顺序锁
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_up_lock_seqlock_h__
#define __xwos_up_lock_seqlock_h__

#include <xwos/standard.h>

#define XWUP_SQLK_GRANULARITY (1U)

/**
 * @brief 顺序锁
 */
struct xwup_sqlk {
        xwsq_t seq; /* 顺序值 */
};

void xwup_sqlk_init(struct xwup_sqlk * sql);
xwsq_t xwup_sqlk_rd_begin(struct xwup_sqlk * sql);
bool xwup_sqlk_rd_retry(struct xwup_sqlk * sql, xwsq_t start);
xwsq_t xwup_sqlk_get_seq(struct xwup_sqlk * sql);
void xwup_sqlk_rdex_lock(struct xwup_sqlk * sql);
xwer_t xwup_sqlk_rdex_trylock(struct xwup_sqlk * sql);
void xwup_sqlk_rdex_unlock(struct xwup_sqlk * sql);
void xwup_sqlk_rdex_lock_cpuirq(struct xwup_sqlk * sql);
xwer_t xwup_sqlk_rdex_trylock_cpuirq(struct xwup_sqlk * sql);
void xwup_sqlk_rdex_unlock_cpuirq(struct xwup_sqlk * sql);
void xwup_sqlk_rdex_lock_cpuirqsv(struct xwup_sqlk * sql, xwreg_t * flag);
xwer_t xwup_sqlk_rdex_trylock_cpuirqsv(struct xwup_sqlk * sql, xwreg_t * flag);
void xwup_sqlk_rdex_unlock_cpuirqrs(struct xwup_sqlk * sql, xwreg_t flag);
void xwup_sqlk_rdex_lock_irqs(struct xwup_sqlk * sql,
                              const xwirq_t irqs[],
                              xwsz_t num);
xwer_t xwup_sqlk_rdex_trylock_irqs(struct xwup_sqlk * sql,
                                   const xwirq_t irqs[],
                                   xwsz_t num);
void xwup_sqlk_rdex_unlock_irqs(struct xwup_sqlk * sql,
                                const xwirq_t irqs[],
                                xwsz_t num);
void xwup_sqlk_rdex_lock_irqssv(struct xwup_sqlk * sql,
                                const xwirq_t irqs[],
                                xwreg_t flags[], xwsz_t num);
xwer_t xwup_sqlk_rdex_trylock_irqssv(struct xwup_sqlk * sql,
                                     const xwirq_t irqs[],
                                     xwreg_t flags[], xwsz_t num);
void xwup_sqlk_rdex_unlock_irqsrs(struct xwup_sqlk * sql,
                                  const xwirq_t irqs[],
                                  xwreg_t flags[], xwsz_t num);
void xwup_sqlk_rdex_lock_bh(struct xwup_sqlk * sql);
xwer_t xwup_sqlk_rdex_trylock_bh(struct xwup_sqlk * sql);
void xwup_sqlk_rdex_unlock_bh(struct xwup_sqlk * sql);

void xwup_sqlk_wr_lock(struct xwup_sqlk * sql);
xwer_t xwup_sqlk_wr_trylock(struct xwup_sqlk * sql);
void xwup_sqlk_wr_unlock(struct xwup_sqlk * sql);
void xwup_sqlk_wr_lock_cpuirq(struct xwup_sqlk * sql);
xwer_t xwup_sqlk_wr_trylock_cpuirq(struct xwup_sqlk * sql);
void xwup_sqlk_wr_unlock_cpuirq(struct xwup_sqlk * sql);
void xwup_sqlk_wr_lock_cpuirqsv(struct xwup_sqlk * sql, xwreg_t * flag);
xwer_t xwup_sqlk_wr_trylock_cpuirqsv(struct xwup_sqlk * sql, xwreg_t * flag);
void xwup_sqlk_wr_unlock_cpuirqrs(struct xwup_sqlk * sql, xwreg_t flag);
void xwup_sqlk_wr_lock_irqs(struct xwup_sqlk * sql,
                            const xwirq_t irqs[],
                            xwsz_t num);
xwer_t xwup_sqlk_wr_trylock_irqs(struct xwup_sqlk * sql,
                                 const xwirq_t irqs[],
                                 xwsz_t num);
void xwup_sqlk_wr_unlock_irqs(struct xwup_sqlk * sql,
                              const xwirq_t irqs[],
                              xwsz_t num);
void xwup_sqlk_wr_lock_irqssv(struct xwup_sqlk * sql,
                              const xwirq_t irqs[],
                              xwreg_t flags[], xwsz_t num);
xwer_t xwup_sqlk_wr_trylock_irqssv(struct xwup_sqlk * sql,
                                   const xwirq_t irqs[],
                                   xwreg_t flags[], xwsz_t num);
void xwup_sqlk_wr_unlock_irqsrs(struct xwup_sqlk * sql,
                                const xwirq_t irqs[],
                                xwreg_t flags[], xwsz_t num);
void xwup_sqlk_wr_lock_bh(struct xwup_sqlk * sql);
xwer_t xwup_sqlk_wr_trylock_bh(struct xwup_sqlk * sql);
void xwup_sqlk_wr_unlock_bh(struct xwup_sqlk * sql);

#endif /* xwos/up/lock/seqlock.h */
