/**
 * @file
 * @brief XuanWuOS的同步机制：条件量
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * @note
 * - 锁的顺序：同级的锁不可同时获得
 *   + ① 等待队列的锁（plwq->lock）
 *     + ② 等待队列节点的锁（wqn->lock）
 *     + ② 线程控制块的状态锁（tcb->stlock）
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/object.h>
#include <xwos/lib/xwbop.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#if defined(XWSMPCFG_SYNC_CDT_MEMSLICE) && (1 == XWSMPCFG_SYNC_CDT_MEMSLICE)
  #include <xwos/mm/memslice.h>
#endif /* XWSMPCFG_SYNC_CDT_MEMSLICE */
#include <xwos/smp/irq.h>
#include <xwos/smp/scheduler.h>
#include <xwos/smp/tt.h>
#include <xwos/smp/thread.h>
#include <xwos/smp/plwq.h>
#include <xwos/smp/lock/spinlock.h>
#include <xwos/smp/lock/seqlock.h>
#include <xwos/smp/lock/mutex.h>
#include <xwos/smp/sync/condition.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWSYNC_CDT_NEGTIVE  ((xwssq_t)(-1)) /**< 负的条件量的值 */
#define XWSYNC_CDT_POSITIVE ((xwssq_t)(1)) /**< 正的条件量的值 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if defined(XWSMPCFG_SYNC_CDT_MEMSLICE) && (1 == XWSMPCFG_SYNC_CDT_MEMSLICE)
/**
 * @brief 结构体xwsync_cdt的对象缓存
 */
static __xwos_data struct xwmm_memslice * xwsync_cdt_cache = NULL;

/**
 * @brief 结构体xwsync_cdt的对象缓存的名字
 */
__xwos_rodata const char xwsync_cdt_cache_name[] = "cache.cdt.xwsync";
#endif /* XWSMPCFG_SYNC_CDT_MEMSLICE */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwos_code
struct xwsync_cdt * xwsync_cdt_alloc(void);

static __xwos_code
void xwsync_cdt_free(struct xwsync_cdt * cdt);

static __xwos_code
xwer_t xwsync_cdt_gc(void * cdt);

static __xwos_code
xwer_t xwsync_cdt_broadcast_once(struct xwsync_cdt * cdt, bool * retry);

static __xwos_code
xwer_t xwsync_cdt_do_timedblkthrd_unlkwq_cpuirqrs(struct xwsync_cdt * cdt,
                                                  struct xwos_tcb * tcb,
                                                  void * lock, xwsq_t lktype,
                                                  void * lkdata, xwsz_t datanum,
                                                  xwtm_t * xwtm, xwsq_t * lkst,
                                                  xwreg_t cpuirq);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** memslice ******** ********/
#if defined(XWSMPCFG_SYNC_CDT_MEMSLICE) && (1 == XWSMPCFG_SYNC_CDT_MEMSLICE)
/**
 * @brief XWOS INIT CODE：初始化结构体xwsync_cdt的对象缓存。
 * @param zone_origin: (I) 内存区域的首地址
 * @param zone_size: (I) 内存区域的大小
 * @return 错误码
 * @note
 * - 重入性：只可在系统初始化时使用一次。
 */
__xwos_init_code
xwer_t xwsync_cdt_cache_init(xwptr_t zone_origin, xwsz_t zone_size)
{
        struct xwmm_memslice * msa;
        xwer_t rc;

        rc = xwmm_memslice_create(&msa, zone_origin, zone_size,
                                  sizeof(struct xwsync_cdt),
                                  xwsync_cdt_cache_name,
                                  (ctor_f)xwsync_cdt_construct,
                                  (dtor_f)xwsync_cdt_destruct);
        if (__unlikely(rc < 0)) {
                xwsync_cdt_cache = NULL;
        } else {
                xwsync_cdt_cache = msa;
        }
        return rc;
}
#endif /* XWSMPCFG_SYNC_CDT_MEMSLICE */

/**
 * @brief 从条件量对象缓存中申请对象。
 * @return 条件量对象的指针
 */
