/**
 * @file
 * @brief 操作系统抽象层：信号选择器
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_osal_sync_selector_h__
#define __xwos_osal_sync_selector_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/osdl/os.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 操作系统抽象层：信号选择器
 */
struct xwosal_selector {
        struct xwosdl_selector osslt; /**< 操作系统的信号选择器 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWOSAL_SELECTOR_MAXNUM XWOSDL_SELECTOR_MAXNUM /**< 信号选择器的最大信号数量 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       APIs        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 操作系统抽象层API：静态方式初始化信号选择器。
 * @param selector: (I) 信号选择器的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *selector* ，不可重入
 */
static __xwos_inline_api
xwer_t xwosal_selector_init(struct xwosal_selector * slt)
{
        return xwosdl_selector_init(&slt->osslt);
}

/**
 * @brief 操作系统抽象层API：销毁静态方式初始化的信号选择器。
 * @param selector: (I) 信号选择器的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *selector* ，不可重入
 */
static __xwos_inline_api
xwer_t xwosal_selector_destroy(struct xwosal_selector * slt)
{
        return xwosdl_selector_destroy(&slt->osslt);
}

/**
 * @brief 操作系统抽象层API：动态方式创建信号选择器。
 * @param sltbuf: (O) 指向缓冲区的指针，通过此缓冲区返回ID
 * @param val: (I) 信号选择器的初始值
 * @param max: (I) 信号选择器的最大值
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwosal_selector_create(xwid_t * sltbuf)
{
        return xwosdl_selector_create(sltbuf);
}

/**
 * @brief 操作系统抽象层API：删除动态方式创建的信号选择器。
 * @param sltid: (I) 信号选择器ID
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *selector* ，不可重入
 */
static __xwos_inline_api
xwer_t xwosal_selector_delete(xwid_t sltid)
{
        return xwosdl_selector_delete(sltid);
}

/**
 * @brief 操作系统抽象层API：从软件信号选择器对象指针获取其ID。
 * @param selector: (I) 信号选择器对象的指针
 * @return 信号选择器ID
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwid_t xwosal_selector_get_id(struct xwosal_selector * slt)
{
        return (xwid_t)xwosdl_selector_get_id(&slt->osslt);
}

/**
 * @brief 操作系统抽象层API：从信号选择器ID获取对象的指针。
 * @param sltid: (I) 信号选择器ID
 * @return 信号选择器对象的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
struct xwosal_selector * xwosal_selector_get_obj(xwid_t sltid)
{
        return (struct xwosal_selector *)xwosdl_selector_get_obj(sltid);
}

/**
 * @brief 操作系统抽象层API：绑定源信号选择器到目的信号选择器，产生级联效果。
 * @param srcid: (I) 源信号选择器的ID
 * @param dstid: (I) 目的信号选择器的ID
 * @param pos: (I) 信号量对象映射到位图中的位置
 * @return 错误码
 * @retval OK: OK
 * @retval -ETYPE: 信号选择器或信号量类型错误
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *srcid* ，不可重入
 * @note
 * - 源信号选择器选择到的信号，将依次往目的信号选择器链上传递。
 */
static __xwos_inline_api
xwer_t xwosal_selector_bind(xwid_t srcid, xwid_t dstid, xwsq_t pos)
{
        return xwosdl_selector_bind(srcid, dstid, pos);
}

/**
 * @brief 操作系统抽象层API：从目的信号选择器上解绑源信号选择器。
 * @param srcid: (I) 源信号选择器的ID
 * @param dstid: (I) 信号选择器的ID
 * @return 错误码
 * @retval OK: OK
 * @retval -ETYPE: 信号选择器类型错误
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *srcid* ，不可重入
 * @note
 * - 源信号选择器选择到的信号，将依次往目的信号选择器链上传递。
 */
static __xwos_inline_api
xwer_t xwosal_selector_unbind(xwid_t srcid, xwid_t dstid)
{
        return xwosdl_selector_unbind(srcid, dstid);
}

/**
 * @brief 操作系统抽象层API：中断信号选择器等待队列中的所有节点。
 * @param sltid: (I) 信号选择器ID
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
xwer_t xwosal_selector_intr_all(xwid_t sltid)
{
        return xwosdl_selector_intr_all(sltid);
}

/**
 * @brief 操作系统抽象层API：测试一下事件对象中绑定的同步对象，不进行阻塞等待
 * @param sltid: (I) 信号选择器ID
 * @param msk: (I) 待触发的信号量的位图掩码
 * @param trg: (O) 指向缓冲区的指针，通过此缓冲区返回已触发的信号量的位图
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ENODATA: 没有任何信号或事件
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwosal_selector_tryselect(xwid_t sltid, xwbmp_t msk[], xwbmp_t trg[])
{
        return xwosdl_selector_select(sltid, msk, trg);
}

/**
 * @brief 操作系统抽象层API：等待信号选择器中绑定的信号量，
 *                           任何信号量的V操作都可唤醒线程。
 * @param sltid: (I) 信号选择器ID
 * @param msk: (I) 待触发的信号量的位图掩码
 * @param trg: (O) 指向缓冲区的指针，通过此缓冲区返回已触发的信号量的位图
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 信号选择器类型错误
 * @retval -EINTR: 等待被中断
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwosal_selector_select(xwid_t sltid, xwbmp_t msk[], xwbmp_t trg[])
{
        return xwosdl_selector_select(sltid, msk, trg);
}

/**
 * @brief 操作系统抽象层API：限时等待信号选择器中绑定的信号量，
 *                           任何信号量的V操作都可唤醒线程。
 * @param sltid: (I) 信号选择器ID
 * @param msk: (I) 待触发的信号量的位图掩码
 * @param trg: (O) 指向缓冲区的指针，通过此缓冲区返回已触发的信号量的位图
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 信号选择器类型错误
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
xwer_t xwosal_selector_timedselect(xwid_t sltid, xwbmp_t msk[], xwbmp_t trg[],
                                   xwtm_t * xwtm)
{
        return xwosdl_selector_timedselect(sltid, msk, trg, xwtm);
}

#endif /* xwos/osal/sync/selector.h */
