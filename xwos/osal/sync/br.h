/**
 * @file
 * @brief 操作系统抽象层：线程栅栏
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_osal_sync_br_h__
#define __xwos_osal_sync_br_h__

#include <xwos/standard.h>
#include <xwos/osal/jack/sync/br.h>
#include <xwos/osal/time.h>
#include <xwos/osal/sync/sel.h>

/**
 * @defgroup xwos_sync_br 线程栅栏
 * @ingroup xwos_sync
 * @{
 */

/**
 * @brief XWOS API：线程栅栏
 */
struct xwos_br {
        struct xwosdl_br osbr;
};

/**
 * @brief XWOS API：线程栅栏对象描述符
 */
typedef struct {
        struct xwos_br * br; /**< 线程栅栏对象的指针 */
        xwsq_t tik; /**< 标签 */
} xwos_br_d;

/**
 * @brief XWOS API：空的线程栅栏对象描述符
 */
#define XWOS_BR_NILD ((xwos_br_d){NULL, 0,})

/**
 * @brief XWOS API：静态方式初始化线程栅栏
 * @param[in] br: 线程栅栏对象的指针
 * @param[in] num: 线程栅栏中的线程数量
 * @param[in] bmp: 线程栅栏用来记录线程抵达事件的位图缓冲区
 * @param[in] msk: 线程栅栏用来记录线程掩码状态的位图缓冲区，长度应与bmp一致
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个线程栅栏对象，不可重入
 */
static __xwos_inline_api
xwer_t xwos_br_init(struct xwos_br * br, xwsz_t num,
                    xwbmp_t * bmp, xwbmp_t * msk)
{
        return xwosdl_br_init(&br->osbr, num, bmp, msk);
}

/**
 * @brief XWOS API：销毁静态方式初始化的线程栅栏
 * @param[in] br: 线程栅栏对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个线程栅栏对象，不可重入
 */
static __xwos_inline_api
xwer_t xwos_br_fini(struct xwos_br * br)
{
        return xwosdl_br_fini(&br->osbr);
}

/**
 * @brief XWOS API：动态方式创建线程栅栏
 * @param[out] brbuf: 指向缓冲区的指针，通过此缓冲区返回线程栅栏对象的指针
 * @param[in] num: 线程栅栏中的线程数量
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
xwer_t xwos_br_create(struct xwos_br ** brbuf, xwsz_t num)
{
        return xwosdl_br_create((struct xwosdl_br **)brbuf, num);
}

/**
 * @brief XWOS API：删除动态方式创建的线程栅栏
 * @param[in] br: 线程栅栏指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个线程栅栏对象，不可重入
 */
static __xwos_inline_api
xwer_t xwos_br_delete(struct xwos_br * br)
{
        return xwosdl_br_delete(&br->osbr);
}

/**
 * @brief XWOS API：获取线程栅栏对象的标签
 * @param[in] br: 线程栅栏对象的指针
 * @return 线程栅栏对象的标签
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwsq_t xwos_br_gettik(struct xwos_br * br)
{
        return xwosdl_br_gettik(&br->osbr);
}

/**
 * @brief XWOS API：获取线程栅栏对象的描述符
 * @param[in] br: 线程栅栏对象的指针
 * @return 线程栅栏对象的描述符
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwos_br_d xwos_br_getd(struct xwos_br * br)
{
        xwos_br_d brd;

        brd.br = br;
        brd.tik = xwosdl_br_gettik(&br->osbr);
        return brd;
}

/**
 * @brief XWOS API：检查线程栅栏对象的标签并增加引用计数
 * @param[in] brd: 线程栅栏对象的描述符
 * @return 错误码
 * @retval XWOK: OK
 * @retval -ENILOBJD: 空的对象描述符
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_br_acquire(xwos_br_d brd)
{
        return xwosdl_br_acquire(&brd.br->osbr, brd.tik);
}

/**
 * @brief XWOS API：检查对象的标签并减少引用计数
 * @param[in] brd: 线程栅栏对象的描述符
 * @return 错误码
 * @retval XWOK: OK
 * @retval -ENILOBJD: 空的对象描述符
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_br_release(xwos_br_d brd)
{
        return xwosdl_br_release(&brd.br->osbr, brd.tik);
}

/**
 * @brief XWOS API：增加线程栅栏对象的引用计数
 * @param[in] br: 线程栅栏对象的指针
 * @return 错误码
 * @retval XWOK: OK
 * @retval -EOBJDEAD: 对象无效
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @details
 * 此函数主要用于管理**静态对象**的引用计数。
 * 若用于**动态对象**，需要确保对象的指针一定不是野指针。
 */
static __xwos_inline_api
xwer_t xwos_br_grab(struct xwos_br * br)
{
        return xwosdl_br_grab(&br->osbr);
}

/**
 * @brief XWOS API：减少线程栅栏对象的引用计数
 * @param[in] br: 线程栅栏对象的指针
 * @return 错误码
 * @retval XWOK: OK
 * @retval -EOBJDEAD: 对象无效
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @details
 * 此函数主要用于管理**静态对象**的引用计数。
 * 若用于**动态对象**，需要确保对象的指针一定不是野指针。
 */
static __xwos_inline_api
xwer_t xwos_br_put(struct xwos_br * br)
{
        return xwosdl_br_put(&br->osbr);
}

/**
 * @brief XWOS API：绑定线程栅栏到信号选择器
 * @param[in] br: 线程栅栏对象的指针
 * @param[in] sel: 信号选择器的指针
 * @param[in] pos: 线程栅栏对象映射到位图中的位置
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ECHRNG: 位置超出范围
 * @retval -EALREADY: 同步对象已经绑定到事件对象
 * @retval -EBUSY: 通道已经被其他同步对象独占
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个线程栅栏对象，不可重入
 * - 绑定方式：非独占绑定
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
 * @param[in] br: 线程栅栏对象的指针
 * @param[in] sel: 信号选择器的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个线程栅栏对象，不可重入
 */
static __xwos_inline_api
xwer_t xwos_br_unbind(struct xwos_br * br, struct xwos_sel * sel)
{
        return xwosdl_br_unbind(&br->osbr, &sel->ossel);
}

/**
 * @brief XWOS API：中断线程栅栏等待队列中的所有节点
 * @param[in] br: 线程栅栏对象的指针
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
 * @brief XWOS API：获取线程栅栏中线程槽数量
 * @param[in] br: 线程栅栏对象的指针
 * @param[out] numbuf: 指向缓冲区的指针，通过此缓冲区返回线程槽数量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_br_get_num(struct xwos_br * br, xwsz_t * numbuf)
{
        return xwosdl_br_get_num(&br->osbr, numbuf);
}

/**
 * @brief XWOS API：等待所有线程到达栅栏
 * @param[in] br: 线程栅栏对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ECHRNG: 位置超出范围
 * @retval -EINTR: 等待被中断
 * @retval -ENOTTHDCTX：不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_br_wait(struct xwos_br * br)
{
        return xwosdl_br_wait(&br->osbr);
}

/**
 * @brief XWOS API：限时等待所有线程到达栅栏
 * @param[in] br: 线程栅栏对象的指针
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ECHRNG: 位置超出范围
 * @retval -ETIMEDOUT: 超时
 * @retval -EINTR: 等待被中断
 * @retval -ENOTTHDCTX：不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @details
 * 如果 ```to``` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
static __xwos_inline_api
xwer_t xwos_br_wait_to(struct xwos_br * br, xwtm_t to)
{
        return xwosdl_br_wait_to(&br->osbr, to);
}

/**
 * @} xwos_sync_br
 */

#endif /* xwos/osal/sync/br.h */
