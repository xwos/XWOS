/**
 * @file
 * @brief 操作系统抽象层：信号选择器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_osal_sync_sel_h__
#define __xwos_osal_sync_sel_h__

#include <xwos/standard.h>
#include <xwos/osal/jack/sync/sel.h>

/**
 * @defgroup xwos_sel 信号选择器
 * @{
 */

/**
 * @brief XWOS API：信号选择器对象
 */
struct xwos_sel {
        struct xwosdl_sel ossel;
};

/**
 * @brief XWOS API：信号选择器对象描述符
 */
typedef struct {
        struct xwos_sel * sel; /**< 信号选择器对象的指针 */
        xwsq_t tik; /**< 标签 */
} xwos_sel_d;

/**
 * @brief XWOS API：空的信号选择器对象描述符
 */
#define XWOS_SEL_NILD ((xwos_sel_d){NULL, 0,})

/**
 * @brief XWOS API：静态方式初始化信号选择器
 * @param[in] sel: 信号选择器对象的指针
 * @param[in] num: 信号选择器中信号的数量
 * @param[in] bmp: 信号选择器用来记录信号状态的位图缓冲区
 * @param[in] msk: 信号选择器用来记录掩码状态的位图缓冲区
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个信号选择器，不可重入
 */
static __xwos_inline_api
xwer_t xwos_sel_init(struct xwos_sel * sel, xwsz_t num,
                     xwbmp_t * bmp, xwbmp_t * msk)
{
        return xwosdl_sel_init(&sel->ossel, num, bmp, msk);
}

/**
 * @brief XWOS API：销毁静态方式初始化的信号选择器
 * @param[in] sel: 信号选择器对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个信号选择器，不可重入
 */
static __xwos_inline_api
xwer_t xwos_sel_fini(struct xwos_sel * sel)
{
        return xwosdl_sel_fini(&sel->ossel);
}

/**
 * @brief XWOS API：动态方式创建信号选择器
 * @param[out] selbuf: 指向缓冲区的指针，通过此缓冲区返回信号选择器对象的指针
 * @param[in] num: 信号选择器中信号的数量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOMEM: 内存不足
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_sel_create(struct xwos_sel ** selbuf, xwsz_t num)
{
        return xwosdl_sel_create((struct xwosdl_sel **)selbuf, num);
}

/**
 * @brief XWOS API：删除动态方式创建的信号选择器
 * @param[in] sel: 信号选择器对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个信号选择器，不可重入
 */
static __xwos_inline_api
xwer_t xwos_sel_delete(struct xwos_sel * sel)
{
        return xwosdl_sel_delete(&sel->ossel);
}

/**
 * @brief XWOS API：获取信号选择器对象的标签
 * @param[in] sel: 信号选择器对象的指针
 * @return 信号选择器对象的标签
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwsq_t xwos_sel_gettik(struct xwos_sel * sel)
{
        return xwosdl_sel_gettik(&sel->ossel);
}

/**
 * @brief XWOS API：获取信号选择器对象的描述符
 * @param[in] sel: 信号选择器对象的指针
 * @return 信号选择器对象的描述符
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwos_sel_d xwos_sel_getd(struct xwos_sel * sel)
{
        xwos_sel_d seld;

        seld.sel = sel;
        seld.tik = xwosdl_sel_gettik(&sel->ossel);
        return seld;
}

/**
 * @brief XWOS API：检查信号选择器对象的标签并增加引用计数
 * @param[in] seld: 信号选择器对象的描述符
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
xwer_t xwos_sel_acquire(xwos_sel_d seld)
{
        return xwosdl_sel_acquire(&seld.sel->ossel, seld.tik);
}

/**
 * @brief XWOS API：检查对象的标签并减少引用计数
 * @param[in] seld: 信号选择器对象的描述符
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
xwer_t xwos_sel_release(xwos_sel_d seld)
{
        return xwosdl_sel_release(&seld.sel->ossel, seld.tik);
}

/**
 * @brief XWOS API：增加信号选择器对象的引用计数
 * @param[in] sel: 信号选择器对象的指针
 * @return 错误码
 * @retval XWOK: OK
 * @retval -EOBJDEAD: 对象无效
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_sel_grab(struct xwos_sel * sel)
{
        return xwosdl_sel_grab(&sel->ossel);
}

/**
 * @brief XWOS API：减少信号选择器对象的引用计数
 * @param[in] sel: 信号选择器对象的指针
 * @return 错误码
 * @retval XWOK: OK
 * @retval -EOBJDEAD: 对象无效
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_sel_put(struct xwos_sel * sel)
{
        return xwosdl_sel_put(&sel->ossel);
}

/**
 * @brief XWOS API：绑定源信号选择器到目的信号选择器
 * @param[in] src: 源信号选择器对象的指针
 * @param[in] dst: 目的信号选择器对象的指针
 * @param[in] pos: 同步对象映射到位图中的位置
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ECHRNG: 位置超出范围
 * @retval -EALREADY: 同步对象已经绑定到事件对象
 * @retval -EBUSY: 通道已经被其他同步对象独占
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个src，不可重入
 * - 绑定方式：非独占绑定
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
 * @param[in] src: 源信号选择器对象的指针
 * @param[in] dst: 目的信号选择器对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个src，不可重入
 */
static __xwos_inline_api
xwer_t xwos_sel_unbind(struct xwos_sel * src, struct xwos_sel * dst)
{
        return xwosdl_sel_unbind(&src->ossel, &dst->ossel);
}

/**
 * @brief XWOS API：中断信号选择器等待队列中的所有节点
 * @param[in] sel: 信号选择器对象的指针
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
 * @brief XWOS API：获取信号选择器中的信号槽的数量
 * @param[in] sel: 信号选择器对象的指针
 * @param[out] numbuf: 指向缓冲区的指针，通过此缓冲区返回信号槽的数量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_sel_get_num(struct xwos_sel * sel, xwsz_t * numbuf)
{
        return xwosdl_sel_get_num(&sel->ossel, numbuf);
}

/**
 * @brief XWOS API：等待信号选择器中的触发信号
 * @param[in] sel: 信号选择器对象的指针
 * @param[in] msk: 同步对象位图掩码，表示只关注掩码内的同步对象
 * @param[out] trg: 指向缓冲区的指针，通过此缓冲区返回已触发的同步对象位图掩码
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHD: 不在线程上下文中
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
 * @brief XWOS API：检测一下信号选择器中的触发信号，不会阻塞调用线程
 * @param[in] sel: 信号选择器对象的指针
 * @param[in] msk: 同步对象位图掩码，表示只关注掩码内的同步对象
 * @param[out] trg: 指向缓冲区的指针，通过此缓冲区返回已触发的同步对象位图掩码
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
 * @brief XWOS API：限时等待信号选择器中的信号
 * @param[in] sel: 信号选择器对象的指针
 * @param[in] msk: 同步对象位图掩码，表示只关注掩码内的同步对象
 * @param[out] trg: 指向缓冲区的指针，通过此缓冲区返回已触发的同步对象位图掩码
 * @param[in,out] xwtm: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示期望的阻塞等待时间
 * + (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETIMEDOUT: 超时，xwtm指向的缓冲区内的时间会减为0
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_sel_timedselect(struct xwos_sel * sel,
                            xwbmp_t msk[], xwbmp_t trg[],
                            xwtm_t * xwtm)
{
        return xwosdl_sel_timedselect(&sel->ossel, msk, trg, xwtm);
}

/**
 * @} xwos_sel
 */

#endif /* xwos/osal/sync/sel.h */
