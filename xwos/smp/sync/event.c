/**
 * @file
 * @brief XuanWuOS的同步机制：事件
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * @note
 * - 锁的顺序：
 *   + ① plwq.lock
 *   + ① rtwq.lock
 *     + ② evt.lock
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <string.h>
#include <xwos/object.h>
#include <xwos/lib/xwbop.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#if defined(XWSMPCFG_SYNC_EVT_MEMSLICE) && (1 == XWSMPCFG_SYNC_EVT_MEMSLICE)
  #include <xwos/mm/memslice.h>
#endif /* XWSMPCFG_SYNC_EVT_MEMSLICE */
#include <xwos/smp/irq.h>
#include <xwos/smp/thread.h>
#include <xwos/smp/lock/spinlock.h>
#include <xwos/smp/sync/object.h>
#include <xwos/smp/sync/condition.h>
#include <xwos/smp/sync/event.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if defined(XWSMPCFG_SYNC_EVT_MEMSLICE) && (1 == XWSMPCFG_SYNC_EVT_MEMSLICE)
/**
 * @brief 结构体xwsync_evt的对象缓存
 */
static __xwos_data struct xwmm_memslice * xwsync_evt_cache = NULL;

/**
 * @brief 结构体xwsync_evt的对象缓存的名字
 */
__xwos_rodata const char xwsync_evt_cache_name[] = "cache.evt.xwsync";
#endif /* XWSMPCFG_SYNC_EVT_MEMSLICE */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwos_code
struct xwsync_evt * xwsync_evt_alloc(void);

static __xwos_code
void xwsync_evt_free(struct xwsync_evt * evt);

static __xwos_code
void xwsync_evt_construct(struct xwsync_evt * evt);

static __xwos_code
void xwsync_evt_destruct(struct xwsync_evt * evt);

static __xwos_code
xwer_t xwsync_evt_gc(void * evt);

static __xwos_code
xwer_t xwsync_evt_activate(struct xwsync_evt * evt, xwbmp_t initval[],
                           xwsq_t attr, xwobj_gc_f gcfunc);

static __xwos_code
xwer_t xwsync_evt_trywait_level(struct xwsync_evt * evt,
                                xwsq_t trigger, xwsq_t action,
                                xwbmp_t origin[], xwbmp_t msk[]);

static __xwos_code
xwer_t xwsync_evt_trywait_edge(struct xwsync_evt * evt, xwsq_t trigger,
                               xwbmp_t origin[], xwbmp_t msk[]);

static __xwos_code
xwer_t xwsync_evt_timedwait_level(struct xwsync_evt * evt,
                                  xwsq_t trigger, xwsq_t action,
                                  xwbmp_t origin[], xwbmp_t msk[],
                                  xwtm_t * xwtm);

static __xwos_code
xwer_t xwsync_evt_timedwait_edge(struct xwsync_evt * evt, xwsq_t trigger,
                                 xwbmp_t origin[], xwbmp_t msk[],
                                 xwtm_t * xwtm);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** memslice ******** ********/
#if defined(XWSMPCFG_SYNC_EVT_MEMSLICE) && (1 == XWSMPCFG_SYNC_EVT_MEMSLICE)
/**
 * @brief XWOS INIT CODE：初始化结构体xwsync_evt的对象缓存
 * @param zone_origin: (I) 内存区域的首地址
 * @param zone_size: (I) 内存区域的大小
 * @return 错误码
 * @note
 * - 重入性：只可在系统初始化时使用一次
 */
__xwos_init_code
xwer_t xwsync_evt_cache_init(xwptr_t zone_origin, xwsz_t zone_size)
{
        struct xwmm_memslice * msa;
        xwer_t rc;

        rc = xwmm_memslice_create(&msa, zone_origin, zone_size,
                                  sizeof(struct xwsync_evt),
                                  xwsync_evt_cache_name,
                                  (ctor_f)xwsync_evt_construct,
                                  (dtor_f)xwsync_evt_destruct);
        if (__unlikely(rc < 0)) {
                xwsync_evt_cache = NULL;
        } else {
                xwsync_evt_cache = msa;
        }
        return rc;
}
#endif /* XWSMPCFG_SYNC_EVT_MEMSLICE */

/**
 * @brief 从事件对象缓存中申请对象
 * @return 事件对象的指针
 */
static __xwos_code
struct xwsync_evt * xwsync_evt_alloc(void)
{
#if defined(XWSMPCFG_SYNC_EVT_MEMSLICE) && (1 == XWSMPCFG_SYNC_EVT_MEMSLICE)
        union {
                struct xwsync_evt * evt;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_memslice_alloc(xwsync_evt_cache, &mem.anon);
        if (rc < 0) {
                mem.evt = err_ptr(rc);
        }/* else {} */
        return mem.evt;
#else /* XWSMPCFG_SYNC_EVT_MEMSLICE */
        union {
                struct xwsync_evt * evt;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwsync_evt), XWMM_ALIGNMENT, &mem.anon);
        if (XWOK == rc) {
                xwsync_evt_construct(mem.evt);
        } else {
                mem.evt = err_ptr(rc);
        }
        return mem.evt;
#endif /* !XWSMPCFG_SYNC_EVT_MEMSLICE */
}

/**
 * @brief 释放事件对象
 * @param evt: (I) 事件对象的指针
 */
static __xwos_code
void xwsync_evt_free(struct xwsync_evt * evt)
{
#if defined(XWSMPCFG_SYNC_EVT_MEMSLICE) && (1 == XWSMPCFG_SYNC_EVT_MEMSLICE)
        xwmm_memslice_free(xwsync_evt_cache, evt);
#else /* XWSMPCFG_SYNC_EVT_MEMSLICE */
        xwsync_evt_destruct(evt);
        xwmm_kma_free(evt);
#endif /* !XWSMPCFG_SYNC_EVT_MEMSLICE */
}

