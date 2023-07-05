/**
 * @file
 * @brief 消息队列
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/lib/object.h>
#include <xwos/mm/memslice.h>
#include <xwmd/isc/xwmq/mif.h>

static __xwmd_code
void xwmq_msg_construct(struct xwmq_msg * msg);

static __xwmd_code
void xwmq_construct(struct xwmq * mq)
{
        xwos_object_construct(&mq->xwobj);
}

static __xwmd_code
void xwmq_destruct(struct xwmq * mq)
{
        xwos_object_destruct(&mq->xwobj);
}

static __xwmd_code
xwer_t xwmq_gc(struct xwos_object * obj)
{
        struct xwmq * mq;

        mq = xwcc_derof(obj, struct xwmq, xwobj);
        xwos_sem_fini(&mq->txqsem);
        xwos_sem_fini(&mq->rxqsem);
        xwmq_destruct(mq);
        return XWOK;
}

__xwmd_api
xwsq_t xwmq_gettik(struct xwmq * mq)
{
        xwsq_t tik;

        if (NULL != mq) {
                tik = mq->xwobj.tik;
        } else {
                tik = 0;
        }
        return tik;
}

__xwmd_api
xwer_t xwmq_acquire(struct xwmq * mq, xwsq_t tik)
{
        return xwos_object_acquire(&mq->xwobj, tik);
}

__xwmd_api
xwer_t xwmq_release(struct xwmq * mq, xwsq_t tik)
{
        return xwos_object_release(&mq->xwobj, tik);
}

__xwmd_api
xwer_t xwmq_grab(struct xwmq * mq)
{
        XWOS_VALIDATE((mq), "nullptr", -EFAULT);

        return xwos_object_grab(&mq->xwobj);
}

__xwmd_api
xwer_t xwmq_put(struct xwmq * mq)
{
        XWOS_VALIDATE((mq), "nullptr", -EFAULT);

        return xwos_object_put(&mq->xwobj);
}

__xwmd_rodata
const __xwmd_rodata char xwmq_txq_name[] = "xwmq.txq";

static __xwmd_code
xwer_t xwmq_activate(struct xwmq * mq,
                     struct xwmq_msg * txq, xwsz_t num,
                     xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwos_object_activate(&mq->xwobj, gcfunc);
        if (rc < 0) {
                goto err_xwobj_activate;
        }
        xwmm_memslice_init(&mq->txq,
                           (xwptr_t)txq, sizeof(struct xwmq_msg) * num,
                           sizeof(struct xwmq_msg), xwmq_txq_name,
                           (ctor_f)xwmq_msg_construct, NULL);
        xwos_sem_init(&mq->txqsem, (xwssq_t)num, (xwssq_t)num);
        xwlib_bclst_init_head(&mq->rxq);
        xwos_splk_init(&mq->rxqlock);
        xwos_sem_init(&mq->rxqsem, 0, (xwssq_t)num);
        return XWOK;

err_xwobj_activate:
        return rc;
}

__xwmd_api
xwer_t xwmq_init(struct xwmq * mq, struct xwmq_msg * txq, xwsz_t num)
{
        XWOS_VALIDATE((mq), "nullptr", -EFAULT);
        XWOS_VALIDATE((txq), "nullptr", -EFAULT);

        xwmq_construct(mq);
        return xwmq_activate(mq, txq, num, xwmq_gc);
}

__xwmd_api
xwer_t xwmq_fini(struct xwmq * mq)
{
        XWOS_VALIDATE((mq), "nullptr", -EFAULT);
        return xwmq_put(mq);
}

/**
 * @brief 消息槽的构造函数
 * @param[in] msg: 消息槽的指针
 */
static __xwmd_code
void xwmq_msg_construct(struct xwmq_msg * msg)
{
        xwlib_bclst_init_node(&msg->node);
        msg->topic = 0;
        msg->data = NULL;
}

/**
 * @brief 获取一个消息槽
 */
static __xwmd_code
struct xwmq_msg * xwmq_msg_get(struct xwmq * mq)
{
        union {
                struct xwmq_msg * msg;
                void * anon;
        } mem;

        xwmm_memslice_alloc(&mq->txq, &mem.anon);
        return mem.msg;
}

/**
 * @brief 释放一个消息槽
 */
static __xwmd_code
void xwmq_msg_put(struct xwmq * mq, struct xwmq_msg * msg)
{
        xwmm_memslice_free(&mq->txq, msg);
}

__xwmd_api
xwer_t xwmq_eq(struct xwmq * mq, xwsq_t topic, void * data)
{
        return xwmq_eq_to(mq, topic, data, XWTM_MAX);
}