static __xwos_code
struct xwsync_cdt * xwsync_cdt_alloc(void)
{
#if defined(XWSMPCFG_SYNC_CDT_MEMSLICE) && (1 == XWSMPCFG_SYNC_CDT_MEMSLICE)
        union {
                struct xwsync_cdt * cdt;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_memslice_alloc(xwsync_cdt_cache, &mem.anon);
        if (rc < 0) {
                mem.cdt = err_ptr(rc);
        }/* else {} */
        return mem.cdt;
#else /* XWSMPCFG_SYNC_CDT_MEMSLICE */
        union {
                struct xwsync_cdt * cdt;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwsync_cdt), XWMM_ALIGNMENT, &mem.anon);
        if (OK == rc) {
                xwsync_cdt_construct(mem.cdt);
        } else {
                mem.cdt = err_ptr(-ENOMEM);
        }
        return mem.cdt;
#endif /* !XWSMPCFG_SYNC_CDT_MEMSLICE */
}

/**
 * @brief 释放条件量对象。
 * @param cdt: (I) 条件量对象的指针
 */
static __xwos_code
void xwsync_cdt_free(struct xwsync_cdt * cdt)
{
#if defined(XWSMPCFG_SYNC_CDT_MEMSLICE) && (1 == XWSMPCFG_SYNC_CDT_MEMSLICE)
        xwmm_memslice_free(xwsync_cdt_cache, cdt);
#else /* XWSMPCFG_SYNC_CDT_MEMSLICE */
        xwsync_cdt_destruct(cdt);
        xwmm_kma_free(cdt);
#endif /* !XWSMPCFG_SYNC_CDT_MEMSLICE */
}

/**
 * @brief 条件量对象的构造函数。
 * @param cdt: (I) 条件量对象的指针
 */
__xwos_code
void xwsync_cdt_construct(struct xwsync_cdt * cdt)
{
        xwos_object_construct(&cdt->xwobj);
}

/**
 * @brief 条件量对象的析构函数。
 * @param cdt: (I) 条件量对象的指针
 */
__xwos_code
void xwsync_cdt_destruct(struct xwsync_cdt * cdt)
{
        xwos_object_destruct(&cdt->xwobj);
}

/**
 * @brief 条件量对象的垃圾回收函数。
 * @param cdt: (I) 条件量对象的指针
 * @return 错误码
 */
static __xwos_code
xwer_t xwsync_cdt_gc(void * cdt)
{
        xwsync_cdt_free((struct xwsync_cdt *)cdt);
        return OK;
}

/**
 * @brief 激活条件量。
 * @param cdt: (I) 条件量对象的指针
 * @param gcfunc: (I) 垃圾回收函数的指针，由于静态初始化的线程所有资源都是由
 *                    用户自己提供的，因此当条件量不使用时，回收资源的函数也
 *                    需要用户自己提供。
 * @return 错误码
 * @retval OK: OK
 */
__xwos_code
xwer_t xwsync_cdt_activate(struct xwsync_cdt * cdt, xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwos_object_activate(&cdt->xwobj, gcfunc);
        if (__likely(OK == rc)) {
                xwos_plwq_init(&cdt->wq.pl);
                cdt->count = XWSYNC_CDT_POSITIVE;
        }/* else {} */
        return rc;
}

/**
 * @brief XWOS API：静态始化成条件量。
 * @param cdt: (I) 条件量对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *cdt* ，不可重入
 */
__xwos_api
xwer_t xwsync_cdt_init(struct xwsync_cdt * cdt)
{
        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);

        xwsync_cdt_construct(cdt);
        return xwsync_cdt_activate(cdt, NULL);
}

/**
 * @brief XWOS API：销毁静态方式初始化的条件量对象。
 * @param cdt: (I) 条件量对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *cdt* ，不可重入
 */
__xwos_api
xwer_t xwsync_cdt_destroy(struct xwsync_cdt * cdt)
{
        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);

        return xwsync_cdt_put(cdt);
}

/**
 * @brief XWOS API：动态创建条件量。
 * @param ptrbuf: (O) 指向缓冲区的指针，此缓冲区用于返回对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwsync_cdt_create(struct xwsync_cdt ** ptrbuf)
{
        struct xwsync_cdt * cdt;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);

        *ptrbuf = NULL;
        cdt = xwsync_cdt_alloc();
        if (__unlikely(is_err(cdt))) {
                rc = ptr_err(cdt);
        } else {
                rc = xwsync_cdt_activate(cdt, xwsync_cdt_gc);
                if (__unlikely(rc < 0)) {
                        xwsync_cdt_free(cdt);
                } else {
                        *ptrbuf = cdt;
                }
        }
        return rc;
}

/**
 * @brief XWOS API：删除动态创建的条件量。
 * @param cdt: (I) 条件量对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *cdt* ，不可重入
 */
