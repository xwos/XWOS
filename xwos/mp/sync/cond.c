/**
 * @file
 * @brief 玄武OS MP内核同步机制：条件量
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * @note
 * - 锁的顺序：同级的锁不可同时获得
 *   + ① 等待队列的锁（plwq->lock）
 *     + ② 等待队列节点的锁（wqn->lock）
 *     + ② 线程控制块的状态锁（thd->stlock）
 */

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#if defined(XWMPCFG_SYNC_COND_MEMSLICE) && (1 == XWMPCFG_SYNC_COND_MEMSLICE)
#  include <xwos/mm/memslice.h>
#elif defined(XWMPCFG_SYNC_COND_STDC_MM) && (1 == XWMPCFG_SYNC_COND_STDC_MM)
#  include <stdlib.h>
#endif
#include <xwos/ospl/irq.h>
#include <xwos/mp/skd.h>
#include <xwos/mp/tt.h>
#include <xwos/mp/thd.h>
#include <xwos/mp/plwq.h>
#include <xwos/mp/lock/spinlock.h>
#include <xwos/mp/lock/seqlock.h>
#include <xwos/mp/lock/mtx.h>
#include <xwos/mp/sync/obj.h>
#include <xwos/mp/sync/evt.h>
#include <xwos/mp/sync/cond.h>

#define XWMP_COND_NEGTIVE  ((xwssq_t)(-1))
#define XWMP_COND_POSITIVE ((xwssq_t)(1))

#if defined(XWMPCFG_SYNC_COND_MEMSLICE) && (1 == XWMPCFG_SYNC_COND_MEMSLICE)
/**
 * @brief 结构体xwmp_cond的对象缓存
 */
static __xwmp_data struct xwmm_memslice xwmp_cond_cache;

/**
 * @brief 结构体xwmp_cond的对象缓存的名字
 */
const __xwmp_rodata char xwmp_cond_cache_name[] = "xwos.mp.sync.cond.cache";
#endif

static __xwmp_code
struct xwmp_cond * xwmp_cond_alloc(void);

static __xwmp_code
void xwmp_cond_free(struct xwmp_cond * cond);

static __xwmp_code
xwer_t xwmp_cond_gc(void * cond);

static __xwmp_code
xwer_t xwmp_cond_broadcast_once(struct xwmp_cond * cond, bool * retry);

static __xwmp_code
xwer_t xwmp_cond_do_timedblkthd_unlkwq_cpuirqrs(struct xwmp_cond * cond,
                                                struct xwmp_thd * thd,
                                                void * lock, xwsq_t lktype,
                                                void * lkdata,
                                                xwtm_t * xwtm, xwsq_t * lkst,
                                                xwreg_t cpuirq);

#if defined(XWMPCFG_SYNC_COND_MEMSLICE) && (1 == XWMPCFG_SYNC_COND_MEMSLICE)
/**
 * @brief XWMP INIT CODE：初始化结构体xwmp_cond的对象缓存
 * @param[in] zone_origin: 内存区域的首地址
 * @param[in] zone_size: 内存区域的大小
 * @return 错误码
 * @note
 * - 重入性：只可在系统初始化时使用一次
 */
__xwmp_init_code
xwer_t xwmp_cond_cache_init(xwptr_t zone_origin, xwsz_t zone_size)
{
        xwer_t rc;

        rc = xwmm_memslice_init(&xwmp_cond_cache, zone_origin, zone_size,
                                sizeof(struct xwmp_cond),
                                xwmp_cond_cache_name,
                                (ctor_f)xwmp_cond_construct,
                                (dtor_f)xwmp_cond_destruct);
        return rc;
}
#endif

/**
 * @brief 从条件量对象缓存中申请对象
 * @return 条件量对象的指针
 */
