/**
 * @file
 * @brief 消息队列：应用接口层
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/lib/object.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#if defined(XWMDCFG_isc_xwmq_MEMSLICE) && (1 == XWMDCFG_isc_xwmq_MEMSLICE)
  #include <xwos/mm/memslice.h>
#elif defined(XWMDCFG_isc_xwmq_STDC_MM) && (1 == XWMDCFG_isc_xwmq_STDC_MM)
  #include <stdlib.h>
#endif /* XWMDCFG_isc_xwmq_MEMSLICE */
#include <xwmd/isc/xwmq/mif.h>

/**
 * @brief 消息队列对象的构造函数
 * @param mq: (I) 消息队列对象的指针
 */
static __xwmd_code
void xwmq_construct(struct xwmq * mq)
{
        xwos_object_construct(&mq->xwobj);
        mq->name = NULL;
        xwlib_bclst_init_head(&mq->head);
        xwos_splk_init(&mq->lock);
        xwos_sem_init(&mq->sem, 0, XWSSQ_MAX);
}

/**
 * @brief 消息队列对象的析构函数
 * @param mq: (I) 消息队列对象的指针
 */
static __xwmd_code
void xwmq_destruct(struct xwmq * mq)
{
        xwos_sem_destroy(&mq->sem);
        xwos_object_destruct(&mq->xwobj);
}

#if defined(XWMDCFG_isc_xwmq_MEMSLICE) && (1 == XWMDCFG_isc_xwmq_MEMSLICE)
/**
 * @brief 消息队列对象缓存
 */
__xwmd_data static struct xwmm_memslice * xwmq_cache = NULL;

/**
 * @brief 消息队列对象缓存的名字
 */
__xwmd_rodata const char xwmq_cache_name[] = "xwmd.mq.cache";
#endif /* XWMDCFG_isc_xwmq_MEMSLICE */

#if defined(XWMDCFG_isc_xwmq_MEMSLICE) && (1 == XWMDCFG_isc_xwmq_MEMSLICE)
/**
 * @brief XWMD INIT CODE：初始化消息队列对象缓存
 * @param zone_origin: (I) 内存区域首地址
 * @param zone_size: (I) 内存区域大小
 * @return 错误码
 * @note
 * - 重入性：只可在系统初始化时使用一次
 */
__xwmd_init_code
xwer_t xwmq_cache_init(xwptr_t zone_origin, xwsz_t zone_size)
{
        struct xwmm_memslice * msa;
        xwer_t rc;

        rc = xwmm_memslice_create(&msa, zone_origin, zone_size,
                                  sizeof(struct xwmq),
                                  xwmq_cache_name,
                                  (ctor_f)xwmq_construct,
                                  (dtor_f)xwmq_destruct);
        if (__xwcc_unlikely(rc < 0)) {
                xwmq_cache = NULL;
        } else {
                xwmq_cache = msa;
        }
        return rc;
}
#endif /* XWMDCFG_isc_xwmq_MEMSLICE */

/**
 * @brief 从消息队列对象缓存中申请一个对象
 * @return 消息队列对象的指针
 */
static __xwmd_code
struct xwmq * xwmq_alloc(void)
{
#if defined(XWMDCFG_isc_xwmq_MEMSLICE) && (1 == XWMDCFG_isc_xwmq_MEMSLICE)
        union {
                struct xwmq * mq;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_memslice_alloc(xwmq_cache, &mem.anon);
        if (rc < 0) {
                mem.mq = err_ptr(rc);
        }/* else {} */
        return mem.mq;
#elif defined(XWMDCFG_isc_xwmq_STDC_MM) && (1 == XWMDCFG_isc_xwmq_STDC_MM)
        struct xwmq * mq;