__xwos_api
xwer_t xwsync_cdt_delete(struct xwsync_cdt * cdt)
{
        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);

        return xwsync_cdt_put(cdt);
}

/**
 * @brief XWOS API：冻结条件量（值设置为负）。
 * @param cdt: (I) 条件量对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -EALREADY: 条件量已被冻结
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 的条件量不允许单播或广播，但可以被等待，
 *   测试条件量的线程会加入到条件量等待队列中阻塞等待。
 */
__xwos_api
xwer_t xwsync_cdt_freeze(struct xwsync_cdt * cdt)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);

        rc = xwsync_cdt_grab(cdt);
        if (__likely(OK == rc)) {
                xwos_plwq_lock_cpuirqsv(&cdt->wq.pl, &cpuirq);
                if (__unlikely(cdt->count < 0)) {
                        rc = -EALREADY;
                } else {
                        cdt->count = XWSYNC_CDT_NEGTIVE;
                }
                xwos_plwq_unlock_cpuirqrs(&cdt->wq.pl, cpuirq);
                xwsync_cdt_put(cdt);
        }/* else {} */
        return rc;
}

/**
 * @brief XWOS API：解冻条件量，并重新初始化。
 * @param cdt: (I) 条件量对象的指针
 * @param val: (I) 条件量的初始值
 * @param max: (I) 条件量的最大值
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -EALREADY: 条件量未被冻结
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对已冻结的条件量起作用。对未冻结的条件量调用此函数将返回错误码。
 */
__xwos_api
xwer_t xwsync_cdt_thaw(struct xwsync_cdt * cdt)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);

        rc = xwsync_cdt_grab(cdt);
        if (__likely(OK == rc)) {
                xwos_plwq_lock_cpuirqsv(&cdt->wq.pl, &cpuirq);
                if (__unlikely(cdt->count >= 0)) {
                        rc = -EALREADY;
                } else {
                        cdt->count = XWSYNC_CDT_POSITIVE;
                        rc = OK;
                }
                xwos_plwq_unlock_cpuirqrs(&cdt->wq.pl, cpuirq);
                xwsync_cdt_put(cdt);
        }/* else {} */
        return rc;
}

/**
 * @brief 中断条件量等待队列中的一个节点。
 * @param cdt: (I) 条件量对象的指针
 * @param wqn: (I) 等待队列节点
 * @return 错误码
 * @retval OK: OK
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_code
xwer_t xwsync_cdt_intr(struct xwsync_cdt * cdt, struct xwos_wqn * wqn)
{
        xwos_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwsync_cdt_grab(cdt);
        if (__likely(OK == rc)) {
                xwos_plwq_lock_cpuirqsv(&cdt->wq.pl, &cpuirq);
                xwlk_splk_lock(&wqn->lock);
                rc = xwos_plwq_remove_locked(&cdt->wq.pl, wqn);
                if (OK == rc) {
                        wqn->wq = NULL;
                        wqn->type = XWOS_WQTYPE_UNKNOWN;
                        wqn->rsmrs = XWOS_WQN_RSMRS_INTR;
                        cb = wqn->cb;
                        wqn->cb = NULL;
                        xwlk_splk_unlock(&wqn->lock);
                        xwos_plwq_unlock_cpuirqrs(&cdt->wq.pl, cpuirq);
                        xwsync_cdt_put(cdt);
                        cb(wqn->owner);
                } else {
                        xwlk_splk_unlock(&wqn->lock);
                        xwos_plwq_unlock_cpuirqrs(&cdt->wq.pl, cpuirq);
                        xwsync_cdt_put(cdt);
                }
        }/* else {} */
        return rc;
}

