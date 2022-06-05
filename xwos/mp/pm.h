/**
 * @file
 * @brief 玄武OS MP内核：电源管理
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_mp_pm_h__
#define __xwos_mp_pm_h__

#include <xwos/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/mp/lock/spinlock.h>

struct xwmp_pmdm;

/**
 * @brief 电源管理阶段枚举
 */
enum xwmp_pm_stage_em {
        XWMP_PM_STAGE_SUSPENDED = 0, /**< 已经暂停 */
        XWMP_PM_STAGE_SUSPENDING, /**< 正在暂停 */
        XWMP_PM_STAGE_RESUMING = XWMP_PM_STAGE_SUSPENDING, /**< 正在恢复 */
        XWMP_PM_STAGE_RESERVED,
        XWMP_PM_STAGE_FREEZING, /**< 正在冻结线程 */
        XWMP_PM_STAGE_THAWING = XWMP_PM_STAGE_FREEZING, /**< 正在解冻线程 */
        XWMP_PM_STAGE_RUNNING, /**< 正常运行 */
};

typedef void (* xwmp_pmdm_cb_f)(void *); /**< 电源管理回调函数 */

/**
 * @brief 电源管理领域回调函数集合
 */
struct xwmp_pmdm_callback {
        xwmp_pmdm_cb_f resume; /**< 电源管理领域从暂停模式恢复的回调函数 */
        xwmp_pmdm_cb_f suspend; /**< 电源管理领域进入暂停模式的回调函数 */
        xwmp_pmdm_cb_f wakeup; /**< 唤醒电源管理领域的回调函数 */
        xwmp_pmdm_cb_f sleep; /**< 电源管理领域休眠的回调函数 */
        void * arg; /**< 各回调函数的参数 */
};

/**
 * @brief 电源管理领域
 */
struct xwmp_pmdm {
        /* 配置 */
        xwsz_t xwskd_num; /**< 调度器数量 */
        struct xwmp_pmdm_callback cb; /**< 回调函数集合 */

        /* 私有成员 */
        atomic_xwsq_t stage; /**< 电源管理阶段，取值@ref xwmp_pmdm_stage_em */
        atomic_xwsz_t suspended_xwskd_cnt; /**< 已暂停的调度器的计数器 */
        struct xwmp_splk rslock; /**< 防止cb.resume与cb.suspend被同时调用的锁 */
};

extern struct xwmp_pmdm xwmp_pmdm;

void xwmp_pmdm_init(void);
void xwmp_pmdm_set_cb(xwmp_pmdm_cb_f resume_cb,
                      xwmp_pmdm_cb_f suspend_cb,
                      xwmp_pmdm_cb_f wakeup_cb,
                      xwmp_pmdm_cb_f sleep_cb,
                      void * arg);
xwer_t xwmp_pmdm_suspend(void);
xwer_t xwmp_pmdm_resume(void);
xwsq_t xwmp_pmdm_get_stage(void);

#endif /* xwos/mp/pm.h */