/**
 * @brief 事件对象的构造函数
 * @param evt: (I) 事件对象的指针
 */
static __xwos_code
void xwsync_evt_construct(struct xwsync_evt * evt)
{
        xwsync_cdt_construct(&evt->cdt);
}

/**
 * @brief 事件对象的析构函数
 * @param evt: (I) 事件对象的指针
 */
static __xwos_code
void xwsync_evt_destruct(struct xwsync_evt * evt)
{
        xwsync_cdt_destruct(&evt->cdt);
}

/**
 * @brief 事件对象的垃圾回收函数
 * @param evt: (I) 事件对象的指针
 * @return 错误码
 */
static __xwos_code
xwer_t xwsync_evt_gc(void * evt)
{
        xwsync_evt_free((struct xwsync_evt *)evt);
        return XWOK;
}

/**
 * @brief XWOS API：激活并初始化事件对象
 * @param evt: (I) 事件对象的指针
 * @param initval: (I) 事件的数组的初始态
 * @param attr: (I) 事件的属性，取值 @ref xwsync_evt_attr_em
 * @param gcfunc: (I) 垃圾回收函数的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *evt* ，不可重入
 * @note
 * - 由于静态初始化的对象所有资源都是由用户自己提供的，
 *   因此当信号量不使用时，回收资源的函数也需要用户自己提供。
 */
static __xwos_code
xwer_t xwsync_evt_activate(struct xwsync_evt * evt, xwbmp_t initval[],
                           xwsq_t attr, xwobj_gc_f gcfunc)
{
        xwer_t rc;
        xwsz_t size;

        size = BITS_TO_BMPS(XWSYNC_EVT_MAXNUM);
        rc = xwsync_cdt_activate(&evt->cdt, gcfunc);
        if (__likely(XWOK == rc)) {
                evt->attr = attr;
                switch (attr & XWSYNC_EVT_TYPE_MASK) {
                case XWSYNC_EVT_TYPE_FLAG:
                        memset(evt->msk, 0xFF, size);
                        break;
                case XWSYNC_EVT_TYPE_BARRIER:
                        memset(evt->msk, 0, size);
                        break;
                case XWSYNC_EVT_TYPE_SELECTOR:
                        memset(evt->msk, 0, size);
                        break;
                default:
                        memset(evt->msk, 0xFF, size);
                        break;
                }
                if (NULL != initval) {
                        memcpy(evt->bmp, initval, size);
                } else {
                        memset(evt->bmp, 0, size);
                }
                xwlk_splk_init(&evt->lock);
        }/* else {} */
        return rc;
}

/**
 * @brief XWOS API：动态创建事件对象
 * @param ptrbuf: (O) 指向缓冲区的指针，通过此缓冲区返回对象的指针
 * @param initval: (I) 事件的数组的初始态，如果为NULL，初始值全部为0
 * @param attr: (I) 事件的属性，取值范围 @ref xwsync_evt_attr_em
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 无效参数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwsync_evt_create(struct xwsync_evt ** ptrbuf, xwbmp_t initval[], xwsq_t attr)
{
        struct xwsync_evt * evt;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);
        XWOS_VALIDATE(((attr & XWSYNC_EVT_TYPE_MASK) < XWSYNC_EVT_TYPE_MAX),
                      "type-error", -EINVAL);

        *ptrbuf = NULL;
        evt = xwsync_evt_alloc();
        if (__unlikely(is_err(evt))) {
                rc = ptr_err(evt);
        } else {
                rc = xwsync_evt_activate(evt, initval, attr, xwsync_evt_gc);
                if (__unlikely(rc < 0)) {
                        xwsync_evt_free(evt);
                } else {
                        *ptrbuf = evt;
                }
        }
        return rc;
}

/**
 * @brief XWOS API：删除动态创建的事件对象
 * @param evt: (I) 事件对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *evt* ，不可重入
 */
__xwos_api
xwer_t xwsync_evt_delete(struct xwsync_evt * evt)
{
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        return xwsync_evt_put(evt);
}

/**
 * @brief XWOS API：静态初始化事件对象
 * @param evt: (I) 事件对象的指针
 * @param initval: (I) 事件的数组的初始态，如果为NULL，初始值全部为0
 * @param attr: (I) 事件的属性，取值范围 @ref xwsync_evt_attr_em
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 无效参数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *evt* ，不可重入
 */
__xwos_api
xwer_t xwsync_evt_init(struct xwsync_evt * evt, xwbmp_t initval[], xwsq_t attr)
{
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE(((attr & XWSYNC_EVT_TYPE_MASK) < XWSYNC_EVT_TYPE_MAX),
                      "type-error", -EINVAL);

        xwsync_evt_construct(evt);
        return xwsync_evt_activate(evt, initval, attr, NULL);
}

/**
 * @brief XWOS API：销毁静态方式初始化的事件对象
 * @param evt: (I) 事件对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *evt* ，不可重入
 */
__xwos_api
xwer_t xwsync_evt_destroy(struct xwsync_evt * evt)
{
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        return xwsync_evt_put(evt);
}

/**
 * @brief XWOS API：绑定事件对象(evt)到另一个事件对象(slt)，
 *                  另一个事件对象类型为XWSYNC_EVT_TYPE_SELECTOR
 * @param evt: (I) 事件对象的指针
 * @param slt: (I) 类型为XWSYNC_EVT_TYPE_SELECTOR的事件对象的指针
 * @param pos: (I) 事件对象对象映射到位图中的位置
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ETYPE: 事件对象或事件对象类型错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *evt* ，不可重入
 */
