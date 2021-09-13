/**
 * @file
 * @brief 玄武OS UP内核锁机制：顺序锁
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/ospl/irq.h>
#include <xwos/ospl/skd.h>
#include <xwos/up/lock/seqlock.h>

__xwup_api
void xwup_sqlk_init(struct xwup_sqlk * sql)
{
        sql->seq = 0;
}

__xwup_api
xwsq_t xwup_sqlk_rd_begin(struct xwup_sqlk * sql)
{
        xwsq_t ret;

retry:
        xwmb_read(xwsq_t, ret, &sql->seq);
#if (XWUP_SQLK_GRANULARITY > 1)
        ret &= ((~XWUP_SQLK_GRANULARITY) + 1U);
#endif
        if (__xwcc_unlikely(ret & XWUP_SQLK_GRANULARITY)) {
                goto retry;
        }
        xwmb_mp_rmb();
        return ret;
}

__xwup_api
bool xwup_sqlk_rd_retry(struct xwup_sqlk * sql, xwsq_t start)
{
        xwmb_mp_rmb(); /* ensure all read accesses are finished */
#if (XWUP_SQLK_GRANULARITY > 1)
        return ((sql->seq & ((~XWUP_SQLK_GRANULARITY) + 1U)) != start);
#else
        return (sql->seq != start);
#endif
}

__xwup_api
xwsq_t xwup_sqlk_get_seq(struct xwup_sqlk * sql)
{
        xwsq_t ret;

        xwmb_read(xwsq_t, ret, &sql->seq);
        xwmb_mp_rmb();
#if (XWUP_SQLK_GRANULARITY > 1)
        return ret & ((~XWUP_SQLK_GRANULARITY) + 1U);
#else
        return ret;
#endif
}

__xwup_api
void xwup_sqlk_rdex_lock(struct xwup_sqlk * sql)
{
        XWOS_UNUSED(sql);
        xwup_skd_dspmpt_lc();
}

__xwup_api
xwer_t xwup_sqlk_rdex_trylock(struct xwup_sqlk * sql)
{
        xwup_sqlk_rdex_lock(sql);
        return XWOK;
}

__xwup_api
void xwup_sqlk_rdex_unlock(struct xwup_sqlk * sql)
{
        XWOS_UNUSED(sql);
        xwup_skd_enpmpt_lc();
}

__xwup_api
void xwup_sqlk_rdex_lock_cpuirq(struct xwup_sqlk * sql)
{
        XWOS_UNUSED(sql);
        xwospl_cpuirq_disable_lc();
        xwup_skd_dspmpt_lc();
}

__xwup_api
xwer_t xwup_sqlk_rdex_trylock_cpuirq(struct xwup_sqlk * sql)
{
        xwup_sqlk_rdex_lock_cpuirq(sql);
        return XWOK;
}

__xwup_api
void xwup_sqlk_rdex_unlock_cpuirq(struct xwup_sqlk * sql)
{
        XWOS_UNUSED(sql);
        xwup_skd_enpmpt_lc();
        xwospl_cpuirq_enable_lc();
}

__xwup_api
void xwup_sqlk_rdex_lock_cpuirqsv(struct xwup_sqlk * sql, xwreg_t * flag)
{
        XWOS_UNUSED(sql);
        xwospl_cpuirq_save_lc(flag);
        xwup_skd_dspmpt_lc();
}

__xwup_api
xwer_t xwup_sqlk_rdex_trylock_cpuirqsv(struct xwup_sqlk * sql, xwreg_t * flag)
{
        xwup_sqlk_rdex_lock_cpuirqsv(sql, flag);
        return XWOK;
}

__xwup_api
void xwup_sqlk_rdex_unlock_cpuirqrs(struct xwup_sqlk * sql, xwreg_t flag)
{
        XWOS_UNUSED(sql);
        xwup_skd_enpmpt_lc();
        xwospl_cpuirq_restore_lc(flag);
}

__xwup_api
void xwup_sqlk_rdex_lock_irqs(struct xwup_sqlk * sql,
                              const struct xwos_irq_resource * irqs,
                              xwsz_t num)
{
        xwssz_t i;

        XWOS_UNUSED(sql);
        for (i = 0; i < (xwssz_t)num; i++) {
                xwup_irq_disable(irqs[i].irqn);
        }
        xwup_skd_dspmpt_lc();
}