        mq = malloc(sizeof(struct xwmq));
        if (NULL == mq) {
                mq = (struct xwmq *)err_ptr(-ENOMEM);
        }
        xwmq_construct(mq);
        return mq;
#else /* !XWMDCFG_isc_xwmq_STDC_MM */
        union {
                struct xwmq * mq;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwmq), XWMM_ALIGNMENT, &mem.anon);
        if (XWOK == rc) {
                xwmq_construct(mem.mq);
        } else {
                mem.mq = err_ptr(-ENOMEM);
        }
        return mem.mq;
#endif /* XWMDCFG_isc_xwmq_MEMSLICE */
}

/**
 * @brief 释放消息队列对象
 * @param mq: (I) 消息队列对象的指针
 */
static __xwmd_code
void xwmq_free(struct xwmq * mq)
{
#if defined(XWMDCFG_isc_xwmq_MEMSLICE) && (1 == XWMDCFG_isc_xwmq_MEMSLICE)
        xwmm_memslice_free(xwmq_cache, mq);
#elif defined(XWMDCFG_isc_xwmq_STDC_MM) && (1 == XWMDCFG_isc_xwmq_STDC_MM)
        xwmq_destruct(mq);
        free(mq);
#else /* XWMDCFG_isc_xwmq_MEMSLICE */
        xwmq_destruct(mq);
        xwmm_kma_free(mq);
#endif /* !XWMDCFG_isc_xwmq_MEMSLICE */
}

/**
 * @brief 消息队列对象的垃圾回收函数
 * @param mq: (I) 消息队列对象的指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwmq_gc(void * mq)
{
        xwmq_free(mq);
        return XWOK;
}

/**
 * @brief XWMQ API：获取消息队列对象的标签
 * @param mq: (I) 消息队列对象的指针
 * @return 消息队列对象的标签
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
xwsq_t xwmq_gettik(struct xwmq * mq)
{
        xwsq_t tik;

        if (mq) {
                tik = mq->xwobj.tik;
        } else {
                tik = 0;
        }
        return tik;
}

/**
 * @brief XWMQ API：检查消息队列对象的标签并增加引用计数
 * @param mq: (I) 消息队列对象指针
 * @param tik: (I) 标签
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwmq_acquire(struct xwmq * mq, xwsq_t tik)
{
        XWOS_VALIDATE((mq), "nullptr", -EFAULT);
        return xwos_object_acquire(&mq->xwobj, tik);
}

/**
 * @brief XWMQ API：检查消息队列对象的标签并增加引用计数
 * @param mq: (I) 消息队列对象指针
 * @param tik: (I) 标签
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwmq_release(struct xwmq * mq, xwsq_t tik)
{
        XWOS_VALIDATE((mq), "nullptr", -EFAULT);
        return xwos_object_release(&mq->xwobj, tik);
}

/**
 * @brief XWMQ API：增加消息队列对象的引用计数
 * @param mq: (I) 消息队列控制块对象指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwmq_grab(struct xwmq * mq)
{
        XWOS_VALIDATE((mq), "nullptr", -EFAULT);
        return xwos_object_grab(&mq->xwobj);
}

/**
 * @brief XWMQ API：减少消息队列对象的引用计数
 * @param mq: (I) 消息队列控制块对象指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwmq_put(struct xwmq * mq)
{
        XWOS_VALIDATE((mq), "nullptr", -EFAULT);
        return xwos_object_put(&mq->xwobj);
}

/**
 * @brief 激活消息队列对象
 * @param mq: (I) 消息队列对象的指针
 * @param name: (I) 名字
 * @param gcfunc: (I) 垃圾回收函数：当对象应用计数为0，调用此函数回收资源。
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwmd_code
xwer_t xwmq_activate(struct xwmq * mq, const char * name, xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwos_object_activate(&mq->xwobj, gcfunc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_xwobj_activate;
        }
        mq->name = name;
        return XWOK;

err_xwobj_activate:
        return rc;
}

/**
 * @brief XWMQ API：静态方式初始化消息队列
 * @param mq: (I) 消息队列对象的指针
 * @param name: (I) 名字
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmd_api
xwer_t xwmq_init(struct xwmq * mq, const char * name)
{
        XWOS_VALIDATE((mq), "nullptr", -EFAULT);
        xwmq_construct(mq);
        return xwmq_activate(mq, name, NULL);
}

/**
 * @brief XWMQ API：销毁静态方式初始化的消息队列对象
 * @param mq: (I) 消息队列对象的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmd_api
xwer_t xwmq_destroy(struct xwmq * mq)
{
        XWOS_VALIDATE((mq), "nullptr", -EFAULT);
        return xwmq_put(mq);
}

/**
 * @brief XWMQ API：动态创建消息队列对象
 * @param ptrbuf: (O) 指向缓冲区的指针，此缓冲区用于返回消息队列对象的指针
 * @param name: (I) 名字
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOMEM: 内存不足
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmd_api
xwer_t xwmq_create(struct xwmq ** ptrbuf, const char * name)
{
        struct xwmq * mq;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);

        *ptrbuf = NULL;
        mq = xwmq_alloc();
        if (is_err(mq)) {
                rc = ptr_err(mq);
                goto err_mq_alloc;
        }
        rc = xwmq_activate(mq, name, xwmq_gc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mq_activate;
        }
        *ptrbuf = mq;
        return XWOK;

err_mq_activate:
        xwmq_free(mq);
err_mq_alloc:
        return rc;
}

/**
 * @brief XWMQ API：删除动态创建的消息队列对象
 * @param mq: (I) 消息队列对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmd_api
xwer_t xwmq_delete(struct xwmq * mq)
{
        XWOS_VALIDATE((mq), "nullptr", -EFAULT);
        return xwmq_put(mq);
}

/******** ******** xwmq_msg ******** ********/