__xwos_api
xwer_t xwsync_evt_bind(struct xwsync_evt * evt, struct xwsync_evt * slt, xwsq_t pos)
{
        return xwsync_cdt_bind(&evt->cdt, slt, pos);
}

/**
 * @brief XWOS API：从另一个事件对象(slt)上解绑事件对象(evt)，
 *                  另一个事件对象类型为XWSYNC_EVT_TYPE_SELECTOR
 * @param evt: (I) 事件对象的指针
 * @param slt: (I) 类型为XWSYNC_EVT_TYPE_SELECTOR的事件对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ETYPE: 事件对象或条件量类型错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *evt* ，不可重入
 */
__xwos_api
xwer_t xwsync_evt_unbind(struct xwsync_evt * evt, struct xwsync_evt * slt)
{
        return xwsync_cdt_unbind(&evt->cdt, slt);
}

/**
 * @brief XWOS API：中断事件对象等待队列中的所有节点
 * @param evt: (I) 事件对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 类型不匹配
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwsync_evt_intr_all(struct xwsync_evt * evt)
{
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        return xwsync_cdt_intr_all(&evt->cdt);
}

/******** type:XWSYNC_EVT_TYPE_FLAG ********/
/**
 * @brief XWOS API：在事件对象中同时设置多个位图的位，
 *                  事件对象类型为XWSYNC_EVT_TYPE_FLAG
 * @param evt: (I) 事件对象的指针
 * @param msk: (I) 事件对象的位图掩码
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 事件对象类型错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwsync_evt_s1m(struct xwsync_evt * evt, xwbmp_t msk[])
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->attr & XWSYNC_EVT_TYPE_MASK) == XWSYNC_EVT_TYPE_FLAG),
                      "type-error", -ETYPE);

        rc = xwsync_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_s1m(evt->bmp, msk, XWSYNC_EVT_MAXNUM);
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwsync_cdt_broadcast(&evt->cdt);
        xwsync_evt_put(evt);
        return XWOK;

err_evt_grab:
        return rc;
}

/**
 * @brief XWOS API：在事件对象中设置单个位图的位，
 *                  事件对象类型为XWSYNC_EVT_TYPE_FLAG
 * @param evt: (I) 事件对象的指针
 * @param pos: (I) 位的序号
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 事件对象类型错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwsync_evt_s1i(struct xwsync_evt * evt, xwsq_t pos)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->attr & XWSYNC_EVT_TYPE_MASK) == XWSYNC_EVT_TYPE_FLAG),
                      "type-error", -ETYPE);

        rc = xwsync_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_s1i(evt->bmp, pos);
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwsync_cdt_broadcast(&evt->cdt);
        xwsync_evt_put(evt);
        return XWOK;

err_evt_grab:
        return rc;
}

/**
 * @brief XWOS API：在事件对象中同时清除多个位图的位，
 *                  事件对象类型为XWSYNC_EVT_TYPE_FLAG
 * @param evt: (I) 事件对象的指针
 * @param msk: (I) 事件对象的位图掩码
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 事件对象类型错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwsync_evt_c0m(struct xwsync_evt * evt, xwbmp_t msk[])
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->attr & XWSYNC_EVT_TYPE_MASK) == XWSYNC_EVT_TYPE_FLAG),
                      "type-error", -ETYPE);

        rc = xwsync_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_c0m(evt->bmp, msk, XWSYNC_EVT_MAXNUM);
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwsync_cdt_broadcast(&evt->cdt);
        xwsync_evt_put(evt);
        return XWOK;

err_evt_grab:
        return rc;
}

/**
 * @brief XWOS API：在事件对象中清除单个位图的位，
 *                  事件对象类型为XWSYNC_EVT_TYPE_FLAG
 * @param evt: (I) 事件对象的指针
 * @param pos: (I) 位的序号
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ECHRNG: 位置超出范围
 * @retval -ETYPE: 事件对象类型错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwsync_evt_c0i(struct xwsync_evt * evt, xwsq_t pos)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((pos < XWSYNC_EVT_MAXNUM), "out-of-range", -ECHRNG);
        XWOS_VALIDATE(((evt->attr & XWSYNC_EVT_TYPE_MASK) == XWSYNC_EVT_TYPE_FLAG),
                      "type-error", -ETYPE);

        rc = xwsync_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_c0i(evt->bmp, pos);
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwsync_cdt_broadcast(&evt->cdt);
        xwsync_evt_put(evt);
        return XWOK;

err_evt_grab:
        return rc;
}

/**
 * @brief XWOS API：在事件对象中同时翻转多个位图的位，
 *                  事件对象类型为XWSYNC_EVT_TYPE_FLAG
 * @param evt: (I) 事件对象的指针
 * @param msk: (I) 事件对象的位图掩码
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 事件对象类型错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwsync_evt_x1m(struct xwsync_evt * evt, xwbmp_t msk[])
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->attr & XWSYNC_EVT_TYPE_MASK) == XWSYNC_EVT_TYPE_FLAG),
                      "type-error", -ETYPE);

        rc = xwsync_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_x1m(evt->bmp, msk, XWSYNC_EVT_MAXNUM);
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwsync_cdt_broadcast(&evt->cdt);
        xwsync_evt_put(evt);
        return XWOK;

err_evt_grab:
        return rc;
}

/**
 * @brief XWOS API：在事件对象中翻转单个位图的位，
 *                  事件对象类型为XWSYNC_EVT_TYPE_FLAG
 * @param evt: (I) 事件对象的指针
 * @param pos: (I) 位的序号
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ECHRNG: 位置超出范围
 * @retval -ETYPE: 事件对象类型错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwsync_evt_x1i(struct xwsync_evt * evt, xwsq_t pos)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((pos < XWSYNC_EVT_MAXNUM), "out-of-range", -ECHRNG);
        XWOS_VALIDATE(((evt->attr & XWSYNC_EVT_TYPE_MASK) == XWSYNC_EVT_TYPE_FLAG),
                      "type-error", -ETYPE);

        rc = xwsync_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_x1i(evt->bmp, pos);
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwsync_cdt_broadcast(&evt->cdt);
        xwsync_evt_put(evt);
        return XWOK;

err_evt_grab:
        return rc;
}

/**
 * @brief XWOS API：读取事件对象位图的值
 * @param evt: (I) 事件对象的指针
 * @param out: (O) 指向缓冲区的指针，通过此缓冲区返回事件对象位图的值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwsync_evt_read(struct xwsync_evt * evt, xwbmp_t out[])
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((out), "nullptr", -EFAULT);

        rc = xwsync_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_assign(out, evt->bmp, XWSYNC_EVT_MAXNUM);
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwsync_evt_put(evt);
        return XWOK;

err_evt_grab:
        return rc;
}

static __xwos_code
xwer_t xwsync_evt_trywait_level(struct xwsync_evt * evt,
                                xwsq_t trigger, xwsq_t action,
                                xwbmp_t origin[], xwbmp_t msk[])
{
        xwreg_t cpuirq;
        bool triggered;
        xwer_t rc;

        XWOS_VALIDATE((trigger <= XWSYNC_EVT_TRIGGER_CLR_ANY),
                      "illegal-trigger", -EINVAL);
        XWOS_VALIDATE((action < XWSYNC_EVT_ACTION_NUM),
                      "illegal-action", -EINVAL);

        rc = XWOK;
        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        if (origin) {
                xwbmpop_assign(origin, evt->bmp, XWSYNC_EVT_MAXNUM);
        }
        if (XWSYNC_EVT_ACTION_CONSUMPTION == action) {
                switch (trigger) {
                case XWSYNC_EVT_TRIGGER_SET_ALL:
                        triggered = xwbmpop_t1ma_then_c0m(evt->bmp, msk,
                                                          XWSYNC_EVT_MAXNUM);
                        break;
                case XWSYNC_EVT_TRIGGER_SET_ANY:
                        triggered = xwbmpop_t1mo_then_c0m(evt->bmp, msk,
                                                          XWSYNC_EVT_MAXNUM);
                        break;
                case XWSYNC_EVT_TRIGGER_CLR_ALL:
                        triggered = xwbmpop_t0ma_then_s1m(evt->bmp, msk,
                                                          XWSYNC_EVT_MAXNUM);
                        break;
                case XWSYNC_EVT_TRIGGER_CLR_ANY:
                        triggered = xwbmpop_t0mo_then_s1m(evt->bmp, msk,
                                                          XWSYNC_EVT_MAXNUM);
                        break;
                default:
                        triggered = true;
                        rc = -EINVAL;
                        break;
                }
        } else {
                switch (trigger) {
                case XWSYNC_EVT_TRIGGER_SET_ALL:
                        triggered = xwbmpop_t1ma(evt->bmp, msk,
                                                 XWSYNC_EVT_MAXNUM);
                        break;
                case XWSYNC_EVT_TRIGGER_SET_ANY:
                        triggered = xwbmpop_t1mo(evt->bmp, msk,
                                                 XWSYNC_EVT_MAXNUM);
                        break;
                case XWSYNC_EVT_TRIGGER_CLR_ALL:
                        triggered = xwbmpop_t0ma(evt->bmp, msk,
                                                 XWSYNC_EVT_MAXNUM);
                        break;
                case XWSYNC_EVT_TRIGGER_CLR_ANY:
                        triggered = xwbmpop_t0mo(evt->bmp, msk,
                                                 XWSYNC_EVT_MAXNUM);
                        break;
                default:
                        triggered = true;
                        rc = -EINVAL;
                        break;
                }
        }
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        if (!triggered) {
                rc = -ENODATA;
        }
        return rc;
}

static __xwos_code
xwer_t xwsync_evt_trywait_edge(struct xwsync_evt * evt, xwsq_t trigger,
                               xwbmp_t origin[], xwbmp_t msk[])
{
        xwreg_t cpuirq;
        xwssq_t cmprc;
        bool triggered;
        xwer_t rc;
        xwsync_evt_declare_bitmap(cur);
        xwsync_evt_declare_bitmap(tmp);

        XWOS_VALIDATE((origin), "nullptr", -EFAULT);

        xwbmpop_and(origin, msk, XWSYNC_EVT_MAXNUM);
        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_assign(cur, evt->bmp, XWSYNC_EVT_MAXNUM);
        xwbmpop_and(cur, msk, XWSYNC_EVT_MAXNUM);
        if (XWSYNC_EVT_TRIGGER_TGL_ALL == trigger) {
                xwbmpop_assign(tmp, cur, XWSYNC_EVT_MAXNUM);
                xwbmpop_xor(tmp, origin, XWSYNC_EVT_MAXNUM);
                cmprc = xwbmpop_cmp(tmp, msk, XWSYNC_EVT_MAXNUM);
                if (0 == cmprc) {
                        triggered = true;
                        rc = XWOK;
                } else {
                        triggered = false;
                        rc = -ENODATA;
                }
        } else if (XWSYNC_EVT_TRIGGER_TGL_ANY == trigger) {
                cmprc = xwbmpop_cmp(origin, cur, XWSYNC_EVT_MAXNUM);
                if (0 == cmprc) {
                        triggered = false;
                        rc = -ENODATA;
                } else {
                        triggered = true;
                        rc = XWOK;
                }
        } else {
                triggered = true;
                rc = -EINVAL;
        }
        if (triggered) {
                xwbmpop_assign(origin, cur, XWSYNC_EVT_MAXNUM);
                xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        } else {
                xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        }
        return rc;
}

/**
 * @brief XWOS API：测试事件对象中的位，事件对象类型为XWSYNC_EVT_TYPE_FLAG
 * @param evt: (I) 事件对象的指针
 * @param trigger: (I) 事件触发条件，取值 @ref xwsync_evt_trigger_em
 * @param action: (I) 事件触发后的动作，取值 @ref xwsync_evt_action_em
 *                    仅当trigger取值
 *                    @ref XWSYNC_EVT_TRIGGER_SET_ALL
 *                    @ref XWSYNC_EVT_TRIGGER_SET_ANY
 *                    @ref XWSYNC_EVT_TRIGGER_CLR_ALL
 *                    @ref XWSYNC_EVT_TRIGGER_CLR_ALL
 *                    时有效，其他情况不使用此参数，可填 @ref XWOS_UNUSED_ARGUMENT
 * @param origin: 指向缓冲区的指针：
 *                - 当trigger取值
 *                  @ref XWSYNC_EVT_TRIGGER_SET_ALL
 *                  @ref XWSYNC_EVT_TRIGGER_SET_ANY
 *                  @ref XWSYNC_EVT_TRIGGER_CLR_ALL
 *                  @ref XWSYNC_EVT_TRIGGER_CLR_ANY
 *                  (O) 返回事件对象中位图状态（action之前）
 *                - 当trigger取值
 *                  @ref XWSYNC_EVT_TRIGGER_TGL_ALL
 *                  @ref XWSYNC_EVT_TRIGGER_TGL_ANY
 *                  (I) 作为输入时，作为用于比较的初始值
 *                  (O) 作为输出时，返回事件对象中位图状态
 *                      （可作为下一次调用的初始值）
 * @param msk: (I) 事件对象的位图掩码，表示只关注掩码部分的位
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 事件对象类型错误
 * @retval -EINVAL: 参数无效
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwsync_evt_trywait(struct xwsync_evt * evt,
                          xwsq_t trigger, xwsq_t action,
                          xwbmp_t origin[], xwbmp_t msk[])
{
        xwer_t rc;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->attr & XWSYNC_EVT_TYPE_MASK) == XWSYNC_EVT_TYPE_FLAG),
                      "type-error", -ETYPE);

        rc = xwsync_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        if (trigger <= XWSYNC_EVT_TRIGGER_CLR_ANY) {
                rc = xwsync_evt_trywait_level(evt, trigger, action,
                                              origin, msk);
        } else {
                rc = xwsync_evt_trywait_edge(evt, trigger, origin, msk);
        }
        xwsync_evt_put(evt);

err_evt_grab:
        return rc;
}

static __xwos_code
xwer_t xwsync_evt_timedwait_level(struct xwsync_evt * evt,
                                  xwsq_t trigger, xwsq_t action,
                                  xwbmp_t origin[], xwbmp_t msk[],
                                  xwtm_t * xwtm)
{
        xwreg_t cpuirq;
        bool triggered;
        xwsq_t lkst;
        xwer_t rc;

        XWOS_VALIDATE((trigger <= XWSYNC_EVT_TRIGGER_CLR_ANY),
                      "illegal-trigger", -EINVAL);
        XWOS_VALIDATE((action < XWSYNC_EVT_ACTION_NUM),
                      "illegal-action", -EINVAL);

        rc = XWOK;
        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        while (true) {
                if (origin) {
                        xwbmpop_assign(origin, evt->bmp, XWSYNC_EVT_MAXNUM);
                }
                if (XWSYNC_EVT_ACTION_CONSUMPTION == action) {
                        switch (trigger) {
                        case XWSYNC_EVT_TRIGGER_SET_ALL:
                                triggered = xwbmpop_t1ma_then_c0m(evt->bmp, msk,
                                                                  XWSYNC_EVT_MAXNUM);
                                break;
                        case XWSYNC_EVT_TRIGGER_SET_ANY:
                                triggered = xwbmpop_t1mo_then_c0m(evt->bmp, msk,
                                                                  XWSYNC_EVT_MAXNUM);
                                break;
                        case XWSYNC_EVT_TRIGGER_CLR_ALL:
                                triggered = xwbmpop_t0ma_then_s1m(evt->bmp, msk,
                                                                  XWSYNC_EVT_MAXNUM);
                                break;
                        case XWSYNC_EVT_TRIGGER_CLR_ANY:
                                triggered = xwbmpop_t0mo_then_s1m(evt->bmp, msk,
                                                                  XWSYNC_EVT_MAXNUM);
                                break;
                        default:
                                triggered = true;
                                rc = -EINVAL;
                                break;
                        }
                } else {
                        switch (trigger) {
                        case XWSYNC_EVT_TRIGGER_SET_ALL:
                                triggered = xwbmpop_t1ma(evt->bmp, msk,
                                                         XWSYNC_EVT_MAXNUM);
                                break;
                        case XWSYNC_EVT_TRIGGER_SET_ANY:
                                triggered = xwbmpop_t1mo(evt->bmp, msk,
                                                         XWSYNC_EVT_MAXNUM);
                                break;
                        case XWSYNC_EVT_TRIGGER_CLR_ALL:
                                triggered = xwbmpop_t0ma(evt->bmp, msk,
                                                         XWSYNC_EVT_MAXNUM);
                                break;
                        case XWSYNC_EVT_TRIGGER_CLR_ANY:
                                triggered = xwbmpop_t0mo(evt->bmp, msk,
                                                         XWSYNC_EVT_MAXNUM);
                                break;
                        default:
                                triggered = true;
                                rc = -EINVAL;
                                break;
                        }
                }
                if (triggered) {
                        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        break;
                } else {
                        rc = xwsync_cdt_timedwait(&evt->cdt,
                                                  &evt->lock, XWLK_TYPE_SPLK, NULL,
                                                  xwtm, &lkst);
                        if (XWOK == rc) {
                                if (XWLK_STATE_UNLOCKED == lkst) {
                                        xwlk_splk_lock(&evt->lock);
                                }
                        } else {
                                if (XWLK_STATE_LOCKED == lkst) {
                                        xwlk_splk_unlock(&evt->lock);
                                }
                                xwos_cpuirq_restore_lc(cpuirq);
                                break;
                        }
                }
        }
        return rc;
}

static __xwos_code
xwer_t xwsync_evt_timedwait_edge(struct xwsync_evt * evt, xwsq_t trigger,
                                 xwbmp_t origin[], xwbmp_t msk[],
                                 xwtm_t * xwtm)
{
        xwreg_t cpuirq;
        xwsq_t lkst;
        xwssq_t cmprc;
        bool triggered;
        xwer_t rc;
        xwsync_evt_declare_bitmap(cur);
        xwsync_evt_declare_bitmap(tmp);

        XWOS_VALIDATE((origin), "nullptr", -EFAULT);

        xwbmpop_and(origin, msk, XWSYNC_EVT_MAXNUM);
        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        while (true) {
                xwbmpop_assign(cur, evt->bmp, XWSYNC_EVT_MAXNUM);
                xwbmpop_and(cur, msk, XWSYNC_EVT_MAXNUM);
                if (XWSYNC_EVT_TRIGGER_TGL_ALL == trigger) {
                        xwbmpop_assign(tmp, cur, XWSYNC_EVT_MAXNUM);
                        xwbmpop_xor(tmp, origin, XWSYNC_EVT_MAXNUM);
                        cmprc = xwbmpop_cmp(tmp, msk, XWSYNC_EVT_MAXNUM);
                        if (0 == cmprc) {
                                triggered = true;
                                rc = XWOK;
                        } else {
                                triggered = false;
                        }
                } else if (XWSYNC_EVT_TRIGGER_TGL_ANY == trigger) {
                        cmprc = xwbmpop_cmp(origin, cur, XWSYNC_EVT_MAXNUM);
                        if (0 == cmprc) {
                                triggered = false;
                        } else {
                                triggered = true;
                                rc = XWOK;
                        }
                } else {
                        triggered = true;
                        rc = -EINVAL;
                }
                if (triggered) {
                        xwbmpop_assign(origin, cur, XWSYNC_EVT_MAXNUM);
                        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        break;
                } else {
                        rc = xwsync_cdt_timedwait(&evt->cdt,
                                                  &evt->lock, XWLK_TYPE_SPLK, NULL,
                                                  xwtm, &lkst);
                        if (XWOK == rc) {
                                if (XWLK_STATE_UNLOCKED == lkst) {
                                        xwlk_splk_lock(&evt->lock);
                                }
                        } else {
                                if (XWLK_STATE_LOCKED == lkst) {
                                        xwlk_splk_unlock(&evt->lock);
                                }
                                xwos_cpuirq_restore_lc(cpuirq);
                                break;
                        }
                }
        }
        return rc;
}

/**
 * @brief XWOS API：限时等待事件对象中的位，事件对象类型为XWSYNC_EVT_TYPE_FLAG
 * @param evt: (I) 事件对象的指针
 * @param trigger: (I) 事件触发条件，取值 @ref xwsync_evt_trigger_em
 * @param action: (I) 事件触发后的动作，取值 @ref xwsync_evt_action_em
 *                    仅当trigger取值
 *                    @ref XWSYNC_EVT_TRIGGER_SET_ALL
 *                    @ref XWSYNC_EVT_TRIGGER_SET_ANY
 *                    @ref XWSYNC_EVT_TRIGGER_CLR_ALL
 *                    @ref XWSYNC_EVT_TRIGGER_CLR_ALL
 *                    时有效，其他情况不使用此参数，可填 @ref XWOS_UNUSED_ARGUMENT
 * @param origin: 指向缓冲区的指针：
 *                - 当trigger取值
 *                  @ref XWSYNC_EVT_TRIGGER_SET_ALL
 *                  @ref XWSYNC_EVT_TRIGGER_SET_ANY
 *                  @ref XWSYNC_EVT_TRIGGER_CLR_ALL
 *                  @ref XWSYNC_EVT_TRIGGER_CLR_ANY
 *                  (O) 返回事件对象中位图状态（action之前）
 *                - 当trigger取值
 *                  @ref XWSYNC_EVT_TRIGGER_TGL_ALL
 *                  @ref XWSYNC_EVT_TRIGGER_TGL_ANY
 *                  (I) 作为输入时，作为用于比较的初始值
 *                  (O) 作为输出时，返回事件对象中位图状态
 *                      （可作为下一次调用的初始值）
 * @param msk: (I) 事件对象的位图掩码，表示只关注掩码部分的位
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 事件对象类型错误
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
xwer_t xwsync_evt_timedwait(struct xwsync_evt * evt,
                            xwsq_t trigger, xwsq_t action,
                            xwbmp_t origin[], xwbmp_t msk[],
                            xwtm_t * xwtm)
{
        xwer_t rc;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->attr & XWSYNC_EVT_TYPE_MASK) == XWSYNC_EVT_TYPE_FLAG),
                      "type-error", -ETYPE);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHRD == xwos_irq_get_id(NULL)),
                      "not-in-thrd", -ENOTINTHRD);

        rc = xwsync_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }
        if (trigger <= XWSYNC_EVT_TRIGGER_CLR_ANY) {
                rc = xwsync_evt_timedwait_level(evt, trigger, action,
                                                origin, msk, xwtm);
        } else {
                rc = xwsync_evt_timedwait_edge(evt, trigger, origin, msk, xwtm);
        }
        xwsync_evt_put(evt);

err_evt_grab:
        return rc;
}

/******** type:XWSYNC_EVT_TYPE_SELECTOR ********/
/**
 * @brief 绑定同步对象到事件对象，事件对象类型为XWSYNC_EVT_TYPE_SELECTOR
 * @param evt: (I) 事件对象的指针
 * @param obj: (I) 同步对象的指针
 * @param pos: (I) 同步对象映射到位图中的位置
 * @param exclusive: (I) 是否为独占绑定
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ETYPE: 事件对象类型错误
 * @retval -ECHRNG: 位置超出范围
 * @retval -EALREADY: 同步对象已经绑定到事件对象
 * @retval -EBUSY: 通道已经被其他同步对象独占
 */
