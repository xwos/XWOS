/**
 * @file
 * @brief 玄武OS MP内核：软件定时器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/lib/object.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#if defined(XWMPCFG_SKD_SWT_MEMSLICE) && (1 == XWMPCFG_SKD_SWT_MEMSLICE)
#  include <xwos/mm/memslice.h>
#elif defined(XWMPCFG_SKD_SWT_STDC_MM) && (1 == XWMPCFG_SKD_SWT_STDC_MM)
#  include <stdlib.h>
#endif
#include <xwos/mp/skd.h>
#include <xwos/mp/tt.h>
#include <xwos/mp/swt.h>

static __xwmp_code
struct xwmp_swt * xwmp_swt_alloc(void);

static __xwmp_code
void xwmp_swt_free(struct xwmp_swt * swt);

static __xwmp_code
void xwmp_swt_construct(struct xwmp_swt * swt);

static __xwmp_code
void xwmp_swt_destruct(struct xwmp_swt * swt);

static __xwmp_code
xwer_t xwmp_swt_activate(struct xwmp_swt * swt, xwsq_t flag, xwobj_gc_f gcfunc);

static __xwmp_code
xwer_t xwmp_swt_gc(void * swt);

static __xwmp_code
void xwmp_swt_ttn_cb(void * entry);

#if defined(XWMPCFG_SKD_SWT_MEMSLICE) && (1 == XWMPCFG_SKD_SWT_MEMSLICE)
/**
 * @brief 软件定时器对象缓存
 */
static __xwmp_data struct xwmm_memslice xwmp_swt_cache;

/**
 * @brief 软件定时器对象缓存的名字
 */
const __xwmp_rodata char xwmp_swt_cache_name[] = "xwos.mp.swt.cache";
#endif

#if defined(XWMPCFG_SKD_SWT_MEMSLICE) && (1 == XWMPCFG_SKD_SWT_MEMSLICE)
/**
 * @brief XWMP INIT CODE：初始化软件定时器对象缓存
 * @param[in] zone_origin: 内存区域首地址
 * @param[in] zone_size: 内存区域大小
 * @return 错误码
 * @note
 * - 重入性：只可在系统初始化时使用一次
 */
__xwmp_init_code
xwer_t xwmp_swt_cache_init(xwptr_t zone_origin, xwsz_t zone_size)
{
        xwer_t rc;

        rc = xwmm_memslice_init(&xwmp_swt_cache, zone_origin, zone_size,
                                sizeof(struct xwmp_swt),
                                xwmp_swt_cache_name,
                                (ctor_f)xwmp_swt_construct,
                                (dtor_f)xwmp_swt_destruct);
        return rc;
}
#endif

/**
 * @brief 从软件定时器对象缓存中申请一个对象
 * @return 软件定时器对象的指针
 */
static __xwmp_code
struct xwmp_swt * xwmp_swt_alloc(void)
{
#if defined(XWMPCFG_SKD_SWT_MEMSLICE) && (1 == XWMPCFG_SKD_SWT_MEMSLICE)
        union {
                struct xwmp_swt * swt;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_memslice_alloc(&xwmp_swt_cache, &mem.anon);
        if (rc < 0) {
                mem.swt = err_ptr(rc);
        }/* else {} */
        return mem.swt;
#elif defined(XWMPCFG_SKD_SWT_STDC_MM) && (1 == XWMPCFG_SKD_SWT_STDC_MM)
        struct xwmp_swt * swt;

        swt = malloc(sizeof(struct xwmp_swt));
        if (NULL == swt) {
                swt = err_ptr(-ENOMEM);
        } else {
                xwmp_swt_construct(swt);
        }
        return swt;
#else
        union {
                struct xwmp_swt * swt;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwmp_swt), XWMM_ALIGNMENT, &mem.anon);
        if (XWOK == rc) {
                xwmp_swt_construct(mem.swt);
        } else {
                mem.swt = err_ptr(-ENOMEM);
        }
        return mem.swt;
#endif
}

/**
 * @brief 释放软件定时器对象
 * @param[in] swt: 软件定时器对象的指针
 */
static __xwmp_code
void xwmp_swt_free(struct xwmp_swt * swt)
{
#if defined(XWMPCFG_SKD_SWT_MEMSLICE) && (1 == XWMPCFG_SKD_SWT_MEMSLICE)
        xwmm_memslice_free(&xwmp_swt_cache, swt);
#elif defined(XWMPCFG_SKD_SWT_STDC_MM) && (1 == XWMPCFG_SKD_SWT_STDC_MM)
        xwmp_swt_destruct(swt);
        free(swt);
#else
        xwmp_swt_destruct(swt);
        xwmm_kma_free(swt);
#endif
}