static __xwmp_code
struct xwmp_cond * xwmp_cond_alloc(void)
{
#if defined(XWMPCFG_SYNC_COND_MEMSLICE) && (1 == XWMPCFG_SYNC_COND_MEMSLICE)
        union {
                struct xwmp_cond * cond;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_memslice_alloc(&xwmp_cond_cache, &mem.anon);
        if (rc < 0) {
                mem.cond = err_ptr(rc);
        }/* else {} */
        return mem.cond;
#elif defined(XWMPCFG_SYNC_COND_STDC_MM) && (1 == XWMPCFG_SYNC_COND_STDC_MM)
        struct xwmp_cond * cond;

        cond = malloc(sizeof(struct xwmp_cond));
        if (NULL == cond) {
                cond = err_ptr(-ENOMEM);
        }
        return cond;
#else
        union {
                struct xwmp_cond * cond;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwmp_cond), XWMM_ALIGNMENT, &mem.anon);
        if (XWOK == rc) {
                xwmp_cond_construct(mem.cond);
        } else {
                mem.cond = err_ptr(-ENOMEM);
        }
        return mem.cond;
#endif
}

/**
 * @brief 释放条件量对象
 * @param[in] cond: 条件量对象的指针
 */
static __xwmp_code
void xwmp_cond_free(struct xwmp_cond * cond)
{
#if defined(XWMPCFG_SYNC_COND_MEMSLICE) && (1 == XWMPCFG_SYNC_COND_MEMSLICE)
        xwmm_memslice_free(&xwmp_cond_cache, cond);
#elif defined(XWMPCFG_SYNC_COND_STDC_MM) && (1 == XWMPCFG_SYNC_COND_STDC_MM)
        xwmp_cond_destruct(cond);
        free(cond);
#else
        xwmp_cond_destruct(cond);
        xwmm_kma_free(cond);
#endif
}

/**
 * @brief 条件量对象的构造函数
 * @param[in] cond: 条件量对象的指针
 */
__xwmp_code
void xwmp_cond_construct(struct xwmp_cond * cond)
{
        xwmp_synobj_construct(&cond->synobj);
}

/**
 * @brief 条件量对象的析构函数
 * @param[in] cond: 条件量对象的指针
 */
__xwmp_code
void xwmp_cond_destruct(struct xwmp_cond * cond)
{
        xwmp_synobj_destruct(&cond->synobj);
}

/**
 * @brief 条件量对象的垃圾回收函数
 * @param[in] cond: 条件量对象的指针
 * @return 错误码
 */
static __xwmp_code
xwer_t xwmp_cond_gc(void * cond)
{
        xwmp_cond_free((struct xwmp_cond *)cond);
        return XWOK;
}

/**
 * @brief 激活条件量
 * @param[in] cond: 条件量对象的指针
 * @param[in] gcfunc: 垃圾回收函数的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入，除非对象的引用计数重新为0
 * @note
 * - 静态初始化的对象所有资源都是由用户自己提供的，
 *   因此当对象销毁时，垃圾回收函数也需要用户自己提供。
 */
__xwmp_code
xwer_t xwmp_cond_activate(struct xwmp_cond * cond, xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwmp_synobj_activate(&cond->synobj, gcfunc);
        if (__xwcc_likely(XWOK == rc)) {
                xwmp_plwq_init(&cond->wq.pl);
                cond->count = XWMP_COND_POSITIVE;
        }/* else {} */
        return rc;
}

__xwmp_api
xwer_t xwmp_cond_init(struct xwmp_cond * cond)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        xwmp_cond_construct(cond);
        return xwmp_cond_activate(cond, NULL);
}

__xwmp_api
xwer_t xwmp_cond_fini(struct xwmp_cond * cond)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        return xwmp_cond_put(cond);
}

__xwmp_api
xwer_t xwmp_cond_create(struct xwmp_cond ** ptrbuf)
{
        struct xwmp_cond * cond;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);

        *ptrbuf = NULL;
        cond = xwmp_cond_alloc();
        if (__xwcc_unlikely(is_err(cond))) {
                rc = ptr_err(cond);
        } else {
                rc = xwmp_cond_activate(cond, xwmp_cond_gc);
                if (__xwcc_unlikely(rc < 0)) {
                        xwmp_cond_free(cond);
                } else {
                        *ptrbuf = cond;
                }
        }
        return rc;
}

