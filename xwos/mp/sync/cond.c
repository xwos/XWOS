/**
 * @file
 * @brief 玄武OS MP内核同步机制：条件量
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
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
xwer_t xwmp_cond_sgc(void * cond);

static __xwmp_code
xwer_t xwmp_cond_dgc(void * cond);

static __xwmp_code
xwer_t xwmp_cond_broadcast_once(struct xwmp_cond * cond, bool * retry);

static __xwmp_code
xwer_t xwmp_cond_unlock(void * lock, xwsq_t lktype, void * lkdata);

static __xwmp_code
xwer_t xwmp_cond_lock(void * lock, xwsq_t lktype, xwtm_t to, bool unintr,
                      void * lkdata);

static __xwmp_code
xwer_t xwmp_cond_test(struct xwmp_cond * cond,
                      struct xwmp_skd * xwskd, struct xwmp_thd * thd,
                      void * lock, xwsq_t lktype, void * lkdata,
                      xwtm_t to, xwsq_t * lkst);

static __xwmp_code
xwer_t xwmp_cond_blkthd_to_unlkwq_cpuirqrs(struct xwmp_cond * cond,
                                           struct xwmp_skd * xwskd,
                                           struct xwmp_thd * thd,
                                           void * lock, xwsq_t lktype,
                                           void * lkdata,
                                           xwtm_t to, xwsq_t * lkst,
                                           xwreg_t cpuirq);

static __xwmp_code
xwer_t xwmp_cond_blkthd_unlkwq_cpuirqrs(struct xwmp_cond * cond,
                                        struct xwmp_thd * thd,
                                        void * lock, xwsq_t lktype,
                                        void * lkdata,
                                        xwsq_t * lkst,
                                        xwreg_t cpuirq);

__xwmp_code
xwer_t xwmp_cond_test_unintr(struct xwmp_cond * cond,
                             struct xwmp_thd * thd,
                             void * lock, xwsq_t lktype, void * lkdata,
                             xwsq_t * lkst);

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
        } else {
                xwmp_cond_construct(cond);
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
 * @brief 静态条件量对象的垃圾回收函数
 * @param[in] cond: 条件量对象的指针
 */
static __xwmp_code
xwer_t xwmp_cond_sgc(void * cond)
{
        xwmp_cond_destruct((struct xwmp_cond *)cond);
        return XWOK;
}

/**
 * @brief 动态条件量对象的垃圾回收函数
 * @param[in] cond: 条件量对象的指针
 */
static __xwmp_code
xwer_t xwmp_cond_dgc(void * cond)
{
        xwmp_cond_free((struct xwmp_cond *)cond);
        return XWOK;
}

/**
 * @brief 激活条件量
 * @param[in] cond: 条件量对象的指针
 * @param[in] gcfunc: 垃圾回收函数的指针
 */
__xwmp_code
xwer_t xwmp_cond_activate(struct xwmp_cond * cond, xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwmp_synobj_activate(&cond->synobj, gcfunc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_synobj_activate;
        }
        xwmp_plwq_init(&cond->wq.pl);
        cond->count = XWMP_COND_POSITIVE;
        return XWOK;

err_synobj_activate:
        return rc;
}

__xwmp_api
xwer_t xwmp_cond_init(struct xwmp_cond * cond)
{
        xwmp_cond_construct(cond);
        return xwmp_cond_activate(cond, xwmp_cond_sgc);
}

__xwmp_api
xwer_t xwmp_cond_fini(struct xwmp_cond * cond)
{
        return xwmp_cond_put(cond);
}

__xwmp_api
xwer_t xwmp_cond_create(struct xwmp_cond ** condbuf)
{
        struct xwmp_cond * cond;
        xwer_t rc;

        *condbuf = NULL;
        cond = xwmp_cond_alloc();
        if (__xwcc_unlikely(is_err(cond))) {
                rc = ptr_err(cond);
        } else {
                rc = xwmp_cond_activate(cond, xwmp_cond_dgc);
                if (__xwcc_unlikely(rc < 0)) {
                        xwmp_cond_free(cond);
                } else {
                        *condbuf = cond;
                }
        }
        return rc;
}

__xwmp_api
xwer_t xwmp_cond_delete(struct xwmp_cond * cond, xwsq_t tik)
{
        return xwmp_cond_release(cond, tik);
}

