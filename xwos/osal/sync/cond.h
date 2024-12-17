/**
 * @file
 * @brief 操作系统抽象层：条件量
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_osal_sync_cond_h__
#define __xwos_osal_sync_cond_h__

#include <xwos/standard.h>
#include <xwos/osal/jack/sync/cond.h>
#include <xwos/osal/time.h>
#include <xwos/osal/sync/sel.h>
#include <xwos/osal/lock/mtx.h>

/**
 * @defgroup xwos_sync_cond 条件量
 * @ingroup xwos_sync
 * 技术参考手册： [条件量](../Docs/TechRefManual/Sync/Cond)
 *
 *
 * ## 条件量的静态初始化、销毁
 *
 * + `xwos_cond_init()` ：静态初始化
 * + `xwos_cond_fini()` ：销毁
 *
 *
 * ## 条件量的动态创建、删除
 *
 * + `xwos_cond_create()` ：动态创建
 * + `xwos_cond_delete()` ：删除
 *
 *
 * ## 触发条件量
 *
 * + `xwos_cond_unicast()` ：单播条件量，可在 **任意** 上下文使用
 * + `xwos_cond_broadcast()` ：广播条件量，可在 **任意** 上下文使用
 *
 *
 * ## 等待条件量
 *
 * + `xwos_cond_wait()` ：等待条件量，只能在 **线程** 上下文使用
 * + `xwos_cond_wait_to()` ：限时等待条件量，只能在 **线程** 上下文使用
 * + `xwos_cond_wait_unintr()` ：不可中断地等待条件量，只能在 **线程** 上下文使用
 *
 *
 * ## 冻结与解冻条件量
 *
 * + `xwos_cond_freeze()` ：冻结，可在 **任意** 上下文使用
 * + `xwos_cond_thaw()` ：解冻，可在 **任意** 上下文使用
 *
 *
 * ## 使用信号选择器选择条件量
 *
 * + `xwos_cond_bind()` ：绑定条件量到 [信号选择器](../Docs/TechRefManual/Sync/Sel)
 * + `xwos_cond_unbind()` ：从 [信号选择器](../Docs/TechRefManual/Sync/Sel) 上解绑
 *   条件量
 *
 *
 * ## 条件量对象的生命周期管理
 *
 * + 通过 **对象指针** 管理生命周期：
 *   + `xwos_cond_grab()` ：增加引用计数
 *   + `xwos_cond_put()` ：减少引用计数
 * + 通过 **对象描述符** 管理生命周期：
 *   + `xwos_cond_acquire()` ：增加引用计数
 *   + `xwos_cond_release()` ：减少引用计数
 *
 *
 * ## 对象描述符和对象标签
 *
 * 已知条件量对象指针的情况下，可以通过 `xwos_cond_get_d()` 获取 @ref xwos_cond_d ，
 * 或可以通过 `xwos_cond_get_tik()` 获取对象标签。
 *
 *
 * ## C++
 *
 * C++头文件： @ref xwos/osal/sync/cond.hxx
 * @{
 */

/**
 * @brief XWOS API：条件量对象
 */
struct xwos_cond {
        struct xwosdl_cond oscond;
};

/**
 * @brief XWOS API：条件量对象描述符
 */
typedef struct {
        struct xwos_cond * cond; /**< 条件量对象的指针 */
        xwsq_t tik; /**< 标签 */
} xwos_cond_d;

/**
 * @brief XWOS API：空的条件量对象描述符
 */
#define XWOS_COND_NILD ((xwos_cond_d){NULL, 0,})

