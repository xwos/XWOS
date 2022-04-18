/**
 * @file
 * @brief 消息队列：模块接口
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_isc_xwmq_mif_h__
#define __xwmd_isc_xwmq_mif_h__

#include <xwos/standard.h>
#include <xwos/lib/object.h>
#include <xwos/lib/bclst.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/sem.h>
#include <xwos/osal/sync/sel.h>

/**
 * @defgroup xwmd_isc_xwmq 消息队列
 * @ingroup xwmd_isc
 * @{
 */

/**
 * @brief 消息队列对象
 */
struct xwmq {
        struct xwos_object xwobj; /**< C语言面向对象：继承struct xwos_object */
        const char * name; /**< 名称 */
        struct xwlib_bclst_head head; /**< 链表头 */
        struct xwos_splk lock; /**< 保护接收队列的自旋锁 */
        struct xwos_sem sem; /**< 接收队列的信号量 */
};

/**
 * @brief XWMQ消息对象
 */
struct xwmq_msg {
        struct xwos_object xwobj; /**< C语言面向对象：继承struct xwos_object */
        struct xwlib_bclst_node node; /**< 链表节点 */
        struct xwmq * mq; /**< 队列指针 */
        int topic; /**< 消息的标题 */
        void * data; /**< 消息数据 */
};

xwer_t xwmq_cache_init(xwptr_t zone_origin, xwsz_t zone_size);
xwer_t xwmq_init(struct xwmq * mq, const char * name);
xwer_t xwmq_fini(struct xwmq * mq);
xwer_t xwmq_create(struct xwmq ** ptrbuf, const char * name);
xwer_t xwmq_delete(struct xwmq * mq);
xwsq_t xwmq_gettik(struct xwmq * mq);
xwer_t xwmq_acquire(struct xwmq * mq, xwsq_t tik);
xwer_t xwmq_release(struct xwmq * mq, xwsq_t tik);
xwer_t xwmq_grab(struct xwmq * mq);
xwer_t xwmq_put(struct xwmq * mq);

xwer_t xwmq_msg_cache_init(xwptr_t zone_origin, xwsz_t zone_size);
xwer_t xwmq_msg_init(struct xwmq_msg * msg);
xwer_t xwmq_msg_fini(struct xwmq_msg * msg);
xwer_t xwmq_msg_create(struct xwmq_msg ** ptrbuf);
xwer_t xwmq_msg_delete(struct xwmq_msg * msg);
xwsq_t xwmq_msg_gettik(struct xwmq_msg * msg);
xwer_t xwmq_msg_acquire(struct xwmq_msg * msg, xwsq_t tik);
xwer_t xwmq_msg_release(struct xwmq_msg * msg, xwsq_t tik);
xwer_t xwmq_msg_grab(struct xwmq_msg * msg);
xwer_t xwmq_msg_put(struct xwmq_msg * msg);

xwer_t xwmq_eq(struct xwmq * mq, struct xwmq_msg * msg, int topic, void * data);
xwer_t xwmq_jq(struct xwmq * mq, struct xwmq_msg * msg, int topic, void * data);
xwer_t xwmq_dq(struct xwmq * mq, struct xwmq_msg ** ptrbuf);
xwer_t xwmq_dq_to(struct xwmq * mq, struct xwmq_msg ** ptrbuf, xwtm_t to);
xwer_t xwmq_trydq(struct xwmq * mq, struct xwmq_msg ** ptrbuf);

xwer_t xwmq_select(struct xwos_sel * sel,
                   struct xwmq * mqs[], xwsz_t * mqsz);
xwer_t xwmq_select_to(struct xwos_sel * sel,
                      struct xwmq * mqs[], xwsz_t * mqsz,
                      xwtm_t to);
xwer_t xwmq_tryselect(struct xwos_sel * sel,
                      struct xwmq * mqs[], xwsz_t * mqsz);

/**
 * @} xwmd_isc_xwmq
 */

#endif /* xwmd/isc/xwmq/mif.h */
