/**
 * @file
 * @brief 玄武OS MP内核同步机制：信号量
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * @note
 * - 锁的顺序：同级的锁不可同时获得
 *   + ① 等待队列的锁（rtwq->lock & plwq->lock）
 *     + ② 等待队列节点的锁（wqn->lock）
 *     + ② 线程控制块的状态锁（thd->stlock）
 *     + ② 事件对象的锁（evt->lock）
 */

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#if defined(XWMPCFG_SYNC_SEM_MEMSLICE) && (1 == XWMPCFG_SYNC_SEM_MEMSLICE)
  #include <xwos/mm/memslice.h>
#endif /* XWMPCFG_SYNC_SEM_MEMSLICE */
#include <xwos/ospl/irq.h>
#include <xwos/mp/skd.h>
#include <xwos/mp/tt.h>
#include <xwos/mp/thd.h>
#include <xwos/mp/rtwq.h>
#include <xwos/mp/plwq.h>
#include <xwos/mp/lock/spinlock.h>
#include <xwos/mp/lock/seqlock.h>
#include <xwos/mp/lock/mtx.h>
#if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
  #include <xwos/mp/sync/evt.h>
#endif /* XWMPCFG_SYNC_EVT */
#include <xwos/mp/sync/obj.h>
#include <xwos/mp/sync/sem.h>

#if defined(XWMPCFG_SYNC_SEM_MEMSLICE) && (1 == XWMPCFG_SYNC_SEM_MEMSLICE)
/**
 * @brief 结构体xwmp_sem的对象缓存
 */
static __xwmp_data struct xwmm_memslice * xwmp_sem_cache = NULL;

/**
 * @brief 结构体xwmp_sem的对象缓存的名字
 */
__xwmp_rodata const char xwmp_sem_cache_name[] = "xwos.mp.sync.sem.cache";
#endif /* XWMPCFG_SYNC_SEM_MEMSLICE */

static __xwmp_code
struct xwmp_sem * xwmp_sem_alloc(void);

static __xwmp_code
void xwmp_sem_free(struct xwmp_sem * sem);

static __xwmp_code
void xwmp_sem_construct(struct xwmp_sem * sem);

static __xwmp_code
void xwmp_sem_destruct(struct xwmp_sem * sem);

static __xwmp_code
xwer_t xwmp_sem_gc(void * sem);

#if defined(XWMPCFG_SYNC_PLSEM) && (1 == XWMPCFG_SYNC_PLSEM)
static __xwmp_code
xwer_t xwmp_plsem_activate(struct xwmp_sem * sem, xwssq_t val, xwssq_t max,
                           xwobj_gc_f gcfunc);

static __xwmp_code
xwer_t xwmp_plsem_do_timedblkthd_unlkwq_cpuirqrs(struct xwmp_sem * sem,
                                                 struct xwmp_thd * thd,
                                                 xwtm_t * xwtm,
                                                 xwreg_t cpuirq);

static __xwmp_code
xwer_t xwmp_plsem_do_timedwait(struct xwmp_sem * sem, struct xwmp_thd * thd,
                               xwtm_t * xwtm);

static __xwmp_code
xwer_t xwmp_plsem_do_blkthd_unlkwq_cpuirqrs(struct xwmp_sem * sem,
                                            struct xwmp_thd * thd,
                                            xwreg_t cpuirq);

static __xwmp_code
xwer_t xwmp_plsem_do_wait_unintr(struct xwmp_sem * sem, struct xwmp_thd * thd);
#endif /* XWMPCFG_SYNC_PLSEM */

#if defined(XWMPCFG_SYNC_RTSEM) && (1 == XWMPCFG_SYNC_RTSEM)
static __xwmp_code
xwer_t xwmp_rtsem_activate(struct xwmp_sem * sem, xwssq_t val, xwssq_t max,
                           xwobj_gc_f gcfunc);

static __xwmp_code
xwer_t xwmp_rtsem_do_timedblkthd_unlkwq_cpuirqrs(struct xwmp_sem * sem,
                                                 struct xwmp_thd * thd,
                                                 xwtm_t * xwtm,
                                                 xwreg_t cpuirq);

static __xwmp_code
xwer_t xwmp_rtsem_do_timedwait(struct xwmp_sem * sem, struct xwmp_thd * thd,
                               xwtm_t * xwtm);

static __xwmp_code
xwer_t xwmp_rtsem_do_blkthd_unlkwq_cpuirqrs(struct xwmp_sem * sem,
                                            struct xwmp_thd * thd,
                                            xwreg_t cpuirq);

static __xwmp_code
xwer_t xwmp_rtsem_do_wait_unintr(struct xwmp_sem * sem, struct xwmp_thd * thd);
#endif /* XWMPCFG_SYNC_RTSEM */

#if defined(XWMPCFG_SYNC_SEM_MEMSLICE) && (1 == XWMPCFG_SYNC_SEM_MEMSLICE)
/**
 * @brief XWMP INIT CODE：初始化结构体xwmp_sem的对象缓存
 * @param zone_origin: (I) 内存区域的首地址
 * @param zone_size: (I) 内存区域的大小
 * @return 错误码
 * @note
 * - 重入性：只可在系统初始化时使用一次
 */
__xwmp_init_code
xwer_t xwmp_sem_cache_init(xwptr_t zone_origin, xwsz_t zone_size)
{
        struct xwmm_memslice * msa;
        xwer_t rc;

        rc = xwmm_memslice_create(&msa, zone_origin, zone_size,
                                  sizeof(struct xwmp_sem),
                                  xwmp_sem_cache_name,
                                  (ctor_f)xwmp_sem_construct,
                                  (dtor_f)xwmp_sem_destruct);
        if (__xwcc_unlikely(rc < 0)) {
                xwmp_sem_cache = NULL;
        } else {
                xwmp_sem_cache = msa;
        }
        return rc;
}
#endif /* XWMPCFG_SYNC_SEM_MEMSLICE */

/**
 * @brief 从信号量对象缓存中申请对象
 * @return 信号量对象的指针
 */
static __xwmp_code
struct xwmp_sem * xwmp_sem_alloc(void)
{
#if defined(XWMPCFG_SYNC_SEM_MEMSLICE) && (1 == XWMPCFG_SYNC_SEM_MEMSLICE)
        union {
                struct xwmp_sem * sem;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_memslice_alloc(xwmp_sem_cache, &mem.anon);
        if (rc < 0) {
                mem.sem = err_ptr(rc);
        }/* else {} */
        return mem.sem;
#else /* XWMPCFG_SYNC_SEM_MEMSLICE */
        union {
                struct xwmp_sem * sem;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwmp_sem), XWMM_ALIGNMENT, &mem.anon);
        if (XWOK == rc) {
                xwmp_sem_construct(mem.sem);
        } else {
                mem.sem = err_ptr(-ENOMEM);
        }
        return mem.sem;
#endif /* !XWMPCFG_SYNC_SEM_MEMSLICE */
}

