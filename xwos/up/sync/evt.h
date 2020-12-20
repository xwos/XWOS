/**
 * @file
 * @brief 玄武OS UP内核同步机制：事件
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_sync_evt_h__
#define __xwos_up_sync_evt_h__

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/up/lock/fakespinlock.h>
#include <xwos/up/sync/obj.h>
#include <xwos/up/sync/cond.h>

#define XWUP_EVT_MAXNUM (XWUPCFG_SYNC_EVT_MAXNUM) /**< 事件最大数量 */

/**
 * @brief 声明事件位图
 */
#define xwup_evt_declare_bitmap(name) xwbmpop_declare(name, XWUP_EVT_MAXNUM)

/**
 * @brief 事件类型枚举
 */
enum xwup_evt_type_em {
        XWUP_EVT_TYPE_FLG = 0, /**< 事件标志 */
        XWUP_EVT_TYPE_SEL, /**< 信号选择器 */
        XWUP_EVT_TYPE_BR, /**< 线程栅栏 */
        XWUP_EVT_TYPE_MAX,
        XWUP_EVT_TYPE_MASK = 0xFF,
};

/**
 * @brief 事件触发条件枚举
 */
enum xwup_evt_trigger_em {
        XWUP_EVT_TRIGGER_SET_ALL = 0, /**< 掩码中所有位被置1 */
        XWUP_EVT_TRIGGER_SET_ANY, /**< 掩码中任何位被置1 */
        XWUP_EVT_TRIGGER_CLR_ALL, /**< 掩码中所有位被清0 */
        XWUP_EVT_TRIGGER_CLR_ANY, /**< 掩码中任何位被清0 */
        XWUP_EVT_TRIGGER_TGL_ALL, /**< 掩码中任何位翻转 */
        XWUP_EVT_TRIGGER_TGL_ANY, /**< 掩码中任何位翻转 */
        XWUP_EVT_TRIGGER_NUM,
};

/**
 * @brief 事件触发后的动作枚举
 */
enum xwup_evt_action_em {
        XWUP_EVT_ACTION_NONE = 0, /**< 无动作 */
        XWUP_EVT_ACTION_CONSUMPTION, /**< 消费事件 */
        XWUP_EVT_ACTION_NUM,
};

/**
 * @brief 事件对象
 */
struct xwup_evt {
        struct xwup_cond cond; /**< C语言面向对象：继承struct xwup_cond */
        xwsq_t attr; /**< 属性 */
        xwup_evt_declare_bitmap(bmp); /**< 事件位图 */
        xwup_evt_declare_bitmap(msk); /**< 掩码位图 */
        struct xwup_splk lock; /**< 保护位图的锁 */
};

xwer_t xwup_evt_obj_bind(struct xwup_evt * evt,
                         struct xwup_synobj * synobj,
                         xwsq_t pos,
                         bool exclusive);
xwer_t xwup_evt_obj_unbind(struct xwup_evt * evt,
                           struct xwup_synobj * synobj,
                           bool exclusive);
xwer_t xwup_evt_obj_s1i(struct xwup_evt * evt, struct xwup_synobj * synobj);
xwer_t xwup_evt_obj_c0i(struct xwup_evt * evt, struct xwup_synobj * synobj);

xwer_t xwup_evt_init(struct xwup_evt * evt, xwbmp_t initval[], xwsq_t attr);
xwer_t xwup_evt_destroy(struct xwup_evt * evt);
xwer_t xwup_evt_create(struct xwup_evt ** ptrbuf, xwbmp_t initval[], xwsq_t attr);
xwer_t xwup_evt_delete(struct xwup_evt * evt);
xwer_t xwup_evt_bind(struct xwup_evt * evt, struct xwup_evt * slt, xwsq_t pos);
xwer_t xwup_evt_unbind(struct xwup_evt * evt, struct xwup_evt * slt);
xwer_t xwup_evt_intr_all(struct xwup_evt * evt);
xwer_t xwup_evt_s1m(struct xwup_evt * evt, xwbmp_t msk[]);
xwer_t xwup_evt_s1i(struct xwup_evt * evt, xwsq_t pos);
xwer_t xwup_evt_c0m(struct xwup_evt * evt, xwbmp_t msk[]);
xwer_t xwup_evt_c0i(struct xwup_evt * evt, xwsq_t pos);
xwer_t xwup_evt_x1m(struct xwup_evt * evt, xwbmp_t msk[]);
xwer_t xwup_evt_x1i(struct xwup_evt * evt, xwsq_t pos);
xwer_t xwup_evt_read(struct xwup_evt * evt, xwbmp_t out[]);
xwer_t xwup_evt_wait(struct xwup_evt * evt,
                     xwsq_t trigger, xwsq_t action,
                     xwbmp_t origin[], xwbmp_t msk[]);
xwer_t xwup_evt_trywait(struct xwup_evt * evt,
                        xwsq_t trigger, xwsq_t action,
                        xwbmp_t origin[], xwbmp_t msk[]);
xwer_t xwup_evt_timedwait(struct xwup_evt * evt,
                          xwsq_t trigger, xwsq_t action,
                          xwbmp_t origin[], xwbmp_t msk[],
                          xwtm_t * xwtm);
xwer_t xwup_evt_select(struct xwup_evt * evt, xwbmp_t msk[], xwbmp_t trg[]);
xwer_t xwup_evt_tryselect(struct xwup_evt * evt, xwbmp_t msk[], xwbmp_t trg[]);
xwer_t xwup_evt_timedselect(struct xwup_evt * evt, xwbmp_t msk[], xwbmp_t trg[],
                            xwtm_t * xwtm);
xwer_t xwup_evt_sync(struct xwup_evt * evt, xwsq_t pos, xwbmp_t msk[], xwbmp_t sync[]);
xwer_t xwup_evt_timedsync(struct xwup_evt * evt, xwsq_t pos,
                          xwbmp_t msk[], xwbmp_t sync[],
                          xwtm_t * xwtm);
#endif /* xwos/up/sync/evt.h */
