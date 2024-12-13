/**
 * @file
 * @brief 操作系统抽象层：互斥锁
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_osal_lock_mtx_h__
#define __xwos_osal_lock_mtx_h__

#include <xwos/standard.h>
#include <xwos/osal/jack/lock/mtx.h>
#include <xwos/osal/time.h>
#include <xwos/osal/skd.h>

/**
 * @defgroup xwos_lock_mtx 互斥锁
 * @ingroup xwos_lock
 * 技术参考手册： [互斥锁](../Docs/TechRefManual/Lock/Mutex)
 *
 *
 * ## 互斥锁的静态初始化、销毁
 *
 * + `xwos_mtx_init()` ：静态初始化
 * + `xwos_mtx_fini()` ：销毁
 *
 *
 * ## 互斥锁的动态创建、删除
 *
 * + `xwos_mtx_create()` ：动态创建
 * + `xwos_mtx_delete()` ：删除
 *
 *
 * ## 解锁
 *
 * + `xwos_mtx_unlock()` ：解锁互斥锁，只能在 **线程** 上下文使用
 *
 *
 * ## 上锁
 *
 * + `xwos_mtx_lock()` ：等待并上锁互斥锁，只能在 **线程** 上下文使用
 * + `xwos_mtx_lock_to()` ：限时等待并上锁互斥锁，只能在 **线程** 上下文使用
 * + `xwos_mtx_lock_unintr()` ：不可中断地等待并上锁互斥锁，只能在 **线程** 上下文使用
 * + `xwos_mtx_trylock()` ：尝试上锁互斥锁，只能在 **线程** 上下文使用
 *
 *
 * ## 获取锁状态
 *
 * + `xwos_mtx_get_status()` ：获取锁的状态，可在 **任意** 上下文使用
 *
 *
 * ## 互斥锁对象的生命周期管理
 *
 * + 通过 **对象指针** 管理生命周期：
 *   + `xwos_mtx_grab()` ：增加引用计数
 *   + `xwos_mtx_put()` ：减少引用计数
 * + 通过 **对象描述符** 管理生命周期：
 *   + `xwos_mtx_acquire()` ：增加引用计数
 *   + `xwos_mtx_release()` ：减少引用计数
 *
 *
 * ## 对象描述符和对象标签
 *
 * 已知互斥锁对象的指针的情况下，可以通过 `xwos_mtx_get_d()` 获取 @ref xwos_mtx_d ，
 * 或可以通过 `xwos_mtx_get_tik()` 获取对象标签。
 *
 *
 * ## C++
 *
 * C++头文件： @ref xwos/osal/lock/mtx.hxx
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
 * + 上下文：任意
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
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_mtx_fini(struct xwos_mtx * mtx)
{
        return xwosdl_mtx_fini(&mtx->osmtx);
}

/**
 * @brief XWOS API：增加互斥锁对象的引用计数
 * @param[in] mtx: 互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EOBJDEAD: 对象无效
 * @note
 * + 上下文：任意
 * @details
 * 此函数主要用于管理 **静态对象** 的引用计数。
 * 若用于 **动态对象** ，需要确保对象的指针一定不是野指针。
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
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EOBJDEAD: 对象无效
 * @note
 * + 上下文：任意
 * @details
 * 此函数主要用于管理 **静态对象** 的引用计数。
 * 若用于 **动态对象** ，需要确保对象的指针一定不是野指针。
 */
static __xwos_inline_api
xwer_t xwos_mtx_put(struct xwos_mtx * mtx)
{
        return xwosdl_mtx_put(&mtx->osmtx);
}