/**
 * @brief 释放信号量对象
 * @param sem: (I) 信号量对象的指针
 */
static __xwmp_code
void xwmp_sem_free(struct xwmp_sem * sem)
{
#if defined(XWMPCFG_SYNC_SEM_MEMSLICE) && (1 == XWMPCFG_SYNC_SEM_MEMSLICE)
        xwmm_memslice_free(xwmp_sem_cache, sem);
#else /* XWMPCFG_SYNC_SEM_MEMSLICE */
        xwmp_sem_destruct(sem);
        xwmm_kma_free(sem);
#endif /* !XWMPCFG_SYNC_SEM_MEMSLICE */
}

/**
 * @brief 信号量对象的构造函数
 * @param sem: (I) 信号量对象的指针
 */
static __xwmp_code
void xwmp_sem_construct(struct xwmp_sem * sem)
{
        xwmp_synobj_construct(&sem->synobj);
}

/**
 * @brief 信号量对象的析构函数
 * @param sem: (I) 信号量对象的指针
 */
static __xwmp_code
void xwmp_sem_destruct(struct xwmp_sem * sem)
{
        xwmp_synobj_destruct(&sem->synobj);
}

/**
 * @brief 信号量对象的垃圾回收函数
 * @param sem: (I) 信号量对象的指针
 * @return 错误码
 */
static __xwmp_code
xwer_t xwmp_sem_gc(void * sem)
{
        xwmp_sem_free((struct xwmp_sem *)sem);
        return XWOK;
}

/**
 * @brief XWMP API：检查信号量对象的标签并增加引用计数
 * @param sem: (I) 信号量对象指针
 * @param tik: (I) 标签
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmp_api
xwer_t xwmp_sem_acquire(struct xwmp_sem * sem, xwsq_t tik)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        return xwmp_synobj_acquire(&sem->synobj, tik);
}

/**
 * @brief XWMP API：检查信号量对象的标签并增加引用计数
 * @param sem: (I) 信号量对象指针
 * @param tik: (I) 标签
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmp_api
xwer_t xwmp_sem_release(struct xwmp_sem * sem, xwsq_t tik)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        return xwmp_synobj_release(&sem->synobj, tik);
}

/**
 * @brief XWMP API：增加信号量对象的引用计数
 * @param sem: (I) 信号量对象指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmp_api
xwer_t xwmp_sem_grab(struct xwmp_sem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        return xwmp_synobj_grab(&sem->synobj);
}

/**
 * @brief XWMP API：减少信号量对象的引用计数
 * @param sem: (I) 信号量对象指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmp_api
xwer_t xwmp_sem_put(struct xwmp_sem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        return xwmp_synobj_put(&sem->synobj);
}

/**
 * @brief XWMP API：动态创建信号量
 * @param ptrbuf: (O) 指向缓冲区的指针，此缓冲区用于返回对象的指针
 * @param type: (I) 信号量的类型，取值范围 @ref xwmp_sem_type_em
 * @param val: (I) 信号量的初始值
 * @param max: (I) 信号量的最大值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 无效参数
 * @retval -ETYPE: 类型错误
 * @retval -ENOMEM: 内存不足
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmp_api
xwer_t xwmp_sem_create(struct xwmp_sem ** ptrbuf, xwid_t type,
                       xwssq_t val, xwssq_t max)
{
        struct xwmp_sem * sem;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);

        *ptrbuf = NULL;
        sem = xwmp_sem_alloc();
        if (__xwcc_unlikely(is_err(sem))) {
                rc = ptr_err(sem);
        } else {
                rc = -ETYPE;
                switch (type) {
#if defined(XWMPCFG_SYNC_PLSEM) && (1 == XWMPCFG_SYNC_PLSEM)
                case XWMP_SEM_TYPE_PIPELINE:
                        rc = xwmp_plsem_activate(sem, val, max, xwmp_sem_gc);
                        break;
#endif /* XWMPCFG_SYNC_PLSEM */
#if defined(XWMPCFG_SYNC_RTSEM) && (1 == XWMPCFG_SYNC_RTSEM)
                case XWMP_SEM_TYPE_RT:
                        rc = xwmp_rtsem_activate(sem, val, max, xwmp_sem_gc);
                        break;
#endif /* XWMPCFG_SYNC_RTSEM */
                }
                if (__xwcc_unlikely(rc < 0)) {
                        xwmp_sem_free(sem);
                } else {
                        *ptrbuf = sem;
                }
        }
        return rc;
}

/**
 * @brief XWMP API：删除动态创建的信号量
 * @param sem: (I) 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个信号量对象，不可重入
 */
__xwmp_api
xwer_t xwmp_sem_delete(struct xwmp_sem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        return xwmp_sem_put(sem);
}

/**
 * @brief XWMP API：销毁静态方式初始化的信号量对象
 * @param sem: (I) 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个信号量对象，不可重入
 */
__xwmp_api
xwer_t xwmp_sem_destroy(struct xwmp_sem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        return xwmp_sem_put(sem);
}

#if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
/**
 * @brief XWMP API：绑定信号量到事件对象，事件对象类型为XWMP_EVT_TYPE_SEL
 * @param sem: (I) 信号量对象的指针
 * @param evt: (I) 事件对象的指针
 * @param pos: (I) 信号量对象映射到位图中的位置
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ETYPE: 事件对象类型错误
 * @retval -EFAULT: 空指针
 * @retval -ECHRNG: 位置超出范围
 * @retval -EALREADY: 同步对象已经绑定到事件对象
 * @retval -EBUSY: 通道已经被其他同步对象独占
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个信号量对象，不可重入
 */
__xwmp_api
xwer_t xwmp_sem_bind(struct xwmp_sem * sem, struct xwmp_evt * evt, xwsq_t pos)
{
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);

        switch (sem->type) {
#if defined(XWMPCFG_SYNC_PLSEM) && (1 == XWMPCFG_SYNC_PLSEM)
        case XWMP_SEM_TYPE_PIPELINE:
                xwmp_plwq_lock_cpuirqsv(&sem->wq.pl, &cpuirq);
                rc = xwmp_evt_obj_bind(evt, &sem->synobj, pos, true);
                if ((XWOK == rc) && (sem->count > 0)) {
                        rc = xwmp_evt_obj_s1i(evt, &sem->synobj);
                }
                xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                break;
#endif /* XWMPCFG_SYNC_PLSEM */
#if defined(XWMPCFG_SYNC_RTSEM) && (1 == XWMPCFG_SYNC_RTSEM)
        case XWMP_SEM_TYPE_RT:
                xwmp_rtwq_lock_cpuirqsv(&sem->wq.rt, &cpuirq);
                rc = xwmp_evt_obj_bind(evt, &sem->synobj, pos, true);
                if ((XWOK == rc) && (sem->count > 0)) {
                        rc = xwmp_evt_obj_s1i(evt, &sem->synobj);
                }
                xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                break;
#endif /* XWMPCFG_SYNC_RTSEM */
        default:
                rc = -ETYPE;
                break;
        }
        return rc;
}

