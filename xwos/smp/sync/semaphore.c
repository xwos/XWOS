/**
 * @file
 * @brief XuanWuOS的同步机制：信号量
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * @note
 * - 锁的顺序：同级的锁不可同时获得
 *   + ① 等待队列的锁（rtwq->lock & plwq->lock）
 *     + ② 等待队列节点的锁（wqn->lock）
 *     + ② 线程控制块的状态锁（tcb->stlock）
 *     + ② 事件对象的锁（evt->lock）
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/object.h>
#include <xwos/lib/xwbop.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#if defined(XWSMPCFG_SYNC_SMR_MEMSLICE) && (1 == XWSMPCFG_SYNC_SMR_MEMSLICE)
  #include <xwos/mm/memslice.h>
#endif /* XWSMPCFG_SYNC_SMR_MEMSLICE */
#include <xwos/smp/irq.h>
#include <xwos/smp/scheduler.h>
#include <xwos/smp/tt.h>
#include <xwos/smp/thread.h>
#include <xwos/smp/rtwq.h>
#include <xwos/smp/plwq.h>
#include <xwos/smp/lock/spinlock.h>
#include <xwos/smp/lock/seqlock.h>
#include <xwos/smp/lock/mutex.h>
#if defined(XWSMPCFG_SYNC_EVT) && (1 == XWSMPCFG_SYNC_EVT)
  #include <xwos/smp/sync/event.h>
#endif /* XWSMPCFG_SYNC_EVT */
#include <xwos/smp/sync/object.h>
#include <xwos/smp/sync/semaphore.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if defined(XWSMPCFG_SYNC_SMR_MEMSLICE) && (1 == XWSMPCFG_SYNC_SMR_MEMSLICE)
/**
 * @brief 结构体xwsync_smr的对象缓存
 */
static __xwos_data struct xwmm_memslice * xwsync_smr_cache = NULL;

/**
 * @brief 结构体xwsync_smr的对象缓存的名字
 */
__xwos_rodata const char xwsync_smr_cache_name[] = "cache.smr.xwsync";
#endif /* XWSMPCFG_SYNC_SMR_MEMSLICE */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwos_code
struct xwsync_smr * xwsync_smr_alloc(void);

static __xwos_code
void xwsync_smr_free(struct xwsync_smr * smr);

static __xwos_code
xwer_t xwsync_smr_gc(void * smr);

#if defined(XWSMPCFG_SYNC_PLSMR) && (1 == XWSMPCFG_SYNC_PLSMR)
static __xwos_code
xwer_t xwsync_plsmr_activate(struct xwsync_smr * smr, xwssq_t val, xwssq_t max,
                             xwobj_gc_f gcfunc);

static __xwos_code
xwer_t xwsync_plsmr_do_timedblkthrd_unlkwq_cpuirqrs(struct xwsync_smr * smr,
                                                    struct xwos_tcb * tcb,
                                                    xwtm_t * xwtm,
                                                    xwreg_t cpuirq);

static __xwos_code
xwer_t xwsync_plsmr_do_timedwait(struct xwsync_smr * smr, struct xwos_tcb * tcb,
                                 xwtm_t * xwtm);

static __xwos_code
xwer_t xwsync_plsmr_do_blkthrd_unlkwq_cpuirqrs(struct xwsync_smr * smr,
                                               struct xwos_tcb * tcb,
                                               xwreg_t cpuirq);

static __xwos_code
xwer_t xwsync_plsmr_do_wait_unintr(struct xwsync_smr * smr, struct xwos_tcb * tcb);
#endif /* XWSMPCFG_SYNC_PLSMR */

#if defined(XWSMPCFG_SYNC_RTSMR) && (1 == XWSMPCFG_SYNC_RTSMR)
static __xwos_code
xwer_t xwsync_rtsmr_activate(struct xwsync_smr * smr, xwssq_t val, xwssq_t max,
                             xwobj_gc_f gcfunc);

static __xwos_code
xwer_t xwsync_rtsmr_do_timedblkthrd_unlkwq_cpuirqrs(struct xwsync_smr * smr,
                                                    struct xwos_tcb * tcb,
                                                    xwtm_t * xwtm,
                                                    xwreg_t cpuirq);

static __xwos_code
xwer_t xwsync_rtsmr_do_timedwait(struct xwsync_smr * smr, struct xwos_tcb * tcb,
                                 xwtm_t * xwtm);

static __xwos_code
xwer_t xwsync_rtsmr_do_blkthrd_unlkwq_cpuirqrs(struct xwsync_smr * smr,
                                               struct xwos_tcb * tcb,
                                               xwreg_t cpuirq);

static __xwos_code
xwer_t xwsync_rtsmr_do_wait_unintr(struct xwsync_smr * smr, struct xwos_tcb * tcb);
#endif /* XWSMPCFG_SYNC_RTSMR */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** memslice ******** ********/
#if defined(XWSMPCFG_SYNC_SMR_MEMSLICE) && (1 == XWSMPCFG_SYNC_SMR_MEMSLICE)
/**
 * @brief XWOS INIT CODE：初始化结构体xwsync_smr的对象缓存
 * @param zone_origin: (I) 内存区域的首地址
 * @param zone_size: (I) 内存区域的大小
 * @return 错误码
 * @note
 * - 重入性：只可在系统初始化时使用一次
 */
__xwos_init_code
xwer_t xwsync_smr_cache_init(xwptr_t zone_origin, xwsz_t zone_size)
{
        struct xwmm_memslice * msa;
        xwer_t rc;

        rc = xwmm_memslice_create(&msa, zone_origin, zone_size,
                                  sizeof(struct xwsync_smr),
                                  xwsync_smr_cache_name,
                                  (ctor_f)xwsync_smr_construct,
                                  (dtor_f)xwsync_smr_destruct);
        if (__unlikely(rc < 0)) {
                xwsync_smr_cache = NULL;
        } else {
                xwsync_smr_cache = msa;
        }
        return rc;
}
#endif /* XWSMPCFG_SYNC_SMR_MEMSLICE */

/**
 * @brief 从信号量对象缓存中申请对象
 * @return 信号量对象的指针
 */
static __xwos_code
struct xwsync_smr * xwsync_smr_alloc(void)
{
#if defined(XWSMPCFG_SYNC_SMR_MEMSLICE) && (1 == XWSMPCFG_SYNC_SMR_MEMSLICE)
        union {
                struct xwsync_smr * smr;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_memslice_alloc(xwsync_smr_cache, &mem.anon);
        if (rc < 0) {
                mem.smr = err_ptr(rc);
        }/* else {} */
        return mem.smr;
#else /* XWSMPCFG_SYNC_SMR_MEMSLICE */
        union {
                struct xwsync_smr * smr;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwsync_smr), XWMM_ALIGNMENT, &mem.anon);
        if (OK == rc) {
                xwsync_smr_construct(mem.smr);
        } else {
                mem.smr = err_ptr(-ENOMEM);
        }
        return mem.smr;
#endif /* !XWSMPCFG_SYNC_SMR_MEMSLICE */
}

/**
 * @brief 释放信号量对象
 * @param smr: (I) 信号量对象的指针
 */
static __xwos_code
void xwsync_smr_free(struct xwsync_smr * smr)
{
#if defined(XWSMPCFG_SYNC_SMR_MEMSLICE) && (1 == XWSMPCFG_SYNC_SMR_MEMSLICE)
        xwmm_memslice_free(xwsync_smr_cache, smr);
#else /* XWSMPCFG_SYNC_SMR_MEMSLICE */
        xwsync_smr_destruct(smr);
        xwmm_kma_free(smr);
#endif /* !XWSMPCFG_SYNC_SMR_MEMSLICE */
}

/**
 * @brief 信号量对象的构造函数
 * @param smr: (I) 信号量对象的指针
 */
__xwos_code
void xwsync_smr_construct(struct xwsync_smr * smr)
{
        xwsync_object_construct(&smr->xwsyncobj);
}

/**
 * @brief 信号量对象的析构函数
 * @param smr: (I) 信号量对象的指针
 */
__xwos_code
void xwsync_smr_destruct(struct xwsync_smr * smr)
{
        xwsync_object_destruct(&smr->xwsyncobj);
}