/**
 * @brief 消息对象的构造函数
 * @param msg: (I) 消息对象的指针
 */
static __xwmd_code
void xwmq_msg_construct(struct xwmq_msg * msg)
{
        xwos_object_construct(&msg->xwobj);
        xwlib_bclst_init_node(&msg->node);
        msg->mq = NULL;
        msg->topic = 0;
        msg->data = NULL;
}

/**
 * @brief 消息对象的析构函数
 * @param msg: (I) 消息对象的指针
 */
static __xwmd_code
void xwmq_msg_destruct(struct xwmq_msg * msg)
{
        xwos_object_destruct(&msg->xwobj);
}

#if defined(XWMDCFG_isc_xwmq_MEMSLICE) && (1 == XWMDCFG_isc_xwmq_MEMSLICE)
/**
 * @brief 消息对象缓存
 */
__xwmd_data static struct xwmm_memslice * xwmq_msg_cache = NULL;

/**
 * @brief 消息对象缓存的名字
 */
__xwmd_rodata const char xwmq_msg_cache_name[] = "xwmd.mq.msg.cache";
#endif /* XWMDCFG_isc_xwmq_MEMSLICE */

#if defined(XWMDCFG_isc_xwmq_MEMSLICE) && (1 == XWMDCFG_isc_xwmq_MEMSLICE)
/**
 * @brief XWMD INIT CODE：初始化消息对象缓存
 * @param zone_origin: (I) 内存区域首地址
 * @param zone_size: (I) 内存区域大小
 * @return 错误码
 * @note
 * - 重入性：只可在系统初始化时使用一次
 */
__xwmd_init_code
xwer_t xwmq_msg_cache_init(xwptr_t zone_origin, xwsz_t zone_size)
{
        struct xwmm_memslice * msa;
        xwer_t rc;

        rc = xwmm_memslice_create(&msa, zone_origin, zone_size,
                                  sizeof(struct xwmq_msg),
                                  xwmq_msg_cache_name,
                                  (ctor_f)xwmq_msg_construct,
                                  (dtor_f)xwmq_msg_destruct);
        if (__xwcc_unlikely(rc < 0)) {
                xwmq_msg_cache = NULL;
        } else {
                xwmq_msg_cache = msa;
        }
        return rc;
}
#endif /* XWMDCFG_isc_xwmq_MEMSLICE */

