/**
 * @file
 * @brief 玄武OS MP内核：软件定时器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_mp_swt_h__
#define __xwos_mp_swt_h__

#include <xwos/standard.h>
#include <xwos/lib/object.h>
#include <xwos/mp/skd.h>
#include <xwos/mp/tt.h>

struct xwmp_skd;
struct xwmp_swt;

/**
 * @brief 软件定时器标志枚举
 */
enum xwmp_swt_flag_em {
        XWMP_SWT_FLAG_NULL = 0, /**< 无（默认标志） */
        XWMP_SWT_FLAG_RESTART = (1 << 0), /**< 自动重启 */
        XWMP_SWT_FLAG_MASK = XWMP_SWT_FLAG_RESTART,
};

/**
 * @brief 软件定时器回调函数
 */
typedef void (* xwmp_swt_f)(struct xwmp_swt *, void *);

/**
 * @brief 软件定时器控制块
 */
struct xwmp_swt {
        struct xwos_object xwobj; /**< C语言面向对象：继承struct xwos_object */
        const char * name; /**< 名字 */
        struct xwmp_skd * xwskd; /**< 调度器 */
        xwsq_t flag; /**< 标志 */
        struct xwmp_ttn ttn; /**< 继承：时间树节点，被锁tt->lock保护 */
        xwmp_swt_f cb; /**< 回调函数 */
        void * arg; /**< 回调函数参数 */
        xwtm_t period; /**< 周期 */
};

#if defined(XWMPCFG_SKD_SWT_MEMSLICE) && (1 == XWMPCFG_SKD_SWT_MEMSLICE)
xwer_t xwmp_swt_cache_init(xwptr_t zone_origin, xwsz_t zone_size);
#endif

xwer_t xwmp_swt_init(struct xwmp_swt * swt,
                     const char * name,
                     xwsq_t flag);
xwer_t xwmp_swt_fini(struct xwmp_swt * swt);
xwer_t xwmp_swt_create(struct xwmp_swt ** ptrbuf,
                       const char * name,
                       xwsq_t flag);
xwer_t xwmp_swt_delete(struct xwmp_swt * swt);

xwer_t xwmp_swt_acquire(struct xwmp_swt * swt, xwsq_t tik);
xwer_t xwmp_swt_release(struct xwmp_swt * swt, xwsq_t tik);
xwer_t xwmp_swt_grab(struct xwmp_swt * swt);
xwer_t xwmp_swt_put(struct xwmp_swt * swt);

xwer_t xwmp_swt_start(struct xwmp_swt * swt,
                      xwtm_t base, xwtm_t period,
                      xwmp_swt_f cb, void * arg);
xwer_t xwmp_swt_stop(struct xwmp_swt * swt);

#endif /* xwos/mp/swt.h */