/**
 * @brief 信号量对象的垃圾回收函数
 * @param smr: (I) 信号量对象的指针
 * @return 错误码
 */
static __xwos_code
xwer_t xwsync_smr_gc(void * smr)
{
        xwsync_smr_free((struct xwsync_smr *)smr);
        return OK;
}

/**
 * @brief XWOS API：动态创建信号量
 * @param ptrbuf: (O) 指向缓冲区的指针，此缓冲区用于返回对象的指针
 * @param type: (I) 信号量的类型，取值范围 @ref xwsync_smr_type_em
 * @param val: (I) 信号量的初始值
 * @param max: (I) 信号量的最大值
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 无效参数
 * @retval -ETYPE: 类型错误
 * @retval -EACCES: 无效的引用计数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwsync_smr_create(struct xwsync_smr ** ptrbuf, xwid_t type,
                         xwssq_t val, xwssq_t max)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);

        *ptrbuf = NULL;
        smr = xwsync_smr_alloc();
        if (__unlikely(is_err(smr))) {
                rc = ptr_err(smr);
        } else {
                rc = -ETYPE;
                switch (type) {
#if defined(XWSMPCFG_SYNC_PLSMR) && (1 == XWSMPCFG_SYNC_PLSMR)
                case XWSYNC_SMR_TYPE_PIPELINE:
                        rc = xwsync_plsmr_activate(smr, val, max, xwsync_smr_gc);
                        break;
#endif /* XWSMPCFG_SYNC_PLSMR */
#if defined(XWSMPCFG_SYNC_RTSMR) && (1 == XWSMPCFG_SYNC_RTSMR)
                case XWSYNC_SMR_TYPE_RT:
                        rc = xwsync_rtsmr_activate(smr, val, max, xwsync_smr_gc);
                        break;
#endif /* XWSMPCFG_SYNC_RTSMR */
                }
                if (__unlikely(rc < 0)) {
                        xwsync_smr_free(smr);
                } else {
                        *ptrbuf = smr;
                }
        }
        return rc;
}

/**
 * @brief XWOS API：删除动态创建的信号量
 * @param smr: (I) 信号量对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -EACCES: 无效的引用计数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *smr* ，不可重入
 */
__xwos_api
xwer_t xwsync_smr_delete(struct xwsync_smr * smr)
{
        XWOS_VALIDATE((smr), "nullptr", -EFAULT);

        return xwsync_smr_put(smr);
}

/**
 * @brief XWOS API：销毁静态方式初始化的信号量对象
 * @param smr: (I) 信号量对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EACCES: 无效的引用计数
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *smr* ，不可重入
 */
__xwos_api
xwer_t xwsync_smr_destroy(struct xwsync_smr * smr)
{
        XWOS_VALIDATE((smr), "nullptr", -EFAULT);

        return xwsync_smr_put(smr);
}

#if defined(XWSMPCFG_SYNC_EVT) && (1 == XWSMPCFG_SYNC_EVT)
/**
 * @brief XWOS API：绑定信号量到事件对象，事件对象类型为XWSYNC_EVT_TYPE_SELECTOR
 * @param smr: (I) 信号量对象的指针
 * @param evt: (I) 事件对象的指针
 * @param pos: (I) 信号量对象映射到位图中的位置
 * @return 错误码
 * @retval OK: OK
 * @retval -ETYPE: 事件对象或信号量类型错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *smr* ，不可重入
 */
__xwos_api
xwer_t xwsync_smr_bind(struct xwsync_smr * smr, struct xwsync_evt * evt, xwsq_t pos)
{
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((smr), "nullptr", -EFAULT);

        switch (smr->type) {
#if defined(XWSMPCFG_SYNC_PLSMR) && (1 == XWSMPCFG_SYNC_PLSMR)
        case XWSYNC_SMR_TYPE_PIPELINE:
                xwos_plwq_lock_cpuirqsv(&smr->wq.pl, &cpuirq);
                rc = xwsync_evt_obj_bind(evt, &smr->xwsyncobj, pos, true);
                if ((OK == rc) && (smr->count > 0)) {
                        rc = xwsync_evt_obj_s1i(evt, &smr->xwsyncobj);
                }
                xwos_plwq_unlock_cpuirqrs(&smr->wq.pl, cpuirq);
                break;
#endif /* XWSMPCFG_SYNC_PLSMR */
#if defined(XWSMPCFG_SYNC_RTSMR) && (1 == XWSMPCFG_SYNC_RTSMR)
        case XWSYNC_SMR_TYPE_RT:
                xwos_rtwq_lock_cpuirqsv(&smr->wq.rt, &cpuirq);
                rc = xwsync_evt_obj_bind(evt, &smr->xwsyncobj, pos, true);
                if ((OK == rc) && (smr->count > 0)) {
                        rc = xwsync_evt_obj_s1i(evt, &smr->xwsyncobj);
                }
                xwos_rtwq_unlock_cpuirqrs(&smr->wq.rt, cpuirq);
                break;
#endif /* XWSMPCFG_SYNC_RTSMR */
        default:
                rc = -ETYPE;
                break;
        }
        return rc;
}

/**
 * @brief XWOS API：从事件对象上解绑信号量，事件对象类型为XWSYNC_EVT_TYPE_SELECTOR
 * @param smr: (I) 信号量对象的指针
 * @param evt: (I) 事件对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -ETYPE: 事件对象或信号量类型错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *smr* ，不可重入
 */
__xwos_api
xwer_t xwsync_smr_unbind(struct xwsync_smr * smr, struct xwsync_evt * evt)
{
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((smr), "nullptr", -EFAULT);
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        switch (smr->type) {
#if defined(XWSMPCFG_SYNC_PLSMR) && (1 == XWSMPCFG_SYNC_PLSMR)
        case XWSYNC_SMR_TYPE_PIPELINE:
                xwos_plwq_lock_cpuirqsv(&smr->wq.pl, &cpuirq);
                rc = xwsync_evt_obj_unbind(evt, &smr->xwsyncobj, true);
                if (OK == rc) {
                        rc = xwsync_evt_obj_c0i(evt, &smr->xwsyncobj);
                }
                xwos_plwq_unlock_cpuirqrs(&smr->wq.pl, cpuirq);
                break;
#endif /* XWSMPCFG_SYNC_PLSMR */
#if defined(XWSMPCFG_SYNC_RTSMR) && (1 == XWSMPCFG_SYNC_RTSMR)
        case XWSYNC_SMR_TYPE_RT:
                xwos_rtwq_lock_cpuirqsv(&smr->wq.rt, &cpuirq);
                rc = xwsync_evt_obj_unbind(evt, &smr->xwsyncobj, true);
                if (OK == rc) {
                        rc = xwsync_evt_obj_c0i(evt, &smr->xwsyncobj);
                }
                xwos_rtwq_unlock_cpuirqrs(&smr->wq.rt, cpuirq);
                break;
#endif /* XWSMPCFG_SYNC_RTSMR */
        default:
                rc = -ETYPE;
                break;
        }
        return rc;
}
#endif /* XWSMPCFG_SYNC_EVT */

#if defined(XWSMPCFG_SYNC_PLSMR) && (1 == XWSMPCFG_SYNC_PLSMR)
/**
 * @brief XWOS API：激活并初始化管道信号量对象
 * @param smr: (I) 信号量对象的指针
 * @param val: (I) 信号量的初始值
 * @param max: (I) 信号量的最大值
 * @param gcfunc: (I) 垃圾回收函数的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EINVAL: 无效参数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *smr* ，不可重入
 */
static __xwos_code
xwer_t xwsync_plsmr_activate(struct xwsync_smr * smr, xwssq_t val, xwssq_t max,
                             xwobj_gc_f gcfunc)
{
        xwer_t rc;

        XWOS_VALIDATE(((val >= 0) && (max > 0) && (val <= max)),
                      "invalid-value", -EINVAL);

        rc = xwsync_object_activate(&smr->xwsyncobj, gcfunc);
        if (__likely(OK == rc)) {
                smr->max = max;
                xwos_plwq_init(&smr->wq.pl);
                smr->type = XWSYNC_SMR_TYPE_PIPELINE;
                smr->count = val;
        }/* else {} */
        return rc;
}

