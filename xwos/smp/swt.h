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

#ifndef __xwos_smp_swt_h__
#define __xwos_smp_swt_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/object.h>
#include <xwos/smp/scheduler.h>
#include <xwos/smp/tt.h>

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
        XWOS_SWT_FLAG_NULL = 0, /**< 空 */
        XWOS_SWT_FLAG_RESTART = (1 << 0), /**< 自动重启 */
        XWOS_SWT_FLAG_AUTORM = (1 << 1), /**< 自动删除  */
        XWOS_SWT_FLAG_MASK = XWOS_SWT_FLAG_RESTART | XWOS_SWT_FLAG_AUTORM,
};

/**
 * @brief 软件定时器回调函数
 */
typedef void (*xwos_swt_f)(struct xwos_swt *, void *);

/**
 * @brief 软件定时器控制块
 */
struct xwos_swt {
        struct xwos_object xwobj; /**< C语言面向对象：继承struct xwos_object */
        const char * name; /**< 名字 */
        struct xwos_scheduler * xwsd; /**< 调度器 */
        xwsq_t flag; /**< 标志 */
        struct xwos_ttn ttn; /**< 继承：时间树节点，被锁tt->lock保护 */
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
/**
 * @brief 增加对象的引用计数
 * @param swt: (I) 软件定时器控制块对象指针
 * @return 错误码
 */
static __xw_inline
xwer_t xwos_swt_grab(struct xwos_swt * swt)
{
        return xwos_object_grab(&swt->xwobj);
}

/**
 * @brief 减少对象的引用计数
 * @param swt: (I) 软件定时器控制块对象指针
 * @return 错误码
 */
static __xw_inline
xwer_t xwos_swt_put(struct xwos_swt * swt)
{
        return xwos_object_put(&swt->xwobj);
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********       API function prototypes       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if defined(XWSMPCFG_SD_SWT_MEMSLICE) && (1 == XWSMPCFG_SD_SWT_MEMSLICE)
__xwos_init_code
xwer_t xwos_swt_cache_init(xwptr_t zone_origin, xwsz_t zone_size);
#endif /* XWSMPCFG_SD_SWT_MEMSLICE */

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

#endif /* xwos/smp/swt.h */
