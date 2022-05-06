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
void xwmq_msg_construct(void * obj);

static __xwmd_code
void xwmq_construct(struct xwmq * mq)
{
        xwos_object_construct(&mq->xwobj);
        mq->name = NULL;
}

static __xwmd_code
void xwmq_destruct(struct xwmq * mq)
{
        xwos_object_destruct(&mq->xwobj);
}

static __xwmd_code
xwer_t xwmq_gc(void * obj)
{
        struct xwmq * mq = obj;

        xwos_sem_fini(&mq->txqsem);
        xwos_sem_fini(&mq->rxqsem);
        xwmq_destruct(mq);
        return XWOK;
}

/**
 * @brief XWMQ API：获取消息队列对象的标签
 * @param[in] mq: 消息队列对象的指针
 * @return 消息队列对象的标签
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmd_api
xwsq_t xwmq_gettik(struct xwmq * mq)
{
        xwsq_t tik;

        if (mq) {
                tik = mq->xwobj.tik;
        } else {
                tik = 0;
        }
        return tik;
}

/**
 * @brief XWMQ API：检查消息队列对象的标签并增加引用计数
 * @param[in] mq: 消息队列对象指针
 * @param[in] tik: 标签
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwmq_acquire(struct xwmq * mq, xwsq_t tik)
{
        return xwos_object_acquire(&mq->xwobj, tik);
}

/**
 * @brief XWMQ API：检查消息队列对象的标签并增加引用计数
 * @param[in] mq: 消息队列对象指针
 * @param[in] tik: 标签
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwmq_release(struct xwmq * mq, xwsq_t tik)
{
        return xwos_object_release(&mq->xwobj, tik);
}

/**
 * @brief XWMQ API：增加消息队列对象的引用计数
 * @param[in] mq: 消息队列控制块对象指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwmq_grab(struct xwmq * mq)
{
        XWOS_VALIDATE((mq), "nullptr", -EFAULT);
        return xwos_object_grab(&mq->xwobj);
}

/**
 * @brief XWMQ API：减少消息队列对象的引用计数
 * @param[in] mq: 消息队列控制块对象指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwmq_put(struct xwmq * mq)
{
        XWOS_VALIDATE((mq), "nullptr", -EFAULT);
        return xwos_object_put(&mq->xwobj);
}

const __xwmd_rodata char xwmq_txq_name[] = "xwmq.txq";

static __xwmd_code
xwer_t xwmq_activate(struct xwmq * mq, const char * name,
                     struct xwmq_msg * txq, xwsz_t num,
                     xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwos_object_activate(&mq->xwobj, gcfunc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_xwobj_activate;
        }
        mq->name = name;
        xwmm_memslice_init(&mq->txq,
                           (xwptr_t)txq, sizeof(struct xwmq_msg) * num,
                           sizeof(struct xwmq_msg), xwmq_txq_name,
                           xwmq_msg_construct, NULL);
        xwos_sem_init(&mq->txqsem, (xwssq_t)num, (xwssq_t)num);
        xwlib_bclst_init_head(&mq->rxq);
        xwos_splk_init(&mq->rxqlock);
        xwos_sem_init(&mq->rxqsem, 0, (xwssq_t)num);
        return XWOK;

err_xwobj_activate:
        return rc;
}

/**
 * @brief XWMQ API：静态方式初始化消息队列
 * @param[in] mq: 消息队列对象的指针
 * @param[in] name: 名字
 * @param[in] txq: 消息槽内存池
 * @param[in] num: 消息槽数量
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmd_api
xwer_t xwmq_init(struct xwmq * mq, const char * name,
                 struct xwmq_msg * txq, xwsz_t num)
{
        XWOS_VALIDATE((mq), "nullptr", -EFAULT);
        XWOS_VALIDATE((txq), "nullptr", -EFAULT);
        xwmq_construct(mq);
        return xwmq_activate(mq, name, txq, num, xwmq_gc);
}

/**
 * @brief XWMQ API：销毁静态方式初始化的消息队列对象
 * @param[in] mq: 消息队列对象的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmd_api
xwer_t xwmq_fini(struct xwmq * mq)
{
        XWOS_VALIDATE((mq), "nullptr", -EFAULT);
        return xwmq_put(mq);
}

/******** ******** xwmq_msg ******** ********/

/**
 * @brief 消息对象的构造函数
 * @param[in] msg: 消息对象的指针
 */
static __xwmd_code
void xwmq_msg_construct(void * obj)
{
        struct xwmq_msg * msg = obj;
        xwlib_bclst_init_node(&msg->node);
        msg->topic = 0;
        msg->data = NULL;
}

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

static __xwmd_code
void xwmq_msg_put(struct xwmq * mq, struct xwmq_msg * msg)
{
        xwmm_memslice_free(&mq->txq, msg);
}

/**
 * @brief XWMQ API：将消息放入到消息队列的尾端（入队）
 * @param[in] mq: 消息队列对象的指针
 * @param[in] topic: 消息的标题
 * @param[in] data: 消息的数据
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @details
 * 若消息槽队列中没有空闲的消息槽，就阻塞发送线程，直到有空闲的消息槽或被中断。
 */