__xwup_api
xwer_t xwup_sqlk_rdex_trylock_irqs(struct xwup_sqlk * sql,
                                   const struct xwos_irq_resource * irqs,
                                   xwsz_t num)
{
        xwup_sqlk_rdex_lock_irqs(sql, irqs, num);
        return XWOK;
}

__xwup_api
void xwup_sqlk_rdex_unlock_irqs(struct xwup_sqlk * sql,
                                const struct xwos_irq_resource * irqs,
                                xwsz_t num)
{
        xwssz_t i;

        XWOS_UNUSED(sql);
        xwup_skd_enpmpt_lc();
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                xwup_irq_enable(irqs[i].irqn);
        }
}

__xwup_api
void xwup_sqlk_rdex_lock_irqssv(struct xwup_sqlk * sql,
                                const struct xwos_irq_resource * irqs,
                                xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;

        XWOS_UNUSED(sql);
        for (i = 0; i < (xwssz_t)num; i++) {
                xwup_irq_save(irqs[i].irqn, &flags[i]);
        }
        xwup_skd_dspmpt_lc();
}

__xwup_api
xwer_t xwup_sqlk_rdex_trylock_irqssv(struct xwup_sqlk * sql,
                                     const struct xwos_irq_resource * irqs,
                                     xwreg_t flags[], xwsz_t num)
{
        xwup_sqlk_rdex_lock_irqssv(sql, irqs, flags, num);
        return XWOK;
}

__xwup_api
void xwup_sqlk_rdex_unlock_irqsrs(struct xwup_sqlk * sql,
                                  const struct xwos_irq_resource * irqs,
                                  xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;

        XWOS_UNUSED(sql);
        xwup_skd_enpmpt_lc();
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                xwup_irq_restore(irqs[i].irqn, flags[i]);
        }
}

#if defined(XWUPCFG_SKD_BH) && (1 == XWUPCFG_SKD_BH)
__xwup_api
void xwup_sqlk_rdex_lock_bh(struct xwup_sqlk * sql)
{
        XWOS_UNUSED(sql);
        xwup_skd_dspmpt_lc();
        xwup_skd_dsbh_lc();
}

__xwup_api
xwer_t xwup_sqlk_rdex_trylock_bh(struct xwup_sqlk * sql)
{
        xwup_sqlk_rdex_lock_bh(sql);
        return XWOK;
}

__xwup_api
void xwup_sqlk_rdex_unlock_bh(struct xwup_sqlk * sql)
{
        XWOS_UNUSED(sql);
        xwup_skd_enbh_lc();
        xwup_skd_enpmpt_lc();
}
#endif

__xwup_api
void xwup_sqlk_wr_lock(struct xwup_sqlk * sql)
{
        xwup_skd_dspmpt_lc();
        sql->seq += XWUP_SQLK_GRANULARITY;
        xwmb_mp_wmb();
}

__xwup_api
xwer_t xwup_sqlk_wr_trylock(struct xwup_sqlk * sql)
{
        xwup_sqlk_wr_lock(sql);
        return XWOK;
}

__xwup_api
void xwup_sqlk_wr_unlock(struct xwup_sqlk * sql)
{
        xwmb_mp_wmb();
        sql->seq += XWUP_SQLK_GRANULARITY;
        xwup_skd_enpmpt_lc();
}

__xwup_api
void xwup_sqlk_wr_lock_cpuirq(struct xwup_sqlk * sql)
{
        xwospl_cpuirq_disable_lc();
        xwup_skd_dspmpt_lc();
        sql->seq += XWUP_SQLK_GRANULARITY;
        xwmb_mp_wmb();
}

__xwup_api
xwer_t xwup_sqlk_wr_trylock_cpuirq(struct xwup_sqlk * sql)
{
        xwup_sqlk_wr_lock_cpuirq(sql);
        return XWOK;
}

__xwup_api
void xwup_sqlk_wr_unlock_cpuirq(struct xwup_sqlk * sql)
{
        xwmb_mp_wmb();
        sql->seq += XWUP_SQLK_GRANULARITY;
        xwup_skd_enpmpt_lc();
        xwospl_cpuirq_enable_lc();
}