__xwos_code
xwer_t xwsync_evt_obj_bind(struct xwsync_evt * evt,
                           struct xwsync_object * obj,
                           xwsq_t pos,
                           bool exclusive)
{
        struct xwsync_evt * owner;
        xwreg_t cpuirq;
        bool existed;
        xwer_t rc;

        XWOS_VALIDATE(((evt->attr & XWSYNC_EVT_TYPE_MASK) == XWSYNC_EVT_TYPE_SELECTOR),
                      "type-error", -ETYPE);
        XWOS_VALIDATE((pos < XWSYNC_EVT_MAXNUM), "out-of-range", -ECHRNG);

        rc = xwsync_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }

        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        owner = obj->selector.evt;
        if (NULL != owner) {
                rc = -EALREADY;
                goto err_already;
        }
        existed = xwbmpop_t1i(evt->msk, pos);
        if (existed) {
                rc = -EBUSY;
                goto err_busy;
        }
        if (exclusive) {
                xwbmpop_s1i(evt->msk, pos);
        }
        obj->selector.evt = evt;
        obj->selector.pos = pos;
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return XWOK;

err_busy:
err_already:
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwsync_evt_put(evt);
err_evt_grab:
        return rc;
}

/**
 * @brief XWOS API：从事件对象上解绑同步对象，事件对象类型为XWSYNC_EVT_TYPE_SELECTOR
 * @param evt: (I) 事件对象的指针
 * @param obj: (I) 同步对象的指针
 * @param exclusive: (I) 是否为独占绑定
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ETYPE: 事件对象类型错误
 * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
 */
