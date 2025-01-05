/**
 * @file
 * @brief XWOS UP内核同步机制：事件
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_up_sync_evt_h__
#define __xwos_up_sync_evt_h__

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#if defined(XWOSCFG_SYNC_EVT_MEMPOOL) && (1 == XWOSCFG_SYNC_EVT_MEMPOOL)
#  include <xwos/mm/mempool/allocator.h>
#elif defined(XWOSCFG_SYNC_EVT_MEMSLICE) && (1 == XWOSCFG_SYNC_EVT_MEMSLICE)
#  include <xwos/mm/memslice.h>
#elif defined(XWOSCFG_SYNC_EVT_SMA) && (1 == XWOSCFG_SYNC_EVT_SMA)
#  include <xwos/mm/sma.h>
#endif
#include <xwos/up/lock/fakespinlock.h>
#include <xwos/up/sync/obj.h>
#include <xwos/up/sync/cond.h>

/**
 * @brief 事件类型枚举
 */
enum xwup_evt_type_em {
        XWUP_EVT_TYPE_FLG = 0U, /**< 事件标志 */
        XWUP_EVT_TYPE_SEL = 1U, /**< 信号选择器 */
        XWUP_EVT_TYPE_BR = 2U, /**< 线程栅栏 */
        XWUP_EVT_TYPE_NUM,
        XWUP_EVT_TYPE_MASK = 0xFFU,
};

/**
 * @brief 事件对象
 */
struct xwup_evt {
        struct xwup_cond cond; /**< C语言面向对象：继承 `struct xwup_cond` */
        xwsq_t type; /**< 属性 */
        xwsz_t num; /**< 事件的数量 */
        struct xwup_splk lock; /**< 保护位图的锁 */
        xwbmp_t * bmp; /**< 事件位图 */
        xwbmp_t * msk; /**< 掩码位图 */
};

xwer_t xwup_evt_intr_all(struct xwup_evt * evt);

#if defined(XWOSCFG_SYNC_EVT_MEMPOOL) && (1 == XWOSCFG_SYNC_EVT_MEMPOOL)
xwer_t xwup_evt_cache_init(struct xwmm_mempool * mp, xwsq_t page_order);
#elif defined(XWOSCFG_SYNC_EVT_MEMSLICE) && (1 == XWOSCFG_SYNC_EVT_MEMSLICE)
xwer_t xwup_evt_cache_init(xwptr_t zone_origin, xwsz_t zone_size);
#elif defined(XWOSCFG_SYNC_EVT_SMA) && (1 == XWOSCFG_SYNC_EVT_SMA)
void xwup_evt_cache_init(struct xwmm_sma * sma);
#endif

xwer_t xwup_evt_init(struct xwup_evt * evt, xwsq_t type, xwsz_t num,
                     xwbmp_t * bmp, xwbmp_t * msk);
xwer_t xwup_evt_fini(struct xwup_evt * evt);
xwer_t xwup_evt_create(struct xwup_evt ** ptrbuf, xwsq_t type, xwsz_t num);
xwer_t xwup_evt_delete(struct xwup_evt * evt, xwsq_t tik);
xwer_t xwup_evt_acquire(struct xwup_evt * evt, xwsq_t tik);
xwer_t xwup_evt_release(struct xwup_evt * evt, xwsq_t tik);
xwer_t xwup_evt_grab(struct xwup_evt * evt);
xwer_t xwup_evt_put(struct xwup_evt * evt);

xwer_t xwup_evt_bind(struct xwup_evt * evt, struct xwup_evt * sel, xwsq_t pos);
xwer_t xwup_evt_unbind(struct xwup_evt * evt, struct xwup_evt * sel);
xwer_t xwup_evt_get_num(struct xwup_evt * evt, xwsz_t * numbuf);

/******** type:XWUP_EVT_TYPE_FLG ********/
/**
 * @brief 事件触发条件枚举
 */