/**
 * @brief XWOS API：中断条件量等待队列中的所有节点。
 * @param cdt: (I) 条件量对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwsync_cdt_intr_all(struct xwsync_cdt * cdt)
{
        struct xwos_wqn * c;
        xwos_wqn_f cb;
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);

        rc = xwsync_cdt_grab(cdt);
        if (__likely(OK == rc)) {
                xwos_plwq_lock_cpuirqsv(&cdt->wq.pl, &cpuirq);
                xwos_plwq_itr_wqn_rm(c, &cdt->wq.pl) {
                        xwlk_splk_lock(&c->lock);
                        xwos_plwq_remove_locked(&cdt->wq.pl, c);
                        c->wq = NULL;
                        c->type = XWOS_WQTYPE_UNKNOWN;
                        c->rsmrs = XWOS_WQN_RSMRS_INTR;
                        cb = c->cb;
                        c->cb = NULL;
                        xwlk_splk_unlock(&c->lock);
                        xwos_plwq_unlock_cpuirqrs(&cdt->wq.pl, cpuirq);
                        cb(c->owner);
                        xwos_plwq_lock_cpuirq(&cdt->wq.pl);
                }
                xwos_plwq_unlock_cpuirqrs(&cdt->wq.pl, cpuirq);
                xwsync_cdt_put(cdt);
        }/* else {} */
        return rc;
}

static __xwos_code
xwer_t xwsync_cdt_broadcast_once(struct xwsync_cdt * cdt, bool * retry)
{
        struct xwos_wqn * wqn;
        xwos_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = OK;
        xwos_plwq_lock_cpuirqsv(&cdt->wq.pl, &cpuirq);
        if (__unlikely(cdt->count < 0)) {
                xwos_plwq_unlock_cpuirqrs(&cdt->wq.pl, cpuirq);
                rc = -ENEGATIVE;
                *retry = false;
        } else {
                wqn = xwos_plwq_choose_locked(&cdt->wq.pl);
                if (wqn) {
                        wqn->wq = NULL;
                        wqn->type = XWOS_WQTYPE_UNKNOWN;
                        wqn->rsmrs = XWOS_WQN_RSMRS_UP;
                        cb = wqn->cb;
                        wqn->cb = NULL;
                        xwlk_splk_unlock(&wqn->lock);
                        xwos_plwq_unlock_cpuirqrs(&cdt->wq.pl, cpuirq);
                        cb(wqn->owner);
                        *retry = true;
                } else {
                        xwos_plwq_unlock_cpuirqrs(&cdt->wq.pl, cpuirq);
                        *retry = false;
                }
        }
        return rc;
}

/**
 * @brief XWOS API：广播条件量，将等待队列中的所有线程唤醒。
 * @param cdt: (I) 条件量对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ENEGATIVE: 条件量已被冻结
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对未冻结的条件量起作用，已冻结的条件量将得到错误码-ENEGATIVE。
 */
__xwos_api
xwer_t xwsync_cdt_broadcast(struct xwsync_cdt * cdt)
{
        bool retry;
        xwer_t rc;

        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);

        rc = xwsync_cdt_grab(cdt);
        if (__likely(OK == rc)) {
                retry = false;
                do {
                        rc = xwsync_cdt_broadcast_once(cdt, &retry);
                } while (retry);
                xwsync_cdt_put(cdt);
        }/* else {} */
        return rc;
}

__xwos_code
xwer_t xwsync_cdt_do_unicast(struct xwsync_cdt * cdt)
{
        struct xwos_wqn * wqn;
        xwos_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        xwos_plwq_lock_cpuirqsv(&cdt->wq.pl, &cpuirq);
        if (__unlikely(cdt->count < 0)) {
                xwos_plwq_unlock_cpuirqrs(&cdt->wq.pl, cpuirq);
                xwsync_cdt_put(cdt);
                rc = -ENEGATIVE;
        } else {
                wqn = xwos_plwq_choose_locked(&cdt->wq.pl);
                if (wqn) {
                        wqn->wq = NULL;
                        wqn->type = XWOS_WQTYPE_UNKNOWN;
                        wqn->rsmrs = XWOS_WQN_RSMRS_UP;
                        cb = wqn->cb;
                        wqn->cb = NULL;
                        xwlk_splk_unlock(&wqn->lock);
                        xwos_plwq_unlock_cpuirqrs(&cdt->wq.pl, cpuirq);
                        xwsync_cdt_put(cdt);
                        cb(wqn->owner);
                } else {
                        xwos_plwq_unlock_cpuirqrs(&cdt->wq.pl, cpuirq);
                }
                rc = OK;
        }
        return rc;
}

