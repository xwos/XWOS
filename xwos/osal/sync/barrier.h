/**
 * @file
 * @brief 操作系统抽象层：线程屏障
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_osal_sync_barrier_h__
#define __xwos_osal_sync_barrier_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/osdl/os.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 操作系统抽象层：线程屏障
 */
struct xwosal_barrier {
        struct xwosdl_barrier osbar; /**< 操作系统的线程屏障 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWOSAL_BARRIER_MAXNUM XWOSDL_BARRIER_MAXNUM /**< 线程屏障可同步的最大线程数 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       APIs        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 操作系统抽象层API：静态方式初始化线程屏障。
 * @param barrier: (I) 线程屏障的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *barrier* ，不可重入
 */
static __xwos_inline_api
xwer_t xwosal_barrier_init(struct xwosal_barrier * bar)
{
        return xwosdl_barrier_init(&bar->osbar);
}

/**
 * @brief 操作系统抽象层API：销毁静态方式初始化的线程屏障。
 * @param barrier: (I) 线程屏障的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *barrier* ，不可重入
 */
static __xwos_inline_api
xwer_t xwosal_barrier_destroy(struct xwosal_barrier * bar)
{
        return xwosdl_barrier_destroy(&bar->osbar);
}

/**
 * @brief 操作系统抽象层API：动态方式创建线程屏障。
 * @param barbuf: (O) 指向缓冲区的指针，通过此缓冲区返回ID
 * @param val: (I) 线程屏障的初始值
 * @param max: (I) 线程屏障的最大值
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwosal_barrier_create(xwid_t * barbuf)
{
        return xwosdl_barrier_create(barbuf);
}

/**
 * @brief 操作系统抽象层API：删除动态方式创建的线程屏障。
 * @param barid: (I) 线程屏障ID
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *barrier* ，不可重入
 */
static __xwos_inline_api
xwer_t xwosal_barrier_delete(xwid_t barid)
{
        return xwosdl_barrier_delete(barid);
}

/**
 * @brief 操作系统抽象层API：从线程屏障对象指针获取其ID。
 * @param barrier: (I) 线程屏障对象的指针
 * @return 线程屏障ID
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwid_t xwosal_barrier_get_id(struct xwosal_barrier * bar)
{
        return (xwid_t)xwosdl_barrier_get_id(&bar->osbar);
}

/**
 * @brief 操作系统抽象层API：从线程屏障ID获取对象的指针。
 * @param barid: (I) 线程屏障ID
 * @return 线程屏障对象的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
struct xwosal_barrier * xwosal_barrier_get_obj(xwid_t barid)
{
        return (struct xwosal_barrier *)xwosdl_barrier_get_obj(barid);
}

/**
 * @brief 操作系统抽象层API：绑定线程屏障到信号选择器。
 * @param barid: (I) 线程屏障的ID
 * @param sltid: (I) 信号选择器的ID
 * @param pos: (I) 线程屏障对象映射到位图中的位置
 * @return 错误码
 * @retval OK: OK
 * @retval -ETYPE: 信号选择器或线程屏障类型错误
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *barid* ，不可重入
 * @note
 * - 当所有线程到达线程屏障时，绑定了信号选择器的线程屏障将向信号选择器传递一个信号。
 */
static __xwos_inline_api
xwer_t xwosal_barrier_bind(xwid_t barid, xwid_t sltid, xwsq_t pos)
{
        return xwosdl_barrier_bind(barid, sltid, pos);
}

/**
 * @brief 操作系统抽象层API：从信号选择器上解绑线程屏障。
 * @param barid: (I) 线程屏障的ID
 * @param sltid: (I) 信号选择器的ID
 * @return 错误码
 * @retval OK: OK
 * @retval -ETYPE: 信号选择器类型错误
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *barid* ，不可重入
 */
static __xwos_inline_api
xwer_t xwosal_barrier_unbind(xwid_t barid, xwid_t sltid)
{
        return xwosdl_barrier_unbind(barid, sltid);
}

/**
 * @brief 操作系统抽象层API：中断线程屏障等待队列中的所有节点。
 * @param barid: (I) 线程屏障ID
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 类型不匹配
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwosal_barrier_intr_all(xwid_t barid)
{
        return xwosdl_barrier_intr_all(barid);
}

/**
 * @brief 操作系统抽象层API：等待与其他线程同步。
 * @param barid: (I) 线程屏障ID
 * @param pos: (I) 当前线程的位图位置
 * @param sync: (I) 当前线程需要同步的线程掩码
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ECHRNG: 位置超出范围
 * @retval -ETYPE: 事件对象类型错误
 * @retval -EINTR: 等待被中断
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwosal_barrier_sync(xwid_t barid, xwsq_t pos, xwbmp_t sync[])
{
        return xwosdl_barrier_sync(barid, pos, sync);
}

/**
 * @brief 操作系统抽象层API：限时等待与其他线程同步。
 * @param barid: (I) 线程屏障ID
 * @param pos: (I) 当前线程的位图位置
 * @param sync: (I) 当前线程需要同步的线程掩码
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ECHRNG: 位置超出范围
 * @retval -ETYPE: 事件对象类型错误
 * @retval -ETIMEDOUT: 超时
 * @retval -EINTR: 等待被中断
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 函数返回返回-ETIMEDOUT时，*xwtm* 指向的缓冲区内的期望时间会减为0。
 */
static __xwos_inline_api
xwer_t xwosal_barrier_timedsync(xwid_t barid, xwsq_t pos, xwbmp_t sync[],
                                xwtm_t * xwtm)
{
        return xwosdl_barrier_timedsync(barid, pos, sync, xwtm);
}

#endif /* xwos/osal/sync/barrier.h */