__xwmp_api
xwer_t xwmp_cond_delete(struct xwmp_cond * cond)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        return xwmp_cond_put(cond);
}

__xwmp_api
xwer_t xwmp_cond_acquire(struct xwmp_cond * cond, xwsq_t tik)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);
        return xwmp_synobj_acquire(&cond->synobj, tik);
}

__xwmp_api
xwer_t xwmp_cond_release(struct xwmp_cond * cond, xwsq_t tik)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);
        return xwmp_synobj_release(&cond->synobj, tik);
}

__xwmp_api
xwer_t xwmp_cond_grab(struct xwmp_cond * cond)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);
        return xwmp_synobj_grab(&cond->synobj);
}

__xwmp_api
xwer_t xwmp_cond_put(struct xwmp_cond * cond)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);
        return xwmp_synobj_put(&cond->synobj);
}

#if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
__xwmp_api
xwer_t xwmp_cond_bind(struct xwmp_cond * cond,
                      struct xwmp_evt * evt, xwsq_t pos)
{
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        rc = xwmp_cond_grab(cond);
        if (__xwcc_likely(XWOK == rc)) {
                xwmp_plwq_lock_cpuirqsv(&cond->wq.pl, &cpuirq);
                rc = xwmp_sel_obj_bind(evt, &cond->synobj, pos, false);
                xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
        }
        return rc;
}

__xwmp_api
xwer_t xwmp_cond_unbind(struct xwmp_cond * cond, struct xwmp_evt * evt)
{
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((cond), "nullptr", -EFAULT);
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        xwmp_plwq_lock_cpuirqsv(&cond->wq.pl, &cpuirq);
        rc = xwmp_sel_obj_unbind(evt, &cond->synobj, false);
        xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
        if (XWOK == rc) {
                xwmp_cond_put(cond);
        }
        return rc;
}
#endif

__xwmp_api
xwer_t xwmp_cond_freeze(struct xwmp_cond * cond)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        rc = xwmp_cond_grab(cond);
        if (__xwcc_likely(XWOK == rc)) {
                xwmp_plwq_lock_cpuirqsv(&cond->wq.pl, &cpuirq);
                if (__xwcc_unlikely(cond->count < 0)) {
                        rc = -EALREADY;
                } else {
                        cond->count = XWMP_COND_NEGTIVE;
                }
                xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
                xwmp_cond_put(cond);
        }/* else {} */
        return rc;
}

__xwmp_api
xwer_t xwmp_cond_thaw(struct xwmp_cond * cond)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        rc = xwmp_cond_grab(cond);
        if (__xwcc_likely(XWOK == rc)) {
                xwmp_plwq_lock_cpuirqsv(&cond->wq.pl, &cpuirq);
                if (__xwcc_unlikely(cond->count >= 0)) {
                        rc = -EALREADY;
                } else {
                        cond->count = XWMP_COND_POSITIVE;
                        rc = XWOK;
                }
                xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
                xwmp_cond_put(cond);
        }/* else {} */
        return rc;
}

/**
 * @brief 中断条件量等待队列中的一个节点
 * @param[in] cond: 条件量对象的指针
 * @param[in] wqn: 等待队列节点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmp_code
xwer_t xwmp_cond_intr(struct xwmp_cond * cond, struct xwmp_wqn * wqn)
{
        xwmp_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwmp_cond_grab(cond);
        if (__xwcc_likely(XWOK == rc)) {
                xwmp_plwq_lock_cpuirqsv(&cond->wq.pl, &cpuirq);
                xwmp_splk_lock(&wqn->lock);
                rc = xwmp_plwq_remove_locked(&cond->wq.pl, wqn);
                if (XWOK == rc) {
                        wqn->wq = NULL;
                        wqn->type = XWMP_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq, &wqn->reason,
                                    xwmb_modr_release, XWMP_WQN_REASON_INTR);
                        cb = wqn->cb;
                        wqn->cb = NULL;
                        xwmp_splk_unlock(&wqn->lock);
                        xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
                        xwmp_cond_put(cond);
                        cb(wqn->owner);
                } else {
                        xwmp_splk_unlock(&wqn->lock);
                        xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
                        xwmp_cond_put(cond);
                }
        }/* else {} */
        return rc;
}