__xwos_code
xwer_t xwsync_evt_obj_unbind(struct xwsync_evt * evt,
                             struct xwsync_object * obj,
                             bool exclusive)
{
        struct xwsync_evt * owner;
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE(((evt->attr & XWSYNC_EVT_TYPE_MASK) == XWSYNC_EVT_TYPE_SELECTOR),
                      "type-error", -ETYPE);

        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        owner = obj->selector.evt;
        if (evt != owner) {
                rc = -ENOTCONN;
                goto err_notconn;
        }
        if (exclusive) {
                xwbmpop_c0i(evt->msk, obj->selector.pos);
        }
        obj->selector.evt = NULL;
        obj->selector.pos = XWSYNC_EVT_MAXNUM;
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwsync_evt_put(evt);
        return XWOK;

err_notconn:
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return rc;
}

/**
 * @brief 在事件对象上设置同步对象的标志位，事件对象类型为XWSYNC_EVT_TYPE_SELECTOR
 * @param evt: (I) 事件对象的指针
 * @param obj: (I) 同步对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
 */
__xwos_code
xwer_t xwsync_evt_obj_s1i(struct xwsync_evt * evt, struct xwsync_object * obj)
{
        struct xwsync_evt * owner;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwsync_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }

        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        owner = obj->selector.evt;
        if (evt != owner) {
                rc = -ENOTCONN;
                goto err_notconn;
        }
        xwbmpop_s1i(evt->bmp, obj->selector.pos);
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwsync_cdt_broadcast(&evt->cdt);
        xwsync_evt_put(evt);
        return XWOK;

