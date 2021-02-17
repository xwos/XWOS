/**
 * @file
 * @brief 消息队列：应用接口层
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_isc_xwmq_mif_h__
#define __xwmd_isc_xwmq_mif_h__

#include <xwos/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/sem.h>
#include <xwos/osal/sync/sel.h>

/**
 * @breif 消息队列对象
 */
struct xwmq {
        struct xwos_object xwobj; /**< C语言面向对象：继承struct xwos_object */
        const char * name;
        struct xwlib_bclst_head head; /**< 链表头 */
        struct xwos_splk lock; /**< 保护接收队列的自旋锁 */
        struct xwos_sem sem; /**< 接收队列的信号量 */
};

/**
 * @breif XWMQ消息对象
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
xwer_t xwmq_destroy(struct xwmq * mq);
xwer_t xwmq_create(struct xwmq ** ptrbuf, const char * name);
xwer_t xwmq_delete(struct xwmq * mq);
xwsq_t xwmq_gettik(struct xwmq * mq);
xwer_t xwmq_acquire(struct xwmq * mq, xwsq_t tik);
xwer_t xwmq_release(struct xwmq * mq, xwsq_t tik);
xwer_t xwmq_grab(struct xwmq * mq);
xwer_t xwmq_put(struct xwmq * mq);

xwer_t xwmq_msg_cache_init(xwptr_t zone_origin, xwsz_t zone_size);
xwer_t xwmq_msg_init(struct xwmq_msg * msg);
xwer_t xwmq_msg_destroy(struct xwmq_msg * msg);
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
xwer_t xwmq_timedq(struct xwmq * mq, struct xwmq_msg ** ptrbuf, xwtm_t * xwtm);
xwer_t xwmq_trydq(struct xwmq * mq, struct xwmq_msg ** ptrbuf);

xwer_t xwmq_timedselect(struct xwos_sel * sel,
                        struct xwmq * mqs[], xwsz_t * mqsz,
                        xwtm_t * xwtm);
xwer_t xwmq_select(struct xwos_sel * sel,
                   struct xwmq * mqs[], xwsz_t * mqsz);
xwer_t xwmq_tryselect(struct xwos_sel * sel,
                      struct xwmq * mqs[], xwsz_t * mqsz);

#endif /* xwmd/isc/xwmq/mif.h */
