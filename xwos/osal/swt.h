/**
 * @file
 * @brief 操作系统抽象层：软件定时器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_osal_swt_h__
#define __xwos_osal_swt_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/osal/jack/swt.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOS API：软件定时器
 */
struct xwos_swt {
        struct xwosdl_swt osswt;
};

/**
 * @brief XWOS API：软件定时器回调函数指针类型
 */
typedef void (* xwos_swt_f)(struct xwos_swt * /*swt*/, void * /*arg*/);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOS API：软件定时器标志 —— 无
 */
#define XWOS_SWT_FLAG_NULL            XWOSDL_SWT_FLAG_NULL

/**
 * @brief XWOS API：软件定时器标志 —— 自动重启
 * @note 不可与XWOS_SWT_FLAG_RESTART同时使用
 */
#define XWOS_SWT_FLAG_RESTART         XWOSDL_SWT_FLAG_RESTART

/**
 * @brief XWOS API：软件定时器标志 —— 自动销毁
 * @note
 * - 不可与XWOS_SWT_FLAG_RESTART同时使用
 * - 不可用于静态初始化的软件定时器
 */
#define XWOS_SWT_FLAG_AUTORM          XWOSDL_SWT_FLAG_AUTORM

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       APIs        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOS API：静态方式初始化软件定时器
 * @param swt: (I) 软件定时器的指针
 * @param name: (I) 软件定时器的名字
 * @param flag: (I) 软件定时器的标志
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwos_swt_init(struct xwos_swt * swt, const char * name, xwsq_t flag)
{
        return xwosdl_swt_init(&swt->osswt, name, flag);
}

/**
 * @brief XWOS API：销毁静态方式初始化的软件定时器
 * @param swt: (I) 软件定时器的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwos_swt_destroy(struct xwos_swt * swt)
{
        return xwosdl_swt_destroy(&swt->osswt);
}

/**
 * @brief XWOS API：动态方式创建软件定时器
 * @param swtp: (O) 指向缓冲区的指针，通过此缓冲区返回软件定时器的指针
 * @param name: (I) 软件定时器的名字
 * @param flag: (I) 软件定时器的标志
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwos_swt_create(struct xwos_swt ** swtp, const char * name, xwsq_t flag)
{
        return xwosdl_swt_create((struct xwosdl_swt **)swtp, name, flag);
}

/**
 * @brief XWOS API：删除动态方式创建的软件定时器
 * @param swt: (I) 软件定时器的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwos_swt_delete(struct xwos_swt * swt)
{
        return xwosdl_swt_delete(&swt->osswt);
}

/**
 * @brief XWOS API：启动软件定时器
 * @param swt: (I) 软件定时器的指针
 * @param base: (I) 软件定时器的初始时间
 * @param period: (I) 软件定时器的周期时间
 * @param callback: (I) 软件定时器的回调函数
 * @param arg: (I) 回调函数的参数
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwos_swt_start(struct xwos_swt * swt, xwtm_t base, xwtm_t period,
                      xwos_swt_f callback, void * arg)
{
        return xwosdl_swt_start(&swt->osswt, base, period,
                                (xwosdl_swt_f)callback, arg);
}

/**
 * @brief XWOS API：停止软件定时器
 * @param swt: (I) 软件定时器的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwos_swt_stop(struct xwos_swt * swt)
{
        return xwosdl_swt_stop(&swt->osswt);
}

#endif /* xwos/osal/swt.h */