/**
 * @brief XWOS API：静态方式初始化条件量对象
 * @param[in] cond: 条件量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_cond_init(struct xwos_cond * cond)
{
        return xwosdl_cond_init(&cond->oscond);
}

/**
 * @brief XWOS API：销毁静态方式初始化的条件量对象
 * @param[in] cond: 条件量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_cond_fini(struct xwos_cond * cond)
{
        return xwosdl_cond_fini(&cond->oscond);
}

/**
 * @brief XWOS API：增加条件量对象的引用计数
 * @param[in] cond: 条件量对象的指针
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
xwer_t xwos_cond_grab(struct xwos_cond * cond)
{
        return xwosdl_cond_grab(&cond->oscond);
}

/**
 * @brief XWOS API：减少条件量对象的引用计数
 * @param[in] cond: 条件量对象的指针
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
xwer_t xwos_cond_put(struct xwos_cond * cond)
{
        return xwosdl_cond_put(&cond->oscond);
}

/**
 * @brief XWOS API：动态方式创建条件量对象
 * @param[out] condd: 指向缓冲区的指针，通过此缓冲区返回条件量对象描述符
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_cond_create(xwos_cond_d * condd)
{
        return xwosdl_cond_create((xwosdl_cond_d *)condd);
}

/**
 * @brief XWOS API：删除动态方式创建的条件量对象
 * @param[in] condd: 条件量对象描述符
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_cond_delete(xwos_cond_d condd)
{
        return xwosdl_cond_delete(&condd.cond->oscond, condd.tik);
}

/**
 * @brief XWOS API：检查条件量对象的标签并增加引用计数
 * @param[in] condd: 条件量对象描述符
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENILOBJD: 空的对象描述符
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_cond_acquire(xwos_cond_d condd)
{
        return xwosdl_cond_acquire(&condd.cond->oscond, condd.tik);
}

/**
 * @brief XWOS API：检查对象的标签并减少引用计数
 * @param[in] condd: 条件量对象描述符
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENILOBJD: 空的对象描述符
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_cond_release(xwos_cond_d condd)
{
        return xwosdl_cond_release(&condd.cond->oscond, condd.tik);
}

/**
 * @brief XWOS API：获取条件量对象的标签
 * @param[in] cond: 条件量对象的指针
 * @return 条件量对象的标签
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwsq_t xwos_cond_get_tik(struct xwos_cond * cond)
{
        return xwosdl_cond_get_tik(&cond->oscond);
}

/**
 * @brief XWOS API：获取条件量对象描述符
 * @param[in] cond: 条件量对象的指针
 * @return 条件量对象描述符
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwos_cond_d xwos_cond_get_d(struct xwos_cond * cond)
{
        xwos_cond_d condd;

        condd.cond = cond;
        condd.tik = xwosdl_cond_get_tik(&cond->oscond);
        return condd;
}

/**
 * @brief XWOS API：绑定条件量对象到信号选择器
 * @param[in] cond: 条件量对象的指针
 * @param[in] sel: 信号选择器的指针
 * @param[in] pos: 条件量对象映射到位图中的位置
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ECHRNG: 位置超出范围
 * @retval -EALREADY: 同步对象已经绑定到事件对象
 * @retval -EBUSY: 通道已经被其他同步对象独占
 * @note
 * + 上下文：任意
 * + 绑定方式：非独占绑定
 * + 绑定了信号选择器的条件量对象，只有 **广播** 才会向信号选择器发送信号。
 */
static __xwos_inline_api
xwer_t xwos_cond_bind(struct xwos_cond * cond, struct xwos_sel * sel, xwsq_t pos)
{
        return xwosdl_cond_bind(&cond->oscond, &sel->ossel, pos);
}

/**
 * @brief XWOS API：从信号选择器上解绑条件量对象
 * @param[in] cond: 条件量对象的指针
 * @param[in] sel: 信号选择器的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_cond_unbind(struct xwos_cond * cond, struct xwos_sel * sel)
{
        return xwosdl_cond_unbind(&cond->oscond, &sel->ossel);
}

/**
 * @brief XWOS API：冻结条件量对象
 * @param[in] cond: 条件量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EALREADY: 条件量对象已被冻结
 * @note
 * + 上下文：任意
 * @details
 * + 已冻结的条件量不允许单播或广播，但可以被线程等待。
 * + 对已经冻结的条件量再次冻结，将返回 `-EALREADY` 。
 */
static __xwos_inline_api
xwer_t xwos_cond_freeze(struct xwos_cond * cond)
{
        return xwosdl_cond_freeze(&cond->oscond);
}

