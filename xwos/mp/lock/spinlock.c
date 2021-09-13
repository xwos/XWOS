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

#include <xwos/standard.h>
#include <xwos/ospl/irq.h>
#include <xwos/mp/skd.h>
#include <xwos/mp/lock/spinlock.h>

__xwmp_code
void xwmp_rawly_lock(struct xwmp_splk * splk)
{
#if (CPUCFG_CPU_NUM > 1)
        soc_splk_lock(&splk->socsplk);
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
}

__xwmp_code
xwer_t xwmp_rawly_trylock(struct xwmp_splk * splk)
{
        xwer_t rc;

        rc = XWOK;
#if (CPUCFG_CPU_NUM > 1)
        rc = soc_splk_trylock(&splk->socsplk);
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
        return rc;
}

__xwmp_code
void xwmp_rawly_unlock(struct xwmp_splk * splk)
{
#if (CPUCFG_CPU_NUM > 1)
        soc_splk_unlock(&splk->socsplk);
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
}

__xwmp_code
void xwmp_rawly_lock_cpuirq(struct xwmp_splk * splk)
{
        xwospl_cpuirq_disable_lc();
#if (CPUCFG_CPU_NUM > 1)
        soc_splk_lock(&splk->socsplk);
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
}

__xwmp_code
xwer_t xwmp_rawly_trylock_cpuirq(struct xwmp_splk * splk)
{
        xwer_t rc;

        rc = XWOK;
        xwospl_cpuirq_disable_lc();
#if (CPUCFG_CPU_NUM > 1)
        rc = soc_splk_trylock(&splk->socsplk);
        if (rc < 0) {
                xwospl_cpuirq_enable_lc();
        }
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
        return rc;
}

__xwmp_code
void xwmp_rawly_unlock_cpuirq(struct xwmp_splk * splk)
{
#if (CPUCFG_CPU_NUM > 1)
        soc_splk_unlock(&splk->socsplk);
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
        xwospl_cpuirq_enable_lc();
}

__xwmp_code
void xwmp_rawly_lock_cpuirqsv(struct xwmp_splk * splk, xwreg_t * cpuirq)
{
        xwospl_cpuirq_save_lc(cpuirq);
#if (CPUCFG_CPU_NUM > 1)
        soc_splk_lock(&splk->socsplk);
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
}

__xwmp_code
xwer_t xwmp_rawly_trylock_cpuirqsv(struct xwmp_splk * splk, xwreg_t * cpuirq)
{
        xwer_t rc;

        rc = XWOK;
        xwospl_cpuirq_save_lc(cpuirq);
#if (CPUCFG_CPU_NUM > 1)
        rc = soc_splk_trylock(&splk->socsplk);
        if (rc < 0) {
                xwospl_cpuirq_restore_lc(*cpuirq);
                return rc;
        }
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
        return rc;
}

__xwmp_code
void xwmp_rawly_unlock_cpuirqrs(struct xwmp_splk * splk, xwreg_t cpuirq)
{
#if (CPUCFG_CPU_NUM > 1)
        soc_splk_unlock(&splk->socsplk);
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
        xwospl_cpuirq_restore_lc(cpuirq);
}

__xwmp_code
void xwmp_rawly_lock_irqs(struct xwmp_splk * splk,
                          const struct xwos_irq_resource * irqs, xwsz_t num)
{
        xwssz_t i;

        for (i = 0; i < (xwssz_t)num; i++) {
                xwmp_irq_disable(irqs[i].irqn);
        }
#if (CPUCFG_CPU_NUM > 1)
        soc_splk_lock(&splk->socsplk);
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
}

__xwmp_code
xwer_t xwmp_rawly_trylock_irqs(struct xwmp_splk * splk,
                               const struct xwos_irq_resource * irqs, xwsz_t num)
{
        xwer_t rc;
        xwssz_t i;

        rc = XWOK;
        for (i = 0; i < (xwssz_t)num; i++) {
                xwmp_irq_disable(irqs[i].irqn);
        }
#if (CPUCFG_CPU_NUM > 1)
        rc = soc_splk_trylock(&splk->socsplk);
        if (rc < 0) {
                for (i = (xwssz_t)num - 1; i >= 0; i--) {
                        xwmp_irq_enable(irqs[i].irqn);
                }
        }
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
        return rc;
}

__xwmp_code
void xwmp_rawly_unlock_irqs(struct xwmp_splk * splk,
                            const struct xwos_irq_resource * irqs, xwsz_t num)
{
        xwssz_t i;

#if (CPUCFG_CPU_NUM > 1)
        soc_splk_unlock(&splk->socsplk);
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                xwmp_irq_enable(irqs[i].irqn);
        }
}