/**
 * @brief XWOS API：静态初始化管道信号量对象
 * @param smr: (I) 信号量对象的指针
 * @param val: (I) 信号量的初始值
 * @param max: (I) 信号量的最大值
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 无效参数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *smr* ，不可重入
 */
__xwos_api
xwer_t xwsync_plsmr_init(struct xwsync_smr * smr, xwssq_t val, xwssq_t max)
{
        XWOS_VALIDATE((smr), "nullptr", -EFAULT);

        xwsync_smr_construct(smr);
        return xwsync_plsmr_activate(smr, val, max, NULL);
}

/**
 * @brief XWOS API：冻结管道信号量（值设置为负）
 * @param smr: (I) 信号量对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 类型不匹配
 * @retval -EALREADY: 信号量已为负
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 已冻结的信号量不允许增加(V操作)，但可以被测试(P操作)，
 *   测试信号量的线程会被加入到信号量的等待队列。
 */
__xwos_api
xwer_t xwsync_plsmr_freeze(struct xwsync_smr * smr)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((smr), "nullptr", -EFAULT);
        XWOS_VALIDATE((XWSYNC_SMR_TYPE_PIPELINE == smr->type), "type-error", -ETYPE);

        rc = xwsync_smr_grab(smr);
        if (__likely(OK == rc)) {
                xwos_plwq_lock_cpuirqsv(&smr->wq.pl, &cpuirq);
                if (__unlikely(smr->count < 0)) {
                        rc = -EALREADY;
                } else {
                        smr->count = XWSYNC_SMR_NEGTIVE;
#if defined(XWSMPCFG_SYNC_EVT) && (1 == XWSMPCFG_SYNC_EVT)
                        struct xwsync_evt * evt;
                        struct xwsync_object * xwsyncobj;

                        xwsyncobj = &smr->xwsyncobj;
                        xwmb_smp_load_acquire(struct xwsync_evt *,
                                              evt, &xwsyncobj->selector.evt);
                        if (NULL != evt) {
                                xwsync_evt_obj_c0i(evt, xwsyncobj);
                        }
#endif /* XWSMPCFG_SYNC_EVT */
                }
                xwos_plwq_unlock_cpuirqrs(&smr->wq.pl, cpuirq);
                xwsync_smr_put(smr);
        }/* else {} */
        return rc;
}

/**
 * @brief XWOS API：解冻信号量，并重新初始化为管道信号量
 * @param smr: (I) 信号量对象的指针
 * @param val: (I) 信号量的初始值
 * @param max: (I) 信号量的最大值
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 类型不匹配
 * @retval -EINVAL: 参数无效
 * @retval -EALREADY: 信号量未被冻结
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对已冻结的信号量起作用，对未冻结的信号量调用此函数将返回错误码。
 */
__xwos_api
xwer_t xwsync_plsmr_thaw(struct xwsync_smr * smr, xwssq_t val, xwssq_t max)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((smr), "nullptr", -EFAULT);
        XWOS_VALIDATE((XWSYNC_SMR_TYPE_PIPELINE == smr->type), "type-error", -ETYPE);
        XWOS_VALIDATE(((val >= 0) && (max > 0) && (val <= max)),
                      "invalid-value", -EINVAL);

        rc = xwsync_smr_grab(smr);
        if (__likely(OK == rc)) {
                xwos_plwq_lock_cpuirqsv(&smr->wq.pl, &cpuirq);
                if (__unlikely(smr->count >= 0)) {
                        rc = -EALREADY;
                } else {
                        smr->max = max;
                        smr->type = XWSYNC_SMR_TYPE_PIPELINE;
                        smr->count = val;
                        rc = OK;
#if defined(XWSMPCFG_SYNC_EVT) && (1 == XWSMPCFG_SYNC_EVT)
                        if (smr->count > 0) {
                                struct xwsync_evt * evt;
                                struct xwsync_object * xwsyncobj;

                                xwsyncobj = &smr->xwsyncobj;
                                xwmb_smp_load_acquire(struct xwsync_evt *,
                                                      evt,
                                                      &xwsyncobj->selector.evt);
                                if (NULL != evt) {
                                        xwsync_evt_obj_s1i(evt, xwsyncobj);
                                }
                        }
#endif /* XWSMPCFG_SYNC_EVT */
                }
                xwos_plwq_unlock_cpuirqrs(&smr->wq.pl, cpuirq);
                xwsync_smr_put(smr);
        }/* else {} */
        return rc;
}

/**
 * @brief 中断管道信号量等待队列中的一个节点
 * @param smr: (I) 信号量对象的指针
 * @param wqn: (I) 等待队列节点
 * @return 错误码
 * @retval OK: OK
 * @retval -ETYPE: 类型不匹配
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_code
xwer_t xwsync_plsmr_intr(struct xwsync_smr * smr, struct xwos_wqn * wqn)
{
        xwos_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwsync_smr_grab(smr);
        if (__likely(OK == rc)) {
                xwos_plwq_lock_cpuirqsv(&smr->wq.pl, &cpuirq);
                xwlk_splk_lock(&wqn->lock);
                rc = xwos_plwq_remove_locked(&smr->wq.pl, wqn);
                if (OK == rc) {
                        wqn->wq = NULL;
                        wqn->type = XWOS_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq_t, &wqn->rsmrs,
                                    xwmb_modr_release, XWOS_WQN_RSMRS_INTR);
                        cb = wqn->cb;
                        wqn->cb = NULL;
                        xwlk_splk_unlock(&wqn->lock);
                        xwos_plwq_unlock_cpuirqrs(&smr->wq.pl, cpuirq);
                        xwsync_smr_put(smr);
                        cb(wqn->owner);
                } else {
                        xwlk_splk_unlock(&wqn->lock);
                        xwos_plwq_unlock_cpuirqrs(&smr->wq.pl, cpuirq);
                        xwsync_smr_put(smr);
                }
        }/* else {} */
        return rc;
}

/**
 * @brief XWOS API：发布管道信号量
 * @param smr: (I) 信号量对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 类型不匹配
 * @retval -ENEGATIVE: 信号量为负
 * @retval -ERANGE: 信号量的值已经最大
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对未冻结的信号量起作用，已冻结的信号量将得到错误码-ENEGATIVE。
 */
__xwos_api
xwer_t xwsync_plsmr_post(struct xwsync_smr * smr)
{
        struct xwos_wqn * wqn;
        xwos_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((smr), "nullptr", -EFAULT);
        XWOS_VALIDATE((XWSYNC_SMR_TYPE_PIPELINE == smr->type), "type-error", -ETYPE);

        rc = xwsync_smr_grab(smr);
        if (__likely(OK == rc)) {
                xwos_plwq_lock_cpuirqsv(&smr->wq.pl, &cpuirq);
                if (smr->count < 0) {
                        xwos_plwq_unlock_cpuirqrs(&smr->wq.pl, cpuirq);
                        rc = -ENEGATIVE;
                } else {
                        wqn = xwos_plwq_choose_locked(&smr->wq.pl);
                        if (wqn) {
                                wqn->wq = NULL;
                                wqn->type = XWOS_WQTYPE_UNKNOWN;
                                xwaop_store(xwsq_t, &wqn->rsmrs,
                                            xwmb_modr_release, XWOS_WQN_RSMRS_UP);
                                cb = wqn->cb;
                                wqn->cb = NULL;
                                xwlk_splk_unlock(&wqn->lock);
                                xwos_plwq_unlock_cpuirqrs(&smr->wq.pl, cpuirq);
                                xwsync_smr_put(smr);
                                cb(wqn->owner);
                        } else {
                                /* add semaphore counter */
                                if (smr->count < smr->max) {
                                        smr->count++;
                                } else {
                                        rc = -ERANGE;
                                }
#if defined(XWSMPCFG_SYNC_EVT) && (1 == XWSMPCFG_SYNC_EVT)
                                if (smr->count > 0) {
                                        struct xwsync_evt * evt;
                                        struct xwsync_object * xwsyncobj;

                                        xwsyncobj = &smr->xwsyncobj;
                                        xwmb_smp_load_acquire(struct xwsync_evt *,
                                                              evt,
                                                              &xwsyncobj->selector.evt);
                                        if (NULL != evt) {
                                                xwsync_evt_obj_s1i(evt,
                                                                   xwsyncobj);
                                        }
                                }
#endif /* XWSMPCFG_SYNC_EVT */
                                xwos_plwq_unlock_cpuirqrs(&smr->wq.pl, cpuirq);
                                xwsync_smr_put(smr);
                        }
                }
        }/* else {} */
        return rc;
}

