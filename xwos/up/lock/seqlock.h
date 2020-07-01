/**
 * @file
 * @brief XuanWuOS的锁机制：顺序锁
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_lock_seqlock_h__
#define __xwos_up_lock_seqlock_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWLK_SQLK_GRANULARITY           (1U)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 顺序锁
 */
struct xwlk_sqlk {
        xwsq_t seq; /* 顺序值 */
};

struct xwos_irq_resource;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********       API function prototypes       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_api
void xwlk_sqlk_init(struct xwlk_sqlk * sql);

__xwos_api
xwsq_t xwlk_sqlk_rd_begin(struct xwlk_sqlk * sql);

__xwos_api
bool xwlk_sqlk_rd_retry(struct xwlk_sqlk * sql, xwsq_t start);

__xwos_api
xwsq_t xwlk_sqlk_get_seq(struct xwlk_sqlk * sql);

__xwos_api
void xwlk_sqlk_rdex_lock(struct xwlk_sqlk * sql);

__xwos_api
xwer_t xwlk_sqlk_rdex_trylock(struct xwlk_sqlk * sql);

__xwos_api
void xwlk_sqlk_rdex_unlock(struct xwlk_sqlk * sql);

__xwos_api
void xwlk_sqlk_rdex_lock_cpuirq(struct xwlk_sqlk * sql);

__xwos_api
xwer_t xwlk_sqlk_rdex_trylock_cpuirq(struct xwlk_sqlk * sql);

__xwos_api
void xwlk_sqlk_rdex_unlock_cpuirq(struct xwlk_sqlk * sql);

__xwos_api
void xwlk_sqlk_rdex_lock_cpuirqsv(struct xwlk_sqlk * sql, xwreg_t * flag);

__xwos_api
xwer_t xwlk_sqlk_rdex_trylock_cpuirqsv(struct xwlk_sqlk * sql, xwreg_t * flag);

__xwos_api
void xwlk_sqlk_rdex_unlock_cpuirqrs(struct xwlk_sqlk * sql, xwreg_t flag);

__xwos_api
void xwlk_sqlk_rdex_lock_irqs(struct xwlk_sqlk * sql,
                              const struct xwos_irq_resource * irqs,
                              xwsz_t num);

__xwos_api
xwer_t xwlk_sqlk_rdex_trylock_irqs(struct xwlk_sqlk * sql,
                                   const struct xwos_irq_resource * irqs,
                                   xwsz_t num);

__xwos_api
void xwlk_sqlk_rdex_unlock_irqs(struct xwlk_sqlk * sql,
                                const struct xwos_irq_resource * irqs,
                                xwsz_t num);

__xwos_api
void xwlk_sqlk_rdex_lock_irqssv(struct xwlk_sqlk * sql,
                                const struct xwos_irq_resource * irqs,
                                xwreg_t flags[], xwsz_t num);

__xwos_api
xwer_t xwlk_sqlk_rdex_trylock_irqssv(struct xwlk_sqlk * sql,
                                     const struct xwos_irq_resource * irqs,
                                     xwreg_t flags[], xwsz_t num);

__xwos_api
void xwlk_sqlk_rdex_unlock_irqsrs(struct xwlk_sqlk * sql,
                                  const struct xwos_irq_resource * irqs,
                                  xwreg_t flags[], xwsz_t num);

__xwos_api
void xwlk_sqlk_rdex_lock_bh(struct xwlk_sqlk * sql);

__xwos_api
xwer_t xwlk_sqlk_rdex_trylock_bh(struct xwlk_sqlk * sql);

__xwos_api
void xwlk_sqlk_rdex_unlock_bh(struct xwlk_sqlk * sql);

__xwos_api
void xwlk_sqlk_wr_lock(struct xwlk_sqlk * sql);

__xwos_api
xwer_t xwlk_sqlk_wr_trylock(struct xwlk_sqlk * sql);

__xwos_api
void xwlk_sqlk_wr_unlock(struct xwlk_sqlk * sql);

__xwos_api
void xwlk_sqlk_wr_lock_cpuirq(struct xwlk_sqlk * sql);

__xwos_api
xwer_t xwlk_sqlk_wr_trylock_cpuirq(struct xwlk_sqlk * sql);

__xwos_api
void xwlk_sqlk_wr_unlock_cpuirq(struct xwlk_sqlk * sql);

__xwos_api
void xwlk_sqlk_wr_lock_cpuirqsv(struct xwlk_sqlk * sql, xwreg_t * flag);

__xwos_api
xwer_t xwlk_sqlk_wr_trylock_cpuirqsv(struct xwlk_sqlk * sql, xwreg_t * flag);

__xwos_api
void xwlk_sqlk_wr_unlock_cpuirqrs(struct xwlk_sqlk * sql, xwreg_t flag);

__xwos_api
void xwlk_sqlk_wr_lock_irqs(struct xwlk_sqlk * sql,
                            const struct xwos_irq_resource * irqs,
                            xwsz_t num);

__xwos_api
xwer_t xwlk_sqlk_wr_trylock_irqs(struct xwlk_sqlk * sql,
                                 const struct xwos_irq_resource * irqs,
                                 xwsz_t num);

__xwos_api
void xwlk_sqlk_wr_unlock_irqs(struct xwlk_sqlk * sql,
                              const struct xwos_irq_resource * irqs,
                              xwsz_t num);

__xwos_api
void xwlk_sqlk_wr_lock_irqssv(struct xwlk_sqlk * sql,
                              const struct xwos_irq_resource * irqs,
                              xwreg_t flags[], xwsz_t num);

__xwos_api
xwer_t xwlk_sqlk_wr_trylock_irqssv(struct xwlk_sqlk * sql,
                                   const struct xwos_irq_resource * irqs,
                                   xwreg_t flags[], xwsz_t num);

__xwos_api
void xwlk_sqlk_wr_unlock_irqsrs(struct xwlk_sqlk * sql,
                                const struct xwos_irq_resource * irqs,
                                xwreg_t flags[], xwsz_t num);

__xwos_api
void xwlk_sqlk_wr_lock_bh(struct xwlk_sqlk * sql);

__xwos_api
xwer_t xwlk_sqlk_wr_trylock_bh(struct xwlk_sqlk * sql);

__xwos_api
void xwlk_sqlk_wr_unlock_bh(struct xwlk_sqlk * sql);

#endif /* xwos/up/lock/seqlock.h */