__xwmp_api
xwer_t xwmp_cond_intr_all(struct xwmp_cond * cond)
{
        struct xwmp_wqn * c;
        xwmp_wqn_f cb;
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        rc = xwmp_cond_grab(cond);
        if (__xwcc_likely(XWOK == rc)) {
                xwmp_plwq_lock_cpuirqsv(&cond->wq.pl, &cpuirq);
                xwmp_plwq_itr_wqn_rm(c, &cond->wq.pl) {
                        xwmp_splk_lock(&c->lock);
                        xwmp_plwq_remove_locked(&cond->wq.pl, c);
                        c->wq = NULL;
                        c->type = XWMP_WQTYPE_UNKNOWN;
                        c->reason = XWMP_WQN_REASON_INTR;
                        cb = c->cb;
                        c->cb = NULL;
                        xwmp_splk_unlock(&c->lock);
                        xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
                        cb(c->owner);
                        xwmp_plwq_lock_cpuirq(&cond->wq.pl);
                }
                xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
                xwmp_cond_put(cond);
        }/* else {} */
        return rc;
}

static __xwmp_code
xwer_t xwmp_cond_broadcast_once(struct xwmp_cond * cond, bool * retry)
{
        struct xwmp_wqn * wqn;
        xwmp_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = XWOK;
        xwmp_plwq_lock_cpuirqsv(&cond->wq.pl, &cpuirq);
        if (__xwcc_unlikely(cond->count < 0)) {
                xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
                rc = -ENEGATIVE;
                *retry = false;
        } else {
                wqn = xwmp_plwq_choose_locked(&cond->wq.pl);
                if (wqn) {
                        wqn->wq = NULL;
                        wqn->type = XWMP_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq, &wqn->reason,
                                    xwmb_modr_release, XWMP_WQN_REASON_UP);
                        cb = wqn->cb;
                        wqn->cb = NULL;
                        xwmp_splk_unlock(&wqn->lock);
                        xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
                        cb(wqn->owner);
                        *retry = true;
                } else {
                        xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
                        *retry = false;
                }
        }
        return rc;
}

__xwmp_api
xwer_t xwmp_cond_broadcast(struct xwmp_cond * cond)
{
        bool retry;
        xwer_t rc;

        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        rc = xwmp_cond_grab(cond);
        if (__xwcc_likely(XWOK == rc)) {
                retry = false;
                do {
                        rc = xwmp_cond_broadcast_once(cond, &retry);
                } while (retry);
#if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
                if (__xwcc_likely(XWOK == rc)) {
                        struct xwmp_evt * evt;
                        struct xwmp_synobj * synobj;
                        xwreg_t cpuirq;

                        synobj = &cond->synobj;
                        xwmp_plwq_lock_cpuirqsv(&cond->wq.pl, &cpuirq);
                        xwmb_mp_load_acquire(struct xwmp_evt *,
                                             evt, &synobj->sel.evt);
                        if (NULL != evt) {
                                xwmp_sel_obj_s1i(evt, synobj);
                        }
                        xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
                }
#endif
                xwmp_cond_put(cond);
        }/* else {} */
        return rc;
}

__xwmp_code
xwer_t xwmp_cond_do_unicast(struct xwmp_cond * cond)
{
        struct xwmp_wqn * wqn;
        xwmp_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        xwmp_plwq_lock_cpuirqsv(&cond->wq.pl, &cpuirq);
        if (__xwcc_unlikely(cond->count < 0)) {
                xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
                xwmp_cond_put(cond);
                rc = -ENEGATIVE;
        } else {
                wqn = xwmp_plwq_choose_locked(&cond->wq.pl);
                if (wqn) {
                        wqn->wq = NULL;
                        wqn->type = XWMP_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq, &wqn->reason,
                                    xwmb_modr_release, XWMP_WQN_REASON_UP);
                        cb = wqn->cb;
                        wqn->cb = NULL;
                        xwmp_splk_unlock(&wqn->lock);
                        xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
                        xwmp_cond_put(cond);
                        cb(wqn->owner);
                } else {
                        xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
                }
                rc = XWOK;
        }
        return rc;
}