/**
 * @brief XWOS API：尝试获取信号量
 * @param smr: (I) 信号量对象的指针
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 类型不匹配
 * @retval -ENODATA: 信号量资源不可用（信号量无法被获取）
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数可在中断上下文中使用。
 */
__xwos_api
xwer_t xwsync_plsmr_trywait(struct xwsync_smr * smr)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((smr), "nullptr", -EFAULT);
        XWOS_VALIDATE((XWSYNC_SMR_TYPE_PIPELINE == smr->type), "type-error", -ETYPE);

        rc = xwsync_smr_grab(smr);
        if (__likely(OK == rc)) {
                xwos_plwq_lock_cpuirqsv(&smr->wq.pl, &cpuirq);
                if (smr->count > 0) {
                        smr->count--;
#if defined(XWSMPCFG_SYNC_EVT) && (1 == XWSMPCFG_SYNC_EVT)
                        if (0 == smr->count) {
                                struct xwsync_evt * evt;
                                struct xwsync_object * xwsyncobj;

                                xwsyncobj = &smr->xwsyncobj;
                                xwmb_smp_load_acquire(struct xwsync_evt *,
                                                      evt,
                                                      &xwsyncobj->selector.evt);
                                if (NULL != evt) {
                                        xwsync_evt_obj_c0i(evt, xwsyncobj);
                                }
                        }
#endif /* XWSMPCFG_SYNC_EVT */
                } else {
                        rc = -ENODATA;
                }
                xwos_plwq_unlock_cpuirqrs(&smr->wq.pl, cpuirq);
                xwsync_smr_put(smr);
        }/* else {} */
        return rc;
}

static __xwos_code
xwer_t xwsync_plsmr_do_timedblkthrd_unlkwq_cpuirqrs(struct xwsync_smr * smr,
                                                    struct xwos_tcb * tcb,
                                                    xwtm_t * xwtm,
                                                    xwreg_t cpuirq)
{
        struct xwos_scheduler * xwsd;
        struct xwos_tt * xwtt;
        struct xwos_syshwt * hwt;
        xwtm_t expected, currtick;
        xwpr_t dprio;
        xwsq_t rsmrs;
        xwsq_t wkuprs;
        xwer_t rc;

        xwmb_smp_load_acquire(struct xwos_scheduler *, xwsd, &tcb->xwsd);
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
                xwos_plwq_unlock_cpuirqrs(&smr->wq.pl, cpuirq);
                rc = -EINTR;
                goto err_needfrz;
        }
        dprio = tcb->dprio.r;
        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_RUNNING);
        tcb->dprio.r = XWOS_SD_PRIORITY_INVALID;
        tcb->dprio.wq = dprio;
        xwbop_s1m(xwsq_t, &tcb->state, XWSDOBJ_DST_BLOCKING);
        xwlk_splk_unlock(&tcb->stlock);
        xwos_thrd_eq_plwq_locked(tcb, &smr->wq.pl, XWOS_WQTYPE_PLSMR);
        xwos_plwq_unlock_cpuirqrs(&smr->wq.pl, cpuirq);

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
        xwmb_smp_rmb();
        /* 确保对唤醒原因的读取操作发生在线程状态切换之后 */
        rsmrs = xwaop_load(xwsq_t, &tcb->wqn.rsmrs, xwmb_modr_relaxed);
        wkuprs = xwaop_load(xwsq_t, &tcb->ttn.wkuprs, xwmb_modr_relaxed);
        if (XWOS_WQN_RSMRS_INTR == rsmrs) {
                xwlk_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwos_tt_remove_locked(xwtt, &tcb->ttn);
                if (OK == rc) {
                        xwlk_splk_lock(&tcb->stlock);
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_SLEEPING);
                        xwlk_splk_unlock(&tcb->stlock);
                }/* else {} */
                xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = -EINTR;
        } else if (XWOS_WQN_RSMRS_UP == rsmrs) {
                xwlk_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwos_tt_remove_locked(xwtt, &tcb->ttn);
                if (OK == rc) {
                        xwlk_splk_lock(&tcb->stlock);
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_SLEEPING);
                        xwlk_splk_unlock(&tcb->stlock);
                }/* else {} */
                xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = OK;
        } else if (XWOS_TTN_WKUPRS_TIMEDOUT == wkuprs) {
                xwos_plwq_lock_cpuirq(&smr->wq.pl);
                xwlk_splk_lock(&tcb->wqn.lock);
                rc = xwos_plwq_remove_locked(&smr->wq.pl, &tcb->wqn);
                if (OK == rc) {
                        tcb->wqn.wq = NULL;
                        tcb->wqn.type = XWOS_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq_t, &tcb->wqn.rsmrs,
                                    xwmb_modr_release, XWOS_WQN_RSMRS_INTR);
                        tcb->wqn.cb = NULL;
                        xwlk_splk_unlock(&tcb->wqn.lock);
                        xwlk_splk_lock(&tcb->stlock);
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_BLOCKING);
                        tcb->dprio.wq = XWOS_SD_PRIORITY_INVALID;
                        xwlk_splk_unlock(&tcb->stlock);
                        xwos_plwq_unlock_cpuirqrs(&smr->wq.pl, cpuirq);
                        rc = -ETIMEDOUT;
                } else {
                        xwlk_splk_unlock(&tcb->wqn.lock);
                        xwos_plwq_unlock_cpuirqrs(&smr->wq.pl, cpuirq);
                        rsmrs = xwaop_load(xwsq_t, &tcb->wqn.rsmrs, xwmb_modr_relaxed);
                        if (XWOS_WQN_RSMRS_INTR == rsmrs) {
                                rc = -EINTR;
                        } else if (XWOS_WQN_RSMRS_UP == rsmrs) {
                                rc = OK;
                        } else {
                                XWOS_BUG();
                                rc = -EBUG;
                        }
                }
        } else if (XWOS_TTN_WKUPRS_INTR == wkuprs) {
                xwos_plwq_lock_cpuirq(&smr->wq.pl);
                xwlk_splk_unlock(&tcb->wqn.lock);
                rc = xwos_plwq_remove_locked(&smr->wq.pl, &tcb->wqn);
                if (OK == rc) {
                        tcb->wqn.wq = NULL;
                        tcb->wqn.type = XWOS_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq_t, &tcb->wqn.rsmrs,
                                    xwmb_modr_release, XWOS_WQN_RSMRS_INTR);
                        tcb->wqn.cb = NULL;
                        xwlk_splk_unlock(&tcb->wqn.lock);
                        xwlk_splk_lock(&tcb->stlock);
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_BLOCKING);
                        tcb->dprio.wq = XWOS_SD_PRIORITY_INVALID;
                        xwlk_splk_unlock(&tcb->stlock);
                        xwos_plwq_unlock_cpuirqrs(&smr->wq.pl, cpuirq);
                        rc = -EINTR;
                } else {
                        xwlk_splk_unlock(&tcb->wqn.lock);
                        xwos_plwq_unlock_cpuirqrs(&smr->wq.pl, cpuirq);
                        rsmrs = xwaop_load(xwsq_t, &tcb->wqn.rsmrs, xwmb_modr_relaxed);
                        if (XWOS_WQN_RSMRS_INTR == rsmrs) {
                                rc = -EINTR;
                        } else if (XWOS_WQN_RSMRS_UP == rsmrs) {
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

static __xwos_code
xwer_t xwsync_plsmr_do_timedwait(struct xwsync_smr * smr, struct xwos_tcb * tcb,
                                 xwtm_t * xwtm)
{
        struct xwos_scheduler * xwsd;
        xwreg_t cpuirq;
        xwer_t rc;

        xwmb_smp_load_acquire(struct xwos_scheduler *, xwsd, &tcb->xwsd);
        xwos_plwq_lock_cpuirqsv(&smr->wq.pl, &cpuirq);
        if (smr->count <= 0) {
                rc = xwos_scheduler_wakelock_lock(xwsd);
                if (__unlikely(rc < 0)) {
                        /* 当前调度器正准备休眠，线程需被冻结，返回-EINTR。*/
                        xwos_plwq_unlock_cpuirqrs(&smr->wq.pl, cpuirq);
                        rc = -EINTR;
                } else {
                        rc = xwsync_plsmr_do_timedblkthrd_unlkwq_cpuirqrs(smr,
                                                                          tcb,
                                                                          xwtm,
                                                                          cpuirq);
                        xwos_scheduler_wakelock_unlock(xwsd);
                }
        } else {
                smr->count--;
#if defined(XWSMPCFG_SYNC_EVT) && (1 == XWSMPCFG_SYNC_EVT)
                if (0 == smr->count) {
                        struct xwsync_evt * evt;
                        struct xwsync_object * xwsyncobj;

                        xwsyncobj = &smr->xwsyncobj;
                        xwmb_smp_load_acquire(struct xwsync_evt *,
                                              evt, &xwsyncobj->selector.evt);
                        if (NULL != evt) {
                                xwsync_evt_obj_c0i(evt, xwsyncobj);
                        }
                }
#endif /* XWSMPCFG_SYNC_EVT */
                xwos_plwq_unlock_cpuirqrs(&smr->wq.pl, cpuirq);
                rc = OK;
        }
        return rc;
}

/**
 * @brief XWOS API：限时等待获取信号量
 * @param smr: (I) 信号量对象的指针
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 类型错误
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
xwer_t xwsync_plsmr_timedwait(struct xwsync_smr * smr, xwtm_t * xwtm)
{
        struct xwos_tcb * ctcb;
        xwer_t rc;

        XWOS_VALIDATE((smr), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((XWSYNC_SMR_TYPE_PIPELINE == smr->type), "type-error", -ETYPE);
        XWOS_VALIDATE((xwtm_cmp(*xwtm, 0) >= 0), "out-of-time", -ETIMEDOUT);
        XWOS_VALIDATE((-EINTHRD == xwos_irq_get_id(NULL)),
                      "not-in-thrd", -ENOTINTHRD);

        if (__unlikely(0 == xwtm_cmp(*xwtm, 0))) {
                rc = xwsync_plsmr_trywait(smr);
                if (__unlikely(rc < 0)) {
                        if (__likely(-ENODATA == rc)) {
                                rc = -ETIMEDOUT;
                        }/* else {} */
                }/* else {} */
        } else {
                ctcb = xwos_scheduler_get_ctcb_lc();
                rc = xwsync_smr_grab(smr);
                if (__likely(OK == rc)) {
                        rc = xwsync_plsmr_do_timedwait(smr, ctcb, xwtm);
                        xwsync_smr_put(smr);
                }/* else {} */
        }
        return rc;
}