__xwmd_api
xwer_t xwmq_eq(struct xwmq * mq, xwsq_t topic, void * data)
{
        return xwmq_eq_to(mq, topic, data, XWTM_MAX);
}

/**
 * @brief XWMQ API：限时将消息放入到消息队列的尾端（入队）
 * @param[in] mq: 消息队列对象的指针
 * @param[in] topic: 消息的标题
 * @param[in] data: 消息的数据
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ETIMEDOUT: 超时
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @details
 * 若消息槽队列中没有空闲的消息槽，就阻塞发送线程，直到有空闲的消息槽或被中断或超时。
 * 如果 ```to``` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
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

/**
 * @brief XWMQ API：尝试将消息放入到消息队列的尾端（入队）
 * @param[in] mq: 消息队列对象的指针
 * @param[in] topic: 消息的标题
 * @param[in] data: 消息的数据
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENODATA: 接收队列为空
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @details
 * 若消息槽队列中没有空闲的消息槽，就立即返回```-ENODATA```，因此可在中断中使用。
 */
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

/**
 * @brief XWMQ API：将消息放入到消息队列的首端（插队）
 * @param[in] mq: 消息队列对象的指针
 * @param[in] topic: 消息的标题
 * @param[in] data: 消息的数据
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @details
 * 若消息槽队列中没有空闲的消息槽，就阻塞发送线程，直到有空闲的消息槽或被中断。
 */
__xwmd_api
xwer_t xwmq_jq(struct xwmq * mq, xwsq_t topic, void * data)
{
        return xwmq_jq_to(mq, topic, data, XWTM_MAX);
}

/**
 * @brief XWMQ API：限时将消息放入到消息队列的首端（插队）
 * @param[in] mq: 消息队列对象的指针
 * @param[in] topic: 消息的标题
 * @param[in] data: 消息的数据
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ETIMEDOUT: 超时
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @details
 * 若消息槽队列中没有空闲的消息槽，就阻塞发送线程，直到有空闲的消息槽或被中断或超时。
 * 如果 ```to``` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
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

/**
 * @brief XWMQ API：尝试将消息放入到消息队列的首端（插队）
 * @param[in] mq: 消息队列对象的指针
 * @param[in] topic: 消息的标题
 * @param[in] data: 消息的数据
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENODATA: 接收队列为空
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @details
 * 若消息槽队列中没有空闲的消息槽，就立即返回```-ENODATA```，因此可在中断中使用。
 */
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

static __xwmd_code
struct xwmq_msg * xwmq_choose(struct xwmq * mq)
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
 * @brief XWMQ API: 从消息队列头部取出一条消息，若消息队列为空，就无限等待
 * @param[in] mq: 消息队列对象的指针
 * @param[out] :data 指向缓冲区的指针，通过此缓冲区返回接收消息的数据
 * @param[out] :topic 指向缓冲区的指针，通过此缓冲区返回接收消息的标题
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @details
 * 若接收队列中没有消息，就阻塞接收线程，直到有新的消息槽或被中断。
 */
__xwmd_api
xwer_t xwmq_dq(struct xwmq * mq, xwsq_t * topic, void ** databuf)
{
        return xwmq_dq_to(mq, topic, databuf, XWTM_MAX);
}

/**
 * @brief XWMQ API: 限时从消息接收队列前端取出一条消息，若消息队列为空，就限时等待
 * @param[in] mq: 消息队列对象的指针
 * @param[out] :data 指向缓冲区的指针，通过此缓冲区返回接收消息的数据
 * @param[out] :topic 指向缓冲区的指针，通过此缓冲区返回接收消息的标题
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ETIMEDOUT: 超时
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @details
 * 若接收队列中没有消息，就阻塞接收线程，直到新的消息槽或被中断或超时。
 * 如果 ```to``` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
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
        msg = xwmq_choose(mq);
        *databuf = msg->data;
        if (topic) {
                *topic = msg->topic;
        }
        xwmq_msg_put(mq, msg);
        xwos_sem_post(&mq->txqsem);
        return XWOK;

err_sem_wait_to:
        return rc;
}

/**
 * @brief XWMQ API: 尝试从消息队列头部取出一条消息，若消息队列为空，立即返回
 * @param[in] mq: 消息队列对象的指针
 * @param[out] ptrbuf: 指向缓冲区的指针，此缓冲区用于接收消息
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENODATA: 接收队列为空
 * @note
 * - 同步/异步：同步
 * - 上下文：线程、中断、中断底半部
 * - 重入性：可重入
 * @details
 * 若接收队列中没有新的消息，就立即返回```-ENODATA```，因此可在中断中使用。
 */
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
        msg = xwmq_choose(mq);
        *databuf = msg->data;
        if (topic) {
                *topic = msg->topic;
        }
        xwmq_msg_put(mq, msg);
        xwos_sem_post(&mq->txqsem);
        return XWOK;

err_sem_trywait:
        return rc;
}