enum xwup_flg_trigger_em {
        XWUP_FLG_TRIGGER_SET_ALL = 0U, /**< 掩码中所有位被置1 */
        XWUP_FLG_TRIGGER_SET_ANY = 1U, /**< 掩码中任何位被置1 */
        XWUP_FLG_TRIGGER_CLR_ALL = 2U, /**< 掩码中所有位被清0 */
        XWUP_FLG_TRIGGER_CLR_ANY = 3U, /**< 掩码中任何位被清0 */
        XWUP_FLG_TRIGGER_TGL_ALL = 4U, /**< 掩码中任何位翻转 */
        XWUP_FLG_TRIGGER_TGL_ANY = 5U, /**< 掩码中任何位翻转 */
        XWUP_FLG_TRIGGER_NUM,
};

/**
 * @brief 事件触发后的动作枚举
 */
enum xwup_flg_action_em {
        XWUP_FLG_ACTION_NONE = 0U, /**< 无动作 */
        XWUP_FLG_ACTION_CONSUMPTION = 1U, /**< 消费事件 */
        XWUP_FLG_ACTION_NUM,
};

/* public */
xwer_t xwup_flg_s1m(struct xwup_evt * evt, xwbmp_t msk[]);
xwer_t xwup_flg_s1i(struct xwup_evt * evt, xwsq_t pos);
xwer_t xwup_flg_c0m(struct xwup_evt * evt, xwbmp_t msk[]);
xwer_t xwup_flg_c0i(struct xwup_evt * evt, xwsq_t pos);
xwer_t xwup_flg_x1m(struct xwup_evt * evt, xwbmp_t msk[]);
xwer_t xwup_flg_x1i(struct xwup_evt * evt, xwsq_t pos);
xwer_t xwup_flg_read(struct xwup_evt * evt, xwbmp_t out[]);
xwer_t xwup_flg_wait(struct xwup_evt * evt,
                     xwsq_t trigger, xwsq_t action,
                     xwbmp_t origin[], xwbmp_t msk[]);
xwer_t xwup_flg_wait_to(struct xwup_evt * evt,
                        xwsq_t trigger, xwsq_t action,
                        xwbmp_t origin[], xwbmp_t msk[],
                        xwtm_t to);
xwer_t xwup_flg_trywait(struct xwup_evt * evt,
                        xwsq_t trigger, xwsq_t action,
                        xwbmp_t origin[], xwbmp_t msk[]);
xwer_t xwup_flg_wait_unintr(struct xwup_evt * evt,
                            xwsq_t trigger, xwsq_t action,
                            xwbmp_t origin[], xwbmp_t msk[]);

/******** type:XWUP_EVT_TYPE_SEL ********/
/* public(xwup) */
xwer_t xwup_sel_obj_bind(struct xwup_evt * evt,
                         struct xwup_synobj * synobj,
                         xwsq_t pos,
                         bool exclusive);
xwer_t xwup_sel_obj_unbind(struct xwup_evt * evt,
                           struct xwup_synobj * synobj,
                           bool exclusive);
xwer_t xwup_sel_obj_s1i(struct xwup_evt * evt, struct xwup_synobj * synobj);
xwer_t xwup_sel_obj_c0i(struct xwup_evt * evt, struct xwup_synobj * synobj);

/* public */
xwer_t xwup_sel_select(struct xwup_evt * evt, xwbmp_t msk[], xwbmp_t trg[]);
xwer_t xwup_sel_select_to(struct xwup_evt * evt, xwbmp_t msk[], xwbmp_t trg[],
                          xwtm_t to);
xwer_t xwup_sel_tryselect(struct xwup_evt * evt, xwbmp_t msk[], xwbmp_t trg[]);
xwer_t xwup_sel_select_unintr(struct xwup_evt * evt, xwbmp_t msk[], xwbmp_t trg[]);

/******** type:XWUP_EVT_TYPE_BR ********/
/* public */
xwer_t xwup_br_wait(struct xwup_evt * evt);
xwer_t xwup_br_wait_to(struct xwup_evt * evt, xwtm_t to);
xwer_t xwup_br_wait_unintr(struct xwup_evt * evt);

#endif /* xwos/up/sync/evt.h */