static __xwos_code
xwer_t xwsync_plsmr_do_blkthrd_unlkwq_cpuirqrs(struct xwsync_smr * smr,
                                               struct xwos_tcb * tcb,
                                               xwreg_t cpuirq)
{
        struct xwos_scheduler * xwsd;
        xwpr_t dprio;
        xwsq_t rsmrs;
        xwer_t rc;

        xwmb_smp_load_acquire(struct xwos_scheduler *, xwsd, &tcb->xwsd);
        /* 加入等待队列 */
        xwlk_splk_lock(&tcb->stlock);
        XWOS_BUG_ON((XWSDOBJ_DST_BLOCKING | XWSDOBJ_DST_SLEEPING |
                     XWSDOBJ_DST_READY | XWSDOBJ_DST_STANDBY |
                     XWSDOBJ_DST_FROZEN | XWSDOBJ_DST_MIGRATING)
                    & tcb->state);
        dprio = tcb->dprio.r;
        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_RUNNING);
        tcb->dprio.r = XWOS_SD_PRIORITY_INVALID;
        tcb->dprio.wq = dprio;
        xwbop_s1m(xwsq_t, &tcb->state,
                  XWSDOBJ_DST_BLOCKING | XWSDOBJ_DST_UNINTERRUPTED);
        xwlk_splk_unlock(&tcb->stlock);
        xwos_thrd_eq_plwq_locked(tcb, &smr->wq.pl, XWOS_WQTYPE_PLSMR);
        xwos_plwq_unlock_cpuirqrs(&smr->wq.pl, cpuirq);

        /* 调度 */
        xwos_cpuirq_enable_lc();
        xwos_scheduler_req_swcx(xwsd);
        xwos_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        xwmb_smp_rmb();
        /* 确保对唤醒原因的读取操作发生在线程状态切换之后 */
        rsmrs = xwaop_load(xwsq_t, &tcb->wqn.rsmrs, xwmb_modr_relaxed);
        if (XWOS_WQN_RSMRS_UP == rsmrs) {
                rc = OK;
        } else {
                XWOS_BUG();
                rc = -EBUG;
        }
        return rc;
}

static __xwos_code
xwer_t xwsync_plsmr_do_wait_unintr(struct xwsync_smr * smr, struct xwos_tcb * tcb)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwos_plwq_lock_cpuirqsv(&smr->wq.pl, &cpuirq);
        if (smr->count <= 0) {
                rc = xwsync_plsmr_do_blkthrd_unlkwq_cpuirqrs(smr, tcb, cpuirq);
        } else {
                smr->count--;
#if defined(XWSMPCFG_SYNC_EVT) && (1 == XWSMPCFG_SYNC_EVT)
                if (0 == smr->count) {
                        struct xwsync_evt * evt;
                        struct xwsync_object * xwsyncobj;

                        xwsyncobj = &smr->xwsyncobj;
                        xwmb_smp_load_acquire(struct xwsync_evt *,
                                              evt, &xwsyncobj->selector.evt);
                        if (NULL != evt) {
                                xwsync_evt_obj_c0i(evt, xwsyncobj);
                        }
                }
#endif /* XWSMPCFG_SYNC_EVT */
                xwos_plwq_unlock_cpuirqrs(&smr->wq.pl, cpuirq);
                rc = OK;
        }
        return rc;
}

/**
 * @brief XWOS API：等待并获取管道信号量，且等待不可被中断
 * @param smr: (I) 信号量对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 类型错误
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwsync_plsmr_wait_unintr(struct xwsync_smr * smr)
{
        struct xwos_tcb * ctcb;
        xwer_t rc;

        XWOS_VALIDATE((smr), "nullptr", -EFAULT);
        XWOS_VALIDATE((XWSYNC_SMR_TYPE_PIPELINE == smr->type), "type-error", -ETYPE);

        rc = xwsync_smr_grab(smr);
        if (__likely(OK == rc)) {
                ctcb = xwos_scheduler_get_ctcb_lc();
                rc = xwsync_plsmr_do_wait_unintr(smr, ctcb);
                xwsync_smr_put(smr);
        }/* else {} */
        return rc;
}
#endif /* XWSMPCFG_SYNC_PLSMR */

#if defined(XWSMPCFG_SYNC_RTSMR) && (1 == XWSMPCFG_SYNC_RTSMR)
/**
 * @brief 激活并初始化实时信号量对象
 * @param smr: (I) 信号量对象的指针
 * @param val: (I) 信号量的初始值
 * @param max: (I) 信号量的最大值
 * @param gcfunc: (I) 垃圾回收函数的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EINVAL: 无效参数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *smr* ，不可重入
 */
static __xwos_code
xwer_t xwsync_rtsmr_activate(struct xwsync_smr * smr, xwssq_t val, xwssq_t max,
                             xwobj_gc_f gcfunc)
{
        xwer_t rc;

        XWOS_VALIDATE(((val >= 0) && (max > 0) && (val <= max)),
                      "invalid-value", -EINVAL);

        rc = xwsync_object_activate(&smr->xwsyncobj, gcfunc);
        if (__likely(OK == rc)) {
                smr->max = max;
                xwos_rtwq_init(&smr->wq.rt);
                smr->type = XWSYNC_SMR_TYPE_RT;
                smr->count = val;
        }/* else {} */
        return rc;
}

/**
 * @brief XWOS API：静态初始化实时信号量
 * @param smr: (I) 信号量对象的指针
 * @param val: (I) 信号量的初始值
 * @param max: (I) 信号量的最大值
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 无效参数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *smr* ，不可重入
 */
