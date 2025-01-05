/**
 * @file
 * @brief XWOS MP内核同步机制：事件
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_mp_sync_evt_h__
#define __xwos_mp_sync_evt_h__

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#if defined(XWOSCFG_SYNC_EVT_MEMPOOL) && (1 == XWOSCFG_SYNC_EVT_MEMPOOL)
#  include <xwos/mm/mempool/allocator.h>
#elif defined(XWOSCFG_SYNC_EVT_MEMSLICE) && (1 == XWOSCFG_SYNC_EVT_MEMSLICE)
#  include <xwos/mm/memslice.h>
#elif defined(XWOSCFG_SYNC_EVT_SMA) && (1 == XWOSCFG_SYNC_EVT_SMA)
#  include <xwos/mm/sma.h>
#endif
#include <xwos/mp/lock/spinlock.h>
#include <xwos/mp/sync/obj.h>
#include <xwos/mp/sync/cond.h>

/**
 * @brief 事件类型枚举
 */
enum xwmp_evt_type_em {
        XWMP_EVT_TYPE_FLG = 0U, /**< 事件标志 */
        XWMP_EVT_TYPE_SEL = 1U, /**< 信号选择器 */
        XWMP_EVT_TYPE_BR = 2U, /**< 线程栅栏 */
        XWMP_EVT_TYPE_NUM,
        XWMP_EVT_TYPE_MASK = 0xFFU,
};

/**
 * @brief 事件对象
 */
struct xwmp_evt {
        struct xwmp_cond cond; /**< C语言面向对象：继承 `struct xwmp_cond` */
        xwsq_t type; /**< 类型 */
        xwsz_t num; /**< 事件的数量 */
        struct xwmp_splk lock; /**< 保护位图的锁 */
        xwbmp_t * bmp; /**< 事件位图 */
        xwbmp_t * msk; /**< 掩码位图 */
};

#if defined(XWOSCFG_SYNC_EVT_MEMPOOL) && (1 == XWOSCFG_SYNC_EVT_MEMPOOL)
xwer_t xwmp_evt_cache_init(struct xwmm_mempool * mp, xwsq_t page_order);
#elif defined(XWOSCFG_SYNC_EVT_MEMSLICE) && (1 == XWOSCFG_SYNC_EVT_MEMSLICE)
xwer_t xwmp_evt_cache_init(xwptr_t zone_origin, xwsz_t zone_size);
#elif defined(XWOSCFG_SYNC_EVT_SMA) && (1 == XWOSCFG_SYNC_EVT_SMA)
void xwmp_evt_cache_init(struct xwmm_sma * sma);
#endif

xwer_t xwmp_evt_intr_all(struct xwmp_evt * evt);

xwer_t xwmp_evt_init(struct xwmp_evt * evt, xwsq_t type, xwsz_t num,
                     xwbmp_t * bmp, xwbmp_t * msk);
xwer_t xwmp_evt_fini(struct xwmp_evt * evt);
xwer_t xwmp_evt_grab(struct xwmp_evt * evt);
xwer_t xwmp_evt_put(struct xwmp_evt * evt);

xwer_t xwmp_evt_create(struct xwmp_evt ** evtbuf, xwsq_t type, xwsz_t num);
xwer_t xwmp_evt_delete(struct xwmp_evt * evt, xwsq_t tik);
xwer_t xwmp_evt_acquire(struct xwmp_evt * evt, xwsq_t tik);
xwer_t xwmp_evt_release(struct xwmp_evt * evt, xwsq_t tik);

xwer_t xwmp_evt_bind(struct xwmp_evt * evt, struct xwmp_evt * sel, xwsq_t pos);
xwer_t xwmp_evt_unbind(struct xwmp_evt * evt, struct xwmp_evt * sel);
xwer_t xwmp_evt_get_num(struct xwmp_evt * evt, xwsz_t * numbuf);

/******** type:XWMP_EVT_TYPE_FLG ********/
/**
 * @brief 事件触发条件枚举
 */