/**
 * @brief XWMP API：从事件对象上解绑信号量，事件对象类型为XWMP_EVT_TYPE_SEL
 * @param sem: (I) 信号量对象的指针
 * @param evt: (I) 事件对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ETYPE: 事件对象类型错误
 * @retval -EFAULT: 空指针
 * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个信号量对象，不可重入
 */
__xwmp_api
xwer_t xwmp_sem_unbind(struct xwmp_sem * sem, struct xwmp_evt * evt)
{
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        switch (sem->type) {
#if defined(XWMPCFG_SYNC_PLSEM) && (1 == XWMPCFG_SYNC_PLSEM)
        case XWMP_SEM_TYPE_PIPELINE:
                xwmp_plwq_lock_cpuirqsv(&sem->wq.pl, &cpuirq);
                rc = xwmp_evt_obj_unbind(evt, &sem->synobj, true);
                if (XWOK == rc) {
                        rc = xwmp_evt_obj_c0i(evt, &sem->synobj);
                }
                xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                break;
#endif /* XWMPCFG_SYNC_PLSEM */
#if defined(XWMPCFG_SYNC_RTSEM) && (1 == XWMPCFG_SYNC_RTSEM)
        case XWMP_SEM_TYPE_RT:
                xwmp_rtwq_lock_cpuirqsv(&sem->wq.rt, &cpuirq);
                rc = xwmp_evt_obj_unbind(evt, &sem->synobj, true);
                if (XWOK == rc) {
                        rc = xwmp_evt_obj_c0i(evt, &sem->synobj);
                }
                xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                break;
#endif /* XWMPCFG_SYNC_RTSEM */
        default:
                rc = -ETYPE;
                break;
        }
        return rc;
}
#endif /* XWMPCFG_SYNC_EVT */

#if defined(XWMPCFG_SYNC_PLSEM) && (1 == XWMPCFG_SYNC_PLSEM)
/**
 * @brief XWMP API：激活并初始化管道信号量对象
 * @param sem: (I) 信号量对象的指针
 * @param val: (I) 信号量的初始值
 * @param max: (I) 信号量的最大值
 * @param gcfunc: (I) 垃圾回收函数的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 无效参数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个信号量对象，不可重入
 */
static __xwmp_code
xwer_t xwmp_plsem_activate(struct xwmp_sem * sem, xwssq_t val, xwssq_t max,
                           xwobj_gc_f gcfunc)
{
        xwer_t rc;

        XWOS_VALIDATE(((val >= 0) && (max > 0) && (val <= max)),
                      "invalid-value", -EINVAL);

        rc = xwmp_synobj_activate(&sem->synobj, gcfunc);
        if (__xwcc_likely(XWOK == rc)) {
                sem->max = max;
                xwmp_plwq_init(&sem->wq.pl);
                sem->type = XWMP_SEM_TYPE_PIPELINE;
                sem->count = val;
        }/* else {} */
        return rc;
}

/**
 * @brief XWMP API：静态初始化管道信号量对象
 * @param sem: (I) 信号量对象的指针
 * @param val: (I) 信号量的初始值
 * @param max: (I) 信号量的最大值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 无效参数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个信号量对象，不可重入
 */
__xwmp_api
xwer_t xwmp_plsem_init(struct xwmp_sem * sem, xwssq_t val, xwssq_t max)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);

        xwmp_sem_construct(sem);
        return xwmp_plsem_activate(sem, val, max, NULL);
}

/**
 * @brief XWMP API：冻结管道信号量（值设置为负）
 * @param sem: (I) 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 类型不匹配
 * @retval -EALREADY: 信号量已被冻结
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 已冻结的信号量不允许增加(V操作)，但可以被测试(P操作)，
 *   测试信号量的线程会被加入到信号量的等待队列。
 */
__xwmp_api
xwer_t xwmp_plsem_freeze(struct xwmp_sem * sem)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((XWMP_SEM_TYPE_PIPELINE == sem->type), "type-error", -ETYPE);

        rc = xwmp_sem_grab(sem);
        if (__xwcc_likely(XWOK == rc)) {
                xwmp_plwq_lock_cpuirqsv(&sem->wq.pl, &cpuirq);
                if (__xwcc_unlikely(sem->count < 0)) {
                        rc = -EALREADY;
                } else {
                        sem->count = XWMP_SEM_NEGTIVE;
#if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
                        struct xwmp_evt * evt;
                        struct xwmp_synobj * synobj;

                        synobj = &sem->synobj;
                        xwmb_mp_load_acquire(struct xwmp_evt *,
                                             evt, &synobj->sel.evt);
                        if (NULL != evt) {
                                xwmp_evt_obj_c0i(evt, synobj);
                        }
#endif /* XWMPCFG_SYNC_EVT */
                }
                xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                xwmp_sem_put(sem);
        }/* else {} */
        return rc;
}

/**
 * @brief XWMP API：解冻管道信号量
 * @param sem: (I) 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 类型不匹配
 * @retval -EALREADY: 信号量未被冻结
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对已冻结的信号量起作用，对未冻结的信号量调用此函数将返回错误码。
 */
__xwmp_api
xwer_t xwmp_plsem_thaw(struct xwmp_sem * sem)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((XWMP_SEM_TYPE_PIPELINE == sem->type), "type-error", -ETYPE);

        rc = xwmp_sem_grab(sem);
        if (__xwcc_likely(XWOK == rc)) {
                xwmp_plwq_lock_cpuirqsv(&sem->wq.pl, &cpuirq);
                if (__xwcc_unlikely(sem->count >= 0)) {
                        rc = -EALREADY;
                } else {
                        sem->type = XWMP_SEM_TYPE_PIPELINE;
                        sem->count = 0;
                        rc = XWOK;
                }
                xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                xwmp_sem_put(sem);
        }/* else {} */
        return rc;
}