err_notconn:
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwsync_evt_put(evt);
err_evt_grab:
        return rc;
}

/**
 * @brief 在事件对象上清除同步对象的标志位，事件对象类型为XWSYNC_EVT_TYPE_SELECTOR
 * @param evt: (I) 事件对象的指针
 * @param obj: (I) 同步对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
 */
__xwos_code
xwer_t xwsync_evt_obj_c0i(struct xwsync_evt * evt, struct xwsync_object * obj)
{
        struct xwsync_evt * owner;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwsync_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }

        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        owner = obj->selector.evt;
        if (evt != owner) {
                rc = -ENOTCONN;
                goto err_notconn;
        }
        xwbmpop_c0i(evt->bmp, obj->selector.pos);
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwsync_evt_put(evt);
        return XWOK;

err_notconn:
        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwsync_evt_put(evt);
err_evt_grab:
        return rc;
}

/**
 * @brief XWOS API：测试一下事件对象中绑定的同步对象，不进行阻塞等待
 * @param evt: (I) 事件对象的指针
 * @param msk: (I) 待触发的同步对象的位图掩码，表示只关注掩码部分的位
 * @param trg: (O) 指向缓冲区的指针，通过此缓冲区返回已触发的同步对象的位图
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 事件对象类型错误
 * @retval -ENODATA: 没有任何信号或事件
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwsync_evt_tryselect(struct xwsync_evt * evt, xwbmp_t msk[], xwbmp_t trg[])
{
        xwer_t rc;
        xwreg_t cpuirq;
        bool triggered;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->attr & XWSYNC_EVT_TYPE_MASK) == XWSYNC_EVT_TYPE_SELECTOR),
                      "type-error", -ETYPE);

        rc = xwsync_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }

        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        triggered = xwbmpop_t1mo(evt->bmp, msk, XWSYNC_EVT_MAXNUM);
        if (triggered) {
                if (NULL != trg) {
                        xwbmpop_assign(trg, evt->bmp, XWSYNC_EVT_MAXNUM);
                        /* Clear non-exclusive bits */
                        xwbmpop_and(evt->bmp, evt->msk, XWSYNC_EVT_MAXNUM);
                        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        xwbmpop_and(trg, msk, XWSYNC_EVT_MAXNUM);
                } else {
                        /* Clear non-exclusive bits */
                        xwbmpop_and(evt->bmp, evt->msk, XWSYNC_EVT_MAXNUM);
                        xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                }
                rc = XWOK;
        } else {
                rc = -ENODATA;
                xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        }
        xwsync_evt_put(evt);