/**
 * @brief XWOS API：解冻条件量对象
 * @param[in] cond: 条件量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EALREADY: 条件量对象未被冻结
 * @note
 * + 上下文：任意
 * @details
 * + 此函数只对已冻结的条件量对象起作用，
 *   对未冻结的条件量对象调用此函数将返回错误码 `-EALREADY` 。
 */
static __xwos_inline_api
xwer_t xwos_cond_thaw(struct xwos_cond * cond)
{
        return xwosdl_cond_thaw(&cond->oscond);
}

/**
 * @brief XWOS API：广播条件量对象
 * @param[in] cond: 条件量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ENEGATIVE: 条件量对象已被冻结
 * @note
 * + 上下文：任意
 * @details
 * + 广播条件量会唤醒所有等待的线程。
 * + 此CAPI只对未冻结的条件量对象起作用，已冻结的条件量对象将返回
 *   错误码 `-ENEGATIVE` 。
 */
static __xwos_inline_api
xwer_t xwos_cond_broadcast(struct xwos_cond * cond)
{
        return xwosdl_cond_broadcast(&cond->oscond);
}

/**
 * @brief XWOS API：单播条件量对象
 * @param[in] cond: 条件量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ENEGATIVE: 条件量对象已被冻结
 * @note
 * + 上下文：任意
 * @details
 * + 单播条件量会唤醒第一个等待的线程。
 * + 此CAPI只对未冻结的条件量对象起作用，已冻结的条件量对象将返回
 *   错误码 `-ENEGATIVE` 。
 */
static __xwos_inline_api
xwer_t xwos_cond_unicast(struct xwos_cond * cond)
{
        return xwosdl_cond_unicast(&cond->oscond);
}

/**
 * @brief XWOS API：等待条件量
 * @param[in] cond: 条件量对象的指针
 * @param[in] lock: 锁
 * @param[in] lktype: 锁的类型，取值：@ref xwos_lock_type_em
 * @param[in] lkdata: 锁的数据
 * @param[out] lkst: 指向缓冲区的指针，通过此缓冲区返回锁的状态
 *   @arg `XWOS_LKST_LOCKED` ：已上锁
 *   @arg `XWOS_LKST_UNLOCKED` ：未上锁
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EINVAL: 参数无效
 * @retval -EINTR: 等待被中断
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @note
 * + 上下文：线程
 * @details
 * + 调用此CAPI的线程会阻塞等待条件量，等待的同时会解锁一个锁。
 * + 条件量被单播 `xwos_cond_unicast()` 或广播 `xwos_cond_broadcast()` 时，
 *   会唤醒正在等待的线程，线程会重新对锁进行上锁，上锁成功此CAPI返回 `XWOK` 。
 * + 重新上锁时，当锁类型是互斥锁时，若互斥锁被其他线程占用，将继续阻塞等待互斥锁。
 * + 所有类型的锁统一使用 `union xwlk_ulock` 指代，此联合中包含所有锁的定义，
 *   实际是一个指针，有些锁还需要伴生数据 `lkdata` ：
 *   + `XWOS_LK_MTX` ：互斥锁
 *     + `lock` ：代表互斥锁（访问方式： `lock.osal.mtx` ）；
 *     + `lockdata` ：无作用，设置为 `NULL` 即可；
 *   + `XWOS_LK_SPLK` ：自旋锁
 *     + `lock` ：代表自旋锁的指针（访问方式： `lock.osal.splk` ）；
 *     + `lockdata` ：无作用，设置为 `NULL` 即可；
 *   + `XWOS_LK_SQLK_WR` ：顺序写锁
 *     + `lock` ：代表顺序锁的指针（访问方式： `lock.osal.sqlk` ）；
 *     + `lockdata` ：无作用，设置为 `NULL` 即可；
 *   + `XWOS_LK_SQLK_RDEX` ：顺序独占读锁
 *     + `lock` ：代表顺序锁的指针（访问方式： `lock.osal.sqlk` ）；
 *     + `lockdata` ：无作用，设置为 `NULL` 即可；
 *   + `XWOS_LK_CALLBACK` ：自定义的加锁与解锁函数
 *     + `lock` ：代表指向 `struct xwlk_cblk` 的指针（访问方式： `lock.cb` ）；
 *     + `lockdata` ：传递给 `struct xwlk_cblk` 中的 `lock` 与 `unlock` 函数的参数；
 * + `lkst` 用于返回锁的状态。
 *   + 此CAPI返回 `XWOK` 时，锁状态一定是 **已上锁( `XWOS_LKST_LOCKED` )** 。
 *   + 此CAPI返回其他错误码时，锁的状态不确定，需要通过返回值 `lkst` 查询。
 * + 线程的等待被中断后，此CAPI返回 `-EINTR` 。
 */