__xwos_api
xwer_t xwsync_rtsmr_init(struct xwsync_smr * smr, xwssq_t val, xwssq_t max)
{
        XWOS_VALIDATE((smr), "nullptr", -EFAULT);
        XWOS_VALIDATE(((val >= 0) && (max > 0) && (val <= max)),
                      "invalid-value", -EINVAL);

        xwsync_smr_construct(smr);
        return xwsync_rtsmr_activate(smr, val, max, NULL);
}

/**
 * @brief XWOS API：冻结实时信号量（值设置为负）
 * @param smr: (I) 信号量对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 类型不匹配
 * @retval -EALREADY: 信号量已被冻结
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *smr* ，不可重入
 * @note
 * - 已冻结的信号量不允许增加(V操作)，但可以被测试(P操作)，
 *   测试信号量的线程会被加入到信号量的等待队列。
 */
__xwos_api
xwer_t xwsync_rtsmr_freeze(struct xwsync_smr * smr)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((smr), "nullptr", -EFAULT);
        XWOS_VALIDATE((XWSYNC_SMR_TYPE_RT == smr->type), "type-error", -ETYPE);

        rc = xwsync_smr_grab(smr);
        if (__likely(OK == rc)) {
                xwos_rtwq_lock_cpuirqsv(&smr->wq.rt, &cpuirq);
                if (__unlikely(smr->count < 0)) {
                        rc = -EALREADY;
                } else {
                        smr->count = XWSYNC_SMR_NEGTIVE;
#if defined(XWSMPCFG_SYNC_EVT) && (1 == XWSMPCFG_SYNC_EVT)
                        struct xwsync_evt * evt;
                        struct xwsync_object * xwsyncobj;

                        xwsyncobj = &smr->xwsyncobj;
                        xwmb_smp_load_acquire(struct xwsync_evt *,
                                              evt, &xwsyncobj->selector.evt);
                        if (NULL != evt) {
                                xwsync_evt_obj_c0i(evt, xwsyncobj);
                        }
#endif /* XWSMPCFG_SYNC_EVT */
                }
                xwos_rtwq_unlock_cpuirqrs(&smr->wq.rt, cpuirq);
                xwsync_smr_put(smr);
        }/* else {} */
        return rc;
}

/**
 * @brief XWOS API：解冻信号量，并重新初始化为实时信号量
 * @param smr: (I) 信号量对象的指针
 * @param val: (I) 信号量的初始值
 * @param max: (I) 信号量的最大值
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 类型不匹配
 * @retval -EINVAL: 参数无效
 * @retval -EALREADY: 信号量未被冻结
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对已冻结的信号量起作用，对未冻结的信号量调用此函数将返回错误码。
 */
__xwos_api
xwer_t xwsync_rtsmr_thaw(struct xwsync_smr * smr, xwssq_t val, xwssq_t max)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((smr), "nullptr", -EFAULT);
        XWOS_VALIDATE((XWSYNC_SMR_TYPE_RT == smr->type), "type-error", -ETYPE);
        XWOS_VALIDATE(((val >= 0) && (max > 0) && (val <= max)),
                      "invalid-value", -EINVAL);

        rc = xwsync_smr_grab(smr);
        if (__likely(OK == rc)) {
                xwos_rtwq_lock_cpuirqsv(&smr->wq.rt, &cpuirq);
                if (__unlikely(smr->count >= 0)) {
                        rc = -EALREADY;
                } else {
                        smr->max = max;
                        smr->type = XWSYNC_SMR_TYPE_RT;
                        smr->count = val;
                        rc = OK;
#if defined(XWSMPCFG_SYNC_EVT) && (1 == XWSMPCFG_SYNC_EVT)
                        if (smr->count > 0) {
                                struct xwsync_evt * evt;
                                struct xwsync_object * xwsyncobj;

                                xwsyncobj = &smr->xwsyncobj;
                                xwmb_smp_load_acquire(struct xwsync_evt *,
                                                      evt, &xwsyncobj->selector.evt);
                                if (NULL != evt) {
                                        xwsync_evt_obj_s1i(evt, xwsyncobj);
                                }
                        }
#endif /* XWSMPCFG_SYNC_EVT */
                }
                xwos_rtwq_unlock_cpuirqrs(&smr->wq.rt, cpuirq);
                xwsync_smr_put(smr);
        }/* else {} */
        return rc;
}

/**
 * @brief 中断实时信号量等待队列中的一个节点
 * @param smr: (I) 信号量对象的指针
 * @param wqn: (I) 等待队列节点
 * @return 错误码
 * @retval OK: OK
 * @retval -ETYPE: 类型不匹配
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_code
xwer_t xwsync_rtsmr_intr(struct xwsync_smr * smr, struct xwos_wqn * wqn)
{
        xwos_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwsync_smr_grab(smr);
        if (__likely(OK == rc)) {
                xwos_rtwq_lock_cpuirqsv(&smr->wq.rt, &cpuirq);
                xwlk_splk_lock(&wqn->lock);
                rc = xwos_rtwq_remove_locked(&smr->wq.rt, wqn);
                if (OK == rc) {
                        wqn->wq = NULL;
                        wqn->type = XWOS_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq_t, &wqn->rsmrs,
                                    xwmb_modr_release, XWOS_WQN_RSMRS_INTR);
                        cb = wqn->cb;
                        wqn->cb = NULL;
                        xwlk_splk_unlock(&wqn->lock);
                        xwos_rtwq_unlock_cpuirqrs(&smr->wq.rt, cpuirq);
                        xwsync_smr_put(smr);
                        cb(wqn->owner);
                } else {
                        xwlk_splk_unlock(&wqn->lock);
                        xwos_rtwq_unlock_cpuirqrs(&smr->wq.rt, cpuirq);
                        xwsync_smr_put(smr);
                }
        }/* else {} */
        return rc;
}

/**
 * @brief XWOS API：发布实时信号量
 * @param smr: (I) 信号量对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 类型不匹配
 * @retval -ENEGATIVE: 信号量为负
 * @retval -ERANGE: 信号量的值已经最大
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对未冻结的信号量起作用，已冻结的信号量将得到错误码-ENEGATIVE。
 */
__xwos_api
xwer_t xwsync_rtsmr_post(struct xwsync_smr * smr)
{
        struct xwos_wqn * wqn;
        xwreg_t cpuirq;
        xwos_wqn_f cb;
        xwer_t rc;

        XWOS_VALIDATE((smr), "nullptr", -EFAULT);
        XWOS_VALIDATE((XWSYNC_SMR_TYPE_RT == smr->type), "type-error", -ETYPE);

        rc = xwsync_smr_grab(smr);
        if (__likely(OK == rc)) {
                xwos_rtwq_lock_cpuirqsv(&smr->wq.rt, &cpuirq);
                if (__unlikely(smr->count < 0)) {
                        xwos_rtwq_unlock_cpuirqrs(&smr->wq.rt, cpuirq);
                        rc = -ENEGATIVE;
                } else {
                        wqn = xwos_rtwq_choose_locked(&smr->wq.rt);
                        if (wqn) {
                                wqn->wq = NULL;
                                wqn->type = XWOS_WQTYPE_UNKNOWN;
                                xwaop_store(xwsq_t, &wqn->rsmrs,
                                            xwmb_modr_release, XWOS_WQN_RSMRS_UP);
                                cb = wqn->cb;
                                wqn->cb = NULL;
                                xwlk_splk_unlock(&wqn->lock);
                                xwos_rtwq_unlock_cpuirqrs(&smr->wq.rt, cpuirq);
                                xwsync_smr_put(smr);
                                cb(wqn->owner);
                        } else {
                                /* add semaphore counter */
                                if (smr->count < smr->max) {
                                        smr->count++;
                                } else {
                                        rc = -ERANGE;
                                }
#if defined(XWSMPCFG_SYNC_EVT) && (1 == XWSMPCFG_SYNC_EVT)
                                if (smr->count > 0) {
                                        struct xwsync_evt * evt;
                                        struct xwsync_object * xwsyncobj;

                                        xwsyncobj = &smr->xwsyncobj;
                                        xwmb_smp_load_acquire(struct xwsync_evt *,
                                                              evt,
                                                              &xwsyncobj->selector.evt);
                                        if (NULL != evt) {
                                                xwsync_evt_obj_s1i(evt,
                                                                   xwsyncobj);
                                        }
                                }
#endif /* XWSMPCFG_SYNC_EVT */
                                xwos_rtwq_unlock_cpuirqrs(&smr->wq.rt, cpuirq);
                                xwsync_smr_put(smr);
                        }
                }
        }/* else {} */
        return rc;
}