__xwmd_api
xwer_t xwmq_eq_to(struct xwmq * mq, xwsq_t topic, void * data, xwtm_t to)
{
        xwer_t rc;
        xwreg_t cpuirq;
        struct xwmq_msg * msg;

        XWOS_VALIDATE((mq), "nullptr", -EFAULT);
        XWOS_VALIDATE((data), "nullptr", -EFAULT);

        rc = xwos_sem_wait_to(&mq->txqsem, to);
        if (rc < 0) {
                goto err_sem_wait_to;
        }
        msg = xwmq_msg_get(mq);
        msg->topic = topic;
        msg->data = data;
        xwos_splk_lock_cpuirqsv(&mq->rxqlock, &cpuirq);
        xwlib_bclst_add_tail(&mq->rxq, &msg->node);
        xwos_splk_unlock_cpuirqrs(&mq->rxqlock, cpuirq);
        xwos_sem_post(&mq->rxqsem);
        return XWOK;

err_sem_wait_to:
        return rc;
}

__xwmd_api
xwer_t xwmq_tryeq(struct xwmq * mq, xwsq_t topic, void * data)
{
        xwer_t rc;
        xwreg_t cpuirq;
        struct xwmq_msg * msg;

        XWOS_VALIDATE((mq), "nullptr", -EFAULT);
        XWOS_VALIDATE((data), "nullptr", -EFAULT);

        rc = xwos_sem_trywait(&mq->txqsem);
        if (rc < 0) {
                goto err_sem_trywait;
        }
        msg = xwmq_msg_get(mq);
        msg->topic = topic;
        msg->data = data;
        xwos_splk_lock_cpuirqsv(&mq->rxqlock, &cpuirq);
        xwlib_bclst_add_tail(&mq->rxq, &msg->node);
        xwos_splk_unlock_cpuirqrs(&mq->rxqlock, cpuirq);
        xwos_sem_post(&mq->rxqsem);
        return XWOK;

err_sem_trywait:
        return rc;
}

__xwmd_api
xwer_t xwmq_jq(struct xwmq * mq, xwsq_t topic, void * data)
{
        return xwmq_jq_to(mq, topic, data, XWTM_MAX);
}

__xwmd_api
xwer_t xwmq_jq_to(struct xwmq * mq, xwsq_t topic, void * data, xwtm_t to)
{
        xwer_t rc;
        xwreg_t cpuirq;
        struct xwmq_msg * msg;

        XWOS_VALIDATE((mq), "nullptr", -EFAULT);
        XWOS_VALIDATE((data), "nullptr", -EFAULT);

        rc = xwos_sem_wait_to(&mq->txqsem, to);
        if (rc < 0) {
                goto err_sem_wait_to;
        }
        msg = xwmq_msg_get(mq);
        msg->topic = topic;
        msg->data = data;
        xwos_splk_lock_cpuirqsv(&mq->rxqlock, &cpuirq);
        xwlib_bclst_add_head(&mq->rxq, &msg->node);
        xwos_splk_unlock_cpuirqrs(&mq->rxqlock, cpuirq);
        xwos_sem_post(&mq->rxqsem);
        return XWOK;

err_sem_wait_to:
        return rc;
}

__xwmd_api
xwer_t xwmq_tryjq(struct xwmq * mq, xwsq_t topic, void * data)
{
        xwer_t rc;
        xwreg_t cpuirq;
        struct xwmq_msg * msg;

        XWOS_VALIDATE((mq), "nullptr", -EFAULT);
        XWOS_VALIDATE((data), "nullptr", -EFAULT);

        rc = xwos_sem_trywait(&mq->txqsem);
        if (rc < 0) {
                goto err_sem_trywait;
        }
        msg = xwmq_msg_get(mq);
        msg->topic = topic;
        msg->data = data;
        xwos_splk_lock_cpuirqsv(&mq->rxqlock, &cpuirq);
        xwlib_bclst_add_head(&mq->rxq, &msg->node);
        xwos_splk_unlock_cpuirqrs(&mq->rxqlock, cpuirq);
        xwos_sem_post(&mq->rxqsem);
        return XWOK;

err_sem_trywait:
        return rc;
}

/**
 * @brief 从消息接收队列头部选择一条消息
 * @param[in] mq: 消息队列对象的指针
 */
static __xwmd_code
struct xwmq_msg * xwmq_choose_head(struct xwmq * mq)
{
        struct xwmq_msg * msg;
        xwreg_t cpuirq;

