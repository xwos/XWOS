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

/**
 * @brief 电源管理步骤枚举
 */
enum xwos_pmdm_stage_em {
        XWOS_PMDM_STAGE_SUSPENDED = 0, /**< 已经暂停 */
        XWOS_PMDM_STAGE_SUSPENDING, /**< 正在暂停 */
        XWOS_PMDM_STAGE_RESUMING = XWOS_PMDM_STAGE_SUSPENDING, /**< 正在恢复 */
        XWOS_PMDM_STAGE_FREEZING, /**< 正在冻结所有线程 */
        XWOS_PMDM_STAGE_THAWING = XWOS_PMDM_STAGE_FREEZING, /**< 正在解冻所有线程 */
        XWOS_PMDM_STAGE_RUNNING, /**< 电源管理领域内所有调度器正常运行 */
};

typedef void (* xwos_pmdm_cb_f)(struct xwos_pmdm *, void *); /**< 电源管理回调函数 */

/**
 * @brief 电源管理领域回调函数集合
 */
struct xwos_pmdm_callback {
        xwos_pmdm_cb_f resume; /**< 电源管理领域从暂停模式恢复的回调函数 */
        xwos_pmdm_cb_f suspend; /**< 电源管理领域进入暂停模式的回调函数 */
        xwos_pmdm_cb_f wakeup; /**< 唤醒电源管理领域的回调函数 */
        xwos_pmdm_cb_f sleep; /**< 电源管理领域休眠的回调函数 */
        void * arg; /**< 各回调函数的参数 */
};

/**
 * @brief 电源管理领域
 */
struct xwos_pmdm {
        /* 配置 */
        xwsz_t xwsd_num; /**< 调度器数量 */
        struct xwos_pmdm_callback cb; /**< 回调函数集合 */

        /* 私有成员 */
        __atomic xwsq_t stage; /**< 电源管理步骤，取值@ref xwos_pmdm_stage_em */
        __atomic xwsz_t suspended_xwsd_cnt; /**< 已进入暂停模式的调度器的计数器 */
        struct xwlk_splk rslock; /**< 防止resuming_cb与suspending_cb被同时调用的锁 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     internal function prototypes    ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_code
void xwos_pmdm_report_xwsd_suspended(struct xwos_pmdm * pmdm);

__xwos_code
void xwos_pmdm_report_xwsd_resuming(struct xwos_pmdm * pmdm);

/******** ******** ******** ******** ******** ******** ******** ********
 ********       internal inline function implementations        ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********       API function prototypes       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_api
void xwos_pmdm_init(struct xwos_pmdm * pmdm);

__xwos_api
void xwos_pmdm_set_cb(struct xwos_pmdm * pmdm,
                      xwos_pmdm_cb_f resume_cb,
                      xwos_pmdm_cb_f suspend_cb,
                      xwos_pmdm_cb_f wakeup_cb,
                      xwos_pmdm_cb_f sleep_cb,
                      void * arg);

__xwos_api
struct xwos_pmdm * xwos_pmdm_get_lc(void);

__xwos_api
xwer_t xwos_pmdm_suspend(struct xwos_pmdm * pmdm);

__xwos_api
xwer_t xwos_pmdm_resume(struct xwos_pmdm * pmdm);

__xwos_api
xwsq_t xwos_pmdm_get_stage(struct xwos_pmdm * pmdm);

/******** ******** ******** ******** ******** ******** ******** ********
 ********       internal inline function implementations        ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwos/smp/pm.h */