/**
 * @brief 中断管道信号量等待队列中的一个节点
 * @param sem: (I) 信号量对象的指针
 * @param wqn: (I) 等待队列节点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 类型不匹配
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmp_code
xwer_t xwmp_plsem_intr(struct xwmp_sem * sem, struct xwmp_wqn * wqn)
{
        xwmp_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwmp_sem_grab(sem);
        if (__xwcc_likely(XWOK == rc)) {
                xwmp_plwq_lock_cpuirqsv(&sem->wq.pl, &cpuirq);
                xwmp_splk_lock(&wqn->lock);
                rc = xwmp_plwq_remove_locked(&sem->wq.pl, wqn);
                if (XWOK == rc) {
                        wqn->wq = NULL;
                        wqn->type = XWMP_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq, &wqn->reason,
                                    xwmb_modr_release, XWMP_WQN_REASON_INTR);
                        cb = wqn->cb;
                        wqn->cb = NULL;
                        xwmp_splk_unlock(&wqn->lock);
                        xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                        xwmp_sem_put(sem);
                        cb(wqn->owner);
                } else {
                        xwmp_splk_unlock(&wqn->lock);
                        xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                        xwmp_sem_put(sem);
                }
        }/* else {} */
        return rc;
}

/**
 * @brief XWMP API：发布管道信号量
 * @param sem: (I) 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 类型不匹配
 * @retval -ENEGATIVE: 信号量已被冻结
 * @retval -ERANGE: 信号量的值已经最大
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对未冻结的信号量起作用，已冻结的信号量将得到错误码-ENEGATIVE。
 */
__xwmp_api
xwer_t xwmp_plsem_post(struct xwmp_sem * sem)
{
        struct xwmp_wqn * wqn;
        xwmp_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((XWMP_SEM_TYPE_PIPELINE == sem->type), "type-error", -ETYPE);

        rc = xwmp_sem_grab(sem);
        if (__xwcc_likely(XWOK == rc)) {
                xwmp_plwq_lock_cpuirqsv(&sem->wq.pl, &cpuirq);
                if (sem->count < 0) {
                        xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                        xwmp_sem_put(sem);
                        rc = -ENEGATIVE;
                } else {
                        wqn = xwmp_plwq_choose_locked(&sem->wq.pl);
                        if (wqn) {
                                wqn->wq = NULL;
                                wqn->type = XWMP_WQTYPE_UNKNOWN;
                                xwaop_store(xwsq, &wqn->reason,
                                            xwmb_modr_release, XWMP_WQN_REASON_UP);
                                cb = wqn->cb;
                                wqn->cb = NULL;
                                xwmp_splk_unlock(&wqn->lock);
                                xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                                xwmp_sem_put(sem);
                                cb(wqn->owner);
                        } else {
                                if (sem->count < sem->max) {
                                        sem->count++;
                                } else {
                                        rc = -ERANGE;
                                }
#if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
                                if (sem->count > 0) {
                                        struct xwmp_evt * evt;
                                        struct xwmp_synobj * synobj;

                                        synobj = &sem->synobj;
                                        xwmb_mp_load_acquire(struct xwmp_evt *,
                                                             evt,
                                                             &synobj->sel.evt);
                                        if (NULL != evt) {
                                                xwmp_evt_obj_s1i(evt, synobj);
                                        }
                                }
#endif /* XWMPCFG_SYNC_EVT */
                                xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                                xwmp_sem_put(sem);
                        }
                }
        }/* else {} */
        return rc;
}

/**
 * @brief XWMP API：获取管道信号量，若不能获取，就阻塞等待
 * @param sem: (I) 信号量对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 信号量类型错误
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 */
__xwmp_inline_api
xwer_t xwmp_plsem_wait(struct xwmp_sem * sem)
{
        xwtm_t expected = XWTM_MAX;
        return xwmp_plsem_timedwait(sem, &expected);
}

/**
 * @brief XWMP API：尝试获取管道信号量，不会阻塞调用者
 * @param sem: (I) 信号量对象的指针
 * @retval XWOK: 没有错误
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
__xwmp_api
xwer_t xwmp_plsem_trywait(struct xwmp_sem * sem)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((XWMP_SEM_TYPE_PIPELINE == sem->type), "type-error", -ETYPE);

        rc = xwmp_sem_grab(sem);
        if (__xwcc_likely(XWOK == rc)) {
                xwmp_plwq_lock_cpuirqsv(&sem->wq.pl, &cpuirq);
                if (sem->count > 0) {
                        sem->count--;
#if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
                        if (0 == sem->count) {
                                struct xwmp_evt * evt;
                                struct xwmp_synobj * synobj;

                                synobj = &sem->synobj;
                                xwmb_mp_load_acquire(struct xwmp_evt *,
                                                     evt,
                                                     &synobj->sel.evt);
                                if (NULL != evt) {
                                        xwmp_evt_obj_c0i(evt, synobj);
                                }
                        }
#endif /* XWMPCFG_SYNC_EVT */
                } else {
                        rc = -ENODATA;
                }
                xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                xwmp_sem_put(sem);
        }/* else {} */
        return rc;
}