__xwmp_api
xwer_t xwmp_cond_acquire(struct xwmp_cond * cond, xwsq_t tik)
{
        return xwmp_synobj_acquire(&cond->synobj, tik);
}

__xwmp_api
xwer_t xwmp_cond_release(struct xwmp_cond * cond, xwsq_t tik)
{
        return xwmp_synobj_release(&cond->synobj, tik);
}

__xwmp_api
xwer_t xwmp_cond_grab(struct xwmp_cond * cond)
{
        return xwmp_synobj_grab(&cond->synobj);
}

__xwmp_api
xwer_t xwmp_cond_put(struct xwmp_cond * cond)
{
        return xwmp_synobj_put(&cond->synobj);
}

#if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
__xwmp_api
xwer_t xwmp_cond_bind(struct xwmp_cond * cond,
                      struct xwmp_evt * evt, xwsq_t pos)
{
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwmp_cond_grab(cond);
        if (__xwcc_likely(rc > 0)) {
                goto err_cond_bind;
        }
        xwmp_plwq_lock_cpuirqsv(&cond->wq.pl, &cpuirq);
        rc = xwmp_sel_obj_bind(evt, &cond->synobj, pos, false);
        xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
        return XWOK;

err_cond_bind:
        return rc;
}

__xwmp_api
xwer_t xwmp_cond_unbind(struct xwmp_cond * cond, struct xwmp_evt * evt)
{
        xwreg_t cpuirq;
        xwer_t rc;

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

        rc = XWOK;
        xwmp_plwq_lock_cpuirqsv(&cond->wq.pl, &cpuirq);
        if (__xwcc_unlikely(cond->count < 0)) {
                rc = -EALREADY;
        } else {
                cond->count = XWMP_COND_NEGTIVE;
        }
        xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
        return rc;
}

__xwmp_api
xwer_t xwmp_cond_thaw(struct xwmp_cond * cond)
{
        xwer_t rc;
        xwreg_t cpuirq;

        rc = XWOK;
        xwmp_plwq_lock_cpuirqsv(&cond->wq.pl, &cpuirq);
        if (__xwcc_unlikely(cond->count >= 0)) {
                rc = -EALREADY;
        } else {
                cond->count = XWMP_COND_POSITIVE;
        }
        xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
        return rc;
}

/**
 * @brief 中断条件量等待队列中的一个节点
 * @param[in] cond: 条件量对象的指针
 * @param[in] wqn: 等待队列节点
 */
__xwmp_code
xwer_t xwmp_cond_intr(struct xwmp_cond * cond, struct xwmp_wqn * wqn)
{
        xwmp_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        xwmp_plwq_lock_cpuirqsv(&cond->wq.pl, &cpuirq);
        xwmp_splk_lock(&wqn->lock);
        rc = xwmp_plwq_remove_locked(&cond->wq.pl, wqn);
        if (XWOK == rc) {
                wqn->wq = NULL;
                wqn->type = XWMP_WQTYPE_UNKNOWN;
                xwaop_store(xwsq_t, &wqn->reason,
                            xwaop_mo_release, XWMP_WQN_REASON_INTR);
                cb = wqn->cb;
                wqn->cb = NULL;
                xwmp_splk_unlock(&wqn->lock);
                xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
                cb(wqn->owner);
        } else {
                xwmp_splk_unlock(&wqn->lock);
                xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
        }
        return rc;
}

__xwmp_code
xwer_t xwmp_cond_intr_all(struct xwmp_cond * cond)
{
        struct xwmp_wqn * c;
        xwmp_wqn_f cb;
        xwreg_t cpuirq;

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
        return XWOK;
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
                        xwaop_store(xwsq_t, &wqn->reason,
                                    xwaop_mo_release, XWMP_WQN_REASON_UP);
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
                xwmb_mp_load_acquire(struct xwmp_evt *, evt, &synobj->sel.evt);
                if (NULL != evt) {
                        xwmp_sel_obj_s1i(evt, synobj);
                }
                xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
        }
#endif
        return rc;
}

