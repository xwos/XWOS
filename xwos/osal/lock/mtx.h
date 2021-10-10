/**
 * @file
 * @brief 操作系统抽象层：互斥锁
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_osal_lock_mtx_h__
#define __xwos_osal_lock_mtx_h__

#include <xwos/standard.h>
#include <xwos/osal/jack/lock/mtx.h>
#include <xwos/osal/skd.h>

/**
 * @defgroup xwos_lock_mtx 互斥锁
 * @ingroup xwos_lock
 * @{
 */

/**
 * @brief XWOS API：互斥锁
 */
struct xwos_mtx {
        struct xwosdl_mtx osmtx;
};

/**
 * @brief XWOS API：互斥锁对象描述符
 */
typedef struct {
        struct xwos_mtx * mtx; /**< 互斥锁对象的指针 */
        xwsq_t tik; /**< 标签 */
} xwos_mtx_d;

/**
 * @brief XWOS API：空的互斥锁对象描述符
 */
#define XWOS_MTX_NILD ((xwos_mtx_d){NULL, 0,})

/**
 * @brief XWOS API：静态方式初始化互斥锁
 * @param[in] mtx: 互斥锁的指针
 * @param[in] sprio: 互斥锁的静态优先级
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EINVAL: 无效参数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwos_mtx_init(struct xwos_mtx * mtx, xwpr_t sprio)
{
        return xwosdl_mtx_init(&mtx->osmtx, sprio);
}

/**
 * @brief XWOS API：销毁静态方式初始化的互斥锁
 * @param[in] mtx: 互斥锁的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwos_mtx_fini(struct xwos_mtx * mtx)
{
        return xwosdl_mtx_fini(&mtx->osmtx);
}

/**
 * @brief XWOS API：动态方式创建互斥锁
 * @param[out] mtxbuf: 指向缓冲区的指针，通过此缓冲区返回互斥锁的指针
 * @param[in] sprio: 互斥锁的静态优先级
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EINVAL: 无效参数
 * @retval -ENOMEM: 内存不足
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwos_mtx_create(struct xwos_mtx ** mtxbuf, xwpr_t sprio)
{
        return xwosdl_mtx_create((struct xwosdl_mtx **)mtxbuf, sprio);
}

/**
 * @brief XWOS API：删除动态方式创建的互斥锁
 * @param[in] mtx: 互斥锁指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwos_mtx_delete(struct xwos_mtx * mtx)
{
        return xwosdl_mtx_delete(&mtx->osmtx);
}

/**
 * @brief XWOS API：获取互斥锁对象的标签
 * @param[in] mtx: 互斥锁对象的指针
 * @return 互斥锁对象的标签
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwsq_t xwos_mtx_gettik(struct xwos_mtx * mtx)
{
        return xwosdl_mtx_gettik(&mtx->osmtx);
}

/**
 * @brief XWOS API：获取互斥锁对象的描述符
 * @param[in] mtx: 互斥锁对象的指针
 * @return 互斥锁对象的描述符
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwos_mtx_d xwos_mtx_getd(struct xwos_mtx * mtx)
{
        xwos_mtx_d mtxd;

        mtxd.mtx = mtx;
        mtxd.tik = xwosdl_mtx_gettik(&mtx->osmtx);
        return mtxd;
}

/**
 * @brief XWOS API：检查互斥锁对象的标签并增加引用计数
 * @param[in] mtxd: 互斥锁对象的描述符
 * @return 错误码
 * @retval XWOK: OK
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_mtx_acquire(xwos_mtx_d mtxd)
{
        return xwosdl_mtx_acquire(&mtxd.mtx->osmtx, mtxd.tik);
}

/**
 * @brief XWOS API：检查对象的标签并减少引用计数
 * @param[in] mtxd: 互斥锁对象的描述符
 * @return 错误码
 * @retval XWOK: OK
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_mtx_release(xwos_mtx_d mtxd)
{
        return xwosdl_mtx_release(&mtxd.mtx->osmtx, mtxd.tik);
}

/**
 * @brief XWOS API：增加互斥锁对象的引用计数
 * @param[in] mtx: 互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: OK
 * @retval -EOBJDEAD: 对象无效
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_mtx_grab(struct xwos_mtx * mtx)
{
        return xwosdl_mtx_grab(&mtx->osmtx);
}

/**
 * @brief XWOS API：减少互斥锁对象的引用计数
 * @param[in] mtx: 互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: OK
 * @retval -EOBJDEAD: 对象无效
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_mtx_put(struct xwos_mtx * mtx)
{
        return xwosdl_mtx_put(&mtx->osmtx);
}

/**
 * @brief XWOS API：解锁互斥锁
 * @param[in] mtx: 互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EOWNER: 线程并没有锁定此互斥锁
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwos_mtx_unlock(struct xwos_mtx * mtx)
{
        return xwosdl_mtx_unlock(&mtx->osmtx);
}

/**
 * @brief XWOS API：等待上锁互斥锁
 * @param[in] mtx: 互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwos_mtx_lock(struct xwos_mtx * mtx)
{
        return xwosdl_mtx_lock(&mtx->osmtx);
}

/**
 * @brief XWOS API：尝试上锁互斥锁，不会阻塞调用线程
 * @param[in] mtx: 互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ENODATA: 获取锁失败
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @note
 * - 此函数虽然不会阻塞调用线程，但也不可在中断上下文中使用，因为在OS中
 *   互斥锁通常需要记录自己的拥有者（一般是线程对象结构体），
 *   中断上下文不存在对应的线程对象结构体。
 */
static __xwos_inline_api
xwer_t xwos_mtx_trylock(struct xwos_mtx * mtx)
{
        return xwosdl_mtx_trylock(&mtx->osmtx);
}

/**
 * @brief XWOS API：限时等待上锁互斥锁
 * @param[in] mtx: 互斥锁对象的指针
 * @param[in,out] xwtm: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示期望的阻塞等待时间
 * + (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EINTR: 等待被中断
 * @retval -ETIMEDOUT: 超时，xwtm指向的缓冲区内的时间会减为0
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwos_mtx_timedlock(struct xwos_mtx * mtx, xwtm_t * xwtm)
{
        return xwosdl_mtx_timedlock(&mtx->osmtx, xwtm);
}

/**
 * @brief XWOS API：等待上锁互斥锁，且等待不可被中断
 * @param[in] mtx: 互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwos_mtx_lock_unintr(struct xwos_mtx * mtx)
{
        return xwosdl_mtx_lock_unintr(&mtx->osmtx);
}

/**
 * @brief XWOS API：获取锁的状态
 * @param[in] mtx: 互斥锁对象的指针
 * @param[out] lkst: 指向缓冲区的指针，通过此缓冲区返回锁的状态
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_mtx_getlkst(struct xwos_mtx * mtx, xwsq_t * lkst)
{
        return xwosdl_mtx_getlkst(&mtx->osmtx, lkst);
}

/**
 * @} xwos_lock_mtx
 */

#endif /* xwos/osal/lock/mtx.h */
