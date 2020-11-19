/**
 * @file
 * @brief 玄武OS MP同步机制：事件
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_mp_sync_evt_h__
#define __xwos_mp_sync_evt_h__

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/mp/lock/spinlock.h>
#include <xwos/mp/sync/obj.h>
#include <xwos/mp/sync/cond.h>

#define XWMP_EVT_MAXNUM (XWMPCFG_SYNC_EVT_MAXNUM) /**< 事件最大数量 */

/**
 * @brief 声明事件位图
 */
#define xwmp_evt_declare_bitmap(name) xwbmpop_declare(name, XWMP_EVT_MAXNUM)

/**
 * @brief 事件类型枚举
 */
enum xwmp_evt_type_em {
        XWMP_EVT_TYPE_FLG = 0, /**< 事件标志 */
        XWMP_EVT_TYPE_SEL, /**< 信号选择器 */
        XWMP_EVT_TYPE_BR, /**< 线程栅栏 */
        XWMP_EVT_TYPE_MAX,
        XWMP_EVT_TYPE_MASK = 0xFF,
};

/**
 * @brief 事件触发条件枚举
 */
enum xwmp_evt_trigger_em {
        XWMP_EVT_TRIGGER_SET_ALL = 0, /**< 掩码中所有位被置1 */
        XWMP_EVT_TRIGGER_SET_ANY, /**< 掩码中任何位被置1 */
        XWMP_EVT_TRIGGER_CLR_ALL, /**< 掩码中所有位被清0 */
        XWMP_EVT_TRIGGER_CLR_ANY, /**< 掩码中任何位被清0 */
        XWMP_EVT_TRIGGER_TGL_ALL, /**< 掩码中任何位翻转 */
        XWMP_EVT_TRIGGER_TGL_ANY, /**< 掩码中任何位翻转 */
        XWMP_EVT_TRIGGER_NUM,
};

/**
 * @brief 事件触发后的动作枚举
 */
enum xwmp_evt_action_em {
        XWMP_EVT_ACTION_NONE = 0, /**< 无动作 */
        XWMP_EVT_ACTION_CONSUMPTION, /**< 消费事件 */
        XWMP_EVT_ACTION_NUM,
};

/**
 * @brief 事件对象
 */
struct xwmp_evt {
        struct xwmp_cond cond; /**< C语言面向对象：继承struct xwmp_cond */
        xwsq_t attr; /**< 属性 */
        xwmp_evt_declare_bitmap(bmp); /**< 事件位图 */
        xwmp_evt_declare_bitmap(msk); /**< 掩码位图 */
        struct xwmp_splk lock; /**< 保护位图的锁 */
};

xwer_t xwmp_evt_grab(struct xwmp_evt * evt);
xwer_t xwmp_evt_put(struct xwmp_evt * evt);
xwer_t xwmp_evt_obj_bind(struct xwmp_evt * evt,
                         struct xwmp_synobj * synobj,
                         xwsq_t pos,
                         bool exclusive);
xwer_t xwmp_evt_obj_unbind(struct xwmp_evt * evt,
                           struct xwmp_synobj * synobj,
                           bool exclusive);
xwer_t xwmp_evt_obj_s1i(struct xwmp_evt * evt, struct xwmp_synobj * synobj);
xwer_t xwmp_evt_obj_c0i(struct xwmp_evt * evt, struct xwmp_synobj * synobj);

#if defined(XWMPCFG_SYNC_EVT_MEMSLICE) && (1 == XWMPCFG_SYNC_EVT_MEMSLICE)
xwer_t xwmp_evt_cache_init(xwptr_t zone_origin, xwsz_t zone_size);
#endif /* XWMPCFG_SYNC_EVT_MEMSLICE */

xwer_t xwmp_evt_init(struct xwmp_evt * evt, xwbmp_t initval[], xwsq_t attr);
xwer_t xwmp_evt_destroy(struct xwmp_evt * evt);
xwer_t xwmp_evt_create(struct xwmp_evt ** ptrbuf, xwbmp_t initval[], xwsq_t attr);
xwer_t xwmp_evt_delete(struct xwmp_evt * evt);
xwer_t xwmp_evt_bind(struct xwmp_evt * evt, struct xwmp_evt * slt, xwsq_t pos);
xwer_t xwmp_evt_unbind(struct xwmp_evt * evt, struct xwmp_evt * slt);
xwer_t xwmp_evt_intr_all(struct xwmp_evt * evt);
xwer_t xwmp_evt_s1m(struct xwmp_evt * evt, xwbmp_t msk[]);
xwer_t xwmp_evt_s1i(struct xwmp_evt * evt, xwsq_t pos);
xwer_t xwmp_evt_c0m(struct xwmp_evt * evt, xwbmp_t msk[]);
xwer_t xwmp_evt_c0i(struct xwmp_evt * evt, xwsq_t pos);
xwer_t xwmp_evt_x1m(struct xwmp_evt * evt, xwbmp_t msk[]);
xwer_t xwmp_evt_x1i(struct xwmp_evt * evt, xwsq_t pos);
xwer_t xwmp_evt_read(struct xwmp_evt * evt, xwbmp_t out[]);
xwer_t xwmp_evt_wait(struct xwmp_evt * evt,
                     xwsq_t trigger, xwsq_t action,
                     xwbmp_t origin[], xwbmp_t msk[]);
xwer_t xwmp_evt_trywait(struct xwmp_evt * evt,
                        xwsq_t trigger, xwsq_t action,
                        xwbmp_t origin[], xwbmp_t msk[]);
xwer_t xwmp_evt_timedwait(struct xwmp_evt * evt,
                          xwsq_t trigger, xwsq_t action,
                          xwbmp_t origin[], xwbmp_t msk[],
                          xwtm_t * xwtm);
xwer_t xwmp_evt_select(struct xwmp_evt * evt, xwbmp_t msk[], xwbmp_t trg[]);
xwer_t xwmp_evt_tryselect(struct xwmp_evt * evt, xwbmp_t msk[], xwbmp_t trg[]);
xwer_t xwmp_evt_timedselect(struct xwmp_evt * evt, xwbmp_t msk[], xwbmp_t trg[],
                            xwtm_t * xwtm);
xwer_t xwmp_evt_sync(struct xwmp_evt * evt, xwsq_t pos, xwbmp_t sync[]);
xwer_t xwmp_evt_timedsync(struct xwmp_evt * evt, xwsq_t pos, xwbmp_t sync[],
                          xwtm_t * xwtm);

#endif /* xwos/mp/sync/evt.h */