__xwmp_code
void xwmp_rawly_lock_irqssv(struct xwmp_splk * splk,
                            const struct xwos_irq_resource * irqs,
                            xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;

        for (i = 0; i < (xwssz_t)num; i++) {
                xwmp_irq_save(irqs[i].irqn, &flags[i]);
        }
#if (CPUCFG_CPU_NUM > 1)
        soc_splk_lock(&splk->socsplk);
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
}

__xwmp_code
xwer_t xwmp_rawly_trylock_irqssv(struct xwmp_splk * splk,
                                 const struct xwos_irq_resource * irqs,
                                 xwreg_t flags[], xwsz_t num)
{
        xwer_t rc;
        xwssz_t i;

        rc = XWOK;
        for (i = 0; i < (xwssz_t)num; i++) {
                xwmp_irq_save(irqs[i].irqn, &flags[i]);
        }
#if (CPUCFG_CPU_NUM > 1)
        rc = soc_splk_trylock(&splk->socsplk);
        if (rc < 0) {
                for (i = (xwssz_t)num - 1; i >= 0; i--) {
                        xwmp_irq_restore(irqs[i].irqn, flags[i]);
                }
                return rc;
        }
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
        return rc;
}

__xwmp_code
void xwmp_rawly_unlock_irqsrs(struct xwmp_splk * splk,
                              const struct xwos_irq_resource * irqs,
                              xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;
#if (CPUCFG_CPU_NUM > 1)
        soc_splk_unlock(&splk->socsplk);
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                xwmp_irq_restore(irqs[i].irqn, flags[i]);
        }
}

__xwmp_inline_api
void xwmp_splk_init(struct xwmp_splk * splk)
{
        soc_splk_init(&splk->socsplk);
}

__xwmp_api
void xwmp_splk_lock(struct xwmp_splk * splk)
{
        xwmp_skd_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        soc_splk_lock(&splk->socsplk);
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
}

__xwmp_api
xwer_t xwmp_splk_trylock(struct xwmp_splk * splk)
{
        xwer_t rc;

        rc = XWOK;
        xwmp_skd_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        rc = soc_splk_trylock(&splk->socsplk);
        if (rc < 0) {
                xwmp_skd_enpmpt_lc();
        }
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
        return rc;
}

__xwmp_api
void xwmp_splk_unlock(struct xwmp_splk * splk)
{
#if (CPUCFG_CPU_NUM > 1)
        soc_splk_unlock(&splk->socsplk);
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
        xwmp_skd_enpmpt_lc();
}

__xwmp_api
void xwmp_splk_lock_cpuirq(struct xwmp_splk * splk)
{
        xwospl_cpuirq_disable_lc();
        xwmp_skd_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        soc_splk_lock(&splk->socsplk);
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
}

__xwmp_api
xwer_t xwmp_splk_trylock_cpuirq(struct xwmp_splk * splk)
{
        xwer_t rc;

        rc = XWOK;
        xwospl_cpuirq_disable_lc();
        xwmp_skd_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        rc = soc_splk_trylock(&splk->socsplk);
        if (rc < 0) {
                xwmp_skd_enpmpt_lc();
                xwospl_cpuirq_enable_lc();
        }
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
        return rc;
}

__xwmp_api
void xwmp_splk_unlock_cpuirq(struct xwmp_splk * splk)
{
#if (CPUCFG_CPU_NUM > 1)
        soc_splk_unlock(&splk->socsplk);
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
        xwmp_skd_enpmpt_lc();
        xwospl_cpuirq_enable_lc();
}

__xwmp_api
void xwmp_splk_lock_cpuirqsv(struct xwmp_splk * splk, xwreg_t * cpuirq)
{
        xwospl_cpuirq_save_lc(cpuirq);
        xwmp_skd_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        soc_splk_lock(&splk->socsplk);
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
}

__xwmp_api
xwer_t xwmp_splk_trylock_cpuirqsv(struct xwmp_splk * splk, xwreg_t * cpuirq)
{
        xwer_t rc;

        rc = XWOK;
        xwospl_cpuirq_save_lc(cpuirq);
        xwmp_skd_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        rc = soc_splk_trylock(&splk->socsplk);
        if (rc < 0) {
                xwmp_skd_enpmpt_lc();
                xwospl_cpuirq_restore_lc(*cpuirq);
        }
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
        return rc;
}

__xwmp_api
void xwmp_splk_unlock_cpuirqrs(struct xwmp_splk * splk, xwreg_t cpuirq)
{
#if (CPUCFG_CPU_NUM > 1)
        soc_splk_unlock(&splk->socsplk);
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
        xwmp_skd_enpmpt_lc();
        xwospl_cpuirq_restore_lc(cpuirq);
}