/**
 * @brief 软件定时器对象的构造函数
 * @param[in] swt: 软件定时器对象的指针
 */
static __xwmp_code
void xwmp_swt_construct(struct xwmp_swt * swt)
{
        xwos_object_construct(&swt->xwobj);
        swt->xwskd = NULL;
        swt->flag = 0;
        xwmp_ttn_init(&swt->ttn, (xwptr_t)swt, XWMP_TTN_TYPE_SWT);
        swt->cb = NULL;
        swt->arg = NULL;
        swt->period = 0;
}

/**
 * @brief 软件定时器对象的析构函数
 * @param[in] swt: 软件定时器对象的指针
 */
static __xwmp_code
void xwmp_swt_destruct(struct xwmp_swt * swt)
{
        swt->xwskd = NULL;
        swt->flag = 0;
        xwos_object_destruct(&swt->xwobj);
}

/**
 * @brief 软件定时器对象的垃圾回收函数
 * @param[in] swt: 软件定时器对象的指针
 * @return 错误码
 */
static __xwmp_code
xwer_t xwmp_swt_gc(void * swt)
{
        xwmp_swt_free(swt);
        return XWOK;
}

/**
 * @brief 激活软件定时器对象
 * @param[in] swt: 软件定时器对象的指针
 * @param[in] flag: 标志
 * @param[in] gcfunc: 垃圾回收函数：当对象应用计数为0，调用此函数回收资源
 * @return 错误码
 */
static __xwmp_code
xwer_t xwmp_swt_activate(struct xwmp_swt * swt,
                         xwsq_t flag,
                         xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwos_object_activate(&swt->xwobj, gcfunc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_xwobj_activate;
        }

        swt->xwskd = xwmp_skd_get_lc();
        swt->flag = flag;
        return XWOK;

err_xwobj_activate:
        return rc;
}

__xwmp_api
xwer_t xwmp_swt_init(struct xwmp_swt * swt, xwsq_t flag)
{
        XWOS_VALIDATE((swt), "nullptr", -EFAULT);

        xwmp_swt_construct(swt);
        return xwmp_swt_activate(swt, flag, NULL);
}

__xwmp_api
xwer_t xwmp_swt_fini(struct xwmp_swt * swt)
{
        XWOS_VALIDATE((swt), "nullptr", -EFAULT);

        return xwmp_swt_put(swt);
}

__xwmp_api
xwer_t xwmp_swt_create(struct xwmp_swt ** swtbuf, xwsq_t flag)
{
        struct xwmp_swt * swt;
        xwer_t rc;

        *swtbuf = NULL;
        swt = xwmp_swt_alloc();
        if (is_err(swt)) {
                rc = ptr_err(swt);
                goto err_swt_alloc;
        }
        rc = xwmp_swt_activate(swt, flag, xwmp_swt_gc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_swt_activate;
        }
        *swtbuf = swt;
        return XWOK;

err_swt_activate:
        xwmp_swt_free(swt);
err_swt_alloc:
        return rc;
}

__xwmp_api
xwer_t xwmp_swt_delete(struct xwmp_swt * swt, xwsq_t tik)
{
        return xwmp_swt_release(swt, tik);
}

__xwmp_api
xwer_t xwmp_swt_acquire(struct xwmp_swt * swt, xwsq_t tik)
{
        return xwos_object_acquire(&swt->xwobj, tik);
}

__xwmp_api
xwer_t xwmp_swt_release(struct xwmp_swt * swt, xwsq_t tik)
{
        return xwos_object_release(&swt->xwobj, tik);
}

__xwmp_api
xwer_t xwmp_swt_grab(struct xwmp_swt * swt)
{
        return xwos_object_grab(&swt->xwobj);
}

__xwmp_api
xwer_t xwmp_swt_put(struct xwmp_swt * swt)
{
        return xwos_object_put(&swt->xwobj);
}

/**
 * @brief 软件定时器的时间树节点回调函数
 * @param[in] entry: 软件定时器对象的指针
 */
