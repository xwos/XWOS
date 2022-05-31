/**
 * @file
 * @brief 玄武OS UP内核：软件定时器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#if defined(XWUPCFG_SKD_SWT_STDC_MM) && (1 == XWUPCFG_SKD_SWT_STDC_MM)
#  include <stdlib.h>
#endif
#include <xwos/lib/xwaop.h>
#include <xwos/up/skd.h>
#include <xwos/up/tt.h>
#include <xwos/up/swt.h>

static __xwup_code
struct xwup_swt * xwup_swt_alloc(void);

static __xwup_code
void xwup_swt_free(struct xwup_swt * swt);

static __xwup_code
void xwup_swt_construct(struct xwup_swt * swt);

static __xwup_code
void xwup_swt_destruct(struct xwup_swt * swt);

static __xwup_code
xwer_t xwup_swt_activate(struct xwup_swt * swt, xwsq_t flag, xwobj_gc_f gcfunc);

static __xwup_code
xwer_t xwup_swt_sgc(void * obj);

static __xwup_code
xwer_t xwup_swt_dgc(void * obj);

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
        } else {
                xwup_swt_construct(swt);
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
        } else {
                xwup_swt_construct(mem.swt);
        }
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

static __xwup_code
void xwup_swt_construct(struct xwup_swt * swt)
{
        xwos_object_construct(&swt->xwobj);
        swt->flag = 0;
        xwup_ttn_init(&swt->ttn, (xwptr_t)swt, XWUP_TTN_TYPE_SWT);
        swt->cb = NULL;
        swt->arg = NULL;
        swt->period = 0;
}

static __xwup_code
void xwup_swt_destruct(struct xwup_swt * swt)
{
        xwos_object_destruct(&swt->xwobj);
}

static __xwup_code
xwer_t xwup_swt_activate(struct xwup_swt * swt, xwsq_t flag, xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwos_object_activate(&swt->xwobj, gcfunc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_xwobj_activate;
        }
        swt->flag = flag;
        return XWOK;

err_xwobj_activate:
        return rc;
}

static __xwup_code
xwer_t xwup_swt_sgc(void * obj)
{
        struct xwup_swt * swt = obj;

        xwup_swt_destruct(swt);
        return XWOK;
}

static __xwup_code
xwer_t xwup_swt_dgc(void * obj)
{
        struct xwup_swt * swt = obj;

        xwup_swt_destruct(swt);
        xwup_swt_free(swt);
        return XWOK;
}

__xwup_api
xwer_t xwup_swt_acquire(struct xwup_swt * swt, xwsq_t tik)
{
        return xwos_object_acquire(&swt->xwobj, tik);
}

__xwup_api
xwer_t xwup_swt_release(struct xwup_swt * swt, xwsq_t tik)
{
        return xwos_object_release(&swt->xwobj, tik);
}

__xwup_api
xwer_t xwup_swt_grab(struct xwup_swt * swt)
{
        return xwos_object_grab(&swt->xwobj);
}

__xwup_api
xwer_t xwup_swt_put(struct xwup_swt * swt)
{
        return xwos_object_put(&swt->xwobj);
}

__xwup_api
xwer_t xwup_swt_init(struct xwup_swt * swt, xwsq_t flag)
{
        xwup_swt_construct(swt);
        return xwup_swt_activate(swt, flag, xwup_swt_sgc);
}

__xwup_api
xwer_t xwup_swt_fini(struct xwup_swt * swt)
{
        return xwup_swt_put(swt);
}

__xwup_api
xwer_t xwup_swt_create(struct xwup_swt ** ptrbuf, xwsq_t flag)
{
        struct xwup_swt * swt;
        xwer_t rc;

        *ptrbuf = NULL;
        swt = xwup_swt_alloc();
        if (is_err(swt)) {
                rc = ptr_err(swt);
                goto err_swt_alloc;
        }
        rc = xwup_swt_activate(swt, flag, xwup_swt_dgc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_swt_activate;
        }
        *ptrbuf = swt;
        return XWOK;

err_swt_activate:
        xwup_swt_free(swt);
err_swt_alloc:
        return rc;
}

__xwup_api
xwer_t xwup_swt_delete(struct xwup_swt * swt, xwsq_t tik)
{
        return xwup_swt_release(swt, tik);
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
        xwsq_t refcnt;
        xwtm_t to;
        xwreg_t cpuirq;

        swt = entry;
        xwskd = xwup_skd_get_lc();
        xwtt = &xwskd->tt;
        swt->cb(swt, swt->arg);
        if (XWUP_SWT_FLAG_RESTART & swt->flag) {
                to = xwtm_add_safely(swt->ttn.wkup_xwtm, swt->period);
                xwup_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
                refcnt = xwos_object_get_refcnt(&swt->xwobj);
                if (refcnt >= 3) {
                        swt->ttn.wkup_xwtm = to;
                        swt->ttn.wkuprs = XWUP_TTN_WKUPRS_UNKNOWN;
                        swt->ttn.cb = xwup_swt_ttn_cb;
                        xwup_swt_grab(swt);
                        xwup_tt_add_locked(xwtt, &swt->ttn, cpuirq);
                        xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                        xwup_swt_put(swt);
                } else {
                        xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                        xwup_swt_put(swt);
                }
        } else {
                xwup_swt_put(swt);
        }
}

__xwup_api
xwer_t xwup_swt_start(struct xwup_swt * swt,
                      xwtm_t origin, xwtm_t period,
                      xwup_swt_f cb, void * arg)
{
        struct xwup_skd * xwskd;
        struct xwup_tt * xwtt;
        xwtm_t to;
        xwer_t rc;
        xwreg_t cpuirq;

        rc = xwup_swt_grab(swt);
        if (rc < 0) {
                goto err_swt_grab;
        }
        xwskd = xwup_skd_get_lc();
        xwtt = &xwskd->tt;
        to = xwtm_add_safely(origin, period);
        swt->cb = cb;
        swt->arg = arg;
        swt->period = period;
        xwup_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
        if (__xwcc_unlikely(swt->ttn.cb)) {
                rc = -EALREADY;
                goto err_already;
        }
        if (XWUP_SWT_FLAG_RESTART & swt->flag) {
                xwup_swt_grab(swt);
        }
        swt->ttn.wkup_xwtm = to;
        swt->ttn.wkuprs = XWUP_TTN_WKUPRS_UNKNOWN;
        swt->ttn.cb = xwup_swt_ttn_cb;
        swt->ttn.xwtt = xwtt;
        rc = xwup_tt_add_locked(xwtt, &swt->ttn, cpuirq);
        if (rc < 0) {
                goto err_add;
        }
        xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
        return XWOK;

err_add:
        if (XWUP_SWT_FLAG_RESTART & swt->flag) {
                xwup_swt_put(swt);
        }
err_already:
        xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
        xwup_swt_put(swt);
err_swt_grab:
        return rc;
}

__xwup_api
xwer_t xwup_swt_stop(struct xwup_swt * swt)
{
        struct xwup_skd * xwskd;
        struct xwup_tt * xwtt;
        xwreg_t cpuirq;
        xwer_t rc;

        xwskd = xwup_skd_get_lc();
        xwtt = &xwskd->tt;
        xwup_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
        rc = xwup_tt_remove_locked(xwtt, &swt->ttn);
        if (XWUP_SWT_FLAG_RESTART & swt->flag) {
                xwup_swt_put(swt);
        }
        xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
        if (XWOK == rc) {
                xwup_swt_put(swt);
        }
        return rc;
}