__xwmp_api
void xwmp_splk_lock_irqs(struct xwmp_splk * splk,
                         const struct xwos_irq_resource * irqs, xwsz_t num)
{
        xwssz_t i;

        for (i = 0; i < (xwssz_t)num; i++) {
                xwmp_irq_disable(irqs[i].irqn);
        }
        xwmp_skd_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        soc_splk_lock(&splk->socsplk);
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
}

__xwmp_api
xwer_t xwmp_splk_trylock_irqs(struct xwmp_splk * splk,
                              const struct xwos_irq_resource * irqs, xwsz_t num)
{
        xwer_t rc;
        xwssz_t i;

        rc = XWOK;
        for (i = 0; i < (xwssz_t)num; i++) {
                xwmp_irq_disable(irqs[i].irqn);
        }
        xwmp_skd_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        rc = soc_splk_trylock(&splk->socsplk);
        if (rc < 0) {
                xwmp_skd_enpmpt_lc();
                for (i = (xwssz_t)num - 1; i >= 0; i--) {
                        xwmp_irq_enable(irqs[i].irqn);
                }
        }
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
        return rc;
}

__xwmp_api
void xwmp_splk_unlock_irqs(struct xwmp_splk * splk,
                           const struct xwos_irq_resource * irqs, xwsz_t num)
{
        xwssz_t i;

#if (CPUCFG_CPU_NUM > 1)
        soc_splk_unlock(&splk->socsplk);
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
        xwmp_skd_enpmpt_lc();
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                xwmp_irq_enable(irqs[i].irqn);
        }
}

__xwmp_api
void xwmp_splk_lock_irqssv(struct xwmp_splk * splk,
                           const struct xwos_irq_resource * irqs,
                           xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;

        for (i = 0; i < (xwssz_t)num; i++) {
                xwmp_irq_save(irqs[i].irqn, &flags[i]);
        }
        xwmp_skd_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        soc_splk_lock(&splk->socsplk);
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
}

__xwmp_api
xwer_t xwmp_splk_trylock_irqssv(struct xwmp_splk * splk,
                                const struct xwos_irq_resource * irqs,
                                xwreg_t flags[], xwsz_t num)
{
        xwer_t rc;
        xwssz_t i;

        rc = XWOK;
        for (i = 0; i < (xwssz_t)num; i++) {
                xwmp_irq_save(irqs[i].irqn, &flags[i]);
        }
        xwmp_skd_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        rc = soc_splk_trylock(&splk->socsplk);
        if (rc < 0) {
                xwmp_skd_enpmpt_lc();
                for (i = (xwssz_t)num - 1; i >= 0; i--) {
                        xwmp_irq_restore(irqs[i].irqn, flags[i]);
                }
        }
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
        return rc;
}

__xwmp_api
void xwmp_splk_unlock_irqsrs(struct xwmp_splk * splk,
                             const struct xwos_irq_resource * irqs,
                             xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;

#if (CPUCFG_CPU_NUM > 1)
        soc_splk_unlock(&splk->socsplk);
#else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#endif
        xwmp_skd_enpmpt_lc();
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                xwmp_irq_restore(irqs[i].irqn, flags[i]);
        }
}

#if defined(XWMPCFG_SKD_BH) && (1 == XWMPCFG_SKD_BH)
__xwmp_api
void xwmp_splk_lock_bh(struct xwmp_splk * splk)
{
        struct xwmp_skd * xwskd;

        xwskd = xwmp_skd_dspmpt_lc();
        xwmp_skd_dsbh(xwskd);
#  if (CPUCFG_CPU_NUM > 1)
        soc_splk_lock(&splk->socsplk);
#  else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#  endif
}

__xwmp_api
xwer_t xwmp_splk_trylock_bh(struct xwmp_splk * splk)
{
        struct xwmp_skd * xwskd;
        xwer_t rc;

        rc = XWOK;
        xwskd = xwmp_skd_dspmpt_lc();
        xwmp_skd_dsbh(xwskd);
#  if (CPUCFG_CPU_NUM > 1)
        rc = soc_splk_trylock(&splk->socsplk);
        if (rc < 0) {
                xwmp_skd_enbh(xwskd);
                xwmp_skd_enpmpt_lc();
        }
#  else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#  endif
        return rc;
}

__xwmp_api
void xwmp_splk_unlock_bh(struct xwmp_splk * splk)
{
        struct xwmp_skd * xwskd;

#  if (CPUCFG_CPU_NUM > 1)
        soc_splk_unlock(&splk->socsplk);
#  else
        XWOS_UNUSED(splk);
        xwmb_mp_mb();
#  endif
        xwskd = xwmp_skd_enbh_lc();
        xwmp_skd_enpmpt(xwskd);
}
#endif