static __xwmp_code
xwer_t xwmp_plsem_do_timedblkthd_unlkwq_cpuirqrs(struct xwmp_sem * sem,
                                                 struct xwmp_thd * thd,
                                                 xwtm_t * xwtm,
                                                 xwreg_t cpuirq)
{
        struct xwmp_skd * xwskd;
        struct xwmp_tt * xwtt;
        struct xwmp_syshwt * hwt;
        xwtm_t expected, currtick;
        xwpr_t dprio;
        xwsq_t reason;
        xwsq_t wkuprs;
        xwer_t rc;

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
                xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                rc = -EINTR;
                goto err_needfrz;
        }
        dprio = thd->dprio.r;
        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_RUNNING);
        thd->dprio.r = XWMP_SKD_PRIORITY_INVALID;
        thd->dprio.wq = dprio;
        xwbop_s1m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_BLOCKING);
        xwmp_splk_unlock(&thd->stlock);
        xwmp_thd_eq_plwq_locked(thd, &sem->wq.pl, XWMP_WQTYPE_PLSEM);
        xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);

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
                rc = XWOK;
        } else if (XWMP_TTN_WKUPRS_TIMEDOUT == wkuprs) {
                xwmp_plwq_lock_cpuirq(&sem->wq.pl);
                xwmp_splk_lock(&thd->wqn.lock);
                rc = xwmp_plwq_remove_locked(&sem->wq.pl, &thd->wqn);
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
                        xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                        rc = -ETIMEDOUT;
                } else {
                        xwmp_splk_unlock(&thd->wqn.lock);
                        xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                        reason = xwaop_load(xwsq, &thd->wqn.reason,
                                            xwmb_modr_relaxed);
                        if (XWMP_WQN_REASON_INTR == reason) {
                                rc = -EINTR;
                        } else if (XWMP_WQN_REASON_UP == reason) {
                                rc = XWOK;
                        } else {
                                XWOS_BUG();
                                rc = -EBUG;
                        }
                }
        } else if (XWMP_TTN_WKUPRS_INTR == wkuprs) {
                xwmp_plwq_lock_cpuirq(&sem->wq.pl);
                xwmp_splk_unlock(&thd->wqn.lock);
                rc = xwmp_plwq_remove_locked(&sem->wq.pl, &thd->wqn);
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
                        xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                        rc = -EINTR;
                } else {
                        xwmp_splk_unlock(&thd->wqn.lock);
                        xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                        reason = xwaop_load(xwsq, &thd->wqn.reason,
                                            xwmb_modr_relaxed);
                        if (XWMP_WQN_REASON_INTR == reason) {
                                rc = -EINTR;
                        } else if (XWMP_WQN_REASON_UP == reason) {
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

static __xwmp_code
xwer_t xwmp_plsem_do_timedwait(struct xwmp_sem * sem, struct xwmp_thd * thd,
                               xwtm_t * xwtm)
{
        struct xwmp_skd * xwskd;
        xwreg_t cpuirq;
        xwer_t rc;

        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &thd->xwskd);
        xwmp_plwq_lock_cpuirqsv(&sem->wq.pl, &cpuirq);
        if (sem->count <= 0) {
                rc = xwmp_skd_wakelock_lock(xwskd);
                if (__xwcc_unlikely(rc < 0)) {
                        /* 当前调度器正准备休眠，线程需被冻结，返回-EINTR。*/
                        xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                        rc = -EINTR;
                } else {
                        rc = xwmp_plsem_do_timedblkthd_unlkwq_cpuirqrs(sem,
                                                                       thd,
                                                                       xwtm,
                                                                       cpuirq);
                        xwmp_skd_wakelock_unlock(xwskd);
                }
        } else {
                sem->count--;
#if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
                if (0 == sem->count) {
                        struct xwmp_evt * evt;
                        struct xwmp_synobj * synobj;

                        synobj = &sem->synobj;
                        xwmb_mp_load_acquire(struct xwmp_evt *,
                                             evt, &synobj->sel.evt);
                        if (NULL != evt) {
                                xwmp_evt_obj_c0i(evt, synobj);
                        }
                }
#endif /* XWMPCFG_SYNC_EVT */
                xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief XWMP API：限时等待并获取信号量
 * @param sem: (I) 信号量对象的指针
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 类型错误
 * @retval -ETIMEDOUT: 超时
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 函数返回返回-ETIMEDOUT时，*xwtm* 指向的缓冲区内的期望时间会减为0。
 */
__xwmp_api
xwer_t xwmp_plsem_timedwait(struct xwmp_sem * sem, xwtm_t * xwtm)
{
        struct xwmp_thd * cthd;
        xwer_t rc;

        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((XWMP_SEM_TYPE_PIPELINE == sem->type), "type-error", -ETYPE);
        XWOS_VALIDATE((xwtm_cmp(*xwtm, 0) >= 0), "out-of-time", -ETIMEDOUT);
        XWOS_VALIDATE((-EINTHD == xwmp_irq_get_id(NULL)),
                      "not-in-thd", -ENOTINTHD);

        if (__xwcc_unlikely(0 == xwtm_cmp(*xwtm, 0))) {
                rc = xwmp_plsem_trywait(sem);
                if (__xwcc_unlikely(rc < 0)) {
                        if (__xwcc_likely(-ENODATA == rc)) {
                                rc = -ETIMEDOUT;
                        }/* else {} */
                }/* else {} */
        } else {
                cthd = xwmp_skd_get_cthd_lc();
                rc = xwmp_sem_grab(sem);
                if (__xwcc_likely(XWOK == rc)) {
                        rc = xwmp_plsem_do_timedwait(sem, cthd, xwtm);
                        xwmp_sem_put(sem);
                }/* else {} */
        }
        return rc;
}

static __xwmp_code
xwer_t xwmp_plsem_do_blkthd_unlkwq_cpuirqrs(struct xwmp_sem * sem,
                                            struct xwmp_thd * thd,
                                            xwreg_t cpuirq)
{
        struct xwmp_skd * xwskd;
        xwpr_t dprio;
        xwsq_t reason;
        xwer_t rc;

        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &thd->xwskd);
        /* 加入等待队列 */
        xwmp_splk_lock(&thd->stlock);
        XWOS_BUG_ON((XWMP_SKDOBJ_DST_BLOCKING | XWMP_SKDOBJ_DST_SLEEPING |
                     XWMP_SKDOBJ_DST_READY | XWMP_SKDOBJ_DST_STANDBY |
                     XWMP_SKDOBJ_DST_FROZEN | XWMP_SKDOBJ_DST_MIGRATING)
                    & thd->state);
        dprio = thd->dprio.r;
        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_RUNNING);
        thd->dprio.r = XWMP_SKD_PRIORITY_INVALID;
        thd->dprio.wq = dprio;
        xwbop_s1m(xwsq_t, &thd->state,
                  XWMP_SKDOBJ_DST_BLOCKING | XWMP_SKDOBJ_DST_UNINTERRUPTED);
        xwmp_splk_unlock(&thd->stlock);
        xwmp_thd_eq_plwq_locked(thd, &sem->wq.pl, XWMP_WQTYPE_PLSEM);
        xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);

        /* 调度 */
        xwospl_cpuirq_enable_lc();
        xwmp_skd_req_swcx(xwskd);
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        xwmb_mp_rmb();
        /* 确保对唤醒原因的读取操作发生在线程状态切换之后 */
        reason = xwaop_load(xwsq, &thd->wqn.reason, xwmb_modr_relaxed);
        if (XWMP_WQN_REASON_UP == reason) {
                rc = XWOK;
        } else {
                XWOS_BUG();
                rc = -EBUG;
        }
        return rc;
}