static __xwmp_code
void xwmp_swt_ttn_cb(void * entry)
{
        struct xwmp_swt * swt;
        struct xwmp_tt * xwtt;
        xwsq_t refcnt;
        xwtm_t to;
        xwreg_t cpuirq;

        swt = entry;
        xwtt = &swt->xwskd->tt;
        swt->cb(swt, swt->arg);
        if (XWMP_SWT_FLAG_RESTART & swt->flag) {
                to = xwtm_add_safely(swt->ttn.wkup_xwtm, swt->period);
                /* 若此时，其他CPU调用 `xwmp_swt_stop()` ，会失败，
                   `xwmp_swt_stop()` 只会减少一次引用计数。
                   引用计数的值一定小于3，本地CPU可由此获知需要停止重启软件定时器。*/
                xwmp_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
                refcnt = xwos_object_get_refcnt(&swt->xwobj);
                if (refcnt >= 3) {
                        swt->ttn.wkup_xwtm = to;
                        xwaop_write(xwsq_t, &swt->ttn.wkuprs,
                                    XWMP_TTN_WKUPRS_UNKNOWN, NULL);
                        swt->ttn.cb = xwmp_swt_ttn_cb;
                        /* 在本地CPU执行 `xwmp_tt_add_locked()` 的过程中，其他CPU可能会执行
                           `xwmp_swt_stop() + xwmp_swt_delete()` ，导致 `swt` 成为野指针。
                           因此，需要在 `xwmp_tt_add_locked()` 之前增加对 `swt` 的引用。
                           + 如果[本地CPU] `xwmp_tt_add_locked()` 在
                             [其他CPU] `xwmp_swt_stop()` 之后执行成功，软件定时器将在下一次
                             进入回调函数后彻底停止。
                           + 如果[本地CPU] `xwmp_tt_add_locked()` 执行失败，
                             软件定时器立即停止。
                           + 如果[本地CPU] `xwmp_tt_add_locked()` 执行成功之后，
                             [其他CPU] `xwmp_swt_stop()` 也执行成功，
                             软件定时器不会进入下一次的回调函数。 */
                        xwmp_swt_grab(swt);
                        xwmp_tt_add_locked(xwtt, &swt->ttn, cpuirq);
                        xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                        xwmp_swt_put(swt);
                } else {
                        xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                        xwmp_swt_put(swt);
                }
        } else {
                /* 若此时，其他CPU调用 `xwmp_swt_stop()` ，会失败，
                   `xwmp_swt_stop()` 中减少引用计数的代码不会执行。 */
                xwmp_swt_put(swt);
        }
}

__xwmp_api
xwer_t xwmp_swt_start(struct xwmp_swt * swt,
                      xwtm_t origin, xwtm_t period,
                      xwmp_swt_f cb, void * arg)
{
        struct xwmp_tt * xwtt;
        xwtm_t to;
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((swt), "nullptr", -EFAULT);
        XWOS_VALIDATE((cb), "nullptr", -EFAULT);
        XWOS_VALIDATE(((xwtm_cmp(origin, 0) > 0) && (xwtm_cmp(period, 0) > 0)),
                      "out-of-time", -EINVAL);

        rc = xwmp_swt_grab(swt);
        if (rc < 0) {
                goto err_swt_grab;
        }
        xwtt = &swt->xwskd->tt;
        to = xwtm_add_safely(origin, period);
        xwmp_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
        if (__xwcc_unlikely(NULL != swt->ttn.cb)) {
                rc = -EALREADY;
                goto err_already;
        }
        if (XWMP_SWT_FLAG_RESTART & swt->flag) {
                /* 多增加一次引用计数作为继续重启定时器的依据。 */
                xwmp_swt_grab(swt);
        }
        swt->cb = cb;
        swt->arg = arg;
        swt->period = period;
        swt->ttn.wkup_xwtm = to;
        xwaop_write(xwsq_t, &swt->ttn.wkuprs, XWMP_TTN_WKUPRS_UNKNOWN, NULL);
        swt->ttn.xwtt = xwtt;
        swt->ttn.cb = xwmp_swt_ttn_cb;
        rc = xwmp_tt_add_locked(xwtt, &swt->ttn, cpuirq);
        if (rc < 0) {
                goto err_swt_add;
        }
        xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
        return XWOK;

err_swt_add:
        if (XWMP_SWT_FLAG_RESTART & swt->flag) {
                xwmp_swt_put(swt);
        }
err_already:
        xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
        xwmp_swt_put(swt);
err_swt_grab:
        return rc;
}

__xwmp_api
xwer_t xwmp_swt_stop(struct xwmp_swt * swt)
{
        xwreg_t cpuirq;
        xwer_t rc;
        struct xwmp_tt * xwtt;

        XWOS_VALIDATE((swt), "nullptr", -EFAULT);

        xwtt = &swt->xwskd->tt;
        xwmp_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
        rc = xwmp_tt_remove_locked(xwtt, &swt->ttn);
        if (XWMP_SWT_FLAG_RESTART & swt->flag) {
                xwmp_swt_put(swt);
        }
        xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
        if (XWOK == rc) {
                /* 从时间树上删除成功，回调函数不可能再被执行。
                   回调函数中减少引用计数的代码也不会执行 */
                xwmp_swt_put(swt);
        }
        return rc;
}
