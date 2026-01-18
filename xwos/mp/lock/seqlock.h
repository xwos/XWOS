/**
 * @file
 * @brief XWOS MP内核锁机制：顺序锁
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_mp_lock_seqlock_h__
#define __xwos_mp_lock_seqlock_h__

#include <xwos/standard.h>
#include <xwos/mp/lock/spinlock.h>
#include <xwos/mp/irq.h>

/**
 * @brief 序锁顺序值的粒度
 */
#define XWMP_SQLK_GRANULARITY (1U)

/**
 * @brief 顺序锁
 */
struct xwmp_sqlk {
        struct xwmp_splk splk; /**< 自旋锁 */
        xwsq_t seq; /**< 顺序值 */
};

/**
 * @brief 顺序锁的初始值
 */
#define XWMP_SQLK_INITIALIZER { .splk = XWMP_SPLK_INITIALIZER, .seq = 0, }

static __xwmp_inline_api
void xwmp_sqlk_init(struct xwmp_sqlk * sql)
{
        sql->seq = (xwsq_t)0;
        xwmp_splk_init(&sql->splk);
}

static __xwmp_inline_api
xwsq_t xwmp_sqlk_rd_begin(struct xwmp_sqlk * sql)
{
        xwsq_t ret;

        do {
                ret = xwmb_access(xwsq_t, sql->seq);
#if (XWMP_SQLK_GRANULARITY > 1)
                ret &= ((~(xwsq_t)XWMP_SQLK_GRANULARITY) + (xwsq_t)1);
#endif
        } while ((xwsq_t)0 != (ret & (xwsq_t)XWMP_SQLK_GRANULARITY));
        xwmb_mp_rmb();
        return ret;
}

static __xwmp_inline_api
bool xwmp_sqlk_rd_retry(struct xwmp_sqlk * sql, xwsq_t start)
{
        xwmb_mp_rmb(); /* ensure all read accesses are finished */
#if (XWMP_SQLK_GRANULARITY > 1)
        return ((sql->seq & ((~(xwsq_t)XWMP_SQLK_GRANULARITY) + (xwsq_t)1)) != start);
#else
        return (sql->seq != start);
#endif
}

static __xwmp_inline_api
xwsq_t xwmp_sqlk_get_seq(struct xwmp_sqlk * sql)
{
        xwsq_t ret;

        ret = xwmb_access(xwsq_t, sql->seq);
        xwmb_mp_rmb();
#if (XWMP_SQLK_GRANULARITY > 1)
        return ret & ((~(xwsq_t)XWMP_SQLK_GRANULARITY) + (xwsq_t)1);
#else
        return ret;
#endif
}

static __xwmp_inline_api
void xwmp_sqlk_rdex_lock(struct xwmp_sqlk * sql)
{
        xwmp_splk_lock(&sql->splk);
}

static __xwmp_inline_api
xwer_t xwmp_sqlk_rdex_trylock(struct xwmp_sqlk * sql)
{
        return xwmp_splk_trylock(&sql->splk);
}

static __xwmp_inline_api
void xwmp_sqlk_rdex_unlock(struct xwmp_sqlk * sql)
{
        xwmp_splk_unlock(&sql->splk);
}

static __xwmp_inline_api
void xwmp_sqlk_rdex_lock_cpuirq(struct xwmp_sqlk * sql)
{
        xwmp_splk_lock_cpuirq(&sql->splk);
}

static __xwmp_inline_api
xwer_t xwmp_sqlk_rdex_trylock_cpuirq(struct xwmp_sqlk * sql)
{
        return xwmp_splk_trylock_cpuirq(&sql->splk);
}

static __xwmp_inline_api
void xwmp_sqlk_rdex_unlock_cpuirq(struct xwmp_sqlk * sql)
{
        xwmp_splk_unlock_cpuirq(&sql->splk);
}

static __xwmp_inline_api
void xwmp_sqlk_rdex_lock_cpuirqsv(struct xwmp_sqlk * sql, xwreg_t * cpuirq)
{
        xwmp_splk_lock_cpuirqsv(&sql->splk, cpuirq);
}

static __xwmp_inline_api
xwer_t xwmp_sqlk_rdex_trylock_cpuirqsv(struct xwmp_sqlk * sql, xwreg_t * cpuirq)
{
        return xwmp_splk_trylock_cpuirqsv(&sql->splk, cpuirq);
}

static __xwmp_inline_api
void xwmp_sqlk_rdex_unlock_cpuirqrs(struct xwmp_sqlk * sql, xwreg_t cpuirq)
{
        xwmp_splk_unlock_cpuirqrs(&sql->splk, cpuirq);
}

