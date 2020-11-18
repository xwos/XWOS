/**
 * @file
 * @brief 操作系统抽象层：信号选择器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_osal_sync_sel_h__
#define __xwos_osal_sync_sel_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/osal/jack/sync/sel.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       type        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOS API：信号选择器
 */
struct xwos_sel {
        struct xwosdl_sel ossel;
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macro       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOS API：信号选择器的最大信号数量
 */
#define XWOS_SEL_MAXNUM XWOSDL_SEL_MAXNUM

/**
 * @brief XWOS API：声明信号选择器位图
 */
#define xwos_sel_declare_bitmap(name) xwosdl_sel_declare_bitmap(name)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********        API        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOS API：静态方式初始化信号选择器
 * @param sel: (I) 信号选择器的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的ID或空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个**sel**，不可重入
 */
static __xwos_inline_api
xwer_t xwos_sel_init(struct xwos_sel * sel)
{
        return xwosdl_sel_init(&sel->ossel);
}

/**
 * @brief XWOS API：销毁静态方式初始化的信号选择器
 * @param sel: (I) 信号选择器的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的ID或空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个**sel**，不可重入
 */
static __xwos_inline_api
xwer_t xwos_sel_destroy(struct xwos_sel * sel)
{
        return xwosdl_sel_destroy(&sel->ossel);
}

/**
 * @brief XWOS API：动态方式创建信号选择器
 * @param selp: (O) 指向缓冲区的指针，通过此缓冲区返回信号选择器的指针
 * @param val: (I) 信号选择器的初始值
 * @param max: (I) 信号选择器的最大值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的ID或空指针
 * @retval -ENOMEM: 内存不足
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_sel_create(struct xwos_sel ** selp)
{
        return xwosdl_sel_create((struct xwosdl_sel **)selp);
}

/**
 * @brief XWOS API：删除动态方式创建的信号选择器
 * @param sel: (I) 信号选择器的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的ID或空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个**sel**，不可重入
 */
static __xwos_inline_api
xwer_t xwos_sel_delete(struct xwos_sel * sel)
{
        return xwosdl_sel_delete(&sel->ossel);
}

/**
 * @brief XWOS API：绑定源信号选择器到目的信号选择器
 * @param src: (I) 源信号选择器的指针
 * @param dst: (I) 目的信号选择器的指针
 * @param pos: (I) 信号量对象映射到位图中的位置
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ECHRNG: 位置超出范围
 * @retval -EALREADY: 同步对象已经绑定到事件对象
 * @retval -EBUSY: 通道已经被其他同步对象独占
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个**src**，不可重入
 * @note
 * - 多个信号选择器可以依次绑定，形成信号传递链。源信号选择器被同步对象触发时，
 *   将触发目的信号选择器。
 */
static __xwos_inline_api
xwer_t xwos_sel_bind(struct xwos_sel * src, struct xwos_sel * dst, xwsq_t pos)
{
        return xwosdl_sel_bind(&src->ossel, &dst->ossel, pos);
}

/**
 * @brief XWOS API：从目的信号选择器上解绑源信号选择器
 * @param src: (I) 源信号选择器的指针
 * @param dst: (I) 信号选择器的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个**src**，不可重入
 */
static __xwos_inline_api
xwer_t xwos_sel_unbind(struct xwos_sel * src, struct xwos_sel * dst)
{
        return xwosdl_sel_unbind(&src->ossel, &dst->ossel);
}

/**
 * @brief XWOS API：中断信号选择器等待队列中的所有节点
 * @param sel: (I) 信号选择器的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_sel_intr_all(struct xwos_sel * sel)
{
        return xwosdl_sel_intr_all(&sel->ossel);
}

/**
 * @brief XWOS API：检测一下信号选择器中的触发信号，不会阻塞调用者
 * @param sel: (I) 信号选择器的指针
 * @param msk: (I) 待触发的信号量的位图掩码
 * @param trg: (O) 指向缓冲区的指针，通过此缓冲区返回已触发的信号量的位图
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENODATA: 没有任何同步对象触发信号选择器
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_sel_tryselect(struct xwos_sel * sel, xwbmp_t msk[], xwbmp_t trg[])
{
        return xwosdl_sel_select(&sel->ossel, msk, trg);
}

/**
 * @brief XWOS API：等待信号选择器中的触发信号
 * @param sel: (I) 信号选择器的指针
 * @param msk: (I) 待触发的信号量的位图掩码
 * @param trg: (O) 指向缓冲区的指针，通过此缓冲区返回已触发的信号量的位图
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHRD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_sel_select(struct xwos_sel * sel, xwbmp_t msk[], xwbmp_t trg[])
{
        return xwosdl_sel_select(&sel->ossel, msk, trg);
}

/**
 * @brief XWOS API：限时等待信号选择器中的信号
 * @param sel: (I) 信号选择器的指针
 * @param msk: (I) 待触发的信号量的位图掩码
 * @param trg: (O) 指向缓冲区的指针，通过此缓冲区返回已触发的信号量的位图
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETIMEDOUT: 超时
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHRD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 函数返回返回**-ETIMEDOUT**时，**xwtm**指向的缓冲区内的期望时间会减为0。
 */
static __xwos_inline_api
xwer_t xwos_sel_timedselect(struct xwos_sel * sel,
                            xwbmp_t msk[], xwbmp_t trg[],
                            xwtm_t * xwtm)
{
        return xwosdl_sel_timedselect(&sel->ossel, msk, trg, xwtm);
}

#endif /* xwos/osal/sync/sel.h */
