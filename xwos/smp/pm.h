/**
 * @file
 * @brief XuanWuOS内核：内核电源管理
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_smp_pm_h__
#define __xwos_smp_pm_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/smp/lock/spinlock.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwos_pmdm;

typedef void (*lpmntf_cb_f)(struct xwos_pmdm *, void *); /**< 电源管理领域中的所有
                                                              调度器进入低功耗模式
                                                              时的回调函数 */

/**
 * @brief 电源管理领域
 */
struct xwos_pmdm {
        /* 配置 */
        xwsz_t xwsd_num; /**< 调度器数量 */
        lpmntf_cb_f lpmntf_cb; /**< 电源管理领域中的所有调度器进入低功耗模式
                                    时的回调函数 */
        void * arg; /**< 虚函数参数 */

        /* 私有成员 */
        __atomic xwsz_t lpmxwsd_cnt; /**< 已进入暂停模式的调度器的计数器 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     internal function prototypes    ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_code
void xwos_pmdm_inc_lpmxwsd_cnt(struct xwos_pmdm * pmdm);

__xwos_code
void xwos_pmdm_dec_lpmxwsd_cnt(struct xwos_pmdm * pmdm);

/******** ******** ******** ******** ******** ******** ******** ********
 ********       internal inline function implementations        ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********       API function prototypes       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_api
void xwos_pmdm_init(struct xwos_pmdm * pmdm);

__xwos_api
void xwos_pmdm_set_lpmntf_cb(struct xwos_pmdm * pmdm, lpmntf_cb_f cb, void * arg);

__xwos_api
struct xwos_pmdm * xwos_pmdm_get_lc(void);

__xwos_api
xwer_t xwos_pmdm_suspend(struct xwos_pmdm * pmdm);

__xwos_api
xwer_t xwos_pmdm_resume(struct xwos_pmdm * pmdm);

__xwos_api
bool xwos_pmdm_tst_lpm(struct xwos_pmdm * pmdm);

/******** ******** ******** ******** ******** ******** ******** ********
 ********       internal inline function implementations        ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwos/smp/pm.h */