static __xwmp_inline_api
void xwmp_sqlk_rdex_lock_irqs(struct xwmp_sqlk * sql,
                              const xwirq_t irqs[],
                              xwsz_t num)
{
        xwmp_splk_lock_irqs(&sql->splk, irqs, num);
}

static __xwmp_inline_api
xwer_t xwmp_sqlk_rdex_trylock_irqs(struct xwmp_sqlk * sql,
                                   const xwirq_t irqs[],
                                   xwsz_t num)
{
        return xwmp_splk_trylock_irqs(&sql->splk, irqs, num);
}

static __xwmp_inline_api
void xwmp_sqlk_rdex_unlock_irqs(struct xwmp_sqlk * sql,
                                const xwirq_t irqs[],
                                xwsz_t num)
{
        xwmp_splk_unlock_irqs(&sql->splk, irqs, num);
}

static __xwmp_inline_api
void xwmp_sqlk_rdex_lock_irqssv(struct xwmp_sqlk * sql,
                                const xwirq_t irqs[],
                                xwreg_t flags[], xwsz_t num)
{
        xwmp_splk_lock_irqssv(&sql->splk, irqs, flags, num);
}

static __xwmp_inline_api
xwer_t xwmp_sqlk_rdex_trylock_irqssv(struct xwmp_sqlk * sql,
                                     const xwirq_t irqs[],
                                     xwreg_t flags[], xwsz_t num)
{
        return xwmp_splk_trylock_irqssv(&sql->splk, irqs, flags, num);
}

static __xwmp_inline_api
void xwmp_sqlk_rdex_unlock_irqsrs(struct xwmp_sqlk * sql,
                                  const xwirq_t irqs[],
                                  xwreg_t flags[], xwsz_t num)
{
        xwmp_splk_unlock_irqsrs(&sql->splk, irqs, flags, num);
}

static __xwmp_inline_api
void xwmp_sqlk_rdex_lock_bh(struct xwmp_sqlk * sql)
{
        xwmp_splk_lock_bh(&sql->splk);
}

static __xwmp_inline_api
xwer_t xwmp_sqlk_rdex_trylock_bh(struct xwmp_sqlk * sql)
{
        return xwmp_splk_trylock_bh(&sql->splk);
}

static __xwmp_inline_api
void xwmp_sqlk_rdex_unlock_bh(struct xwmp_sqlk * sql)
{
        xwmp_splk_unlock_bh(&sql->splk);
}

static __xwmp_inline_api
void xwmp_sqlk_wr_lock(struct xwmp_sqlk * sql)
{
        xwmp_splk_lock(&sql->splk);
        sql->seq += (xwsq_t)XWMP_SQLK_GRANULARITY;
        xwmb_mp_wmb();
}

static __xwmp_inline_api
xwer_t xwmp_sqlk_wr_trylock(struct xwmp_sqlk * sql)
{
        xwer_t rc;

        rc = xwmp_splk_trylock(&sql->splk);
        if (XWOK == rc) {
                sql->seq += (xwsq_t)XWMP_SQLK_GRANULARITY;
                xwmb_mp_wmb();
        }
        return rc;
}

static __xwmp_inline_api
void xwmp_sqlk_wr_unlock(struct xwmp_sqlk * sql)
{
        xwmb_mp_wmb();
        sql->seq += (xwsq_t)XWMP_SQLK_GRANULARITY;
        xwmp_splk_unlock(&sql->splk);
}

static __xwmp_inline_api
void xwmp_sqlk_wr_lock_cpuirq(struct xwmp_sqlk * sql)
{
        xwmp_splk_lock_cpuirq(&sql->splk);
        sql->seq += (xwsq_t)XWMP_SQLK_GRANULARITY;
        xwmb_mp_wmb();
}

static __xwmp_inline_api
xwer_t xwmp_sqlk_wr_trylock_cpuirq(struct xwmp_sqlk * sql)
{
        xwer_t rc;

        rc = xwmp_splk_trylock_cpuirq(&sql->splk);
        if (XWOK == rc) {
                sql->seq += (xwsq_t)XWMP_SQLK_GRANULARITY;
                xwmb_mp_wmb();
        }
        return rc;
}

static __xwmp_inline_api
void xwmp_sqlk_wr_unlock_cpuirq(struct xwmp_sqlk * sql)
{
        xwmb_mp_wmb();
        sql->seq += (xwsq_t)XWMP_SQLK_GRANULARITY;
        xwmp_splk_unlock_cpuirq(&sql->splk);
}

static __xwmp_inline_api
void xwmp_sqlk_wr_lock_cpuirqsv(struct xwmp_sqlk * sql, xwreg_t * cpuirq)
{
        xwmp_splk_lock_cpuirqsv(&sql->splk, cpuirq);
        sql->seq += (xwsq_t)XWMP_SQLK_GRANULARITY;
        xwmb_mp_wmb();
}