/**
 * @brief XWOS API：单播条件量，只会唤醒等待队列最前面的一个线程。
 * @param cdt: (I) 条件量对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ENEGATIVE: 条件量已被冻结
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note；
 * - 此函数只对未冻结的条件量起作用，已冻结的条件量将得到错误码-ENEGATIVE。
 */
__xwos_api
xwer_t xwsync_cdt_unicast(struct xwsync_cdt * cdt)
{
        xwer_t rc;

        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);

        rc = xwsync_cdt_grab(cdt);
        if (__likely(OK == rc)) {
                rc = xwsync_cdt_do_unicast(cdt);
        }/* else {} */
        return rc;
}

static __xwos_code
xwer_t xwsync_cdt_do_timedblkthrd_unlkwq_cpuirqrs(struct xwsync_cdt * cdt,
                                                  struct xwos_tcb * tcb,
                                                  void * lock, xwsq_t lktype,
                                                  void * lkdata, xwsz_t datanum,
                                                  xwtm_t * xwtm, xwsq_t * lkst,
                                                  xwreg_t cpuirq)
{
        xwer_t rc;
        xwpr_t dprio;
        xwtm_t expected, currtick;
        struct xwos_scheduler * xwsd;
        struct xwos_tt * xwtt;
        struct xwos_syshwt * hwt;

        xwmb_smp_load_acquire(xwsd, &tcb->xwsd);
        xwtt = &xwsd->tt;
        hwt = &xwtt->hwt;
        currtick = xwos_syshwt_get_timetick(hwt);
        expected = xwtm_add_safely(currtick, *xwtm);

        /* 加入等待队列 */
        xwlk_splk_lock(&tcb->stlock);
        XWOS_BUG_ON((XWSDOBJ_DST_BLOCKING | XWSDOBJ_DST_SLEEPING |
                     XWSDOBJ_DST_READY | XWSDOBJ_DST_STANDBY |
                     XWSDOBJ_DST_FROZEN | XWSDOBJ_DST_MIGRATING)
                    & tcb->state);
        if (XWSDOBJ_DST_FREEZABLE & tcb->state) {
                xwlk_splk_unlock(&tcb->stlock);
                xwos_plwq_unlock_cpuirqrs(&cdt->wq.pl, cpuirq);
                rc = -EINTR;
                goto err_needfrz;
        }
        dprio = tcb->dprio.r;
        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_RUNNING);
        tcb->dprio.r = XWOS_SD_PRIORITY_INVALID;
        tcb->dprio.wq = dprio;
        xwbop_s1m(xwsq_t, &tcb->state, XWSDOBJ_DST_BLOCKING);
        xwlk_splk_unlock(&tcb->stlock);
        xwos_thrd_eq_plwq_locked(tcb, &cdt->wq.pl, XWOS_WQTYPE_CDT);
        xwos_plwq_unlock_cpuirqrs(&cdt->wq.pl, cpuirq);

        /* 同步解锁 */
        rc = xwos_thrd_do_unlock(lock, lktype, lkdata, datanum);
        if (OK == rc){
                *lkst = XWLK_STATE_UNLOCKED;
        }/* else {} */

        /* 加入时间树 */
        xwlk_sqlk_wr_lock_cpuirq(&xwtt->lock);
        xwlk_splk_lock(&tcb->stlock);
        xwbop_s1m(xwsq_t, &tcb->state, XWSDOBJ_DST_SLEEPING);
        xwlk_splk_unlock(&tcb->stlock);
        xwos_thrd_tt_add_locked(tcb, xwtt, expected, cpuirq);
        xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);

        /* 调度 */
        xwos_cpuirq_enable_lc();
        xwos_scheduler_wakelock_unlock(xwsd);
        xwos_scheduler_req_swcx(xwsd);
        xwos_scheduler_wakelock_lock(xwsd);
        xwos_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        if (XWOS_WQN_RSMRS_INTR == tcb->wqn.rsmrs) {
                xwlk_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwos_tt_remove_locked(xwtt, &tcb->ttn);
                if (OK == rc) {
                        xwlk_splk_lock(&tcb->stlock);
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_SLEEPING);
                        xwlk_splk_unlock(&tcb->stlock);
                }/* else {} */
                xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = -EINTR;
        } else if (XWOS_WQN_RSMRS_UP == tcb->wqn.rsmrs) {
                xwlk_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwos_tt_remove_locked(xwtt, &tcb->ttn);
                if (OK == rc) {
                        xwlk_splk_lock(&tcb->stlock);
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_SLEEPING);
                        xwlk_splk_unlock(&tcb->stlock);
                }/* else {} */
                xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                if (__likely(XWLK_STATE_UNLOCKED == *lkst)) {
                        currtick = xwos_syshwt_get_timetick(hwt);
                        *xwtm = xwtm_sub(expected, currtick);
                        rc = xwos_thrd_do_lock(lock, lktype, xwtm, lkdata, datanum);
                        if (OK == rc) {
                                *lkst = XWLK_STATE_LOCKED;
                        }/* else {} */
                }/* else {} */
                rc = OK;
        } else if (XWOS_TTN_WKUPRS_TIMEDOUT == tcb->ttn.wkuprs) {
                xwos_plwq_lock_cpuirq(&cdt->wq.pl);
                xwlk_splk_lock(&tcb->wqn.lock);
                rc = xwos_plwq_remove_locked(&cdt->wq.pl, &tcb->wqn);
                if (OK == rc) {
                        tcb->wqn.wq = NULL;
                        tcb->wqn.type = XWOS_WQTYPE_UNKNOWN;
                        tcb->wqn.rsmrs = XWOS_WQN_RSMRS_INTR;
                        tcb->wqn.cb = NULL;
                        xwlk_splk_unlock(&tcb->wqn.lock);
                        xwlk_splk_lock(&tcb->stlock);
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_BLOCKING);
                        tcb->dprio.wq = XWOS_SD_PRIORITY_INVALID;
                        xwlk_splk_unlock(&tcb->stlock);
                        xwos_plwq_unlock_cpuirqrs(&cdt->wq.pl, cpuirq);
                        rc = -ETIMEDOUT;
                } else {
                        xwlk_splk_unlock(&tcb->wqn.lock);
                        xwos_plwq_unlock_cpuirqrs(&cdt->wq.pl, cpuirq);
                        if (XWOS_WQN_RSMRS_INTR == tcb->wqn.rsmrs) {
                                rc = -EINTR;
                        } else if (XWOS_WQN_RSMRS_UP == tcb->wqn.rsmrs) {
                                if (__likely(XWLK_STATE_UNLOCKED == *lkst)) {
                                        currtick = xwos_syshwt_get_timetick(hwt);
                                        *xwtm = xwtm_sub(expected, currtick);
                                        rc = xwos_thrd_do_lock(lock, lktype, xwtm,
                                                                 lkdata, datanum);
                                        if (OK == rc) {
                                                *lkst = XWLK_STATE_LOCKED;
                                        }/* else {} */
                                }/* else {} */
                                rc = OK;
                        } else {
                                XWOS_BUG();
                                rc = -EBUG;
                        }
                }
        } else if (XWOS_TTN_WKUPRS_INTR == tcb->ttn.wkuprs) {
                xwos_plwq_lock_cpuirq(&cdt->wq.pl);
                xwlk_splk_lock(&tcb->wqn.lock);
                rc = xwos_plwq_remove_locked(&cdt->wq.pl, &tcb->wqn);
                if (OK == rc) {
                        tcb->wqn.wq = NULL;
                        tcb->wqn.type = XWOS_WQTYPE_UNKNOWN;
                        tcb->wqn.rsmrs = XWOS_WQN_RSMRS_INTR;
                        tcb->wqn.cb = NULL;
                        xwlk_splk_unlock(&tcb->wqn.lock);
                        xwlk_splk_lock(&tcb->stlock);
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_BLOCKING);
                        tcb->dprio.wq = XWOS_SD_PRIORITY_INVALID;
                        xwlk_splk_unlock(&tcb->stlock);
                        xwos_plwq_unlock_cpuirqrs(&cdt->wq.pl, cpuirq);
                        rc = -EINTR;
                } else {
                        xwlk_splk_unlock(&tcb->wqn.lock);
                        xwos_plwq_unlock_cpuirqrs(&cdt->wq.pl, cpuirq);
                        if (XWOS_WQN_RSMRS_INTR == tcb->wqn.rsmrs) {
                                rc = -EINTR;
                        } else if (XWOS_WQN_RSMRS_UP == tcb->wqn.rsmrs) {
                                if (__likely(XWLK_STATE_UNLOCKED == *lkst)) {
                                        currtick = xwos_syshwt_get_timetick(hwt);
                                        *xwtm = xwtm_sub(expected, currtick);
                                        rc = xwos_thrd_do_lock(lock, lktype, xwtm,
                                                                 lkdata, datanum);
                                        if (OK == rc) {
                                                *lkst = XWLK_STATE_LOCKED;
                                        }
                                }/* else {} */
                                rc = OK;
                        } else {
                                XWOS_BUG();
                                rc = -EBUG;
                        }
                }
        } else {
                XWOS_BUG();
                rc = -EBUG;
        }
        currtick = xwos_syshwt_get_timetick(hwt);
        *xwtm = xwtm_sub(expected, currtick);