/**
 * @brief XWOS API：尝试获取实时信号量
 * @param smr: (I) 信号量对象的指针
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 类型不匹配
 * @retval -ENODATA: 信号量资源不可用（信号量不可被获取）
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数可在中断上下文中使用。
 */
__xwos_api
xwer_t xwsync_rtsmr_trywait(struct xwsync_smr * smr)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((smr), "nullptr", -EFAULT);
        XWOS_VALIDATE((XWSYNC_SMR_TYPE_RT == smr->type), "type-error", -ETYPE);

        rc = xwsync_smr_grab(smr);
        if (__likely(OK == rc)) {
                xwos_rtwq_lock_cpuirqsv(&smr->wq.rt, &cpuirq);
                if (smr->count > 0) {
                        smr->count--;
#if defined(XWSMPCFG_SYNC_EVT) && (1 == XWSMPCFG_SYNC_EVT)
                        if (0 == smr->count) {
                                struct xwsync_evt * evt;
                                struct xwsync_object * xwsyncobj;

                                xwsyncobj = &smr->xwsyncobj;
                                xwmb_smp_load_acquire(struct xwsync_evt *,
                                                      evt,
                                                      &xwsyncobj->selector.evt);
                                if (NULL != evt) {
                                        xwsync_evt_obj_c0i(evt, xwsyncobj);
                                }
                        }
#endif /* XWSMPCFG_SYNC_EVT */
                } else {
                        rc = -ENODATA;
                }
                xwos_rtwq_unlock_cpuirqrs(&smr->wq.rt, cpuirq);
                xwsync_smr_put(smr);
        }/* else {} */
        return rc;
}

static __xwos_code
xwer_t xwsync_rtsmr_do_timedblkthrd_unlkwq_cpuirqrs(struct xwsync_smr * smr,
                                                    struct xwos_tcb * tcb,
                                                    xwtm_t * xwtm,
                                                    xwreg_t cpuirq)
{
        xwer_t rc;
        xwtm_t expected, currtick;
        struct xwos_scheduler * xwsd;
        struct xwos_tt * xwtt;
        struct xwos_syshwt * hwt;
        xwsq_t rsmrs;
        xwsq_t wkuprs;
        xwpr_t dprio;

        xwmb_smp_load_acquire(struct xwos_scheduler *, xwsd, &tcb->xwsd);
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
                xwos_rtwq_unlock_cpuirqrs(&smr->wq.rt, cpuirq);
                rc = -EINTR;
                goto err_needfrz;
        }
        dprio = tcb->dprio.r;
        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_RUNNING);
        tcb->dprio.r = XWOS_SD_PRIORITY_INVALID;
        tcb->dprio.wq = dprio;
        xwbop_s1m(xwsq_t, &tcb->state, XWSDOBJ_DST_BLOCKING);
        xwlk_splk_unlock(&tcb->stlock);
        xwos_thrd_eq_rtwq_locked(tcb, &smr->wq.rt, XWOS_WQTYPE_RTSMR, dprio);
        xwos_rtwq_unlock_cpuirqrs(&smr->wq.rt, cpuirq);

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
        rsmrs = xwaop_load(xwsq_t, &tcb->wqn.rsmrs, xwmb_modr_relaxed);
        wkuprs = xwaop_load(xwsq_t, &tcb->ttn.wkuprs, xwmb_modr_relaxed);
        if (XWOS_WQN_RSMRS_INTR == rsmrs) {
                xwlk_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwos_tt_remove_locked(xwtt, &tcb->ttn);
                if (OK == rc) {
                        xwlk_splk_lock(&tcb->stlock);
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_SLEEPING);
                        xwlk_splk_unlock(&tcb->stlock);
                }/* else {} */
                xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = -EINTR;
        } else if (XWOS_WQN_RSMRS_UP == rsmrs) {
                xwlk_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwos_tt_remove_locked(xwtt, &tcb->ttn);
                if (OK == rc) {
                        xwlk_splk_lock(&tcb->stlock);
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_SLEEPING);
                        xwlk_splk_unlock(&tcb->stlock);
                }/* else {} */
                xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = OK;
        } else if (XWOS_TTN_WKUPRS_TIMEDOUT == wkuprs) {
                xwos_rtwq_lock_cpuirq(&smr->wq.rt);
                xwlk_splk_lock(&tcb->wqn.lock);
                rc = xwos_rtwq_remove_locked(&smr->wq.rt, &tcb->wqn);
                if (OK == rc) {
                        tcb->wqn.wq = NULL;
                        tcb->wqn.type = XWOS_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq_t, &tcb->wqn.rsmrs,
                                    xwmb_modr_release, XWOS_WQN_RSMRS_INTR);
                        tcb->wqn.cb = NULL;
                        xwlk_splk_unlock(&tcb->wqn.lock);
                        xwlk_splk_lock(&tcb->stlock);
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_BLOCKING);
                        tcb->dprio.wq = XWOS_SD_PRIORITY_INVALID;
                        xwlk_splk_unlock(&tcb->stlock);
                        xwos_rtwq_unlock_cpuirqrs(&smr->wq.rt, cpuirq);
                        rc = -ETIMEDOUT;
                } else {
                        xwlk_splk_unlock(&tcb->wqn.lock);
                        xwos_rtwq_unlock_cpuirqrs(&smr->wq.rt, cpuirq);
                        rsmrs = xwaop_load(xwsq_t, &tcb->wqn.rsmrs, xwmb_modr_relaxed);
                        if (XWOS_WQN_RSMRS_INTR == rsmrs) {
                                rc = -EINTR;
                        } else if (XWOS_WQN_RSMRS_UP == rsmrs) {
                                rc = OK;
                        } else {
                                XWOS_BUG();
                                rc = -EBUG;
                        }
                }
        } else if (XWOS_TTN_WKUPRS_INTR == wkuprs) {
                xwos_rtwq_lock_cpuirq(&smr->wq.rt);
                xwlk_splk_lock(&tcb->wqn.lock);
                rc = xwos_rtwq_remove_locked(&smr->wq.rt, &tcb->wqn);
                if (OK == rc) {
                        tcb->wqn.wq = NULL;
                        tcb->wqn.type = XWOS_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq_t, &tcb->wqn.rsmrs,
                                    xwmb_modr_release, XWOS_WQN_RSMRS_INTR);
                        tcb->wqn.cb = NULL;
                        xwlk_splk_unlock(&tcb->wqn.lock);
                        xwlk_splk_lock(&tcb->stlock);
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_BLOCKING);
                        tcb->dprio.wq = XWOS_SD_PRIORITY_INVALID;
                        xwlk_splk_unlock(&tcb->stlock);
                        xwos_rtwq_unlock_cpuirqrs(&smr->wq.rt, cpuirq);
                        rc = -EINTR;
                } else {
                        xwlk_splk_unlock(&tcb->wqn.lock);
                        xwos_rtwq_unlock_cpuirqrs(&smr->wq.rt, cpuirq);
                        rsmrs = xwaop_load(xwsq_t, &tcb->wqn.rsmrs, xwmb_modr_relaxed);
                        if (XWOS_WQN_RSMRS_INTR == rsmrs) {
                                rc = -EINTR;
                        } else if (XWOS_WQN_RSMRS_UP == rsmrs) {
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

static __xwos_code
xwer_t xwsync_rtsmr_do_timedwait(struct xwsync_smr * smr, struct xwos_tcb * tcb,
                                 xwtm_t * xwtm)
{
        struct xwos_scheduler * xwsd;
        xwreg_t cpuirq;
        xwer_t rc;

        xwmb_smp_load_acquire(struct xwos_scheduler *, xwsd, &tcb->xwsd);
        xwos_rtwq_lock_cpuirqsv(&smr->wq.rt, &cpuirq);
        if (smr->count <= 0) {
                rc = xwos_scheduler_wakelock_lock(xwsd);
                if (__unlikely(rc < 0)) {
                        /* 当前调度器正准备休眠，线程需被冻结，返回-EINTR。*/
                        xwos_rtwq_unlock_cpuirqrs(&smr->wq.rt, cpuirq);
                        rc = -EINTR;
                } else {
                        rc = xwsync_rtsmr_do_timedblkthrd_unlkwq_cpuirqrs(smr,
                                                                          tcb,
                                                                          xwtm,
                                                                          cpuirq);
                        xwos_scheduler_wakelock_unlock(xwsd);
                }
        } else {
                smr->count--;
#if defined(XWSMPCFG_SYNC_EVT) && (1 == XWSMPCFG_SYNC_EVT)
                if (0 == smr->count) {
                        struct xwsync_evt * evt;
                        struct xwsync_object * xwsyncobj;

                        xwsyncobj = &smr->xwsyncobj;
                        xwmb_smp_load_acquire(struct xwsync_evt *,
                                              evt, &xwsyncobj->selector.evt);
                        if (NULL != evt) {
                                xwsync_evt_obj_c0i(evt, xwsyncobj);
                        }
                }
#endif /* XWSMPCFG_SYNC_EVT */
                xwos_rtwq_unlock_cpuirqrs(&smr->wq.rt, cpuirq);
                rc = OK;
        }

        return rc;
}

/**
 * @brief XWOS API：限时等待获取信号量
 * @param smr: (I) 信号量对象的指针
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 类型错误
 * @retval -ETIMEDOUT: 超时
 * @retval -EINTR: 等待被中断
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 函数返回返回-ETIMEDOUT时，*xwtm* 指向的缓冲区内的期望时间会减为0。
 */
__xwos_api
xwer_t xwsync_rtsmr_timedwait(struct xwsync_smr * smr, xwtm_t * xwtm)
{
        struct xwos_tcb * ctcb;
        xwer_t rc;

        XWOS_VALIDATE((smr), "nullptr", -EFAULT);
        XWOS_VALIDATE((XWSYNC_SMR_TYPE_RT == smr->type), "type-error", -ETYPE);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm_cmp(*xwtm, 0) >= 0), "out-of-time", -ETIMEDOUT);
        XWOS_VALIDATE((-EINTHRD == xwos_irq_get_id(NULL)),
                      "not-in-thrd", -ENOTINTHRD);

        if (__unlikely(0 == xwtm_cmp(*xwtm, 0))) {
                rc = xwsync_rtsmr_trywait(smr);
                if (__unlikely(rc < 0)) {
                        if (__likely(-ENODATA == rc))
                                rc = -ETIMEDOUT;
                        /* else {} */
                }/* else {} */
        } else {
                ctcb = xwos_scheduler_get_ctcb_lc();
                rc = xwsync_smr_grab(smr);
                if (__likely(OK == rc)) {
                        rc = xwsync_rtsmr_do_timedwait(smr, ctcb, xwtm);
                        xwsync_smr_put(smr);
                }/* else {} */
        }
        return rc;
}

