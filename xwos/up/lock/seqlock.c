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

#include <xwos/standard.h>
#include <xwos/ospl/irq.h>
#include <xwos/ospl/skd.h>
#include <xwos/up/lock/seqlock.h>

__xwup_api
void xwup_sqlk_init(struct xwup_sqlk * sql)
{
        sql->seq = (xwsq_t)0;
}

__xwup_api
xwsq_t xwup_sqlk_rd_begin(struct xwup_sqlk * sql)
{
        xwsq_t ret;

        do {
                xwmb_read(xwsq_t, ret, &sql->seq);
#if (XWUP_SQLK_GRANULARITY > 1)
                ret &= ((~(xwsq_t)XWUP_SQLK_GRANULARITY) + (xwsq_t)1);
#endif
        } while ((xwsq_t)0 != (ret & (xwsq_t)XWUP_SQLK_GRANULARITY));
        xwmb_mp_rmb();
        return ret;
}

__xwup_api
bool xwup_sqlk_rd_retry(struct xwup_sqlk * sql, xwsq_t start)
{
        xwmb_mp_rmb(); /* ensure all read accesses are finished */
#if (XWUP_SQLK_GRANULARITY > 1)
        return ((sql->seq & ((~(xwsq_t)XWUP_SQLK_GRANULARITY) + (xwsq_t)1)) != start);
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
        return ret & ((~(xwsq_t)XWUP_SQLK_GRANULARITY) + (xwsq_t)1);
#else
        return ret;
#endif
}

__xwup_api
void xwup_sqlk_rdex_lock(struct xwup_sqlk * sql)
{
        XWOS_UNUSED(sql);
        xwup_skd_dspmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
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
        xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
}

__xwup_api
void xwup_sqlk_rdex_lock_cpuirq(struct xwup_sqlk * sql)
{
        XWOS_UNUSED(sql);
        xwospl_cpuirq_disable_lc();
        xwup_skd_dspmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
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
        xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
        xwospl_cpuirq_enable_lc();
}

__xwup_api
void xwup_sqlk_rdex_lock_cpuirqsv(struct xwup_sqlk * sql, xwreg_t * flag)
{
        XWOS_UNUSED(sql);
        xwospl_cpuirq_save_lc(flag);
        xwup_skd_dspmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
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
        xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
        xwospl_cpuirq_restore_lc(flag);
}

__xwup_api
void xwup_sqlk_rdex_lock_irqs(struct xwup_sqlk * sql,
                              const xwirq_t irqs[],
                              xwsz_t num)
{
        xwssz_t i;

        XWOS_UNUSED(sql);
        for (i = 0; i < (xwssz_t)num; i++) {
                xwup_irq_disable(irqs[i]); // cppcheck-suppress [misra-c2012-17.7]
        }
        xwup_skd_dspmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
}

__xwup_api
xwer_t xwup_sqlk_rdex_trylock_irqs(struct xwup_sqlk * sql,
                                   const xwirq_t irqs[],
                                   xwsz_t num)
{
        xwup_sqlk_rdex_lock_irqs(sql, irqs, num);
        return XWOK;
}

__xwup_api
void xwup_sqlk_rdex_unlock_irqs(struct xwup_sqlk * sql,
                                const xwirq_t irqs[],
                                xwsz_t num)
{
        xwssz_t i;

        XWOS_UNUSED(sql);
        xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                xwup_irq_enable(irqs[i]); // cppcheck-suppress [misra-c2012-17.7]
        }
}

__xwup_api
void xwup_sqlk_rdex_lock_irqssv(struct xwup_sqlk * sql,
                                const xwirq_t irqs[],
                                xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;

        XWOS_UNUSED(sql);
        for (i = 0; i < (xwssz_t)num; i++) {
                // cppcheck-suppress [misra-c2012-17.7]
                xwup_irq_save(irqs[i], &flags[i]);
        }
        xwup_skd_dspmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
}

__xwup_api
xwer_t xwup_sqlk_rdex_trylock_irqssv(struct xwup_sqlk * sql,
                                     const xwirq_t irqs[],
                                     xwreg_t flags[], xwsz_t num)
{
        xwup_sqlk_rdex_lock_irqssv(sql, irqs, flags, num);
        return XWOK;
}

__xwup_api
void xwup_sqlk_rdex_unlock_irqsrs(struct xwup_sqlk * sql,
                                  const xwirq_t irqs[],
                                  xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;

        XWOS_UNUSED(sql);
        xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                // cppcheck-suppress [misra-c2012-17.7]
                xwup_irq_restore(irqs[i], flags[i]);
        }
}

