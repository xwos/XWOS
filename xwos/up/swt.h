/**
 * @file
 * @brief XWOS UP内核：软件定时器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_up_swt_h__
#define __xwos_up_swt_h__

#include <xwos/standard.h>
#include <xwos/lib/object.h>
#include <xwos/up/skd.h>
#include <xwos/up/tt.h>

struct xwup_skd;
struct xwup_swt;

/**
 * @brief 软件定时器标志枚举
 */
enum xwup_swt_flag_em {
        XWUP_SWT_FLAG_NULL = 0U, /**< 无（默认标志） */
        XWUP_SWT_FLAG_RESTART = 1U, /**< 自动重启，BIT(0) */
        XWUP_SWT_FLAG_MASK = XWUP_SWT_FLAG_RESTART,
};

/**
 * @brief 软件定时器回调函数
 */
typedef void (* xwup_swt_f)(struct xwup_swt *, void *);

/**
 * @brief 软件定时器控制块
 */
struct xwup_swt {
        struct xwos_object xwobj; /**< C语言面向对象：继承struct xwos_object */
        xwsq_t flag; /**< 标志 */
        struct xwup_ttn ttn; /**< 时间树节点 */
        xwup_swt_f cb; /**< 回调函数 */
        void * arg; /**< 回调函数参数 */
        xwtm_t period; /**< 周期 */
};

#if defined(XWOSCFG_SKD_SWT_MEMSLICE) && (1 == XWOSCFG_SKD_SWT_MEMSLICE)
xwer_t xwup_swt_cache_init(xwptr_t zone_origin, xwsz_t zone_size);
#endif
xwer_t xwup_swt_init(struct xwup_swt * swt, xwsq_t flag);
xwer_t xwup_swt_fini(struct xwup_swt * swt);
xwer_t xwup_swt_create(struct xwup_swt ** ptrbuf, xwsq_t flag);
xwer_t xwup_swt_delete(struct xwup_swt * swt, xwsq_t tik);
xwer_t xwup_swt_acquire(struct xwup_swt * swt, xwsq_t tik);
xwer_t xwup_swt_release(struct xwup_swt * swt, xwsq_t tik);
xwer_t xwup_swt_grab(struct xwup_swt * swt);
xwer_t xwup_swt_put(struct xwup_swt * swt);
xwer_t xwup_swt_start(struct xwup_swt * swt,
                      xwtm_t origin, xwtm_t period,
                      xwup_swt_f cb, void * arg);
xwer_t xwup_swt_stop(struct xwup_swt * swt);

#endif /* xwos/up/swt.h */