static __xwos_inline_api
xwer_t xwos_cond_wait(struct xwos_cond * cond,
                      union xwos_ulock lock, xwsq_t lktype, void * lkdata,
                      xwsq_t * lkst)
{
        return xwosdl_cond_wait(&cond->oscond, lock, lktype, lkdata, lkst);
}

/**
 * @brief XWOS API：限时等待条件量
 * @param[in] cond: 条件量对象的指针
 * @param[in] lock: 锁
 * @param[in] lktype: 锁的类型，取值：@ref xwos_lock_type_em
 * @param[in] lkdata: 锁的数据
 * @param[in] to: 期望唤醒的时间点
 * @param[out] lkst: 指向缓冲区的指针，通过此缓冲区返回锁的状态
 *   @arg `XWOS_LKST_LOCKED` ：已上锁
 *   @arg `XWOS_LKST_UNLOCKED` ：未上锁
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EINVAL: 参数无效
 * @retval -ETIMEDOUT: 超时
 * @retval -EINTR: 等待被中断
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @note
 * + 上下文：线程
 * @details
 * + 调用此CAPI的线程会阻塞等待条件量，等待的同时会解锁一个锁。
 * + 条件量被单播 `xwos_cond_unicast()` 或广播 `xwos_cond_broadcast()` 时，
 *   会唤醒正在等待的线程，线程会重新对锁进行上锁，上锁成功此CAPI返回 `XWOK` 。
 * + 重新上锁时，当锁类型是互斥锁时，若互斥锁被其他线程占用，将继续阻塞等待互斥锁，
 *   直到超时的时间点 `to` 。
 * + 所有类型的锁统一使用 `union xwlk_ulock` 指代，此联合中包含所有锁的定义，
 *   实际是一个指针，有些锁还需要伴生数据 `lkdata` ：
 *   + `XWOS_LK_MTX` ：互斥锁
 *     + `lock` ：代表互斥锁（访问方式： `lock.osal.mtx` ）；
 *     + `lockdata` ：无作用，设置为 `NULL` 即可；
 *   + `XWOS_LK_SPLK` ：自旋锁
 *     + `lock` ：代表自旋锁的指针（访问方式： `lock.osal.splk` ）；
 *     + `lockdata` ：无作用，设置为 `NULL` 即可；
 *   + `XWOS_LK_SQLK_WR` ：顺序写锁
 *     + `lock` ：代表顺序锁的指针（访问方式： `lock.osal.sqlk` ）；
 *     + `lockdata` ：无作用，设置为 `NULL` 即可；
 *   + `XWOS_LK_SQLK_RDEX` ：顺序独占读锁
 *     + `lock` ：代表顺序锁的指针（访问方式： `lock.osal.sqlk` ）；
 *     + `lockdata` ：无作用，设置为 `NULL` 即可；
 *   + `XWOS_LK_CALLBACK` ：自定义的加锁与解锁函数
 *     + `lock` ：代表指向 `struct xwlk_cblk` 的指针（访问方式： `lock.cb` ）；
 *     + `lockdata` ：传递给 `struct xwlk_cblk` 中的 `lock` 与 `unlock` 函数的参数；
 * + `to` 表示等待超时的时间点：
 *   + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *     可以使用 `xwtm_ft(delta)` 表示；
 *   + 如果 `to` 是过去的时间点，此CAPI将直接返回 `-ETIMEDOUT` 。
 * + `lkst` 用于返回锁的状态。
 *   + 此CAPI返回 `XWOK` 时，锁状态一定是 **已上锁( `XWOS_LKST_LOCKED` )** 。
 *   + 此CAPI返回其他错误码时，锁的状态不确定，需要通过返回值 `lkst` 查询。
 * + 线程的等待被中断后，此CAPI返回 `-EINTR` 。
 * + 线程的等待超时后，此CAPI返回 `-ETIMEDOUT` 。
 */
