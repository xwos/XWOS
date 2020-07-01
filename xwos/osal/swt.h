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
#include <xwos/osdl/os.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief [XWOSAL] 软件定时器
 */
struct xwosal_swt {
        struct xwosdl_swt osswt; /**< 操作系统的软件定时器 */
};

/**
 * @brief [XWOSAL] 软件定时器回调函数指针类型
 */
typedef void (* xwosal_swt_f)(struct xwosal_swt * /*swt*/, void * /*arg*/);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief [XWOSAL] 软件定时器标志 —— 无
 */
#define XWOSAL_SWT_FLAG_NULL            XWOSDL_SWT_FLAG_NULL

/**
 * @brief [XWOSAL] 软件定时器标志 —— 自动重启
 */
#define XWOSAL_SWT_FLAG_RESTART         XWOSDL_SWT_FLAG_RESTART

/**
 * @brief [XWOSAL] 软件定时器标志 —— 自动销毁
 */
#define XWOSAL_SWT_FLAG_AUTORM          XWOSDL_SWT_FLAG_AUTORM

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       APIs        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOSAL API：静态方式初始化软件定时器
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
xwer_t xwosal_swt_init(struct xwosal_swt * swt, const char * name, xwsq_t flag)
{
        return xwosdl_swt_init(&swt->osswt, name, flag);
}

/**
 * @brief XWOSAL API：销毁静态方式初始化的软件定时器
 * @param swt: (I) 软件定时器的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwosal_swt_destroy(struct xwosal_swt * swt)
{
        return xwosdl_swt_destroy(&swt->osswt);
}

/**
 * @brief XWOSAL API：动态方式创建软件定时器
 * @param swtidbuf: (O) 指向缓冲区的指针，通过此缓冲区返回ID
 * @param name: (I) 软件定时器的名字
 * @param flag: (I) 软件定时器的标志
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwosal_swt_create(xwid_t * swtidbuf, const char * name, xwsq_t flag)
{
        return xwosdl_swt_create(swtidbuf, name, flag);
}

/**
 * @brief XWOSAL API：删除动态方式创建的软件定时器
 * @param swtid: (I) 软件定时器ID
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwosal_swt_delete(xwid_t swtid)
{
        return xwosdl_swt_delete(swtid);
}

/**
 * @brief XWOSAL API：从软件定时器对象的指针获取软件定时器ID
 * @param swt: (I) 软件定时器对象的指针
 * @return 软件定时器ID
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwid_t xwosal_swt_get_id(struct xwosal_swt * swt)
{
        return xwosdl_swt_get_id(&swt->osswt);
}

/**
 * @brief XWOSAL API：从软件定时器ID获取对象的指针
 * @param swtid: (I) 软件定时器ID
 * @return 软件定时器对象的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
struct xwosal_swt * xwosal_swt_get_obj(xwid_t swtid)
{
        return (struct xwosal_swt *)xwosdl_swt_get_obj(swtid);
}

/**
 * @brief XWOSAL API：启动软件定时器
 * @param swtid: (I) 软件定时器ID
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
xwer_t xwosal_swt_start(xwid_t swtid, xwtm_t base, xwtm_t period,
                        xwosal_swt_f callback, void * arg)
{
        return xwosdl_swt_start(swtid, base, period, (xwosdl_swt_f)callback, arg);
}

/**
 * @brief XWOSAL API：停止软件定时器
 * @param swtid: (I) 软件定时器ID
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwosal_swt_stop(xwid_t swtid)
{
        return xwosdl_swt_stop(swtid);
}

#endif /* xwos/osal/swt.h */