err_evt_grab:
        return rc;
}

/**
 * @brief XWOS API：限时等待事件对象中绑定的同步对象，
 *                  事件对象类型为XWSYNC_EVT_TYPE_SELECTOR
 * @param evt: (I) 事件对象的指针
 * @param msk: (I) 待触发的同步对象的位图掩码（表示只关注掩码部分的同步对象）
 * @param trg: (O) 指向缓冲区的指针，通过此缓冲区返回已触发的同步对象的位图
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 事件对象类型错误
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
xwer_t xwsync_evt_timedselect(struct xwsync_evt * evt, xwbmp_t msk[], xwbmp_t trg[],
                              xwtm_t * xwtm)
{
        xwer_t rc;
        xwreg_t cpuirq;
        bool triggered;
        xwsq_t lkst;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->attr & XWSYNC_EVT_TYPE_MASK) == XWSYNC_EVT_TYPE_SELECTOR),
                      "type-error", -ETYPE);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHRD == xwos_irq_get_id(NULL)),
                      "not-in-thrd", -ENOTINTHRD);

        rc = xwsync_evt_grab(evt);
        if (rc < 0) {
                goto err_evt_grab;
        }

        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        while (true) {
                triggered = xwbmpop_t1mo(evt->bmp, msk, XWSYNC_EVT_MAXNUM);
                if (triggered) {
                        if (NULL != trg) {
                                xwbmpop_assign(trg, evt->bmp, XWSYNC_EVT_MAXNUM);
                                /* Clear non-exclusive bits */
                                xwbmpop_and(evt->bmp, evt->msk, XWSYNC_EVT_MAXNUM);
                                xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                                xwbmpop_and(trg, msk, XWSYNC_EVT_MAXNUM);
                        } else {
                                /* Clear non-exclusive bits */
                                xwbmpop_and(evt->bmp, evt->msk, XWSYNC_EVT_MAXNUM);
                                xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        }
                        rc = XWOK;
                        break;
                } else {
                        /* Clear non-exclusive bits */
                        xwbmpop_and(evt->bmp, evt->msk, XWSYNC_EVT_MAXNUM);
                        rc = xwsync_cdt_timedwait(&evt->cdt,
                                                  &evt->lock, XWLK_TYPE_SPLK, NULL,
                                                  xwtm, &lkst);
                        if (XWOK == rc) {
                                if (XWLK_STATE_UNLOCKED == lkst) {
                                        xwlk_splk_lock(&evt->lock);
                                }
                        } else {
                                if (XWLK_STATE_LOCKED == lkst) {
                                        xwlk_splk_unlock(&evt->lock);
                                }
                                xwos_cpuirq_restore_lc(cpuirq);
                                if (NULL != trg) {
                                        xwbmpop_c0all(trg, XWSYNC_EVT_MAXNUM);
                                }
                                break;
                        }
                }
        }
        xwsync_evt_put(evt);

