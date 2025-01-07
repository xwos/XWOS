/**
 * @file
 * @brief 操作系统抽象层：信号量
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_osal_sync_sem_h__
#define __xwos_osal_sync_sem_h__

#include <xwos/standard.h>
#include <xwos/osal/jack/sync/sem.h>
#include <xwos/osal/time.h>
#include <xwos/osal/sync/sel.h>

/**
 * @defgroup xwos_sync_sem 信号量
 * @ingroup xwos_sync
 * 技术参考手册： [信号量](../Docs/TechRefManual/Sync/Sem)
 *
 *
 * ## 信号量的静态初始化、销毁
 *
 * + `xwos_sem_init()` ：静态初始化
 * + `xwos_sem_fini()` ：销毁
 *
 *
 * ## 信号量的动态创建、删除
 *
 * + `xwos_sem_create()` ：动态创建
 * + `xwos_sem_delete()` ：删除
 *
 *
 * ## 发布信号量
 *
 * + `xwos_sem_post()` ：发布信号量，可在 **任意** 上下文使用
 *
 *
 * ## 等待信号量
 *
 * + `xwos_sem_wait()` ：等待信号量，只能在 **线程** 上下文使用
 * + `xwos_sem_wait_to()` ：限时等待信号量，只能在 **线程** 上下文使用
 * + `xwos_sem_wait_unintr()` ：不可中断地等待信号量，只能在 **线程** 上下文使用
 * + `xwos_sem_trywait()` ：仅测试信号量，可在 **任意** 上下文使用
 *
 *
 * ## 获取信号量的状态
 *
 * + `xwos_sem_get_max()` ：获取信号量中计数器的最大值，可在 **任意** 上下文使用
 * + `xwos_sem_get_value()` ：获取信号量中计数器的值，可在 **任意** 上下文使用
 *
 *
 * ## 冻结与解冻信号量
 *
 * + `xwos_sem_freeze()` ：冻结，可在 **任意** 上下文使用
 * + `xwos_sem_thaw()` ：解冻，可在 **任意** 上下文使用
 *
 *
 * ## 使用信号选择器选择信号量
 *
 * + `xwos_sem_bind()` ：将信号量绑定到 [信号选择器](../Docs/TechRefManual/Sync/Sel) 上
 * + `xwos_sem_unbind()` ：从 [信号选择器](../Docs/TechRefManual/Sync/Sel) 上解绑
 *
 *
 * ## 信号量对象的生命周期管理
 *
 * + 通过 **对象指针** 管理生命周期：
 *   + `xwos_sem_grab()` ：增加引用计数
 *   + `xwos_sem_put()` ：减少引用计数
 * + 通过 **对象描述符** 管理生命周期：
 *   + `xwos_sem_acquire()` ：增加引用计数
 *   + `xwos_sem_release()` ：减少引用计数
 *
 *
 * ## 对象描述符和对象标签
 *
 * 已知信号量对象的指针的情况下，可以通过 `xwos_sem_get_d()` 获取 @ref xwos_sem_d ，
 * 或可以通过 `xwos_sem_get_tik()` 获取对象标签。
 *
 *
 * ## C++
 *
 * C++头文件： @ref xwos/osal/sync/sem.hxx
 * @{
 */

/**
 * @brief XWOS API：信号量对象
 */
struct xwos_sem {
        struct xwosdl_sem ossem;
};

/**
 * @brief XWOS API：信号量对象描述符
 */
typedef struct {
        struct xwos_sem * sem; /**< 信号量对象的指针 */
        xwsq_t tik; /**< 标签 */
} xwos_sem_d;

/**
 * @brief XWOS API：空的信号量对象描述符
 */
#define XWOS_SEM_NILD ((xwos_sem_d){NULL, 0,})