static __xwmp_code
xwer_t xwmp_plsem_do_wait_unintr(struct xwmp_sem * sem, struct xwmp_thd * thd)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwmp_plwq_lock_cpuirqsv(&sem->wq.pl, &cpuirq);
        if (sem->count <= 0) {
                rc = xwmp_plsem_do_blkthd_unlkwq_cpuirqrs(sem, thd, cpuirq);
        } else {
                sem->count--;
#if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
                if (0 == sem->count) {
                        struct xwmp_evt * evt;
                        struct xwmp_synobj * synobj;

                        synobj = &sem->synobj;
                        xwmb_mp_load_acquire(struct xwmp_evt *,
                                             evt, &synobj->sel.evt);
                        if (NULL != evt) {
                                xwmp_evt_obj_c0i(evt, synobj);
                        }
                }
#endif /* XWMPCFG_SYNC_EVT */
                xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief XWMP API：等待并获取管道信号量，且等待不可被中断
 * @param sem: (I) 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 类型错误
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwmp_api
xwer_t xwmp_plsem_wait_unintr(struct xwmp_sem * sem)
{
        struct xwmp_thd * cthd;
        xwer_t rc;

        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((XWMP_SEM_TYPE_PIPELINE == sem->type), "type-error", -ETYPE);
        XWOS_VALIDATE((-EINTHD == xwmp_irq_get_id(NULL)),
                      "not-in-thd", -ENOTINTHD);

        rc = xwmp_sem_grab(sem);
        if (__xwcc_likely(XWOK == rc)) {
                cthd = xwmp_skd_get_cthd_lc();
                rc = xwmp_plsem_do_wait_unintr(sem, cthd);
                xwmp_sem_put(sem);
        }/* else {} */
        return rc;
}
#endif /* XWMPCFG_SYNC_PLSEM */

#if defined(XWMPCFG_SYNC_RTSEM) && (1 == XWMPCFG_SYNC_RTSEM)
/**
 * @brief 激活并初始化实时信号量对象
 * @param sem: (I) 信号量对象的指针
 * @param val: (I) 信号量的初始值
 * @param max: (I) 信号量的最大值
 * @param gcfunc: (I) 垃圾回收函数的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 无效参数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个信号量对象，不可重入
 */
static __xwmp_code
xwer_t xwmp_rtsem_activate(struct xwmp_sem * sem, xwssq_t val, xwssq_t max,
                           xwobj_gc_f gcfunc)
{
        xwer_t rc;

        XWOS_VALIDATE(((val >= 0) && (max > 0) && (val <= max)),
                      "invalid-value", -EINVAL);

        rc = xwmp_synobj_activate(&sem->synobj, gcfunc);
        if (__xwcc_likely(XWOK == rc)) {
                sem->max = max;
                xwmp_rtwq_init(&sem->wq.rt);
                sem->type = XWMP_SEM_TYPE_RT;
                sem->count = val;
        }/* else {} */
        return rc;
}

/**
 * @brief XWMP API：静态初始化实时信号量
 * @param sem: (I) 信号量对象的指针
 * @param val: (I) 信号量的初始值
 * @param max: (I) 信号量的最大值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 无效参数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个信号量对象，不可重入
 */
__xwmp_api
xwer_t xwmp_rtsem_init(struct xwmp_sem * sem, xwssq_t val, xwssq_t max)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE(((val >= 0) && (max > 0) && (val <= max)),
                      "invalid-value", -EINVAL);

        xwmp_sem_construct(sem);
        return xwmp_rtsem_activate(sem, val, max, NULL);
}

/**
 * @brief XWMP API：冻结实时信号量（值设置为负）
 * @param sem: (I) 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 类型不匹配
 * @retval -EALREADY: 信号量已被冻结
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个信号量对象，不可重入
 * @note
 * - 已冻结的信号量不允许增加(V操作)，但可以被测试(P操作)，
 *   测试信号量的线程会被加入到信号量的等待队列。
 */
__xwmp_api
xwer_t xwmp_rtsem_freeze(struct xwmp_sem * sem)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((XWMP_SEM_TYPE_RT == sem->type), "type-error", -ETYPE);

        rc = xwmp_sem_grab(sem);
        if (__xwcc_likely(XWOK == rc)) {
                xwmp_rtwq_lock_cpuirqsv(&sem->wq.rt, &cpuirq);
                if (__xwcc_unlikely(sem->count < 0)) {
                        rc = -EALREADY;
                } else {
                        sem->count = XWMP_SEM_NEGTIVE;
#if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
                        struct xwmp_evt * evt;
                        struct xwmp_synobj * synobj;

                        synobj = &sem->synobj;
                        xwmb_mp_load_acquire(struct xwmp_evt *,
                                             evt, &synobj->sel.evt);
                        if (NULL != evt) {
                                xwmp_evt_obj_c0i(evt, synobj);
                        }
#endif /* XWMPCFG_SYNC_EVT */
                }
                xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                xwmp_sem_put(sem);
        }/* else {} */
        return rc;
}

/**
 * @brief XWMP API：解冻信号量
 * @param sem: (I) 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 类型不匹配
 * @retval -EALREADY: 信号量未被冻结
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对已冻结的信号量起作用，对未冻结的信号量调用此函数将返回错误码。
 */
__xwmp_api
xwer_t xwmp_rtsem_thaw(struct xwmp_sem * sem)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((XWMP_SEM_TYPE_RT == sem->type), "type-error", -ETYPE);

        rc = xwmp_sem_grab(sem);
        if (__xwcc_likely(XWOK == rc)) {
                xwmp_rtwq_lock_cpuirqsv(&sem->wq.rt, &cpuirq);
                if (__xwcc_unlikely(sem->count >= 0)) {
                        rc = -EALREADY;
                } else {
                        sem->type = XWMP_SEM_TYPE_RT;
                        sem->count = 0;
                        rc = XWOK;
                }
                xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                xwmp_sem_put(sem);
        }/* else {} */
        return rc;
}

/**
 * @brief 中断实时信号量等待队列中的一个节点
 * @param sem: (I) 信号量对象的指针
 * @param wqn: (I) 等待队列节点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ETYPE: 类型不匹配
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmp_code
xwer_t xwmp_rtsem_intr(struct xwmp_sem * sem, struct xwmp_wqn * wqn)
{
        xwmp_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwmp_sem_grab(sem);
        if (__xwcc_likely(XWOK == rc)) {
                xwmp_rtwq_lock_cpuirqsv(&sem->wq.rt, &cpuirq);
                xwmp_splk_lock(&wqn->lock);
                rc = xwmp_rtwq_remove_locked(&sem->wq.rt, wqn);
                if (XWOK == rc) {
                        wqn->wq = NULL;
                        wqn->type = XWMP_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq, &wqn->reason,
                                    xwmb_modr_release, XWMP_WQN_REASON_INTR);
                        cb = wqn->cb;
                        wqn->cb = NULL;
                        xwmp_splk_unlock(&wqn->lock);
                        xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                        xwmp_sem_put(sem);
                        cb(wqn->owner);
                } else {
                        xwmp_splk_unlock(&wqn->lock);
                        xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                        xwmp_sem_put(sem);
                }
        }/* else {} */
        return rc;
}