        xwos_splk_lock_cpuirqsv(&mq->rxqlock, &cpuirq);
        msg = xwlib_bclst_first_entry(&mq->rxq,
                                      struct xwmq_msg,
                                      node);
        xwlib_bclst_del_init(&msg->node);
        xwos_splk_unlock_cpuirqrs(&mq->rxqlock, cpuirq);
        return msg;
}

/**
 * @brief 从消息接收队列尾部选择一条消息
 * @param[in] mq: 消息队列对象的指针
 */
static __xwmd_code
struct xwmq_msg * xwmq_choose_tail(struct xwmq * mq)
{
        struct xwmq_msg * msg;
        xwreg_t cpuirq;

        xwos_splk_lock_cpuirqsv(&mq->rxqlock, &cpuirq);
        msg = xwlib_bclst_last_entry(&mq->rxq,
                                     struct xwmq_msg,
                                     node);
        xwlib_bclst_del_init(&msg->node);
        xwos_splk_unlock_cpuirqrs(&mq->rxqlock, cpuirq);
        return msg;
}

__xwmd_api
xwer_t xwmq_dq(struct xwmq * mq, xwsq_t * topic, void ** databuf)
{
        return xwmq_dq_to(mq, topic, databuf, XWTM_MAX);
}

__xwmd_api
xwer_t xwmq_dq_to(struct xwmq * mq, xwsq_t * topic, void ** databuf, xwtm_t to)
{
        xwer_t rc;
        struct xwmq_msg * msg;

        XWOS_VALIDATE((mq), "nullptr", -EFAULT);
        XWOS_VALIDATE((databuf), "nullptr", -EFAULT);

        rc = xwos_sem_wait_to(&mq->rxqsem, to);
        if (rc < 0) {
                goto err_sem_wait_to;
        }
        msg = xwmq_choose_head(mq);
        *databuf = msg->data;
        if (NULL != topic) {
                *topic = msg->topic;
        }
        xwmq_msg_put(mq, msg);
        xwos_sem_post(&mq->txqsem);
        return XWOK;

err_sem_wait_to:
        return rc;
}

__xwmd_api
xwer_t xwmq_trydq(struct xwmq * mq, xwsq_t * topic, void ** databuf)
{
        xwer_t rc;
        struct xwmq_msg * msg;

        XWOS_VALIDATE((mq), "nullptr", -EFAULT);
        XWOS_VALIDATE((databuf), "nullptr", -EFAULT);

        rc = xwos_sem_trywait(&mq->rxqsem);
        if (rc < 0) {
                goto err_sem_trywait;
        }
        msg = xwmq_choose_head(mq);
        *databuf = msg->data;
        if (NULL != topic) {
                *topic = msg->topic;
        }
        xwmq_msg_put(mq, msg);
        xwos_sem_post(&mq->txqsem);
        return XWOK;

err_sem_trywait:
        return rc;
}

__xwmd_api
xwer_t xwmq_rq(struct xwmq * mq, xwsq_t * topic, void ** databuf)
{
        return xwmq_rq_to(mq, topic, databuf, XWTM_MAX);
}

__xwmd_api
xwer_t xwmq_rq_to(struct xwmq * mq, xwsq_t * topic, void ** databuf, xwtm_t to)
{
        xwer_t rc;
        struct xwmq_msg * msg;

        XWOS_VALIDATE((mq), "nullptr", -EFAULT);
        XWOS_VALIDATE((databuf), "nullptr", -EFAULT);

        rc = xwos_sem_wait_to(&mq->rxqsem, to);
        if (rc < 0) {
                goto err_sem_wait_to;
        }
        msg = xwmq_choose_tail(mq);
        *databuf = msg->data;
        if (NULL != topic) {
                *topic = msg->topic;
        }
        xwmq_msg_put(mq, msg);
        xwos_sem_post(&mq->txqsem);
        return XWOK;

err_sem_wait_to:
        return rc;
}

__xwmd_api
xwer_t xwmq_tryrq(struct xwmq * mq, xwsq_t * topic, void ** databuf)
{
        xwer_t rc;
        struct xwmq_msg * msg;

        XWOS_VALIDATE((mq), "nullptr", -EFAULT);
        XWOS_VALIDATE((databuf), "nullptr", -EFAULT);

        rc = xwos_sem_trywait(&mq->rxqsem);
        if (rc < 0) {
                goto err_sem_trywait;
        }
        msg = xwmq_choose_tail(mq);
        *databuf = msg->data;
        if (NULL != topic) {
                *topic = msg->topic;
        }
        xwmq_msg_put(mq, msg);
        xwos_sem_post(&mq->txqsem);
        return XWOK;

err_sem_trywait:
        return rc;
}