/**
 * @brief XWOS API：静态方式初始化信号量对象
 * @param[in] sem: 信号量对象的指针
 * @param[in] val: 信号量的初始值
 * @param[in] max: 信号量的最大值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 无效参数
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_sem_init(struct xwos_sem * sem, xwssq_t val, xwssq_t max)
{
        return xwosdl_sem_init(&sem->ossem, val, max);
}

/**
 * @brief XWOS API：销毁静态方式初始化的信号量对象
 * @param[in] sem: 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_sem_fini(struct xwos_sem * sem)
{
        return xwosdl_sem_fini(&sem->ossem);
}

/**
 * @brief XWOS API：增加信号量对象的引用计数
 * @param[in] sem: 信号量对象的指针
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
xwer_t xwos_sem_grab(struct xwos_sem * sem)
{
        return xwosdl_sem_grab(&sem->ossem);
}

/**
 * @brief XWOS API：减少信号量对象的引用计数
 * @param[in] sem: 信号量对象的指针
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
xwer_t xwos_sem_put(struct xwos_sem * sem)
{
        return xwosdl_sem_put(&sem->ossem);
}

/**
 * @brief XWOS API：动态方式创建信号量对象
 * @param[out] semd: 指向缓冲区的指针，通过此缓冲区返回信号量对象描述符
 * @param[in] val: 信号量的初始值
 * @param[in] max: 信号量的最大值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 无效参数
 * @retval -ENOMEM: 内存不足
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_sem_create(xwos_sem_d * semd, xwssq_t val, xwssq_t max)
{
        return xwosdl_sem_create((xwosdl_sem_d *)semd, val, max);
}

/**
 * @brief XWOS API：删除动态方式创建的信号量对象
 * @param[in] semd: 信号量对象描述符
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_sem_delete(xwos_sem_d semd)
{
        return xwosdl_sem_delete(&semd.sem->ossem, semd.tik);
}

/**
 * @brief XWOS API：检查信号量对象的标签并增加引用计数
 * @param[in] semd: 信号量对象描述符
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENILOBJD: 空的对象描述符
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_sem_acquire(xwos_sem_d semd)
{
        return xwosdl_sem_acquire(&semd.sem->ossem, semd.tik);
}

/**
 * @brief XWOS API：检查对象的标签并减少引用计数
 * @param[in] semd: 信号量对象描述符
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENILOBJD: 空的对象描述符
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_sem_release(xwos_sem_d semd)
{
        return xwosdl_sem_release(&semd.sem->ossem, semd.tik);
}

/**
 * @brief XWOS API：获取信号量对象的标签
 * @param[in] sem: 信号量对象的指针
 * @return 信号量对象的标签
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwsq_t xwos_sem_get_tik(struct xwos_sem * sem)
{
        return xwosdl_sem_get_tik(&sem->ossem);
}

/**
 * @brief XWOS API：获取信号量对象描述符
 * @param[in] sem: 信号量对象的指针
 * @return 信号量对象描述符
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwos_sem_d xwos_sem_get_d(struct xwos_sem * sem)
{
        xwos_sem_d semd;

        semd.sem = sem;
        semd.tik = xwosdl_sem_get_tik(&sem->ossem);
        return semd;
}

/**
 * @brief XWOS API：绑定信号量对象到信号选择器
 * @param[in] sem: 信号量对象的指针
 * @param[in] sel: 信号选择器的指针
 * @param[in] pos: 信号量对象映射到位图中的位置
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ECHRNG: 位置超出范围
 * @retval -EALREADY: 同步对象已经绑定到事件对象
 * @retval -EBUSY: 通道已经被其他同步对象独占
 * @note
 * + 上下文：任意
 * + 绑定方式：独占绑定
 */
static __xwos_inline_api
xwer_t xwos_sem_bind(struct xwos_sem * sem, struct xwos_sel * sel, xwsq_t pos)
{
        return xwosdl_sem_bind(&sem->ossem, &sel->ossel, pos);
}

/**
 * @brief XWOS API：从信号选择器上解绑信号量对象
 * @param[in] sem: 信号量对象的指针
 * @param[in] sel: 信号选择器的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_sem_unbind(struct xwos_sem * sem, struct xwos_sel * sel)
{
        return xwosdl_sem_unbind(&sem->ossem, &sel->ossel);
}

/**
 * @brief XWOS API：冻结信号量对象
 * @param[in] sem: 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EALREADY: 信号量对象已被冻结
 * @note
 * + 上下文：任意
 * @details
 * + 被冻结的信号量对象不允许增加(V操作)，但可以被测试(P操作)。
 *   测试信号量对象的线程会被加入到信号量对象的等待队列。
 * + 对已经冻结的信号量再次冻结，将返回 `-EALREADY` 。
 */