/**
 * @brief XWMP API：发布实时信号量
 * @param sem: (I) 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
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
__xwmp_api
xwer_t xwmp_rtsem_post(struct xwmp_sem * sem)
{
        struct xwmp_wqn * wqn;
        xwreg_t cpuirq;
        xwmp_wqn_f cb;
        xwer_t rc;

        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((XWMP_SEM_TYPE_RT == sem->type), "type-error", -ETYPE);

        rc = xwmp_sem_grab(sem);
        if (__xwcc_likely(XWOK == rc)) {
                xwmp_rtwq_lock_cpuirqsv(&sem->wq.rt, &cpuirq);
                if (__xwcc_unlikely(sem->count < 0)) {
                        xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                        xwmp_sem_put(sem);
                        rc = -ENEGATIVE;
                } else {
                        wqn = xwmp_rtwq_choose_locked(&sem->wq.rt);
                        if (wqn) {
                                wqn->wq = NULL;
                                wqn->type = XWMP_WQTYPE_UNKNOWN;
                                xwaop_store(xwsq, &wqn->reason,
                                            xwmb_modr_release, XWMP_WQN_REASON_UP);
                                cb = wqn->cb;
                                wqn->cb = NULL;
                                xwmp_splk_unlock(&wqn->lock);
                                xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                                xwmp_sem_put(sem);
                                cb(wqn->owner);
                        } else {
                                if (sem->count < sem->max) {
                                        sem->count++;
                                } else {
                                        rc = -ERANGE;
                                }
#if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
                                if (sem->count > 0) {
                                        struct xwmp_evt * evt;
                                        struct xwmp_synobj * synobj;

                                        synobj = &sem->synobj;
                                        xwmb_mp_load_acquire(struct xwmp_evt *,
                                                             evt,
                                                             &synobj->sel.evt);
                                        if (NULL != evt) {
                                                xwmp_evt_obj_s1i(evt, synobj);
                                        }
                                }
#endif /* XWMPCFG_SYNC_EVT */
                                xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                                xwmp_sem_put(sem);
                        }
                }
        }/* else {} */
        return rc;
}

/**
 * @brief XWMP API：获取实时信号量，若不能获取，就阻塞等待
 * @param sem: (I) 信号量对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 信号量类型错误
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 */
__xwmp_inline_api
xwer_t xwmp_rtsem_wait(struct xwmp_sem * sem)
{
        xwtm_t expected = XWTM_MAX;
        return xwmp_rtsem_timedwait(sem, &expected);
}

/**
 * @brief XWMP API：尝试获取实时信号量
 * @param sem: (I) 信号量对象的指针
 * @retval XWOK: 没有错误
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
__xwmp_api
xwer_t xwmp_rtsem_trywait(struct xwmp_sem * sem)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((XWMP_SEM_TYPE_RT == sem->type), "type-error", -ETYPE);

        rc = xwmp_sem_grab(sem);
        if (__xwcc_likely(XWOK == rc)) {
                xwmp_rtwq_lock_cpuirqsv(&sem->wq.rt, &cpuirq);
                if (sem->count > 0) {
                        sem->count--;
#if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
                        if (0 == sem->count) {
                                struct xwmp_evt * evt;
                                struct xwmp_synobj * synobj;

                                synobj = &sem->synobj;
                                xwmb_mp_load_acquire(struct xwmp_evt *,
                                                     evt,
                                                     &synobj->sel.evt);
                                if (NULL != evt) {
                                        xwmp_evt_obj_c0i(evt, synobj);
                                }
                        }
#endif /* XWMPCFG_SYNC_EVT */
                } else {
                        rc = -ENODATA;
                }
                xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                xwmp_sem_put(sem);
        }/* else {} */
        return rc;
}

static __xwmp_code
xwer_t xwmp_rtsem_do_timedblkthd_unlkwq_cpuirqrs(struct xwmp_sem * sem,
                                                 struct xwmp_thd * thd,
                                                 xwtm_t * xwtm,
                                                 xwreg_t cpuirq)
{
        xwer_t rc;
        xwtm_t expected, currtick;
        struct xwmp_skd * xwskd;
        struct xwmp_tt * xwtt;
        struct xwmp_syshwt * hwt;
        xwsq_t reason;
        xwsq_t wkuprs;
        xwpr_t dprio;

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
                xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                rc = -EINTR;
                goto err_needfrz;
        }
        dprio = thd->dprio.r;
        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_RUNNING);
        thd->dprio.r = XWMP_SKD_PRIORITY_INVALID;
        thd->dprio.wq = dprio;
        xwbop_s1m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_BLOCKING);
        xwmp_splk_unlock(&thd->stlock);
        xwmp_thd_eq_rtwq_locked(thd, &sem->wq.rt, XWMP_WQTYPE_RTSEM, dprio);
        xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);

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
                rc = XWOK;
        } else if (XWMP_TTN_WKUPRS_TIMEDOUT == wkuprs) {
                xwmp_rtwq_lock_cpuirq(&sem->wq.rt);
                xwmp_splk_lock(&thd->wqn.lock);
                rc = xwmp_rtwq_remove_locked(&sem->wq.rt, &thd->wqn);
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
                        xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                        rc = -ETIMEDOUT;
                } else {
                        xwmp_splk_unlock(&thd->wqn.lock);
                        xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                        reason = xwaop_load(xwsq, &thd->wqn.reason,
                                            xwmb_modr_relaxed);
                        if (XWMP_WQN_REASON_INTR == reason) {
                                rc = -EINTR;
                        } else if (XWMP_WQN_REASON_UP == reason) {
                                rc = XWOK;
                        } else {
                                XWOS_BUG();
                                rc = -EBUG;
                        }
                }
        } else if (XWMP_TTN_WKUPRS_INTR == wkuprs) {
                xwmp_rtwq_lock_cpuirq(&sem->wq.rt);
                xwmp_splk_lock(&thd->wqn.lock);
                rc = xwmp_rtwq_remove_locked(&sem->wq.rt, &thd->wqn);
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
                        xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                        rc = -EINTR;
                } else {
                        xwmp_splk_unlock(&thd->wqn.lock);
                        xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                        reason = xwaop_load(xwsq, &thd->wqn.reason,
                                            xwmb_modr_relaxed);
                        if (XWMP_WQN_REASON_INTR == reason) {
                                rc = -EINTR;
                        } else if (XWMP_WQN_REASON_UP == reason) {
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

static __xwmp_code
xwer_t xwmp_rtsem_do_timedwait(struct xwmp_sem * sem, struct xwmp_thd * thd,
                               xwtm_t * xwtm)
{
        struct xwmp_skd * xwskd;
        xwreg_t cpuirq;
        xwer_t rc;

        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &thd->xwskd);
        xwmp_rtwq_lock_cpuirqsv(&sem->wq.rt, &cpuirq);
        if (sem->count <= 0) {
                rc = xwmp_skd_wakelock_lock(xwskd);
                if (__xwcc_unlikely(rc < 0)) {
                        /* 当前调度器正准备休眠，线程需被冻结，返回-EINTR。*/
                        xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                        rc = -EINTR;
                } else {
                        rc = xwmp_rtsem_do_timedblkthd_unlkwq_cpuirqrs(sem,
                                                                       thd,
                                                                       xwtm,
                                                                       cpuirq);
                        xwmp_skd_wakelock_unlock(xwskd);
                }
        } else {
                sem->count--;
#if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
                if (0 == sem->count) {
                        struct xwmp_evt * evt;
                        struct xwmp_synobj * synobj;

                        synobj = &sem->synobj;
                        xwmb_mp_load_acquire(struct xwmp_evt *,
                                             evt, &synobj->sel.evt);
                        if (NULL != evt) {
                                xwmp_evt_obj_c0i(evt, synobj);
                        }
                }
#endif /* XWMPCFG_SYNC_EVT */
                xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                rc = XWOK;
        }

        return rc;
}

/**
 * @brief XWMP API：限时等待并获取信号量
 * @param sem: (I) 信号量对象的指针
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 类型错误
 * @retval -ETIMEDOUT: 超时
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 函数返回返回-ETIMEDOUT时，*xwtm* 指向的缓冲区内的期望时间会减为0。
 */
__xwmp_api
xwer_t xwmp_rtsem_timedwait(struct xwmp_sem * sem, xwtm_t * xwtm)
{
        struct xwmp_thd * cthd;
        xwer_t rc;

        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((XWMP_SEM_TYPE_RT == sem->type), "type-error", -ETYPE);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm_cmp(*xwtm, 0) >= 0), "out-of-time", -ETIMEDOUT);
        XWOS_VALIDATE((-EINTHD == xwmp_irq_get_id(NULL)),
                      "not-in-thd", -ENOTINTHD);

        if (__xwcc_unlikely(0 == xwtm_cmp(*xwtm, 0))) {
                rc = xwmp_rtsem_trywait(sem);
                if (__xwcc_unlikely(rc < 0)) {
                        if (__xwcc_likely(-ENODATA == rc))
                                rc = -ETIMEDOUT;
                        /* else {} */
                }/* else {} */
        } else {
                cthd = xwmp_skd_get_cthd_lc();
                rc = xwmp_sem_grab(sem);
                if (__xwcc_likely(XWOK == rc)) {
                        rc = xwmp_rtsem_do_timedwait(sem, cthd, xwtm);
                        xwmp_sem_put(sem);
                }/* else {} */
        }
        return rc;
}

