/**
 * @file
 * @brief 操作系统抽象层：互斥锁
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_osal_lock_mtx_h__
#define __xwos_osal_lock_mtx_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/osal/jack/lock/mtx.h>
#include <xwos/osal/skd.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOS API：互斥锁
 */
struct xwos_mtx {
        struct xwosdl_mtx osmtx;
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       APIs        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOS API：静态方式初始化互斥锁
 * @param mtx: (I) 互斥锁的指针
 * @param sprio: (I) 互斥锁的静态优先级
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
 * @param mtx: (I) 互斥锁的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwos_mtx_destroy(struct xwos_mtx * mtx)
{
        return xwosdl_mtx_destroy(&mtx->osmtx);
}

/**
 * @brief XWOS API：动态方式创建互斥锁
 * @param mtxp: (O) 指向缓冲区的指针，通过此缓冲区返回互斥锁的指针
 * @param sprio: (I) 互斥锁的静态优先级
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
xwer_t xwos_mtx_create(struct xwos_mtx ** mtxp, xwpr_t sprio)
{
        return xwosdl_mtx_create((struct xwosdl_mtx **)mtxp, sprio);
}

/**
 * @brief XWOS API：删除动态方式创建的互斥锁
 * @param mtx: (I) 互斥锁指针
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
 * @brief XWOS API：解锁互斥锁
 * @param mtx: (I) 互斥锁指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EOWNER: 线程并没有锁定此互斥锁
 * @retval -ENOTINTHRD: 不在线程上下文中
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
 * @param mtx: (I) 互斥锁指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHRD: 不在线程上下文中
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
 * @brief XWOS API：尝试上锁互斥锁，不会阻塞调用者
 * @param mtx: (I) 互斥锁指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ENODATA: 获取锁失败
 * @retval -ENOTINTHRD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @note
 * - 此函数虽然不会阻塞调用者，但也不可在中断上下文中使用，因为在OS中
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
 * @param mtx: (I) 互斥锁指针
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EINTR: 等待被中断
 * @retval -ETIMEDOUT: 超时
 * @retval -ENOTINTHRD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @note
 * - 函数返回**-ETIMEDOUT**时，**xwtm**指向的缓冲区内的值减为0。
 */
static __xwos_inline_api
xwer_t xwos_mtx_timedlock(struct xwos_mtx * mtx, xwtm_t * xwtm)
{
        return xwosdl_mtx_timedlock(&mtx->osmtx, xwtm);
}

/**
 * @brief XWOS API：等待上锁互斥锁，且等待不可被中断
 * @param mtx: (I) 互斥锁指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ENOTINTHRD: 不在线程上下文中
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

#endif /* xwos/osal/lock/mtx.h */
