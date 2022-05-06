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
#include <xwos/mm/memslice.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/sem.h>
#include <xwos/osal/sync/sel.h>

/**
 * @defgroup xwmd_isc_xwmq 消息队列
 * @ingroup xwmd_isc
 * @{
 */

/**
 * @brief XWMQ消息
 */
struct xwmq_msg {
        void * data; /**< 消息数据 */
        xwsq_t topic; /**< 消息的主题 */
        struct xwlib_bclst_node node; /**< 链表节点，因memslice实现的原因，不可作为第一个成员 */
};

/**
 * @brief 消息队列对象
 */
struct xwmq {
        struct xwos_object xwobj; /**< C语言面向对象：继承struct xwos_object */
        const char * name; /**< 名称 */
        struct xwmm_memslice txq; /**< 消息槽队列 */
        struct xwos_sem txqsem; /**< 消息槽的信号量 */
        struct xwlib_bclst_head rxq; /**< 接收队列 */
        struct xwos_splk rxqlock; /**< 保护接收队列的自旋锁 */
        struct xwos_sem rxqsem; /**< 接收队列的信号量 */
};

xwer_t xwmq_init(struct xwmq * mq, const char * name,
                 struct xwmq_msg * txq, xwsz_t num);
xwer_t xwmq_fini(struct xwmq * mq);
xwsq_t xwmq_gettik(struct xwmq * mq);
xwer_t xwmq_acquire(struct xwmq * mq, xwsq_t tik);
xwer_t xwmq_release(struct xwmq * mq, xwsq_t tik);
xwer_t xwmq_grab(struct xwmq * mq);
xwer_t xwmq_put(struct xwmq * mq);

xwer_t xwmq_eq(struct xwmq * mq, xwsq_t topic, void * data);
xwer_t xwmq_eq_to(struct xwmq * mq, xwsq_t topic, void * data, xwtm_t to);
xwer_t xwmq_tryeq(struct xwmq * mq, xwsq_t topic, void * data);
xwer_t xwmq_jq(struct xwmq * mq, xwsq_t topic, void * data);
xwer_t xwmq_jq_to(struct xwmq * mq, xwsq_t topic, void * data, xwtm_t to);
xwer_t xwmq_tryjq(struct xwmq * mq, xwsq_t topic, void * data);
xwer_t xwmq_dq(struct xwmq * mq, xwsq_t * topic, void ** databuf);
xwer_t xwmq_dq_to(struct xwmq * mq, xwsq_t * topic, void ** databuf, xwtm_t to);
xwer_t xwmq_trydq(struct xwmq * mq, xwsq_t * topic, void ** databuf);

/**
 * @} xwmd_isc_xwmq
 */

#endif /* xwmd/isc/xwmq/mif.h */