static __xwos_inline_api
xwer_t xwos_cond_wait_to(struct xwos_cond * cond,
                         union xwos_ulock lock, xwsq_t lktype, void * lkdata,
                         xwtm_t to, xwsq_t * lkst)
{
        return xwosdl_cond_wait_to(&cond->oscond, lock, lktype, lkdata, to, lkst);
}

/**
 * @brief XWOS API：不可中断地等待条件量
 * @param[in] cond: 条件量对象的指针
 * @param[in] lock: 锁
 * @param[in] lktype: 锁的类型，取值：@ref xwos_lock_type_em
 * @param[in] lkdata: 锁的数据
 * @param[out] lkst: 指向缓冲区的指针，通过此缓冲区返回锁的状态
 *   @arg `XWOS_LKST_LOCKED` ：已上锁
 *   @arg `XWOS_LKST_UNLOCKED` ：未上锁
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EINVAL: 参数无效
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @note
 * + 上下文：线程
 * @details
 * + 调用此CAPI的线程会阻塞等待条件量，且等待不可被中断，等待的同时会解锁一个锁。
 * + 条件量被单播 `xwos_cond_unicast()` 或广播 `xwos_cond_broadcast()` 时，
 *   会唤醒正在等待的线程，线程会重新对锁进行上锁，上锁成功此CAPI返回 `XWOK` 。
 * + 重新上锁时，当锁类型是互斥锁时，若互斥锁被其他线程占用，将继续阻塞等待互斥锁，
 *   且使用CAPI `xwos_mtx_lock_unitr()` 进行等待。
 * + 所有类型的锁统一使用 `union xwlk_ulock` 指代，此联合中包含所有锁的定义，
 *   实际是一个指针，有些锁还需要伴生数据 `lkdata` ：
 *   + `XWOS_LK_MTX` ：互斥锁
 *     + `lock` ：代表互斥锁（访问方式： `lock.osal.mtx` ）；
 *     + `lockdata` ：无作用，设置为 `NULL` 即可；
 *   + `XWOS_LK_SPLK` ：自旋锁
 *     + `lock` ：代表自旋锁的指针（访问方式： `lock.osal.splk` ）；
 *     + `lockdata` ：无作用，设置为 `NULL` 即可；
 *   + `XWOS_LK_SQLK_WR` ：顺序写锁
 *     + `lock` ：代表顺序锁的指针（访问方式： `lock.osal.sqlk` ）；
 *     + `lockdata` ：无作用，设置为 `NULL` 即可；
 *   + `XWOS_LK_SQLK_RDEX` ：顺序独占读锁
 *     + `lock` ：代表顺序锁的指针（访问方式： `lock.osal.sqlk` ）；
 *     + `lockdata` ：无作用，设置为 `NULL` 即可；
 *   + `XWOS_LK_CALLBACK` ：自定义的加锁与解锁函数
 *     + `lock` ：代表指向 `struct xwlk_cblk` 的指针（访问方式： `lock.cb` ）；
 *     + `lockdata` ：传递给 `struct xwlk_cblk` 中的 `lock` 与 `unlock` 函数的参数；
 * + `lkst` 用于返回锁的状态。
 *   + 此CAPI返回 `XWOK` 时，锁状态一定是 **已上锁( `XWOS_LKST_LOCKED` )** 。
 *   + 此CAPI返回其他错误码时，锁的状态不确定，需要通过返回值 `lkst` 查询。
 */
static __xwos_inline_api
xwer_t xwos_cond_wait_unintr(struct xwos_cond * cond,
                             union xwos_ulock lock, xwsq_t lktype, void * lkdata,
                             xwsq_t * lkst)
{
        return xwosdl_cond_wait_unintr(&cond->oscond, lock, lktype, lkdata, lkst);
}

/**
 * @} xwos_sync_cond
 */

#endif /* xwos/osal/sync/cond.h */