__xwmp_api
xwer_t xwmp_cond_unicast(struct xwmp_cond * cond)
{
        struct xwmp_wqn * wqn;
        xwmp_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        xwmp_plwq_lock_cpuirqsv(&cond->wq.pl, &cpuirq);
        if (__xwcc_unlikely(cond->count < 0)) {
                xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
                rc = -ENEGATIVE;
        } else {
                wqn = xwmp_plwq_choose_locked(&cond->wq.pl);
                if (wqn) {
                        wqn->wq = NULL;
                        wqn->type = XWMP_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq_t, &wqn->reason,
                                    xwaop_mo_release, XWMP_WQN_REASON_UP);
                        cb = wqn->cb;
                        wqn->cb = NULL;
                        xwmp_splk_unlock(&wqn->lock);
                        xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
                        cb(wqn->owner);
                } else {
                        xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
                }
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 解锁给定类型的锁
 * @param[in] lock: 锁的地址
 * @param[in] lktype: 锁的类型
 * @param[in] lkdata: 锁的数据
 * @return 错误码
 */
__xwmp_code
xwer_t xwmp_cond_unlock(void * lock, xwsq_t lktype, void * lkdata)
{
        xwer_t rc;
        union xwos_ulock ulk;

        ulk.anon = lock;

        rc = XWOK;
        switch (lktype) {
        case XWOS_LK_MTX:
                rc = xwmp_mtx_unlock(ulk.xwmp.mtx);
                break;
        case XWOS_LK_SPLK:
                xwmp_splk_unlock(ulk.xwmp.splk);
                break;
        case XWOS_LK_SQLK_WR:
                xwmp_sqlk_wr_unlock(ulk.xwmp.sqlk);
                break;
        case XWOS_LK_SQLK_RDEX:
                xwmp_sqlk_rdex_unlock(ulk.xwmp.sqlk);
                break;
        case XWOS_LK_CALLBACK:
                if (ulk.cb->unlock) {
                        rc = ulk.cb->unlock(lkdata);
                }
                break;
        case XWOS_LK_NONE:
        default:
                break;
        }
        return rc;
}

/**
 * @brief 上锁给定类型的锁
 * @param[in] lock: 锁的地址
 * @param[in] lktype: 锁的类型
 * @param[in] to: 期望唤醒的时间点
 * @param[in] unintr: 是否不可中断
 * @param[in] lkdata: 锁的数据
 * @return 错误码
 */
__xwmp_code
xwer_t xwmp_cond_lock(void * lock, xwsq_t lktype, xwtm_t to, bool unintr,
                      void * lkdata)
{
        xwer_t rc;
        union xwos_ulock ulk;

        ulk.anon = lock;

        rc = XWOK;
        switch (lktype) {
        case XWOS_LK_MTX:
                if (unintr) {
                        rc = xwmp_mtx_lock_unintr(ulk.xwmp.mtx);
                } else {
                        rc = xwmp_mtx_lock_to(ulk.xwmp.mtx, to);
                }
                break;
        case XWOS_LK_SPLK:
                xwmp_splk_lock(ulk.xwmp.splk);
                break;
        case XWOS_LK_SQLK_WR:
                xwmp_sqlk_wr_lock(ulk.xwmp.sqlk);
                break;
        case XWOS_LK_SQLK_RDEX:
                xwmp_sqlk_rdex_lock(ulk.xwmp.sqlk);
                break;
        case XWOS_LK_CALLBACK:
                if (ulk.cb->lock) {
                        rc = ulk.cb->lock(lkdata);
                }
                break;
        case XWOS_LK_NONE:
        default:
                break;
        }
        return rc;
}

static __xwmp_code
xwer_t xwmp_cond_blkthd_to_unlkwq_cpuirqrs(struct xwmp_cond * cond,
                                           struct xwmp_skd * xwskd,
                                           struct xwmp_thd * thd,
                                           void * lock, xwsq_t lktype,
                                           void * lkdata,
                                           xwtm_t to, xwsq_t * lkst,
                                           xwreg_t cpuirq)
{
        xwer_t rc;
        xwpr_t dprio;
        xwsq_t wkuprs;
        xwsq_t reason;
        xwsq_t pmpt;
#if defined(XWMPCFG_SKD_BH) && (1 == XWMPCFG_SKD_BH)
        xwsq_t bh;
#endif
        struct xwmp_tt * xwtt;

        xwtt = &xwskd->tt;
        xwmp_splk_lock(&thd->stlock);
        XWOS_BUG_ON((XWMP_SKDOBJ_ST_BLOCKING | XWMP_SKDOBJ_ST_SLEEPING |
                     XWMP_SKDOBJ_ST_READY | XWMP_SKDOBJ_ST_STANDBY |
                     XWMP_SKDOBJ_ST_FROZEN | XWMP_SKDOBJ_ST_MIGRATING)
                    & thd->state);
        /* 检查是否被中断 */
        if ((XWMP_SKDOBJ_ST_FREEZABLE | XWMP_SKDOBJ_ST_EXITING) & thd->state) {
                xwmp_splk_unlock(&thd->stlock);
                xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
                rc = -EINTR;
                goto err_intr;
        }
        /* 加入等待队列 */
        dprio = thd->dprio.r;
        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_ST_RUNNING);
        thd->dprio.r = XWMP_SKD_PRIORITY_INVALID;
        thd->dprio.wq = dprio;
        xwbop_s1m(xwsq_t, &thd->state, XWMP_SKDOBJ_ST_BLOCKING);
        xwmp_splk_unlock(&thd->stlock);
        xwmp_thd_eq_plwq_locked(thd, &cond->wq.pl, XWMP_WQTYPE_COND);
        xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);

        /* 同步解锁 */
        rc = xwmp_cond_unlock(lock, lktype, lkdata);
        if (XWOK == rc){
                *lkst = XWOS_LKST_UNLOCKED;
        }/* else {} */

        /* 加入时间树 */
        xwmp_sqlk_wr_lock_cpuirq(&xwtt->lock);
        xwmp_splk_lock(&thd->stlock);
        xwbop_s1m(xwsq_t, &thd->state, XWMP_SKDOBJ_ST_SLEEPING);
        xwmp_splk_unlock(&thd->stlock);
        xwmp_thd_tt_add_locked(thd, xwtt, to, cpuirq);
        xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);

        /* 调度 */
        xwmp_skd_svpmpt(xwskd, &pmpt);
        xwmp_skd_rspmpt(xwskd, 0);
