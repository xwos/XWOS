/**
 * @file
 * @brief 玄武OS UP内核：软件定时器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#if defined(XWUPCFG_SKD_SWT_STDC_MM) && (1 == XWUPCFG_SKD_SWT_STDC_MM)
#  include <stdlib.h>
#endif
#include <xwos/up/skd.h>
#include <xwos/up/tt.h>
#include <xwos/up/swt.h>

static __xwup_code
struct xwup_swt * xwup_swt_alloc(void);

static __xwup_code
void xwup_swt_free(struct xwup_swt * swt);

static __xwup_code
void xwup_swt_activate(struct xwup_swt * swt,
                       const char * name,
                       xwsq_t flag);

static __xwup_code
void xwup_swt_deactivate(struct xwup_swt * swt);

static __xwup_code
void xwup_swt_ttn_cb(void * entry);

/**
 * @brief 从软件定时器对象缓存中申请一个对象
 * @return 软件定时器对象的指针
 */
static __xwup_code
struct xwup_swt * xwup_swt_alloc(void)
{
#if defined(XWUPCFG_SKD_SWT_STDC_MM) && (1 == XWUPCFG_SKD_SWT_STDC_MM)
        struct xwup_swt * swt;

        swt = malloc(sizeof(struct xwup_swt));
        if (NULL == swt) {
                swt = err_ptr(-ENOMEM);
        }
        return swt;
#else
        union {
                struct xwup_swt * swt;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwup_swt), XWMM_ALIGNMENT, &mem.anon);
        if (rc < 0) {
                mem.swt = err_ptr(-ENOMEM);
        }/* else {} */
        return mem.swt;
#endif
}

/**
 * @brief 释放软件定时器对象
 * @param[in] swt: 软件定时器对象的指针
 */
static __xwup_code
void xwup_swt_free(struct xwup_swt * swt)
{
#if defined(XWUPCFG_SKD_SWT_STDC_MM) && (1 == XWUPCFG_SKD_SWT_STDC_MM)
        free(swt);
#else
        xwmm_kma_free(swt);
#endif
}

/**
 * @brief 激活软件定时器对象
 * @param[in] swt: 软件定时器对象的指针
 * @param[in] name: 名字
 * @param[in] flag: 标志
 */
static __xwup_code
void xwup_swt_activate(struct xwup_swt * swt,
                       const char * name,
                       xwsq_t flag)
{
        xwup_ttn_init(&swt->ttn, (xwptr_t)swt, XWUP_TTN_TYPE_SWT);
        swt->cb = NULL;
        swt->arg = NULL;
        swt->period = 0;
        swt->name = name;
        swt->flag = flag;
}

/**
 * @brief 使得软件定时器对象无效
 * @param[in] swt: 软件定时器对象的指针
 * @param[in] name: 名字
 * @param[in] flag: 标志
 */
static __xwup_code
void xwup_swt_deactivate(struct xwup_swt * swt)
{
        xwup_swt_stop(swt);
        swt->cb = NULL;
        swt->arg = NULL;
        swt->period = 0;
        swt->name = NULL;
        swt->flag = 0;
}

__xwup_api
xwer_t xwup_swt_init(struct xwup_swt * swt,
                     const char * name,
                     xwsq_t flag)
{
        XWOS_VALIDATE((swt), "nullptr", -EFAULT);
        xwup_swt_activate(swt, name, flag);
        return XWOK;
}

__xwup_api
xwer_t xwup_swt_fini(struct xwup_swt * swt)
{
        XWOS_VALIDATE((swt), "nullptr", -EFAULT);

        xwup_swt_deactivate(swt);
        return XWOK;
}

__xwup_api
xwer_t xwup_swt_create(struct xwup_swt ** ptrbuf,
                       const char * name,
                       xwsq_t flag)
{
        struct xwup_swt * swt;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);

        *ptrbuf = NULL;
        swt = xwup_swt_alloc();
        if (is_err(swt)) {
                rc = ptr_err(swt);
                goto err_swt_alloc;
        }
        xwup_swt_activate(swt, name, flag);
        *ptrbuf = swt;
        return XWOK;

err_swt_alloc:
        return rc;
}

__xwup_api
xwer_t xwup_swt_delete(struct xwup_swt * swt)
{
        XWOS_VALIDATE((swt), "nullptr", -EFAULT);

        xwup_swt_deactivate(swt);
        xwup_swt_free(swt);
        return XWOK;
}

/**
 * @brief 软件定时器的时间树节点回调函数
 * @param[in] entry: 软件定时器对象的指针
 */
static __xwup_code
void xwup_swt_ttn_cb(void * entry)
{
        struct xwup_swt * swt;
        struct xwup_skd * xwskd;
        struct xwup_tt * xwtt;
        xwtm_t expected;
        xwreg_t cpuirq;
        xwer_t rc;

        swt = entry;
        xwskd = xwup_skd_get_lc();
        xwtt = &xwskd->tt;
        swt->cb(swt, swt->arg);
        if (XWUP_SWT_FLAG_RESTART & swt->flag) {
                expected = xwtm_add_safely(swt->ttn.wkup_xwtm, swt->period);
                xwup_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
                swt->ttn.wkup_xwtm = expected;
                swt->ttn.wkuprs = XWUP_TTN_WKUPRS_UNKNOWN;
                swt->ttn.cb = xwup_swt_ttn_cb;
                rc = xwup_tt_add_locked(xwtt, &swt->ttn, cpuirq);
                xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                if (__xwcc_unlikely(rc < 0)) {
                }
        }
}

__xwup_api
xwer_t xwup_swt_start(struct xwup_swt * swt,
                      xwtm_t base, xwtm_t period,
                      xwup_swt_f cb, void * arg)
{
        struct xwup_skd * xwskd;
        struct xwup_tt * xwtt;
        xwtm_t expected;
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((swt), "nullptr", -EFAULT);
        XWOS_VALIDATE((cb), "nullptr", -EFAULT);
        XWOS_VALIDATE(((xwtm_cmp(base, 0) > 0) && (xwtm_cmp(period, 0) > 0)),
                      "out-of-time", -EINVAL);

        if (__xwcc_unlikely(swt->ttn.cb)) {
                rc = -EALREADY;
                goto err_already;
        }
        xwskd = xwup_skd_get_lc();
        xwtt = &xwskd->tt;
        expected = xwtm_add_safely(base, period);
        swt->cb = cb;
        swt->arg = arg;
        swt->period = period;
        xwup_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
        /* add to time tree */
        swt->ttn.wkup_xwtm = expected;
        swt->ttn.wkuprs = XWUP_TTN_WKUPRS_UNKNOWN;
        swt->ttn.cb = xwup_swt_ttn_cb;
        swt->ttn.xwtt = xwtt;
        rc = xwup_tt_add_locked(xwtt, &swt->ttn, cpuirq);
        xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);

err_already:
        return rc;
}

__xwup_api
xwer_t xwup_swt_stop(struct xwup_swt * swt)
{
        struct xwup_skd * xwskd;
        struct xwup_tt * xwtt;
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((swt), "nullptr", -EFAULT);

        xwskd = xwup_skd_get_lc();
        xwtt = &xwskd->tt;
        xwup_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
        rc = xwup_tt_remove_locked(xwtt, &swt->ttn);
        xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
        return rc;
}