static __xwmp_inline_api
xwer_t xwmp_sqlk_wr_trylock_cpuirqsv(struct xwmp_sqlk * sql, xwreg_t * cpuirq)
{
        xwer_t rc;

        rc = xwmp_splk_trylock_cpuirqsv(&sql->splk, cpuirq);
        if (XWOK == rc) {
                sql->seq += (xwsq_t)XWMP_SQLK_GRANULARITY;
                xwmb_mp_wmb();
        }
        return rc;
}

static __xwmp_inline_api
void xwmp_sqlk_wr_unlock_cpuirqrs(struct xwmp_sqlk * sql, xwreg_t cpuirq)
{
        xwmb_mp_wmb();
        sql->seq += (xwsq_t)XWMP_SQLK_GRANULARITY;
        xwmp_splk_unlock_cpuirqrs(&sql->splk, cpuirq);
}

static __xwmp_inline_api
void xwmp_sqlk_wr_lock_irqs(struct xwmp_sqlk * sql,
                            const xwirq_t irqs[],
                            xwsz_t num)
{
        xwmp_splk_lock_irqs(&sql->splk, irqs, num);
        sql->seq += (xwsq_t)XWMP_SQLK_GRANULARITY;
        xwmb_mp_wmb();
}

static __xwmp_inline_api
xwer_t xwmp_sqlk_wr_trylock_irqs(struct xwmp_sqlk * sql,
                                 const xwirq_t irqs[],
                                 xwsz_t num)
{
        xwer_t rc;

        rc = xwmp_splk_trylock_irqs(&sql->splk, irqs, num);
        if (XWOK == rc) {
                sql->seq += (xwsq_t)XWMP_SQLK_GRANULARITY;
                xwmb_mp_wmb();
        }
        return rc;
}

static __xwmp_inline_api
void xwmp_sqlk_wr_unlock_irqs(struct xwmp_sqlk * sql,
                              const xwirq_t irqs[],
                              xwsz_t num)
{
        xwmb_mp_wmb();
        sql->seq += (xwsq_t)XWMP_SQLK_GRANULARITY;
        xwmp_splk_unlock_irqs(&sql->splk, irqs, num);
}

static __xwmp_inline_api
void xwmp_sqlk_wr_lock_irqssv(struct xwmp_sqlk * sql,
                              const xwirq_t irqs[],
                              xwreg_t flags[], xwsz_t num)
{
        xwmp_splk_lock_irqssv(&sql->splk, irqs, flags, num);
        sql->seq += (xwsq_t)XWMP_SQLK_GRANULARITY;
        xwmb_mp_wmb();
}

static __xwmp_inline_api
xwer_t xwmp_sqlk_wr_trylock_irqssv(struct xwmp_sqlk * sql,
                                   const xwirq_t irqs[],
                                   xwreg_t flags[], xwsz_t num)
{
        xwer_t rc;

        rc = xwmp_splk_trylock_irqssv(&sql->splk, irqs, flags, num);
        if (XWOK == rc) {
                sql->seq += (xwsq_t)XWMP_SQLK_GRANULARITY;
                xwmb_mp_wmb();
        }
        return rc;
}

static __xwmp_inline_api
void xwmp_sqlk_wr_unlock_irqsrs(struct xwmp_sqlk * sql,
                                const xwirq_t irqs[],
                                xwreg_t flags[], xwsz_t num)
{
        xwmb_mp_wmb();
        sql->seq += (xwsq_t)XWMP_SQLK_GRANULARITY;
        xwmp_splk_unlock_irqsrs(&sql->splk, irqs, flags, num);
}

static __xwmp_inline_api
void xwmp_sqlk_wr_lock_bh(struct xwmp_sqlk * sql)
{
        xwmp_splk_lock_bh(&sql->splk);
        sql->seq += (xwsq_t)XWMP_SQLK_GRANULARITY;
        xwmb_mp_wmb();
}

static __xwmp_inline_api
xwer_t xwmp_sqlk_wr_trylock_bh(struct xwmp_sqlk * sql)
{
        xwer_t rc;

        rc = xwmp_splk_trylock_bh(&sql->splk);
        if (XWOK == rc) {
                sql->seq += (xwsq_t)XWMP_SQLK_GRANULARITY;
                xwmb_mp_wmb();
        }
        return rc;
}

static __xwmp_inline_api
void xwmp_sqlk_wr_unlock_bh(struct xwmp_sqlk * sql)
{
        xwmb_mp_wmb();
        sql->seq += (xwsq_t)XWMP_SQLK_GRANULARITY;
        xwmp_splk_unlock_bh(&sql->splk);
}

#endif /* xwos/mp/lock/seqlock.h */