err_evt_grab:
        return rc;
}

/******** type:XWSYNC_EVT_TYPE_BARRIER ********/
/**
 * @brief XWOS API：等待所有线程到达屏障，事件对象类型为XWSYNC_EVT_TYPE_BARRIER
 * @param evt: (I) 事件对象的指针
 * @param pos: (I) 当前线程的位图位置
 * @param sync: (I) 当前线程需要同步的线程掩码
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ECHRNG: 位置超出范围
 * @retval -ETYPE: 事件对象类型错误
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
xwer_t xwsync_evt_timedsync(struct xwsync_evt * evt, xwsq_t pos, xwbmp_t sync[],
                            xwtm_t * xwtm)
{
        xwreg_t cpuirq;
        bool triggered;
        xwsq_t lkst;
        xwer_t rc;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((sync), "nullptr", -EFAULT);
        XWOS_VALIDATE((pos < XWSYNC_EVT_MAXNUM), "out-of-range", -ECHRNG);
        XWOS_VALIDATE(((evt->attr & XWSYNC_EVT_TYPE_MASK) == XWSYNC_EVT_TYPE_BARRIER),
                      "type-error", -ETYPE);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHRD == xwos_irq_get_id(NULL)),
                      "not-in-thrd", -ENOTINTHRD);

        rc = xwsync_evt_grab(evt);
        if (__unlikely(rc < 0)) {
                goto err_evt_grab;
        }

        xwlk_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_s1i(evt->bmp, pos);
        triggered = xwbmpop_t1ma(evt->bmp, sync, XWSYNC_EVT_MAXNUM);
        if (triggered) {
                xwbmpop_c0i(evt->bmp, pos);
                xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                xwsync_cdt_broadcast(&evt->cdt);
                xwos_cthrd_yield();
                rc = XWOK;
        } else {
                rc = xwsync_cdt_timedwait(&evt->cdt,
                                          &evt->lock, XWLK_TYPE_SPLK, NULL,
                                          xwtm, &lkst);
                if (XWLK_STATE_UNLOCKED == lkst) {
                        xwlk_splk_lock(&evt->lock);
                }
                xwbmpop_c0i(evt->bmp, pos);
                xwlk_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        }
        xwsync_evt_put(evt);

err_evt_grab:
        return rc;
}