/**
 * @brief 从消息对象缓存中申请一个对象
 * @return 消息对象的指针
 */
static __xwmd_code
struct xwmq_msg * xwmq_msg_alloc(void)
{
#if defined(XWMDCFG_isc_xwmq_MEMSLICE) && (1 == XWMDCFG_isc_xwmq_MEMSLICE)
        union {
                struct xwmq_msg * msg;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_memslice_alloc(xwmq_msg_cache, &mem.anon);
        if (rc < 0) {
                mem.msg = err_ptr(rc);
        }/* else {} */
        return mem.msg;
#elif defined(XWMDCFG_isc_xwmq_STDC_MM) && (1 == XWMDCFG_isc_xwmq_STDC_MM)
        struct xwmq_msg * msg;

        msg = malloc(sizeof(struct xwmq_msg));
        if (NULL == msg) {
                msg = (struct xwmq_msg *)err_ptr(-ENOMEM);
        }
        xwmq_msg_construct(msg);
        return msg;
#else
        union {
                struct xwmq_msg * msg;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwmq_msg), XWMM_ALIGNMENT, &mem.anon);
        if (XWOK == rc) {
                xwmq_msg_construct(mem.msg);
        } else {
                mem.msg = err_ptr(-ENOMEM);
        }
        return mem.msg;
#endif
}

/**
 * @brief 释放消息对象
 * @param msg: (I) 消息对象的指针
 */
static __xwmd_code
void xwmq_msg_free(struct xwmq_msg * msg)
{
#if defined(XWMDCFG_isc_xwmq_MEMSLICE) && (1 == XWMDCFG_isc_xwmq_MEMSLICE)
        xwmm_memslice_free(xwmq_msg_cache, msg);
#elif defined(XWMDCFG_isc_xwmq_STDC_MM) && (1 == XWMDCFG_isc_xwmq_STDC_MM)
        xwmq_msg_destruct(msg);
        free(msg);
#else
        xwmq_msg_destruct(msg);
        xwmm_kma_free(msg);
#endif
}