enum xwmp_flg_trigger_em {
        XWMP_FLG_TRIGGER_SET_ALL = 0U, /**< 掩码中所有位被置1 */
        XWMP_FLG_TRIGGER_SET_ANY = 1U, /**< 掩码中任何位被置1 */
        XWMP_FLG_TRIGGER_CLR_ALL = 2U, /**< 掩码中所有位被清0 */
        XWMP_FLG_TRIGGER_CLR_ANY = 3U, /**< 掩码中任何位被清0 */
        XWMP_FLG_TRIGGER_TGL_ALL = 4U, /**< 掩码中任何位翻转 */
        XWMP_FLG_TRIGGER_TGL_ANY = 5U, /**< 掩码中任何位翻转 */
        XWMP_FLG_TRIGGER_NUM,
};

/**
 * @brief 事件触发后的动作枚举
 */
enum xwmp_flg_action_em {
        XWMP_FLG_ACTION_NONE = 0U, /**< 无动作 */
        XWMP_FLG_ACTION_CONSUMPTION = 1U, /**< 消费事件 */
        XWMP_FLG_ACTION_NUM,
};

xwer_t xwmp_flg_s1m(struct xwmp_evt * evt, xwbmp_t msk[]);
xwer_t xwmp_flg_s1i(struct xwmp_evt * evt, xwsq_t pos);
xwer_t xwmp_flg_c0m(struct xwmp_evt * evt, xwbmp_t msk[]);
xwer_t xwmp_flg_c0i(struct xwmp_evt * evt, xwsq_t pos);
xwer_t xwmp_flg_x1m(struct xwmp_evt * evt, xwbmp_t msk[]);
xwer_t xwmp_flg_x1i(struct xwmp_evt * evt, xwsq_t pos);
xwer_t xwmp_flg_read(struct xwmp_evt * evt, xwbmp_t out[]);
xwer_t xwmp_flg_wait(struct xwmp_evt * evt,
                     xwsq_t trigger, xwsq_t action,
                     xwbmp_t origin[], xwbmp_t msk[]);
xwer_t xwmp_flg_wait_to(struct xwmp_evt * evt,
                        xwsq_t trigger, xwsq_t action,
                        xwbmp_t origin[], xwbmp_t msk[],
                        xwtm_t to);
xwer_t xwmp_flg_trywait(struct xwmp_evt * evt,
                        xwsq_t trigger, xwsq_t action,
                        xwbmp_t origin[], xwbmp_t msk[]);
xwer_t xwmp_flg_wait_unintr(struct xwmp_evt * evt,
                            xwsq_t trigger, xwsq_t action,
                            xwbmp_t origin[], xwbmp_t msk[]);


/******** type:XWMP_EVT_TYPE_SEL ********/
xwer_t xwmp_sel_obj_bind(struct xwmp_evt * evt,
                         struct xwmp_synobj * synobj,
                         xwsq_t pos,
                         bool exclusive);
xwer_t xwmp_sel_obj_unbind(struct xwmp_evt * evt,
                           struct xwmp_synobj * synobj,
                           bool exclusive);
xwer_t xwmp_sel_obj_s1i(struct xwmp_evt * evt, struct xwmp_synobj * synobj);
xwer_t xwmp_sel_obj_c0i(struct xwmp_evt * evt, struct xwmp_synobj * synobj);

xwer_t xwmp_sel_select(struct xwmp_evt * evt, xwbmp_t msk[], xwbmp_t trg[]);
xwer_t xwmp_sel_select_to(struct xwmp_evt * evt,
                          xwbmp_t msk[], xwbmp_t trg[],
                          xwtm_t to);
xwer_t xwmp_sel_tryselect(struct xwmp_evt * evt, xwbmp_t msk[], xwbmp_t trg[]);
xwer_t xwmp_sel_select_unintr(struct xwmp_evt * evt, xwbmp_t msk[], xwbmp_t trg[]);

/******** type:XWMP_EVT_TYPE_BR ********/
xwer_t xwmp_br_wait(struct xwmp_evt * evt);
xwer_t xwmp_br_wait_to(struct xwmp_evt * evt, xwtm_t to);
xwer_t xwmp_br_wait_unintr(struct xwmp_evt * evt);

#endif /* xwos/mp/sync/evt.h */