#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
__xwup_api
void xwup_sqlk_rdex_lock_bh(struct xwup_sqlk * sql)
{
        XWOS_UNUSED(sql);
        xwup_skd_dspmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
        xwup_skd_dsbh_lc(); // cppcheck-suppress [misra-c2012-17.7]
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
        xwup_skd_enbh_lc(); // cppcheck-suppress [misra-c2012-17.7]
        xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
}
#endif

__xwup_api
void xwup_sqlk_wr_lock(struct xwup_sqlk * sql)
{
        xwup_skd_dspmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
        sql->seq += (xwsq_t)XWUP_SQLK_GRANULARITY;
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
        sql->seq += (xwsq_t)XWUP_SQLK_GRANULARITY;
        xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
}

__xwup_api
void xwup_sqlk_wr_lock_cpuirq(struct xwup_sqlk * sql)
{
        xwospl_cpuirq_disable_lc();
        xwup_skd_dspmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
        sql->seq += (xwsq_t)XWUP_SQLK_GRANULARITY;
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
        sql->seq += (xwsq_t)XWUP_SQLK_GRANULARITY;
        xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
        xwospl_cpuirq_enable_lc();
}

__xwup_api
void xwup_sqlk_wr_lock_cpuirqsv(struct xwup_sqlk * sql, xwreg_t * flag)
{
        xwospl_cpuirq_save_lc(flag);
        xwup_skd_dspmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
        sql->seq += (xwsq_t)XWUP_SQLK_GRANULARITY;
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
        sql->seq += (xwsq_t)XWUP_SQLK_GRANULARITY;
        xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
        xwospl_cpuirq_restore_lc(flag);
}

__xwup_api
void xwup_sqlk_wr_lock_irqs(struct xwup_sqlk * sql,
                            const xwirq_t irqs[],
                            xwsz_t num)
{
        xwssz_t i;

        for (i = 0; i < (xwssz_t)num; i++) {
                xwup_irq_disable(irqs[i]); // cppcheck-suppress [misra-c2012-17.7]
        }
        xwup_skd_dspmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
        sql->seq += (xwsq_t)XWUP_SQLK_GRANULARITY;
        xwmb_mp_wmb();
}

__xwup_api
xwer_t xwup_sqlk_wr_trylock_irqs(struct xwup_sqlk * sql,
                                 const xwirq_t irqs[],
                                 xwsz_t num)
{
        xwup_sqlk_wr_lock_irqs(sql, irqs, num);
        return XWOK;
}

__xwup_api
void xwup_sqlk_wr_unlock_irqs(struct xwup_sqlk * sql,
                              const xwirq_t irqs[],
                              xwsz_t num)
{
        xwssz_t i;

        xwmb_mp_wmb();
        sql->seq += (xwsq_t)XWUP_SQLK_GRANULARITY;
        xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                xwup_irq_enable(irqs[i]); // cppcheck-suppress [misra-c2012-17.7]
        }
}

__xwup_api
void xwup_sqlk_wr_lock_irqssv(struct xwup_sqlk * sql,
                              const xwirq_t irqs[],
                              xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;

        for (i = 0; i < (xwssz_t)num; i++) {
                // cppcheck-suppress [misra-c2012-17.7]
                xwup_irq_save(irqs[i], &flags[i]);
        }
        xwup_skd_dspmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
        sql->seq += (xwsq_t)XWUP_SQLK_GRANULARITY;
        xwmb_mp_wmb();
}

__xwup_api
xwer_t xwup_sqlk_wr_trylock_irqssv(struct xwup_sqlk * sql,
                                   const xwirq_t irqs[],
                                   xwreg_t flags[], xwsz_t num)
{
        xwup_sqlk_wr_lock_irqssv(sql, irqs, flags, num);
        return XWOK;
}

__xwup_api
void xwup_sqlk_wr_unlock_irqsrs(struct xwup_sqlk * sql,
                                const xwirq_t irqs[],
                                xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;

        xwmb_mp_wmb();
        sql->seq += (xwsq_t)XWUP_SQLK_GRANULARITY;
        xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                // cppcheck-suppress [misra-c2012-17.7]
                xwup_irq_restore(irqs[i], flags[i]);
        }
}

#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
__xwup_api
void xwup_sqlk_wr_lock_bh(struct xwup_sqlk * sql)
{
        xwup_skd_dspmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
        xwup_skd_dsbh_lc(); // cppcheck-suppress [misra-c2012-17.7]
        sql->seq += (xwsq_t)XWUP_SQLK_GRANULARITY;
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
        sql->seq += (xwsq_t)XWUP_SQLK_GRANULARITY;
        xwup_skd_enbh_lc(); // cppcheck-suppress [misra-c2012-17.7]
        xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
}
#endif