err_needfrz:
        return rc;
}

__xwos_code
xwer_t xwsync_cdt_do_timedwait(struct xwsync_cdt * cdt, struct xwos_tcb * tcb,
                               void * lock, xwsq_t lktype,
                               void * lkdata, xwsz_t datanum,
                               xwtm_t * xwtm, xwsq_t * lkst)
{
        struct xwos_scheduler * xwsd;
        xwreg_t cpuirq;
        xwer_t rc;

        xwmb_smp_load_acquire(xwsd, &tcb->xwsd);
        xwos_plwq_lock_cpuirqsv(&cdt->wq.pl, &cpuirq);
        rc = xwos_scheduler_wakelock_lock(xwsd);
        if (__unlikely(rc < 0)) {
                /* 当前CPU调度器处于休眠态，线程需被冻结，返回-EINTR。*/
                xwos_plwq_unlock_cpuirqrs(&cdt->wq.pl, cpuirq);
                rc = -EINTR;
        } else {
                rc = xwsync_cdt_do_timedblkthrd_unlkwq_cpuirqrs(cdt, tcb,
                                                                lock, lktype,
                                                                lkdata, datanum,
                                                                xwtm, lkst, cpuirq);
                xwos_scheduler_wakelock_unlock(xwsd);
        }
        return rc;
}

