/**
 * @file
 * @brief 玄武OS UP内核：软件定时器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
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
#include <xwos/up/skd.h>
#include <xwos/up/tt.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwup_skd;
struct xwup_swt;

/**
 * @brief 软件定时器标志枚举
 */
enum xwup_swt_flag_em {
        XWUP_SWT_FLAG_NULL = 0, /**< null */
        XWUP_SWT_FLAG_RESTART = (1 << 0), /**< auto-restart */
        XWUP_SWT_FLAG_AUTORM = (1 << 1), /**< auto-remove  */
        XWUP_SWT_FLAG_MASK = XWUP_SWT_FLAG_RESTART | XWUP_SWT_FLAG_AUTORM,
};

/**
 * @brief 软件定时器回调函数
 */
typedef void (* xwup_swt_f)(struct xwup_swt *, void *);

/**
 * @brief 软件定时器控制块
 */
struct xwup_swt {
        const char * name; /**< 名字 */
        xwsq_t flag; /**< 标志 */
        struct xwup_ttn ttn; /**< 时间树节点 */
        xwup_swt_f cb; /**< 回调函数 */
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
xwer_t xwup_swt_init(struct xwup_swt * swt,
                     const char * name,
                     xwsq_t flag);
xwer_t xwup_swt_destroy(struct xwup_swt * swt);
xwer_t xwup_swt_create(struct xwup_swt ** ptrbuf,
                       const char * name,
                       xwsq_t flag);
xwer_t xwup_swt_delete(struct xwup_swt * swt);
xwer_t xwup_swt_start(struct xwup_swt * swt,
                      xwtm_t base, xwtm_t period,
                      xwup_swt_f cb, void * arg);
xwer_t xwup_swt_stop(struct xwup_swt * swt);

#endif /* xwos/up/swt.h */