static __xwmp_code
xwer_t xwmp_rtsem_do_blkthd_unlkwq_cpuirqrs(struct xwmp_sem * sem,
                                            struct xwmp_thd * thd,
                                            xwreg_t cpuirq)
{
        xwer_t rc;
        struct xwmp_skd * xwskd;
        xwsq_t reason;
        xwpr_t dprio;

        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &thd->xwskd);
        /* 加入等待队列 */
        xwmp_splk_lock(&thd->stlock);
        XWOS_BUG_ON((XWMP_SKDOBJ_DST_BLOCKING | XWMP_SKDOBJ_DST_SLEEPING |
                     XWMP_SKDOBJ_DST_READY | XWMP_SKDOBJ_DST_STANDBY |
                     XWMP_SKDOBJ_DST_FROZEN | XWMP_SKDOBJ_DST_MIGRATING)
                    & thd->state);
        dprio = thd->dprio.r;
        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_RUNNING);
        thd->dprio.r = XWMP_SKD_PRIORITY_INVALID;
        thd->dprio.wq = dprio;
        xwbop_s1m(xwsq_t, &thd->state,
                  XWMP_SKDOBJ_DST_BLOCKING | XWMP_SKDOBJ_DST_UNINTERRUPTED);
        xwmp_splk_unlock(&thd->stlock);
        xwmp_thd_eq_rtwq_locked(thd, &sem->wq.rt, XWMP_WQTYPE_RTSEM, dprio);
        xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);

        /* 调度 */
        xwospl_cpuirq_enable_lc();
        xwmp_skd_req_swcx(xwskd);
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        xwmb_mp_rmb();
        /* 确保对唤醒原因的读取操作发生在线程状态切换之后 */
        reason = xwaop_load(xwsq, &thd->wqn.reason, xwmb_modr_relaxed);
        if (XWMP_WQN_REASON_UP == reason) {
                rc = XWOK;
        } else {
                XWOS_BUG();
                rc = -EBUG;
        }
        return rc;
}

static __xwmp_code
xwer_t xwmp_rtsem_do_wait_unintr(struct xwmp_sem * sem, struct xwmp_thd * thd)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwmp_rtwq_lock_cpuirqsv(&sem->wq.rt, &cpuirq);
        if (sem->count <= 0) {
                rc = xwmp_rtsem_do_blkthd_unlkwq_cpuirqrs(sem, thd, cpuirq);
        } else {
                sem->count--;
#if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
                if (0 == sem->count) {
                        struct xwmp_evt * evt;
                        struct xwmp_synobj * synobj;

                        synobj = &sem->synobj;
                        xwmb_mp_load_acquire(struct xwmp_evt *,
                                             evt, &synobj->sel.evt);
                        if (NULL != evt) {
                                xwmp_evt_obj_c0i(evt, synobj);
                        }
                }
#endif /* XWMPCFG_SYNC_EVT */
                xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief XWMP API：等待并获取实时信号量，且等待不可被中断
 * @param sem: (I) 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 类型错误
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwmp_api
xwer_t xwmp_rtsem_wait_unintr(struct xwmp_sem * sem)
{
        struct xwmp_thd * cthd;
        xwer_t rc;

        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((XWMP_SEM_TYPE_RT == sem->type), "type-error", -ETYPE);
        XWOS_VALIDATE((-EINTHD == xwmp_irq_get_id(NULL)),
                      "not-in-thd", -ENOTINTHD);

        rc = xwmp_sem_grab(sem);
        if (__xwcc_likely(XWOK == rc)) {
                cthd = xwmp_skd_get_cthd_lc();
                rc = xwmp_rtsem_do_wait_unintr(sem, cthd);
                xwmp_sem_put(sem);
        }/* else {} */
        return rc;
}
#endif /* XWMPCFG_SYNC_RTSEM */

/**
 * @brief XWMP API：获取信号量计数器的值
 * @param sem: (I) 信号量对象的指针
 * @param sval: (O) 指向缓冲区的指针，通过此缓冲区返回信号量计数器的值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmp_api
xwer_t xwmp_sem_getvalue(struct xwmp_sem * sem, xwssq_t * sval)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((sval), "nullptr", -EFAULT);

        *sval = sem->count;
        return XWOK;
}

/**
 * @brief XWMP API：获取信号量的类型
 * @param sem: (I) 信号量对象的指针
 * @param type: (O) 指向缓冲区的指针，通过此缓冲区返回类型值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmp_api
xwer_t xwmp_sem_gettype(struct xwmp_sem * sem, xwid_t * type)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((type), "nullptr", -EFAULT);

        *type = sem->type;
        return XWOK;
}