/**
 * @brief XWOS API：动态方式创建互斥锁
 * @param[out] mtxd: 指向缓冲区的指针，通过此缓冲区返回互斥锁对象描述符
 * @param[in] sprio: 互斥锁的静态优先级
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EINVAL: 无效参数
 * @retval -ENOMEM: 内存不足
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_mtx_create(xwos_mtx_d * mtxd, xwpr_t sprio)
{
        return xwosdl_mtx_create((xwosdl_mtx_d *)mtxd, sprio);
}

/**
 * @brief XWOS API：删除动态方式创建的互斥锁
 * @param[in] mtxd: 互斥锁对象描述符
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_mtx_delete(xwos_mtx_d mtxd)
{
        return xwosdl_mtx_delete(&mtxd.mtx->osmtx, mtxd.tik);
}

/**
 * @brief XWOS API：检查互斥锁对象的标签并增加引用计数
 * @param[in] mtxd: 互斥锁对象描述符
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENILOBJD: 空的对象描述符
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_mtx_acquire(xwos_mtx_d mtxd)
{
        return xwosdl_mtx_acquire(&mtxd.mtx->osmtx, mtxd.tik);
}

/**
 * @brief XWOS API：检查对象的标签并减少引用计数
 * @param[in] mtxd: 互斥锁对象描述符
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENILOBJD: 空的对象描述符
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_mtx_release(xwos_mtx_d mtxd)
{
        return xwosdl_mtx_release(&mtxd.mtx->osmtx, mtxd.tik);
}

/**
 * @brief XWOS API：获取互斥锁对象的标签
 * @param[in] mtx: 互斥锁对象的指针
 * @return 互斥锁对象的标签
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwsq_t xwos_mtx_get_tik(struct xwos_mtx * mtx)
{
        return xwosdl_mtx_get_tik(&mtx->osmtx);
}

/**
 * @brief XWOS API：获取互斥锁对象的描述符
 * @param[in] mtx: 互斥锁对象的指针
 * @return 互斥锁对象的描述符
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwos_mtx_d xwos_mtx_get_d(struct xwos_mtx * mtx)
{
        xwos_mtx_d mtxd;

        mtxd.mtx = mtx;
        mtxd.tik = xwosdl_mtx_get_tik(&mtx->osmtx);
        return mtxd;
}

/**
 * @brief XWOS API：解锁互斥锁
 * @param[in] mtx: 互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EOBJDEAD: 互斥锁对象无效
 * @retval -EOWNER: 线程并没有锁定此互斥锁
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @note
 * + 上下文：线程
 */
static __xwos_inline_api
xwer_t xwos_mtx_unlock(struct xwos_mtx * mtx)
{
        return xwosdl_mtx_unlock(&mtx->osmtx);
}

/**
 * @brief XWOS API：等待并上锁互斥锁
 * @param[in] mtx: 互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EOBJDEAD: 互斥锁对象无效
 * @retval -EINTR: 等待被中断
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @retval -EDSPMPT: 抢占被关闭
 * @retval -EDSBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * + 如果无法上锁，调用此CAPI的线程会阻塞等待，直到互斥锁可被上锁，或等待被中断。
 * + 线程的阻塞等待可能被中断，此时，API返回 `-EINTR` 。
 */
static __xwos_inline_api
xwer_t xwos_mtx_lock(struct xwos_mtx * mtx)
{
        return xwosdl_mtx_lock(&mtx->osmtx);
}

/**
 * @brief XWOS API：限时等待上锁互斥锁
 * @param[in] mtx: 互斥锁对象的指针
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EOBJDEAD: 互斥锁对象无效
 * @retval -EINTR: 等待被中断
 * @retval -ETIMEDOUT: 超时
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @retval -EDSPMPT: 抢占被关闭
 * @retval -EDSBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
static __xwos_inline_api
xwer_t xwos_mtx_lock_to(struct xwos_mtx * mtx, xwtm_t to)
{
        return xwosdl_mtx_lock_to(&mtx->osmtx, to);
}

/**
 * @brief XWOS API：等待并上锁互斥锁，且等待不可被中断
 * @param[in] mtx: 互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EOBJDEAD: 互斥锁对象无效
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @retval -EDSPMPT: 抢占被关闭
 * @retval -EDSBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 */
static __xwos_inline_api
xwer_t xwos_mtx_lock_unintr(struct xwos_mtx * mtx)
{
        return xwosdl_mtx_lock_unintr(&mtx->osmtx);
}

/**
 * @brief XWOS API：尝试上锁互斥锁，不会阻塞调用线程
 * @param[in] mtx: 互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EOBJDEAD: 互斥锁对象无效
 * @retval -EWOULDBLOCK: 尝试获取锁失败
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @note
 * + 上下文：线程
 * @details
 * 此函数虽然不会阻塞调用线程，但也不可在中断上下文中使用，
 * 因为在OS中互斥锁通常需要记录自己的拥有者（一般是线程对象结构体），
 * 中断上下文不存在对应的线程对象结构体。
 */
static __xwos_inline_api
xwer_t xwos_mtx_trylock(struct xwos_mtx * mtx)
{
        return xwosdl_mtx_trylock(&mtx->osmtx);
}

/**
 * @brief XWOS API：获取锁的状态
 * @param[in] mtx: 互斥锁对象的指针
 * @param[out] status: 指向缓冲区的指针，通过此缓冲区返回锁的状态
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EOBJDEAD: 互斥锁对象无效
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_mtx_get_status(struct xwos_mtx * mtx, xwsq_t * lkst)
{
        return xwosdl_mtx_get_status(&mtx->osmtx, lkst);
}

/**
 * @} xwos_lock_mtx
 */

#endif /* xwos/osal/lock/mtx.h */