/**
 * @brief XWOS API：限时等待条件量。
 * @param cdt: (I) 条件量对象的指针
 * @param lock: (I) 锁的地址
 * @param lktype: (I) 锁的类型
 * @param lkdata: (I) 锁的数据
 * @param datanum: (I) 锁的数据数量
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @param lkst: (O) 指向缓冲区的指针，通过此缓冲区返回锁的状态
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 参数无效
 * @retval -ETIMEDOUT: 超时
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHRD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 函数返回返回-ETIMEDOUT时，*xwtm* 指向的缓冲区内的期望时间会减为0。
 */
__xwos_api
xwer_t xwsync_cdt_timedwait(struct xwsync_cdt * cdt,
                            void * lock, xwsq_t lktype,
                            void * lkdata, xwsz_t datanum,
                            xwtm_t * xwtm, xwsq_t * lkst)
{
        struct xwos_tcb * ctcb;
        xwer_t rc;

        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((lkst), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm_cmp(*xwtm, 0) >= 0), "out-of-time", -ETIMEDOUT);
        XWOS_VALIDATE(((NULL == lock) || (lktype < XWLK_TYPE_NUM)),
                      "invalid-type", -EINVAL);
        XWOS_VALIDATE((-EINTHRD == xwos_irq_get_id(NULL)),
                      "not-in-thrd", -ENOTINTHRD);

        *lkst = XWLK_STATE_LOCKED;
        ctcb = xwos_scheduler_get_ctcb_lc();
        if (__unlikely(0 == xwtm_cmp(*xwtm, 0))) {
                rc = xwos_thrd_do_unlock(lock, lktype, lkdata, datanum);
                if (OK == rc) {
                        *lkst = XWLK_STATE_UNLOCKED;
                }/* else {} */
                rc = -ETIMEDOUT;
        } else {
                rc = xwsync_cdt_grab(cdt);
                if (__likely(OK == rc)) {
                        rc = xwsync_cdt_do_timedwait(cdt, ctcb, lock, lktype,
                                                     lkdata, datanum,
                                                     xwtm, lkst);
                        xwsync_cdt_put(cdt);
                }/* else {} */
        }
        return rc;
}