__xwup_api
void xwup_sqlk_wr_lock_cpuirqsv(struct xwup_sqlk * sql, xwreg_t * flag)
{
        xwospl_cpuirq_save_lc(flag);
        xwup_skd_dspmpt_lc();
        sql->seq += XWUP_SQLK_GRANULARITY;
        xwmb_mp_wmb();
}

__xwup_api
xwer_t xwup_sqlk_wr_trylock_cpuirqsv(struct xwup_sqlk * sql, xwreg_t * flag)
{
        xwup_sqlk_wr_lock_cpuirqsv(sql, flag);
        return XWOK;
}

__xwup_api
void xwup_sqlk_wr_unlock_cpuirqrs(struct xwup_sqlk * sql, xwreg_t flag)
{
        xwmb_mp_wmb();
        sql->seq += XWUP_SQLK_GRANULARITY;
        xwup_skd_enpmpt_lc();
        xwospl_cpuirq_restore_lc(flag);
}

__xwup_api
void xwup_sqlk_wr_lock_irqs(struct xwup_sqlk * sql,
                            const struct xwos_irq_resource * irqs,
                            xwsz_t num)
{
        xwssz_t i;

        for (i = 0; i < (xwssz_t)num; i++) {
                xwup_irq_disable(irqs[i].irqn);
        }
        xwup_skd_dspmpt_lc();
        sql->seq += XWUP_SQLK_GRANULARITY;
        xwmb_mp_wmb();
}

__xwup_api
xwer_t xwup_sqlk_wr_trylock_irqs(struct xwup_sqlk * sql,
                                 const struct xwos_irq_resource * irqs,
                                 xwsz_t num)
{
        xwup_sqlk_wr_lock_irqs(sql, irqs, num);
        return XWOK;
}

__xwup_api
void xwup_sqlk_wr_unlock_irqs(struct xwup_sqlk * sql,
                              const struct xwos_irq_resource * irqs,
                              xwsz_t num)
{
        xwssz_t i;

        xwmb_mp_wmb();
        sql->seq += XWUP_SQLK_GRANULARITY;
        xwup_skd_enpmpt_lc();
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                xwup_irq_enable(irqs[i].irqn);
        }
}

__xwup_api
void xwup_sqlk_wr_lock_irqssv(struct xwup_sqlk * sql,
                              const struct xwos_irq_resource * irqs,
                              xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;

        for (i = 0; i < (xwssz_t)num; i++) {
                xwup_irq_save(irqs[i].irqn, &flags[i]);
        }
        xwup_skd_dspmpt_lc();
        sql->seq += XWUP_SQLK_GRANULARITY;
        xwmb_mp_wmb();
}

__xwup_api
xwer_t xwup_sqlk_wr_trylock_irqssv(struct xwup_sqlk * sql,
                                   const struct xwos_irq_resource * irqs,
                                   xwreg_t flags[], xwsz_t num)
{
        xwup_sqlk_wr_lock_irqssv(sql, irqs, flags, num);
        return XWOK;
}

__xwup_api
void xwup_sqlk_wr_unlock_irqsrs(struct xwup_sqlk * sql,
                                const struct xwos_irq_resource * irqs,
                                xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;

        xwmb_mp_wmb();
        sql->seq += XWUP_SQLK_GRANULARITY;
        xwup_skd_enpmpt_lc();
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                xwup_irq_restore(irqs[i].irqn, flags[i]);
        }
}

#if defined(XWUPCFG_SKD_BH) && (1 == XWUPCFG_SKD_BH)
__xwup_api
void xwup_sqlk_wr_lock_bh(struct xwup_sqlk * sql)
{
        xwup_skd_dspmpt_lc();
        xwup_skd_dsbh_lc();
        sql->seq += XWUP_SQLK_GRANULARITY;
        xwmb_mp_wmb();
}

__xwup_api
xwer_t xwup_sqlk_wr_trylock_bh(struct xwup_sqlk * sql)
{
        xwup_sqlk_wr_lock_bh(sql);
        return XWOK;
}

__xwup_api
void xwup_sqlk_wr_unlock_bh(struct xwup_sqlk * sql)
{
        xwmb_mp_wmb();
        sql->seq += XWUP_SQLK_GRANULARITY;
        xwup_skd_enbh_lc();
        xwup_skd_enpmpt_lc();
}
#endif
