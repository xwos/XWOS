/**
 * @file
 * @brief 操作系统抽象层：条件量
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_osal_sync_condition_h__
#define __xwos_osal_sync_condition_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/osdl/os.h>
#include <xwos/osal/lock/mutex.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 操作系统抽象层：条件量
 */
struct xwosal_cdt {
        struct xwosdl_cdt oscdt; /**< 操作系统的条件量 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       APIs        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 操作系统抽象层API：静态方式初始化条件量
 * @param cdt: (I) 条件量的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *cdt* ，不可重入
 */
static __xwos_inline_api
xwer_t xwosal_cdt_init(struct xwosal_cdt * cdt)
{
        return xwosdl_cdt_init(&cdt->oscdt);
}

/**
 * @brief 操作系统抽象层API：销毁静态方式初始化的条件量
 * @param cdt: (I) 条件量的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *cdt* ，不可重入
 */
static __xwos_inline_api
xwer_t xwosal_cdt_destroy(struct xwosal_cdt * cdt)
{
        return xwosdl_cdt_destroy(&cdt->oscdt);
}

/**
 * @brief 操作系统抽象层API：动态方式创建条件量
 * @param cdtidbuf: (O) 指向缓冲区的指针，通过此缓冲区返回ID
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwosal_cdt_create(xwid_t * cdtidbuf)
{
        return xwosdl_cdt_create(cdtidbuf);
}

/**
 * @brief 操作系统抽象层API：删除动态方式创建的条件量
 * @param cdtid: (I) 条件量ID
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *cdt* ，不可重入
 */
static __xwos_inline_api
xwer_t xwosal_cdt_delete(xwid_t cdtid)
{
        return xwosdl_cdt_delete(cdtid);
}

/**
 * @brief 操作系统抽象层API：从条件量对象指针获取其ID
 * @param cdt: (I) 条件量对象的指针
 * @return 条件量ID
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwid_t xwosal_cdt_get_id(struct xwosal_cdt * cdt)
{
        return xwosdl_cdt_get_id(&cdt->oscdt);
}

/**
 * @brief 操作系统抽象层API：从条件量ID获取对象的指针
 * @param cdtid: (I) 条件量ID
 * @return 条件量对象的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
struct xwosal_cdt * xwosal_cdt_get_obj(xwid_t cdtid)
{
        return (struct xwosal_cdt *)xwosdl_cdt_get_obj(cdtid);
}

/**
 * @brief 操作系统抽象层API：绑定条件量到信号选择器
 * @param cdtid: (I) 条件量的ID
 * @param sltid: (I) 信号选择器的ID
 * @param pos: (I) 条件量对象映射到位图中的位置
 * @return 错误码
 * @retval OK: OK
 * @retval -ETYPE: 信号选择器或条件量类型错误
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *cdtid* ，不可重入
 * @note
 * - 绑定了信号选择器的条件量，只有广播时才会向信号选择器传递选择信号。
 */
static __xwos_inline_api
xwer_t xwosal_cdt_bind(xwid_t cdtid, xwid_t sltid, xwsq_t pos)
{
        return xwosdl_cdt_bind(cdtid, sltid, pos);
}

/**
 * @brief 操作系统抽象层API：从信号选择器上解绑条件量
 * @param cdtid: (I) 条件量的ID
 * @param sltid: (I) 信号选择器的ID
 * @return 错误码
 * @retval OK: OK
 * @retval -ETYPE: 信号选择器类型错误
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *cdtid* ，不可重入
 */
static __xwos_inline_api
xwer_t xwosal_cdt_unbind(xwid_t cdtid, xwid_t sltid)
{
        return xwosdl_cdt_unbind(cdtid, sltid);
}

/**
 * @brief 操作系统抽象层API：冻结条件量
 * @param cdtid: (I) 条件量ID
 * @return 错误码
 * @retval OK: OK
 * @retval -EALREADY: 条件量已被冻结
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 已冻结的条件量不允许单播或广播，但可以被等待，
 *   测试条件量的线程会加入到条件量等待队列中阻塞等待。
 */
static __xwos_inline_api
xwer_t xwosal_cdt_freeze(xwid_t cdtid)
{
        return xwosdl_cdt_freeze(cdtid);
}

/**
 * @brief 操作系统抽象层API：解冻条件量，并重新初始化
 * @param cdtid: (I) 条件量ID
 * @return 错误码
 * @retval OK: OK
 * @retval -EALREADY: 条件量未被冻结
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对已冻结的条件量起作用，对未冻结的条件量调用此函数将返回错误码。
 */
static __xwos_inline_api
xwer_t xwosal_cdt_thaw(xwid_t cdtid)
{
        return xwosdl_cdt_thaw(cdtid);
}

/**
 * @brief 操作系统抽象层API：中断条件量等待队列中所有线程
 * @param cdtid: (I) 条件量ID
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数会将等待队列中的所有线程中断。
 */
static __xwos_inline_api
xwer_t xwosal_cdt_intr_all(xwid_t cdtid)
{
        return xwosdl_cdt_intr_all(cdtid);
}

/**
 * @brief 操作系统抽象层API：广播条件量，等待队列中的所有线程都会被唤醒
 * @param cdtid: (I) 条件量ID
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对未冻结的条件量起作用，已冻结的条件量将得到错误码-ENEGATIVE。
 */
static __xwos_inline_api
xwer_t xwosal_cdt_broadcast(xwid_t cdtid)
{
        return xwosdl_cdt_broadcast(cdtid);
}

/**
 * @brief 操作系统抽象层API：单播条件量，只会唤醒第一个线程
 * @param cdtid: (I) 条件量ID
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对未冻结的条件量起作用，已冻结的条件量将得到错误码-ENEGATIVE。
 */
static __xwos_inline_api
xwer_t xwosal_cdt_unicast(xwid_t cdtid)
{
        return xwosdl_cdt_unicast(cdtid);
}

/**
 * @brief 操作系统抽象层API：等待条件量
 * @param cdtid: (I) 条件量ID
 * @param lock: (I) 锁
 * @param lktype: (I) 锁的类型
 * @param lkdata: (I) 锁的数据
 * @param datanum: (I) 锁的数据数量
 * @param lkst: (O) 指向缓冲区的指针，通过此缓冲区返回锁的状态
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwosal_cdt_wait(xwid_t cdtid,
                       union xwlk_ulock lock, xwsq_t lktype,
                       void * lkdata, xwsz_t datanum,
                       xwsq_t * lkst)
{
        return xwosdl_cdt_wait(cdtid, lock, lktype, lkdata, datanum, lkst);
}

/**
 * @brief 操作系统抽象层API：限时等待条件量
 * @param cdtid: (I) 条件量ID
 * @param lock: (I) 锁
 * @param lktype: (I) 锁的类型
 * @param lkdata: (I) 锁的数据
 * @param datanum: (I) 锁的数据数量
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @param lkst: (O) 指向缓冲区的指针，通过此缓冲区返回锁的状态
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
xwer_t xwosal_cdt_timedwait(xwid_t cdtid,
                            union xwlk_ulock lock, xwsq_t lktype,
                            void * lkdata, xwsz_t datanum,
                            xwtm_t * xwtm, xwsq_t * lkst)
{
        return xwosdl_cdt_timedwait(cdtid, lock, lktype, lkdata, datanum,
                                    xwtm, lkst);
}

#endif /* xwos/osal/sync/condition.h */