__xwmp_api
xwer_t xwmp_cond_unicast(struct xwmp_cond * cond)
{
        xwer_t rc;

        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        rc = xwmp_cond_grab(cond);
        if (__xwcc_likely(XWOK == rc)) {
                rc = xwmp_cond_do_unicast(cond);
        }/* else {} */
        return rc;
}

static __xwmp_code
xwer_t xwmp_cond_do_timedblkthd_unlkwq_cpuirqrs(struct xwmp_cond * cond,
                                                struct xwmp_thd * thd,
                                                void * lock, xwsq_t lktype,
                                                void * lkdata,
                                                xwtm_t * xwtm, xwsq_t * lkst,
                                                xwreg_t cpuirq)
{
        xwer_t rc;
        xwpr_t dprio;
        xwsq_t wkuprs;
        xwsq_t reason;
        xwtm_t expected, currtick;
        struct xwmp_skd * xwskd;
        struct xwmp_tt * xwtt;
        struct xwmp_syshwt * hwt;

        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &thd->xwskd);
        xwtt = &xwskd->tt;
        hwt = &xwtt->hwt;
        currtick = xwmp_syshwt_get_timetick(hwt);
        expected = xwtm_add_safely(currtick, *xwtm);

        /* 加入等待队列 */
        xwmp_splk_lock(&thd->stlock);
        XWOS_BUG_ON((XWMP_SKDOBJ_DST_BLOCKING | XWMP_SKDOBJ_DST_SLEEPING |
                     XWMP_SKDOBJ_DST_READY | XWMP_SKDOBJ_DST_STANDBY |
                     XWMP_SKDOBJ_DST_FROZEN | XWMP_SKDOBJ_DST_MIGRATING)
                    & thd->state);
        if (XWMP_SKDOBJ_DST_FREEZABLE & thd->state) {
                xwmp_splk_unlock(&thd->stlock);
                xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
                rc = -EINTR;
                goto err_needfrz;
        }
        dprio = thd->dprio.r;
        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_RUNNING);
        thd->dprio.r = XWMP_SKD_PRIORITY_INVALID;
        thd->dprio.wq = dprio;
        xwbop_s1m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_BLOCKING);
        xwmp_splk_unlock(&thd->stlock);
        xwmp_thd_eq_plwq_locked(thd, &cond->wq.pl, XWMP_WQTYPE_COND);
        xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);

        /* 同步解锁 */
        rc = xwmp_thd_do_unlock(lock, lktype, lkdata);
        if (XWOK == rc){
                *lkst = XWOS_LKST_UNLOCKED;
        }/* else {} */

        /* 加入时间树 */
        xwmp_sqlk_wr_lock_cpuirq(&xwtt->lock);
        xwmp_splk_lock(&thd->stlock);
        xwbop_s1m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_SLEEPING);
        xwmp_splk_unlock(&thd->stlock);
        xwmp_thd_tt_add_locked(thd, xwtt, expected, cpuirq);
        xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);

        /* 调度 */
        xwospl_cpuirq_enable_lc();
        xwmp_skd_wakelock_unlock(xwskd);
        xwmp_skd_req_swcx(xwskd);
        xwmp_skd_wakelock_lock(xwskd);
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        xwmb_mp_rmb();
        /* 确保对唤醒原因的读取操作发生在线程状态切换之后 */
        reason = xwaop_load(xwsq, &thd->wqn.reason, xwmb_modr_relaxed);
        wkuprs = xwaop_load(xwsq, &thd->ttn.wkuprs, xwmb_modr_relaxed);
        if (XWMP_WQN_REASON_INTR == reason) {
                xwmp_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwmp_tt_remove_locked(xwtt, &thd->ttn);
                if (XWOK == rc) {
                        xwmp_splk_lock(&thd->stlock);
                        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_SLEEPING);
                        xwmp_splk_unlock(&thd->stlock);
                }/* else {} */
                xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = -EINTR;
        } else if (XWMP_WQN_REASON_UP == reason) {
                xwmp_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwmp_tt_remove_locked(xwtt, &thd->ttn);
                if (XWOK == rc) {
                        xwmp_splk_lock(&thd->stlock);
                        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_SLEEPING);
                        xwmp_splk_unlock(&thd->stlock);
                }/* else {} */
                xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                if (__xwcc_likely(XWOS_LKST_UNLOCKED == *lkst)) {
                        currtick = xwmp_syshwt_get_timetick(hwt);
                        *xwtm = xwtm_sub(expected, currtick);
                        rc = xwmp_thd_do_lock(lock, lktype, xwtm, lkdata);
                        if (XWOK == rc) {
                                *lkst = XWOS_LKST_LOCKED;
                        }/* else {} */
                }/* else {} */
                rc = XWOK;
        } else if (XWMP_TTN_WKUPRS_TIMEDOUT == wkuprs) {
                xwmp_plwq_lock_cpuirq(&cond->wq.pl);
                xwmp_splk_lock(&thd->wqn.lock);
                rc = xwmp_plwq_remove_locked(&cond->wq.pl, &thd->wqn);
                if (XWOK == rc) {
                        thd->wqn.wq = NULL;
                        thd->wqn.type = XWMP_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq, &thd->wqn.reason,
                                    xwmb_modr_release, XWMP_WQN_REASON_INTR);
                        thd->wqn.cb = NULL;
                        xwmp_splk_unlock(&thd->wqn.lock);
                        xwmp_splk_lock(&thd->stlock);
                        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_BLOCKING);
                        thd->dprio.wq = XWMP_SKD_PRIORITY_INVALID;
                        xwmp_splk_unlock(&thd->stlock);
                        xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
                        rc = -ETIMEDOUT;
                } else {
                        xwmp_splk_unlock(&thd->wqn.lock);
                        xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
                        reason = xwaop_load(xwsq, &thd->wqn.reason,
                                            xwmb_modr_relaxed);
                        if (XWMP_WQN_REASON_INTR == reason) {
                                rc = -EINTR;
                        } else if (XWMP_WQN_REASON_UP == reason) {
                                if (__xwcc_likely(XWOS_LKST_UNLOCKED == *lkst)) {
                                        currtick = xwmp_syshwt_get_timetick(hwt);
                                        *xwtm = xwtm_sub(expected, currtick);
                                        rc = xwmp_thd_do_lock(lock, lktype, xwtm,
                                                              lkdata);
                                        if (XWOK == rc) {
                                                *lkst = XWOS_LKST_LOCKED;
                                        }/* else {} */
                                }/* else {} */
                                rc = XWOK;
                        } else {
                                XWOS_BUG();
                                rc = -EBUG;
                        }
                }
        } else if (XWMP_TTN_WKUPRS_INTR == wkuprs) {
                xwmp_plwq_lock_cpuirq(&cond->wq.pl);
                xwmp_splk_lock(&thd->wqn.lock);
                rc = xwmp_plwq_remove_locked(&cond->wq.pl, &thd->wqn);
                if (XWOK == rc) {
                        thd->wqn.wq = NULL;
                        thd->wqn.type = XWMP_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq, &thd->wqn.reason,
                                    xwmb_modr_release, XWMP_WQN_REASON_INTR);
                        thd->wqn.cb = NULL;
                        xwmp_splk_unlock(&thd->wqn.lock);
                        xwmp_splk_lock(&thd->stlock);
                        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_BLOCKING);
                        thd->dprio.wq = XWMP_SKD_PRIORITY_INVALID;
                        xwmp_splk_unlock(&thd->stlock);
                        xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
                        rc = -EINTR;
                } else {
                        xwmp_splk_unlock(&thd->wqn.lock);
                        xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
                        if (XWMP_WQN_REASON_INTR == reason) {
                                rc = -EINTR;
                        } else if (XWMP_WQN_REASON_UP == reason) {
                                if (__xwcc_likely(XWOS_LKST_UNLOCKED == *lkst)) {
                                        currtick = xwmp_syshwt_get_timetick(hwt);
                                        *xwtm = xwtm_sub(expected, currtick);
                                        rc = xwmp_thd_do_lock(lock, lktype, xwtm,
                                                              lkdata);
                                        if (XWOK == rc) {
                                                *lkst = XWOS_LKST_LOCKED;
                                        }
                                }/* else {} */
                                rc = XWOK;
                        } else {
                                XWOS_BUG();
                                rc = -EBUG;
                        }
                }
        } else {
                XWOS_BUG();
                rc = -EBUG;
        }
        currtick = xwmp_syshwt_get_timetick(hwt);
        *xwtm = xwtm_sub(expected, currtick);
