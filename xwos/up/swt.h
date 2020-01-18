/**
 * @file
 * @brief XuanWuOS内核：软件定时器
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_swt_h__
#define __xwos_up_swt_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/up/scheduler.h>
#include <xwos/up/tt.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwos_scheduler;
struct xwos_swt;

/**
 * @brief 软件定时器标志枚举
 */
enum xwos_swt_flag_em {
        XWOS_SWT_FLAG_NULL = 0, /**< null */
        XWOS_SWT_FLAG_RESTART = (1 << 0), /**< auto-restart */
        XWOS_SWT_FLAG_AUTORM = (1 << 1), /**< auto-remove  */
        XWOS_SWT_FLAG_MASK = XWOS_SWT_FLAG_RESTART | XWOS_SWT_FLAG_AUTORM,
};

/**
 * @brief 软件定时器回调函数
 */
typedef void (* xwos_swt_f)(struct xwos_swt *, void *);

/**
 * @brief 软件定时器控制块
 */
struct xwos_swt {
        const char * name; /**< 名字 */
        xwsq_t flag; /**< 标志 */
        struct xwos_ttn ttn; /**< 时间树节点 */
        xwos_swt_f cb; /**< 回调函数 */
        void * arg; /**< 回调函数参数 */
        xwtm_t period; /**< 周期 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     internal function prototypes    ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ********       internal inline function implementations        ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********       API function prototypes       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_api
xwer_t xwos_swt_init(struct xwos_swt * swt,
                     const char * name,
                     xwsq_t flag);

__xwos_api
xwer_t xwos_swt_destroy(struct xwos_swt * swt);

__xwos_api
xwer_t xwos_swt_create(struct xwos_swt ** ptrbuf,
                       const char * name,
                       xwsq_t flag);

__xwos_api
xwer_t xwos_swt_delete(struct xwos_swt * swt);

__xwos_api
xwer_t xwos_swt_start(struct xwos_swt * swt,
                      xwtm_t base, xwtm_t period,
                      xwos_swt_f cb, void * arg);

__xwos_api
xwer_t xwos_swt_stop(struct xwos_swt * swt);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      inline API implementations     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwos/up/swt.h */