#if defined(XWMPCFG_SKD_BH) && (1 == XWMPCFG_SKD_BH)
        xwmp_skd_svbh(xwskd, &bh);
        xwmp_skd_rsbh(xwskd, 0);
#endif
        xwospl_cpuirq_enable_lc();
        xwmp_skd_wakelock_unlock(xwskd);
        xwmp_skd_req_swcx(xwskd);
        xwmp_skd_wakelock_lock(xwskd);
        xwospl_cpuirq_restore_lc(cpuirq);
#if defined(XWMPCFG_SKD_BH) && (1 == XWMPCFG_SKD_BH)
        xwmp_skd_rsbh(xwskd, bh);
#endif
        xwmp_skd_rspmpt(xwskd, pmpt);

        /* 判断唤醒原因 */
        xwmb_mp_rmb();
        /* 确保对唤醒原因的读取操作发生在线程状态切换之后 */
        reason = xwaop_load(xwsq_t, &thd->wqn.reason, xwaop_mo_relaxed);
        wkuprs = xwaop_load(xwsq_t, &thd->ttn.wkuprs, xwaop_mo_relaxed);
        if (XWMP_WQN_REASON_INTR == reason) {
                xwmp_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwmp_tt_remove_locked(xwtt, &thd->ttn);
                if (XWOK == rc) {
                        xwmp_splk_lock(&thd->stlock);
                        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_ST_SLEEPING);
                        xwmp_splk_unlock(&thd->stlock);
                }/* else {} */
                xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = -EINTR;
        } else if (XWMP_WQN_REASON_UP == reason) {
                xwmp_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwmp_tt_remove_locked(xwtt, &thd->ttn);
                if (XWOK == rc) {
                        xwmp_splk_lock(&thd->stlock);
                        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_ST_SLEEPING);
                        xwmp_splk_unlock(&thd->stlock);
                }/* else {} */
                xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                if (__xwcc_likely(XWOS_LKST_UNLOCKED == *lkst)) {
                        rc = xwmp_cond_lock(lock, lktype, to, false, lkdata);
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
                        xwaop_store(xwsq_t, &thd->wqn.reason,
                                    xwaop_mo_release, XWMP_WQN_REASON_INTR);
                        thd->wqn.cb = NULL;
                        xwmp_splk_unlock(&thd->wqn.lock);
                        xwmp_splk_lock(&thd->stlock);
                        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_ST_BLOCKING);
                        thd->dprio.wq = XWMP_SKD_PRIORITY_INVALID;
                        xwmp_splk_unlock(&thd->stlock);
                        xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
                        rc = -ETIMEDOUT;
                } else {
                        xwmp_splk_unlock(&thd->wqn.lock);
                        xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
                        reason = xwaop_load(xwsq_t, &thd->wqn.reason, xwaop_mo_relaxed);
                        if (XWMP_WQN_REASON_INTR == reason) {
                                rc = -EINTR;
                        } else if (XWMP_WQN_REASON_UP == reason) {
                                if (__xwcc_likely(XWOS_LKST_UNLOCKED == *lkst)) {
                                        rc = xwmp_cond_lock(lock, lktype, to, false,
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
        } else if (XWMP_TTN_WKUPRS_INTR == wkuprs) {
                xwmp_plwq_lock_cpuirq(&cond->wq.pl);
                xwmp_splk_lock(&thd->wqn.lock);
                rc = xwmp_plwq_remove_locked(&cond->wq.pl, &thd->wqn);
                if (XWOK == rc) {
                        thd->wqn.wq = NULL;
                        thd->wqn.type = XWMP_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq_t, &thd->wqn.reason,
                                    xwaop_mo_release, XWMP_WQN_REASON_INTR);
                        thd->wqn.cb = NULL;
                        xwmp_splk_unlock(&thd->wqn.lock);
                        xwmp_splk_lock(&thd->stlock);
                        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_ST_BLOCKING);
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
                                        rc = xwmp_cond_lock(lock, lktype, to, false,
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
        return rc;

err_intr:
        return rc;
}

__xwmp_code
xwer_t xwmp_cond_test(struct xwmp_cond * cond,
                      struct xwmp_skd * xwskd, struct xwmp_thd * thd,
                      void * lock, xwsq_t lktype, void * lkdata,
                      xwtm_t to, xwsq_t * lkst)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwmp_plwq_lock_cpuirqsv(&cond->wq.pl, &cpuirq);
        rc = xwmp_skd_wakelock_lock(xwskd);
        if (__xwcc_unlikely(rc < 0)) {
                /* 当前调度器正准备休眠，线程需被冻结，返回-EINTR。*/
                xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);
                rc = -EINTR;
        } else {
                rc = xwmp_cond_blkthd_to_unlkwq_cpuirqrs(cond, xwskd, thd,
                                                         lock, lktype, lkdata,
                                                         to, lkst, cpuirq);
                xwmp_skd_wakelock_unlock(xwskd);
        }
        return rc;
}

__xwmp_api
xwer_t xwmp_cond_wait(struct xwmp_cond * cond,
                      void * lock, xwsq_t lktype,
                      void * lkdata, xwsq_t * lkst)
{
        return xwmp_cond_wait_to(cond, lock, lktype, lkdata, XWTM_MAX, lkst);
}

__xwmp_api
xwer_t xwmp_cond_wait_to(struct xwmp_cond * cond,
                         void * lock, xwsq_t lktype, void * lkdata,
                         xwtm_t to, xwsq_t * lkst)
{
        struct xwmp_thd * cthd;
        struct xwmp_skd * xwskd;
        struct xwmp_syshwt * hwt;
        xwtm_t now;
        xwer_t rc;

        *lkst = XWOS_LKST_LOCKED;
        cthd = xwmp_skd_get_cthd_lc();
        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &cthd->xwskd);
        hwt = &xwskd->tt.hwt;
        now = xwmp_syshwt_get_timetick(hwt);
        if (__xwcc_unlikely(xwtm_cmp(to, now) < 0)) {
                rc = -ETIMEDOUT;
        } else if (__xwcc_unlikely(xwtm_cmp(to, now) == 0)) {
                rc = xwmp_cond_unlock(lock, lktype, lkdata);
                if (XWOK == rc) {
                        *lkst = XWOS_LKST_UNLOCKED;
                }
                rc = -ETIMEDOUT;
        } else {
                rc = xwmp_cond_test(cond, xwskd, cthd, lock, lktype, lkdata, to, lkst);
        }
        return rc;
}