err_needfrz:
        return rc;
}

__xwmp_code
xwer_t xwmp_cond_do_timedwait(struct xwmp_cond * cond, struct xwmp_thd * thd,
                              void * lock, xwsq_t lktype, void * lkdata,
                              xwtm_t * xwtm, xwsq_t * lkst)
{
        struct xwmp_skd * xwskd;
        xwreg_t cpuirq;
        xwer_t rc;

        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &thd->xwskd);
        xwmp_plwq_lock_cpuirqsv(&cond->wq.pl, &cpuirq);
        rc = xwmp_skd_wakelock_lock(xwskd);
        if (__xwcc_unlikely(rc < 0)) {
                /* 当前调度器正准备休眠，线程需被冻结，返回-EINTR。*/
                xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
                rc = -EINTR;
        } else {
                rc = xwmp_cond_do_timedblkthd_unlkwq_cpuirqrs(cond, thd,
                                                              lock, lktype, lkdata,
                                                              xwtm, lkst, cpuirq);
                xwmp_skd_wakelock_unlock(xwskd);
        }
        return rc;
}

__xwmp_api
xwer_t xwmp_cond_wait(struct xwmp_cond * cond,
                      void * lock, xwsq_t lktype,
                      void * lkdata, xwsq_t * lkst)
{
        xwtm_t expected = XWTM_MAX;
        return xwmp_cond_timedwait(cond, lock, lktype, lkdata,
                                   &expected, lkst);
}