static __xwos_code
xwer_t xwsync_rtsmr_do_blkthrd_unlkwq_cpuirqrs(struct xwsync_smr * smr,
                                               struct xwos_tcb * tcb,
                                               xwreg_t cpuirq)
{
        xwer_t rc;
        struct xwos_scheduler * xwsd;
        xwsq_t rsmrs;
        xwpr_t dprio;

        xwmb_smp_load_acquire(struct xwos_scheduler *, xwsd, &tcb->xwsd);
        /* 加入等待队列 */
        xwlk_splk_lock(&tcb->stlock);
        XWOS_BUG_ON((XWSDOBJ_DST_BLOCKING | XWSDOBJ_DST_SLEEPING |
                     XWSDOBJ_DST_READY | XWSDOBJ_DST_STANDBY |
                     XWSDOBJ_DST_FROZEN | XWSDOBJ_DST_MIGRATING)
                    & tcb->state);
        dprio = tcb->dprio.r;
        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_RUNNING);
        tcb->dprio.r = XWOS_SD_PRIORITY_INVALID;
        tcb->dprio.wq = dprio;
        xwbop_s1m(xwsq_t, &tcb->state,
                  XWSDOBJ_DST_BLOCKING | XWSDOBJ_DST_UNINTERRUPTED);
        xwlk_splk_unlock(&tcb->stlock);
        xwos_thrd_eq_rtwq_locked(tcb, &smr->wq.rt, XWOS_WQTYPE_RTSMR, dprio);
        xwos_rtwq_unlock_cpuirqrs(&smr->wq.rt, cpuirq);

        /* 调度 */
        xwos_cpuirq_enable_lc();
        xwos_scheduler_req_swcx(xwsd);
        xwos_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        xwmb_smp_rmb();
        /* 确保对唤醒原因的读取操作发生在线程状态切换之后 */
        rsmrs = xwaop_load(xwsq_t, &tcb->wqn.rsmrs, xwmb_modr_relaxed);
        if (XWOS_WQN_RSMRS_UP == rsmrs) {
                rc = OK;
        } else {
                XWOS_BUG();
                rc = -EBUG;
        }
        return rc;
}

static __xwos_code
xwer_t xwsync_rtsmr_do_wait_unintr(struct xwsync_smr * smr, struct xwos_tcb * tcb)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwos_rtwq_lock_cpuirqsv(&smr->wq.rt, &cpuirq);
        if (smr->count <= 0) {
                rc = xwsync_rtsmr_do_blkthrd_unlkwq_cpuirqrs(smr, tcb, cpuirq);
        } else {
                smr->count--;
#if defined(XWSMPCFG_SYNC_EVT) && (1 == XWSMPCFG_SYNC_EVT)
                if (0 == smr->count) {
                        struct xwsync_evt * evt;
                        struct xwsync_object * xwsyncobj;

                        xwsyncobj = &smr->xwsyncobj;
                        xwmb_smp_load_acquire(struct xwsync_evt *,
                                              evt, &xwsyncobj->selector.evt);
                        if (NULL != evt) {
                                xwsync_evt_obj_c0i(evt, xwsyncobj);
                        }
                }
#endif /* XWSMPCFG_SYNC_EVT */
                xwos_rtwq_unlock_cpuirqrs(&smr->wq.rt, cpuirq);
                rc = OK;
        }
        return rc;
}

/**
 * @brief XWOS API：等待并获取实时信号量，且等待不可被中断
 * @param smr: (I) 信号量对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 类型错误
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwsync_rtsmr_wait_unintr(struct xwsync_smr * smr)
{
        struct xwos_tcb * ctcb;
        xwer_t rc;

        XWOS_VALIDATE((smr), "nullptr", -EFAULT);
        XWOS_VALIDATE((XWSYNC_SMR_TYPE_RT == smr->type), "type-error", -ETYPE);

        rc = xwsync_smr_grab(smr);
        if (__likely(OK == rc)) {
                ctcb = xwos_scheduler_get_ctcb_lc();
                rc = xwsync_rtsmr_do_wait_unintr(smr, ctcb);
                xwsync_smr_put(smr);
        }/* else {} */
        return rc;
}
#endif /* XWSMPCFG_SYNC_RTSMR */

/**
 * @brief XWOS API：获取信号量计数器的值
 * @param smr: (I) 信号量对象的指针
 * @param sval: (O) 指向缓冲区的指针，通过此缓冲区返回信号量计数器的值
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwsync_smr_getvalue(struct xwsync_smr * smr, xwssq_t * sval)
{
        XWOS_VALIDATE((smr), "nullptr", -EFAULT);
        XWOS_VALIDATE((sval), "nullptr", -EFAULT);

        *sval = smr->count;
        return OK;
}

/**
 * @brief XWOS API：获取信号量的类型
 * @param smr: (I) 信号量对象的指针
 * @param type: (O) 指向缓冲区的指针，通过此缓冲区返回类型值
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwsync_smr_gettype(struct xwsync_smr * smr, xwid_t * type)
{
        XWOS_VALIDATE((smr), "nullptr", -EFAULT);
        XWOS_VALIDATE((type), "nullptr", -EFAULT);

        *type = smr->type;
        return OK;
}
