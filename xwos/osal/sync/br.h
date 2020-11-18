/**
 * @file
 * @brief 操作系统抽象层：线程栅栏
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_osal_sync_br_h__
#define __xwos_osal_sync_br_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/osal/jack/sync/br.h>
#include <xwos/osal/sync/sel.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       type        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOS API：线程栅栏
 */
struct xwos_br {
        struct xwosdl_br osbr;
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macro       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWOS_BR_MAXNUM XWOSDL_BR_MAXNUM /**< 线程栅栏可同步的最大线程数 */

/**
 * @brief 声明线程栅栏位图
 */
#define xwos_br_declare_bitmap(name) xwosdl_br_declare_bitmap(name)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********        API        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOS API：静态方式初始化线程栅栏
 * @param br: (I) 线程栅栏的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个**br**，不可重入
 */
static __xwos_inline_api
xwer_t xwos_br_init(struct xwos_br * br)
{
        return xwosdl_br_init(&br->osbr);
}

/**
 * @brief XWOS API：销毁静态方式初始化的线程栅栏
 * @param br: (I) 线程栅栏的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个**br**，不可重入
 */
static __xwos_inline_api
xwer_t xwos_br_destroy(struct xwos_br * br)
{
        return xwosdl_br_destroy(&br->osbr);
}

/**
 * @brief XWOS API：动态方式创建线程栅栏
 * @param brp: (O) 指向缓冲区的指针，通过此缓冲区返回线程栅栏的指针
 * @param val: (I) 线程栅栏的初始值
 * @param max: (I) 线程栅栏的最大值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ENOMEM: 内存不足
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_br_create(struct xwos_br ** brp)
{
        return xwosdl_br_create((struct xwosdl_br **)brp);
}

/**
 * @brief XWOS API：删除动态方式创建的线程栅栏
 * @param br: (I) 线程栅栏指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个**br**，不可重入
 */
static __xwos_inline_api
xwer_t xwos_br_delete(struct xwos_br * br)
{
        return xwosdl_br_delete(&br->osbr);
}

/**
 * @brief XWOS API：绑定线程栅栏到信号选择器
 * @param br: (I) 线程栅栏的指针
 * @param sel: (I) 信号选择器的指针
 * @param pos: (I) 线程栅栏对象映射到位图中的位置
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ECHRNG: 位置超出范围
 * @retval -EALREADY: 同步对象已经绑定到事件对象
 * @retval -EBUSY: 通道已经被其他同步对象独占
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个**br**，不可重入
 * @note
 * - 当所有线程到达线程栅栏时，绑定了信号选择器的线程栅栏将向信号选择器发送信号。
 */
static __xwos_inline_api
xwer_t xwos_br_bind(struct xwos_br * br, struct xwos_sel * sel, xwsq_t pos)
{
        return xwosdl_br_bind(&br->osbr, &sel->ossel, pos);
}

/**
 * @brief XWOS API：从信号选择器上解绑线程栅栏
 * @param br: (I) 线程栅栏的指针
 * @param sel: (I) 信号选择器的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个**br**，不可重入
 */
static __xwos_inline_api
xwer_t xwos_br_unbind(struct xwos_br * br, struct xwos_sel * sel)
{
        return xwosdl_br_unbind(&br->osbr, &sel->ossel);
}

/**
 * @brief XWOS API：中断线程栅栏等待队列中的所有节点
 * @param br: (I) 线程栅栏指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_br_intr_all(struct xwos_br * br)
{
        return xwosdl_br_intr_all(&br->osbr);
}

/**
 * @brief XWOS API：等待所有线程到达栅栏
 * @param br: (I) 线程栅栏指针
 * @param pos: (I) 当前线程的位图位置
 * @param sync: (I) 当前线程需要同步的线程掩码
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ECHRNG: 位置超出范围
 * @retval -EINTR: 等待被中断
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_br_sync(struct xwos_br * br, xwsq_t pos, xwbmp_t sync[])
{
        return xwosdl_br_sync(&br->osbr, pos, sync);
}

/**
 * @brief XWOS API：限时等待所有线程到达栅栏
 * @param br: (I) 线程栅栏指针
 * @param pos: (I) 当前线程的位图位置
 * @param sync: (I) 当前线程需要同步的线程掩码
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ECHRNG: 位置超出范围
 * @retval -ETIMEDOUT: 超时
 * @retval -EINTR: 等待被中断
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 函数返回返回**-ETIMEDOUT**时，**xwtm**指向的缓冲区内的期望时间会减为0。
 */
static __xwos_inline_api
xwer_t xwos_br_timedsync(struct xwos_br * br, xwsq_t pos,
                         xwbmp_t sync[], xwtm_t * xwtm)
{
        return xwosdl_br_timedsync(&br->osbr, pos, sync, xwtm);
}

#endif /* xwos/osal/sync/br.h */