static __xwmp_code
xwer_t xwmp_cond_blkthd_unlkwq_cpuirqrs(struct xwmp_cond * cond,
                                        struct xwmp_thd * thd,
                                        void * lock, xwsq_t lktype,
                                        void * lkdata,
                                        xwsq_t * lkst,
                                        xwreg_t cpuirq)
{
        struct xwmp_skd * xwskd;
        xwpr_t dprio;
        xwsq_t reason;
        xwsq_t pmpt;
#if defined(XWMPCFG_SKD_BH) && (1 == XWMPCFG_SKD_BH)
        xwsq_t bh;
#endif
        xwer_t rc;

        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &thd->xwskd);
        xwmp_splk_lock(&thd->stlock);
        XWOS_BUG_ON((XWMP_SKDOBJ_ST_BLOCKING | XWMP_SKDOBJ_ST_SLEEPING |
                     XWMP_SKDOBJ_ST_READY | XWMP_SKDOBJ_ST_STANDBY |
                     XWMP_SKDOBJ_ST_FROZEN | XWMP_SKDOBJ_ST_MIGRATING)
                    & thd->state);
        /* 加入等待队列 */
        dprio = thd->dprio.r;
        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_ST_RUNNING);
        thd->dprio.r = XWMP_SKD_PRIORITY_INVALID;
        thd->dprio.wq = dprio;
        xwbop_s1m(xwsq_t, &thd->state,
                  XWMP_SKDOBJ_ST_BLOCKING | XWMP_SKDOBJ_ST_UNINTERRUPTED);
        xwmp_splk_unlock(&thd->stlock);
        xwmp_thd_eq_plwq_locked(thd, &cond->wq.pl, XWMP_WQTYPE_COND);
        xwmp_plwq_unlock_cpuirqrs(&cond->wq.pl, cpuirq);

        /* 同步解锁 */
        rc = xwmp_cond_unlock(lock, lktype, lkdata);
        if (XWOK == rc){
                *lkst = XWOS_LKST_UNLOCKED;
        }/* else {} */

        /* 调度 */
        xwmp_skd_svpmpt(xwskd, &pmpt);
        xwmp_skd_rspmpt(xwskd, 0);