/**
 * @brief 消息对象的垃圾回收函数
 * @param msg: (I) 消息对象的指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwmq_msg_gc(void * msg)
{
        xwmq_msg_free(msg);
        return XWOK;
}

/**
 * @brief XWMQ API：获取消息对象的标签
 * @param msg: (I) 消息对象的指针
 * @return 消息对象的标签
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
xwsq_t xwmq_msg_gettik(struct xwmq_msg * msg)
{
        xwsq_t tik;

        if (msg) {
                tik = msg->xwobj.tik;
        } else {
                tik = 0;
        }
        return tik;
}

/**
 * @brief XWMQ API：检查消息对象的标签并增加引用计数
 * @param msg: (I) 消息对象指针
 * @param tik: (I) 标签
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwmq_msg_acquire(struct xwmq_msg * msg, xwsq_t tik)
{
        XWOS_VALIDATE((msg), "nullptr", -EFAULT);
        return xwos_object_acquire(&msg->xwobj, tik);
}

/**
 * @brief XWMQ API：检查消息对象的标签并增加引用计数
 * @param msg: (I) 消息对象指针
 * @param tik: (I) 标签
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwmq_msg_release(struct xwmq_msg * msg, xwsq_t tik)
{
        XWOS_VALIDATE((msg), "nullptr", -EFAULT);
        return xwos_object_release(&msg->xwobj, tik);
}

/**
 * @brief XWMQ API：增加消息对象的引用计数
 * @param msg: (I) 消息控制块对象指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwmq_msg_grab(struct xwmq_msg * msg)
{
        XWOS_VALIDATE((msg), "nullptr", -EFAULT);
        return xwos_object_grab(&msg->xwobj);
}

/**
 * @brief XWMQ API：减少消息对象的引用计数
 * @param msg: (I) 消息控制块对象指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwmq_msg_put(struct xwmq_msg * msg)
{
        XWOS_VALIDATE((msg), "nullptr", -EFAULT);
        return xwos_object_put(&msg->xwobj);
}

/**
 * @brief 激活消息对象
 * @param msg: (I) 消息对象的指针
 * @param gcfunc: (I) 垃圾回收函数：当对象应用计数为0，调用此函数回收资源。
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwmd_code
xwer_t xwmq_msg_activate(struct xwmq_msg * msg, xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwos_object_activate(&msg->xwobj, gcfunc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_xwobj_activate;
        }
        return XWOK;

err_xwobj_activate:
        return rc;
}

/**
 * @brief XWMQ API：静态方式初始化消息
 * @param msg: (I) 消息对象的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmd_api
xwer_t xwmq_msg_init(struct xwmq_msg * msg)
{
        XWOS_VALIDATE((msg), "nullptr", -EFAULT);
        xwmq_msg_construct(msg);
        return xwmq_msg_activate(msg, NULL);
}

/**
 * @brief XWMQ API：销毁静态方式初始化的消息对象
 * @param msg: (I) 消息对象的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmd_api
xwer_t xwmq_msg_destroy(struct xwmq_msg * msg)
{
        XWOS_VALIDATE((msg), "nullptr", -EFAULT);
        return xwmq_msg_put(msg);
}

/**
 * @brief XWMQ API：动态创建消息对象
 * @param ptrbuf: (O) 指向缓冲区的指针，此缓冲区用于返回消息对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOMEM: 内存不足
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmd_api
xwer_t xwmq_msg_create(struct xwmq_msg ** ptrbuf)
{
        struct xwmq_msg * msg;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);

        *ptrbuf = NULL;
        msg = xwmq_msg_alloc();
        if (is_err(msg)) {
                rc = ptr_err(msg);
                goto err_mq_alloc;
        }
        rc = xwmq_msg_activate(msg, xwmq_msg_gc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mq_activate;
        }
        *ptrbuf = msg;
        return XWOK;

err_mq_activate:
        xwmq_msg_free(msg);
err_mq_alloc:
        return rc;
}

/**
 * @brief XWMQ API：删除动态创建的消息对象
 * @param msg: (I) 消息对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmd_api
xwer_t xwmq_msg_delete(struct xwmq_msg * msg)
{
        XWOS_VALIDATE((msg), "nullptr", -EFAULT);
        return xwmq_msg_put(msg);
}

/**
 * @brief XWMQ API：将消息放入到消息队列的尾端（入队）
 * @param mq: (I) 消息队列对象的指针
 * @param msg: (I) 消息对象的指针
 * @param topic: (I) 消息的标题
 * @param data: (I) 消息的数据
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmd_api
xwer_t xwmq_eq(struct xwmq * mq, struct xwmq_msg * msg, int topic, void * data)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((mq), "nullptr", -EFAULT);
        XWOS_VALIDATE((msg), "nullptr", -EFAULT);

        rc = xwmq_grab(mq);
        if (rc < 0) {
                goto err_mq_grab;
        }
        rc = xwmq_msg_grab(msg);
        if (rc < 0) {
                goto err_msg_grab;
        }

        msg->topic = topic;
        msg->data = data;
        xwos_splk_lock_cpuirqsv(&mq->lock, &cpuirq);
        msg->mq = mq;
        xwlib_bclst_add_tail(&mq->head, &msg->node);
        xwos_splk_unlock_cpuirqrs(&mq->lock, cpuirq);
        rc = xwos_sem_post(&mq->sem);
        XWOS_BUG_ON(rc < 0);
        return XWOK;

err_msg_grab:
        xwmq_put(mq);
err_mq_grab:
        return rc;
}

/**
 * @brief XWMQ API：将消息放入到消息队列的首端（插队）
 * @param mq: (I) 消息队列对象的指针
 * @param msg: (I) 消息对象的指针
 * @param topic: (I) 消息的标题
 * @param data: (I) 消息的数据
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmd_api
xwer_t xwmq_jq(struct xwmq * mq, struct xwmq_msg * msg, int topic, void * data)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((mq), "nullptr", -EFAULT);
        XWOS_VALIDATE((msg), "nullptr", -EFAULT);

        rc = xwmq_grab(mq);
        if (rc < 0) {
                goto err_mq_grab;
        }
        rc = xwmq_msg_grab(msg);
        if (rc < 0) {
                goto err_msg_grab;
        }

        msg->topic = topic;
        msg->data = data;
        xwos_splk_lock_cpuirqsv(&mq->lock, &cpuirq);
        msg->mq = mq;
        xwlib_bclst_add_head(&mq->head, &msg->node);
        xwos_splk_unlock_cpuirqrs(&mq->lock, cpuirq);
        rc = xwos_sem_post(&mq->sem);
        XWOS_BUG_ON(rc < 0);
        return XWOK;

err_msg_grab:
        xwmq_put(mq);
err_mq_grab:
        return rc;
}

static __xwmd_code
struct xwmq_msg * xwmq_choose(struct xwmq * mq)
{
        struct xwmq_msg * msg;
        xwreg_t cpuirq;

        xwos_splk_lock_cpuirqsv(&mq->lock, &cpuirq);
        msg = xwlib_bclst_first_entry(&mq->head,
                                      struct xwmq_msg,
                                      node);
        xwlib_bclst_del_init(&msg->node);
        msg->mq = NULL;
        xwos_splk_unlock_cpuirqrs(&mq->lock, cpuirq);
        return msg;
}

/**
 * @brief XWMQ API: 从消息队列头部取出一条消息，若消息队列为空，就无限等待
 * @param mq: (I) 消息队列对象的指针
 * @param ptrbuf: (O) 指向缓冲区的指针，此缓冲区用于接收消息
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwmq_dq(struct xwmq * mq, struct xwmq_msg ** ptrbuf)
{
        xwtm_t expected;

        expected = XWTM_MAX;
        return xwmq_timedq(mq, ptrbuf, &expected);
}

/**
 * @brief XWMQ API: 从消息队列头部取出一条消息，若消息队列为空，就限时等待
 * @param mq: (I) 消息队列对象的指针
 * @param ptrbuf: (O) 指向缓冲区的指针，此缓冲区用于接收消息
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETIMEDOUT: 超时
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwmq_timedq(struct xwmq * mq, struct xwmq_msg ** ptrbuf, xwtm_t * xwtm)
{
        xwer_t rc;
        struct xwmq_msg * msg;

        XWOS_VALIDATE((mq), "nullptr", -EFAULT);
        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);

        rc = xwos_sem_timedwait(&mq->sem, xwtm);
        if (rc < 0) {
                goto err_sem_timedwait;
        }
        msg = xwmq_choose(mq);
        *ptrbuf = msg;
        xwmq_msg_put(msg);
        xwmq_put(mq);
        return XWOK;

err_sem_timedwait:
        return rc;
}

/**
 * @brief XWMQ API: 尝试从消息队列头部取出一条消息，若消息队列为空，立即返回错误码
 * @param mq: (I) 消息队列对象的指针
 * @param ptrbuf: (O) 指向缓冲区的指针，此缓冲区用于接收消息
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENODATA: 接收队列为空
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwmq_trydq(struct xwmq * mq, struct xwmq_msg ** ptrbuf)
{
        xwer_t rc;
        struct xwmq_msg * msg;

        XWOS_VALIDATE((mq), "nullptr", -EFAULT);
        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);

        rc = xwos_sem_trywait(&mq->sem);
        if (rc < 0) {
                goto err_sem_trywait;
        }
        msg = xwmq_choose(mq);
        *ptrbuf = msg;
        xwmq_msg_put(msg);
        xwmq_put(mq);
        return XWOK;

err_sem_trywait:
        return rc;
}