__xwmp_api
xwer_t xwmp_cond_timedwait(struct xwmp_cond * cond,
                           void * lock, xwsq_t lktype, void * lkdata,
                           xwtm_t * xwtm, xwsq_t * lkst)
{
        struct xwmp_thd * cthd;
        xwer_t rc;

        XWOS_VALIDATE((cond), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((lkst), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm_cmp(*xwtm, 0) >= 0), "out-of-time", -ETIMEDOUT);
        XWOS_VALIDATE((lktype < XWOS_LK_NUM), "invalid-type", -EINVAL);
        XWOS_VALIDATE((((NULL == lock) && (XWOS_LK_NONE == lktype)) ||
                       ((lock) && (lktype > XWOS_LK_NONE))),
                      "invalid-lock", -EINVAL);
        XWOS_VALIDATE((-EINTHD == xwmp_irq_get_id(NULL)),
                      "not-in-thd", -ENOTINTHD);

        *lkst = XWOS_LKST_LOCKED;
        cthd = xwmp_skd_get_cthd_lc();
        if (__xwcc_unlikely(0 == xwtm_cmp(*xwtm, 0))) {
                rc = xwmp_thd_do_unlock(lock, lktype, lkdata);
                if (XWOK == rc) {
                        *lkst = XWOS_LKST_UNLOCKED;
                }/* else {} */
                rc = -ETIMEDOUT;
        } else {
                rc = xwmp_cond_grab(cond);
                if (__xwcc_likely(XWOK == rc)) {
                        rc = xwmp_cond_do_timedwait(cond, cthd, lock, lktype, lkdata,
                                                    xwtm, lkst);
                        xwmp_cond_put(cond);
                }/* else {} */
        }
        return rc;
}