#if defined(XWMPCFG_SKD_BH) && (1 == XWMPCFG_SKD_BH)
        xwmp_skd_svbh(xwskd, &bh);
        xwmp_skd_rsbh(xwskd, 0);
#endif
        xwospl_cpuirq_enable_lc();
        xwmp_skd_req_swcx(xwskd);
        xwospl_cpuirq_restore_lc(cpuirq);
#if defined(XWMPCFG_SKD_BH) && (1 == XWMPCFG_SKD_BH)
        xwmp_skd_rsbh(xwskd, bh);
#endif
        xwmp_skd_rspmpt(xwskd, pmpt);

        /* 判断唤醒原因 */
        xwmb_mp_rmb();
        /* 确保对唤醒原因的读取操作发生在线程状态切换之后 */
        reason = xwaop_load(xwsq_t, &thd->wqn.reason, xwaop_mo_relaxed);
        if (XWMP_WQN_REASON_UP == reason) {
                if (__xwcc_likely(XWOS_LKST_UNLOCKED == *lkst)) {
                        rc = xwmp_cond_lock(lock, lktype, 0, true, lkdata);
                        if (XWOK == rc) {
                                *lkst = XWOS_LKST_LOCKED;
                        }/* else {} */
                }/* else {} */
                rc = XWOK;
        } else {
                XWOS_BUG();
                rc = -EBUG;
        }
        return rc;
}

__xwmp_code
xwer_t xwmp_cond_test_unintr(struct xwmp_cond * cond,
                             struct xwmp_thd * thd,
                             void * lock, xwsq_t lktype, void * lkdata,
                             xwsq_t * lkst)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwmp_plwq_lock_cpuirqsv(&cond->wq.pl, &cpuirq);
        rc = xwmp_cond_blkthd_unlkwq_cpuirqrs(cond, thd, lock, lktype, lkdata,
                                              lkst, cpuirq);
        return rc;
}

__xwmp_api
xwer_t xwmp_cond_wait_unintr(struct xwmp_cond * cond,
                             void * lock, xwsq_t lktype, void * lkdata,
                             xwsq_t * lkst)
{
        struct xwmp_thd * cthd;
        xwer_t rc;

        *lkst = XWOS_LKST_LOCKED;
        cthd = xwmp_skd_get_cthd_lc();
        rc = xwmp_cond_test_unintr(cond, cthd, lock, lktype, lkdata, lkst);
        return rc;
}