static __xwos_inline_api
xwer_t xwos_sem_freeze(struct xwos_sem * sem)
{
        return xwosdl_sem_freeze(&sem->ossem);
}

/**
 * @brief XWOS API：解冻信号量对象
 * @param[in] sem: 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EALREADY: 信号量对象未被冻结
 * @note
 * + 上下文：任意
 * @details
 * + 此CAPI只对已冻结的信号量对象起作用，
 *   对未冻结的信号量对象调用此函数将返回错误码 `-EALREADY` 。
 */
static __xwos_inline_api
xwer_t xwos_sem_thaw(struct xwos_sem * sem)
{
        return xwosdl_sem_thaw(&sem->ossem);
}

/**
 * @brief XWOS API：发布信号量
 * @param[in] sem: 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENEGATIVE: 信号量对象已被冻结
 * @retval -ERANGE: 信号量对象的值已经最大
 * @note
 * + 上下文：任意
 * @details
 * 此CAPI只对未冻结的信号量对象起作用，已冻结的信号量对象将得到错误码 `-ENEGATIVE` 。
 */
static __xwos_inline_api
xwer_t xwos_sem_post(struct xwos_sem * sem)
{
        return xwosdl_sem_post(&sem->ossem);
}

/**
 * @brief XWOS API：等待并获取信号量
 * @param[in] sem: 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @retval -EDSPMPT: 抢占被关闭
 * @retval -EDSBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 */
static __xwos_inline_api
xwer_t xwos_sem_wait(struct xwos_sem * sem)
{
        return xwosdl_sem_wait(&sem->ossem);
}

/**
 * @brief XWOS API：限时等待并获取信号量
 * @param[in] sem: 信号量对象的指针
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETIMEDOUT: 超时
 * @retval -EINTR: 等待被中断
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
xwer_t xwos_sem_wait_to(struct xwos_sem * sem, xwtm_t to)
{
        return xwosdl_sem_wait_to(&sem->ossem, to);
}

/**
 * @brief XWOS API：等待并获取信号量，且等待不可被中断
 * @param[in] sem: 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @retval -EDSPMPT: 抢占被关闭
 * @retval -EDSBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 */
static __xwos_inline_api
xwer_t xwos_sem_wait_unintr(struct xwos_sem * sem)
{
        return xwosdl_sem_wait_unintr(&sem->ossem);
}

/**
 * @brief XWOS API：尝试获取信号量
 * @param[in] sem: 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENODATA: 信号量对象资源不可用（信号量对象无法被获取）
 * @note
 * + 上下文：任意
 * @details
 * 若没有检测到信号，立即返回错误码 `-ENODATA` ，不会阻塞。
 */
static __xwos_inline_api
xwer_t xwos_sem_trywait(struct xwos_sem * sem)
{
        return xwosdl_sem_trywait(&sem->ossem);
}

/**
 * @brief XWOS API：获取信号量对象计数器的最大值
 * @param[in] sem: 信号量对象的指针
 * @param[out] max: 指向缓冲区的指针，通过此缓冲区返回信号量对象计数器的最大值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_sem_get_max(struct xwos_sem * sem, xwssq_t * max)
{
        return xwosdl_sem_get_max(&sem->ossem, max);
}

/**
 * @brief XWOS API：获取信号量对象计数器的值
 * @param[in] sem: 信号量对象的指针
 * @param[out] val: 指向缓冲区的指针，通过此缓冲区返回信号量对象计数器的值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_sem_get_value(struct xwos_sem * sem, xwssq_t * val)
{
        return xwosdl_sem_get_value(&sem->ossem, val);
}

/**
 * @} xwos_sync_sem
 */

#endif /* xwos/osal/sync/sem.h */
