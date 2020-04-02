/**
 * @file
 * @brief 操作系统抽象层：信号量
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_osal_sync_semaphore_h__
#define __xwos_osal_sync_semaphore_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/osdl/os.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 操作系统抽象层：信号量
 */
struct xwosal_smr {
        struct xwosdl_smr ossmr; /**< 操作系统的信号量 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       APIs        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 操作系统抽象层API：静态方式初始化信号量
 * @param smr: (I) 信号量的指针
 * @param val: (I) 信号量的初始值
 * @param max: (I) 信号量的最大值
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *smr* ，不可重入
 */
static __xwos_inline_api
xwer_t xwosal_smr_init(struct xwosal_smr * smr, xwssq_t val, xwssq_t max)
{
        return xwosdl_smr_init(&smr->ossmr, val, max);
}

/**
 * @brief 操作系统抽象层API：销毁静态方式初始化的信号量
 * @param smr: (I) 信号量的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *smr* ，不可重入
 */
static __xwos_inline_api
xwer_t xwosal_smr_destroy(struct xwosal_smr * smr)
{
        return xwosdl_smr_destroy(&smr->ossmr);
}

/**
 * @brief 操作系统抽象层API：动态方式创建信号量
 * @param smridbuf: (O) 指向缓冲区的指针，通过此缓冲区返回ID
 * @param val: (I) 信号量的初始值
 * @param max: (I) 信号量的最大值
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwosal_smr_create(xwid_t * smridbuf, xwssq_t val, xwssq_t max)
{
        return xwosdl_smr_create(smridbuf, val, max);
}

/**
 * @brief 操作系统抽象层API：删除动态方式创建的信号量
 * @param smrid: (I) 信号量ID
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *smr* ，不可重入
 */
static __xwos_inline_api
xwer_t xwosal_smr_delete(xwid_t smrid)
{
        return xwosdl_smr_delete(smrid);
}

/**
 * @brief 操作系统抽象层API：从软件信号量对象指针获取其ID
 * @param smr: (I) 信号量对象的指针
 * @return 信号量ID
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwid_t xwosal_smr_get_id(struct xwosal_smr * smr)
{
        return (xwid_t)xwosdl_smr_get_id(&smr->ossmr);
}

/**
 * @brief 操作系统抽象层API：从信号量ID获取对象的指针
 * @param smrid: (I) 信号量ID
 * @return 信号量对象的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
struct xwosal_smr * xwosal_smr_get_obj(xwid_t smrid)
{
        return (struct xwosal_smr *)xwosdl_smr_get_obj(smrid);
}

/**
 * @brief 操作系统抽象层API：绑定信号量到信号选择器
 * @param smrid: (I) 信号量的ID
 * @param sltid: (I) 信号选择器的ID
 * @param pos: (I) 信号量对象映射到位图中的位置
 * @return 错误码
 * @retval OK: OK
 * @retval -ETYPE: 信号选择器或信号量类型错误
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *smrid* ，不可重入
 */
static __xwos_inline_api
xwer_t xwosal_smr_bind(xwid_t smrid, xwid_t sltid, xwsq_t pos)
{
        return xwosdl_smr_bind(smrid, sltid, pos);
}

/**
 * @brief 操作系统抽象层API：从信号选择器上解绑信号量
 * @param smrid: (I) 信号量的ID
 * @param sltid: (I) 信号选择器的ID
 * @return 错误码
 * @retval OK: OK
 * @retval -ETYPE: 信号选择器类型错误
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *smrid* ，不可重入
 */
static __xwos_inline_api
xwer_t xwosal_smr_unbind(xwid_t smrid, xwid_t sltid)
{
        return xwosdl_smr_unbind(smrid, sltid);
}

/**
 * @brief 操作系统抽象层API：冻结信号量
 * @param smrid: (I) 信号量ID
 * @return 错误码
 * @retval OK: OK
 * @retval -EALREADY: 信号量已被冻结
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 已冻结的信号量不允许增加(V操作)，但可以被测试(P操作)，
 *   测试信号量的线程会被加入到信号量的等待队列。
 */
static __xwos_inline_api
xwer_t xwosal_smr_freeze(xwid_t smrid)
{
        return xwosdl_smr_freeze(smrid);
}

/**
 * @brief 操作系统抽象层API：解冻信号量，并重新初始化
 * @param smrid: (I) 信号量ID
 * @param val: (I) 信号量的初始值
 * @param max: (I) 信号量的最大值
 * @return 错误码
 * @retval OK: OK
 * @retval -EALREADY: 信号量未被冻结
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对已冻结的信号量起作用，对未冻结的信号量调用此函数将返回错误码。
 */
static __xwos_inline_api
xwer_t xwosal_smr_thaw(xwid_t smrid, xwssq_t val, xwssq_t max)
{
        return xwosdl_smr_thaw(smrid, val, max);
}

/**
 * @brief 操作系统抽象层API：发布信号量
 * @param smrid: (I) 信号量ID
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对未冻结的信号量起作用，已冻结的信号量将得到错误码-ENEGATIVE。
 */
static __xwos_inline_api
xwer_t xwosal_smr_post(xwid_t smrid)
{
        return xwosdl_smr_post(smrid);
}

/**
 * @brief 操作系统抽象层API：等待并获取信号量
 * @param smrid: (I) 信号量ID
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwosal_smr_wait(xwid_t smrid)
{
        return xwosdl_smr_wait(smrid);
}

/**
 * @brief 操作系统抽象层API：尝试获取信号量
 * @param smrid: (I) 信号量ID
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数不会阻塞调用者，因此可以在中断上下文中使用。
 */
static __xwos_inline_api
xwer_t xwosal_smr_trywait(xwid_t smrid)
{
        return xwosdl_smr_trywait(smrid);
}

/**
 * @brief 操作系统抽象层API：限时等待获取信号量
 * @param smrid: (I) 信号量ID
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval OK: OK
 * @retval -ETIMEDOUT: 超时
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 函数返回 *-ETIMEDOUT* 时， *xwtm* 指向的缓冲区内的值减为0。
 */
static __xwos_inline_api
xwer_t xwosal_smr_timedwait(xwid_t smrid, xwtm_t * xwtm)
{
        return xwosdl_smr_timedwait(smrid, xwtm);
}

/**
 * @brief 操作系统抽象层API：等待并获取信号量，且等待不可被中断
 * @param smrid: (I) 信号量ID
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwosal_smr_wait_unintr(xwid_t smrid)
{
        return xwosdl_smr_wait_unintr(smrid);
}

/**
 * @brief 操作系统抽象层API：获取信号量计数器的值
 * @param smrid: (I) 信号量ID
 * @param sval: (O) 指向缓冲区的指针，通过此缓冲区返回信号量计数器的值
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwosal_smr_getvalue(xwid_t smrid, xwssq_t * sval)
{
        return xwosdl_smr_getvalue(smrid, sval);
}

#endif /* xwos/osal/sync/semaphore.h */
