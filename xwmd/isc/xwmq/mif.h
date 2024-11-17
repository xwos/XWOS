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
 * 技术参考手册： [消息队列](../docs/TechRefManual/Isc/Xwmq)
 *
 *
 * ## 消息队列的静态初始化、销毁
 *
 * + `xwmq_init()` ：初始化
 * + `xwmq_fini()` ：销毁
 *
 * ## 消息队列的动态创建、删除
 *
 * XWOS并未提供基于动态内存管理的创建与删除CAPI。
 *
 *
 * ## 发送消息
 * + 入队
 *   + `xwmq_eq()` ：等待消息槽，只能在 **线程** 上下文使用
 *   + `xwmq_eq_to()` ：限时等待消息槽，只能在 **线程** 上下文使用
 *   + `xwmq_eq_unintr()` ：不可中断地等待消息槽，只能在 **线程** 上下文使用
 *   + `xwmq_tryeq()` ：尝试获取消息槽，可在 **任意** 上下文使用
 * + 插队
 *   + `xwmq_jq()` ：等待消息槽，只能在 **线程** 上下文使用
 *   + `xwmq_jq_to()` ：限时等待消息槽，只能在 **线程** 上下文使用
 *   + `xwmq_jq_unintr()` ：不可中断地等待消息槽，只能在 **线程** 上下文使用
 *   + `xwmq_tryjq()` ：尝试获取消息槽，可在 **任意** 上下文使用
 *
 *
 * ## 接收消息
 *
 * + 首端离队
 *   + `xwmq_dq()` ：等待消息，只能在 **线程** 上下文使用
 *   + `xwmq_dq_to()` ：限时等待消息，只能在 **线程** 上下文使用
 *   + `xwmq_dq_unintr()` ：不可中断地等待消息，只能在 **线程** 上下文使用
 *   + `xwmq_trydq()` ：尝试获取消息，可在 **任意** 上下文使用
 * + 尾端离队
 *   + `xwmq_rq()` ：等待消息，只能在 **线程** 上下文使用
 *   + `xwmq_rq_to()` ：限时等待消息，只能在 **线程** 上下文使用
 *   + `xwmq_rq_unintr()` ：不可中断地等待消息，只能在 **线程** 上下文使用
 *   + `xwmq_tryrq()` ：尝试获取消息，可在 **任意** 上下文使用
 *
 *
 * ## 消息队列对象的生命周期管理
 *
 * + 通过 **对象指针** 管理生命周期：
 *   + `xwmq_grab()` ：增加引用计数
 *   + `xwmq_put()` ：减少引用计数
 * + 通过 **对象描述符** 管理生命周期：
 *   + `xwmq_acquire()` ：增加引用计数
 *   + `xwmq_release()` ：减少引用计数
 *
 *
 * ## 对象描述符和对象标签
 *
 * 已知消息队列对象的指针的情况下，可以通过 `xwmq_get_d()` 获取 @ref xwmq_d ，
 * 或可以通过 `xwmq_get_tik()` 获取对象标签。
 * @{
 */

/**
 * @brief 消息队列的消息
 */
struct xwmq_msg {
        void * data; /**< 消息数据 */
        xwsq_t topic; /**< 消息的主题 */
        struct xwlib_bclst_node node; /**< 链表节点，因 `xwmm_memslice` 实现的原因，
                                           不可作为结构体第一个成员 */
};

/**
 * @brief 消息队列对象
 */
struct xwmq {
        struct xwos_object xwobj; /**< C语言面向对象：继承 `struct xwos_object` */
        const char * name; /**< 名称 */
        struct xwmm_memslice txq; /**< 消息槽队列 */
        struct xwos_sem txqsem; /**< 消息槽的信号量 */
        struct xwlib_bclst_head rxq; /**< 接收队列 */
        struct xwos_splk rxqlock; /**< 保护接收队列的自旋锁 */
        struct xwos_sem rxqsem; /**< 接收队列的信号量 */
};

/**
 * @brief 消息队列对象描述符
 */
typedef struct {
        struct xwmq * mq; /**< 消息队列对象的指针 */
        xwsq_t tik; /**< 标签 */
} xwmq_d;

/**
 * @brief XWMQ API：静态方式初始化消息队列
 * @param[in] mq: 消息队列对象的指针
 * @param[in] txq: 消息槽内存池
 * @param[in] num: 消息槽数量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 无效参数
 * @note
 * + 上下文：任意
 */
xwer_t xwmq_init(struct xwmq * mq, struct xwmq_msg * txq, xwsz_t num);

/**
 * @brief XWMQ API：销毁静态方式初始化的消息队列对象
 * @param[in] mq: 消息队列对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：任意
 */
xwer_t xwmq_fini(struct xwmq * mq);

/**
 * @brief XWMQ API：增加消息队列对象的引用计数
 * @param[in] mq: 消息队列控制块对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EOBJDEAD: 对象无效
 * @note
 * + 上下文：任意
 */
xwer_t xwmq_grab(struct xwmq * mq);

/**
 * @brief XWMQ API：减少消息队列对象的引用计数
 * @param[in] mq: 消息队列控制块对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EOBJDEAD: 对象无效
 * @note
 * + 上下文：任意
 */
xwer_t xwmq_put(struct xwmq * mq);

/**
 * @brief XWMQ API：获取消息队列对象的标签
 * @param[in] mq: 消息队列对象的指针
 * @return 消息队列对象的标签
 * @note
 * + 上下文：任意
 */
xwsq_t xwmq_get_tik(struct xwmq * mq);

/**
 * @brief XWMQ API：获取消息队列对象描述符
 * @param[in] mq: 消息队列对象的指针
 * @return 消息队列对象描述符
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwmq_d xwmq_get_d(struct xwmq * mq)
{
        xwmq_d mqd;

        mqd.mq = mq;
        mqd.tik = xwmq_get_tik(mq);
        return mqd;
}

/**
 * @brief XWMQ API：检查消息队列对象的标签并增加引用计数
 * @param[in] mqd: 消息队列对象描述符
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENILOBJD: 空的对象描述符
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * + 上下文：任意
 */
xwer_t xwmq_acquire(xwmq_d mqd);

/**
 * @brief XWMQ API：检查消息队列对象的标签并减少引用计数
 * @param[in] mqd: 消息队列对象描述符
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENILOBJD: 空的对象描述符
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * + 上下文：任意
 */
xwer_t xwmq_release(xwmq_d mqd);

/**
 * @brief XWMQ API：等待消息槽，成功后将消息发送到消息队列的 **尾端** （入队）
 * @param[in] mq: 消息队列对象的指针
 * @param[in] topic: 消息的标题
 * @param[in] data: 消息的数据
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @retval -EDISIRQ: 中断被关闭
 * @retval -EDISPMPT: 抢占被关闭
 * @retval -EDISBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * 若消息槽队列中没有空闲的消息槽，就阻塞发送线程，直到有空闲的消息槽或被中断。
 *
 * 如果等待被中断，此CAPI将返回 `-EINTR` 。
 */
xwer_t xwmq_eq(struct xwmq * mq, xwsq_t topic, void * data);

/**
 * @brief XWMQ API：限时等待消息槽，成功后将消息发送到消息队列的 **尾端** （入队）
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
 * @retval -EDISIRQ: 中断被关闭
 * @retval -EDISPMPT: 抢占被关闭
 * @retval -EDISBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * + 若消息槽队列中没有空闲的消息槽，就阻塞发送线程，直到有空闲的消息槽或被中断或超时。
 * + 如果等待被中断，此CAPI将返回 `-EINTR` 。
 * + `to` 表示等待超时的时间点：
 *   + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *     可以使用 `xwtm_ft(delta)` 表示；
 *   + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwmq_eq_to(struct xwmq * mq, xwsq_t topic, void * data, xwtm_t to);

/**
 * @brief XWMQ API：等待消息槽，且等待不可被中断，
 *        成功后将消息发送到消息队列的 **尾端** （入队）
 * @param[in] mq: 消息队列对象的指针
 * @param[in] topic: 消息的标题
 * @param[in] data: 消息的数据
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @retval -EDISIRQ: 中断被关闭
 * @retval -EDISPMPT: 抢占被关闭
 * @retval -EDISBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * 若消息槽队列中没有空闲的消息槽，就阻塞发送线程，直到有空闲的消息槽。
 * 并且发送线程的等待不可被中断。
 */
xwer_t xwmq_eq_unintr(struct xwmq * mq, xwsq_t topic, void * data);

/**
 * @brief XWMQ API：尝试获取消息槽，成功后将消息发送到消息队列的 **尾端** （入队）
 * @param[in] mq: 消息队列对象的指针
 * @param[in] topic: 消息的标题
 * @param[in] data: 消息的数据
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENODATA: 接收队列为空
 * @note
 * + 上下文：任意
 * @details
 * 若消息槽队列中没有空闲的消息槽，就立即返回 `-ENODATA`，此CAPI可在中断中使用。
 */
xwer_t xwmq_tryeq(struct xwmq * mq, xwsq_t topic, void * data);

/**
 * @brief XWMQ API：等待消息槽，成功后将消息发送到消息队列的 **首端** （插队）
 * @param[in] mq: 消息队列对象的指针
 * @param[in] topic: 消息的标题
 * @param[in] data: 消息的数据
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @retval -EDISIRQ: 中断被关闭
 * @retval -EDISPMPT: 抢占被关闭
 * @retval -EDISBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * 若消息槽队列中没有空闲的消息槽，就阻塞发送线程，直到有空闲的消息槽或被中断。
 *
 * 如果等待被中断，此CAPI将返回 `-EINTR` 。
 */
xwer_t xwmq_jq(struct xwmq * mq, xwsq_t topic, void * data);

/**
 * @brief XWMQ API：限时等待消息槽，成功后将消息发送到消息队列的 **首端** （插队）
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
 * @retval -EDISIRQ: 中断被关闭
 * @retval -EDISPMPT: 抢占被关闭
 * @retval -EDISBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * + 若消息槽队列中没有空闲的消息槽，就阻塞发送线程，直到有空闲的消息槽或被中断或超时。
 * + 如果等待被中断，此CAPI将返回 `-EINTR` 。
 * + `to` 表示等待超时的时间点：
 *   + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *     可以使用 `xwtm_ft(delta)` 表示；
 *   + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwmq_jq_to(struct xwmq * mq, xwsq_t topic, void * data, xwtm_t to);

/**
 * @brief XWMQ API：等待消息槽，且等待不可被中断，
 *        成功后将消息发送到消息队列的 **首端** （插队）
 * @param[in] mq: 消息队列对象的指针
 * @param[in] topic: 消息的标题
 * @param[in] data: 消息的数据
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @retval -EDISIRQ: 中断被关闭
 * @retval -EDISPMPT: 抢占被关闭
 * @retval -EDISBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * 若消息槽队列中没有空闲的消息槽，就阻塞发送线程，直到有空闲的消息槽。
 * 并且发送线程的等待不可被中断。
 */
xwer_t xwmq_jq_unintr(struct xwmq * mq, xwsq_t topic, void * data);

/**
 * @brief XWMQ API：尝试获取消息槽，成功后将消息发送到消息队列的 **首端** （插队）
 * @param[in] mq: 消息队列对象的指针
 * @param[in] topic: 消息的标题
 * @param[in] data: 消息的数据
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENODATA: 接收队列为空
 * @note
 * + 上下文：任意
 * @details
 * 若消息槽队列中没有空闲的消息槽，就立即返回 `-ENODATA`，此CAPI可在中断中使用。
 */
xwer_t xwmq_tryjq(struct xwmq * mq, xwsq_t topic, void * data);

/**
 * @brief XWMQ API: 等待从消息队列 **首端** 接收一条消息
 * @param[in] mq: 消息队列对象的指针
 * @param[out] :topic 指向缓冲区的指针，通过此缓冲区返回接收消息的标题
 * @param[out] :databuf 指向缓冲区的指针，通过此缓冲区返回接收消息的数据
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @retval -EDISIRQ: 中断被关闭
 * @retval -EDISPMPT: 抢占被关闭
 * @retval -EDISBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * 若接收队列中没有消息，就阻塞接收线程，直到有新的消息或被中断。
 *
 * 接收消息后，会释放消息槽。若有发送线程正在等待消息槽，将唤醒发送线程。
 *
 * 如果等待被中断，此CAPI将返回 `-EINTR` 。
 */
xwer_t xwmq_dq(struct xwmq * mq, xwsq_t * topic, void ** databuf);

/**
 * @brief XWMQ API: 限时等待从消息队列 **首端** 接收一条消息
 * @param[in] mq: 消息队列对象的指针
 * @param[out] :topic 指向缓冲区的指针，通过此缓冲区返回接收消息的标题
 * @param[out] :databuf 指向缓冲区的指针，通过此缓冲区返回接收消息的数据
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ETIMEDOUT: 超时
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @retval -EDISIRQ: 中断被关闭
 * @retval -EDISPMPT: 抢占被关闭
 * @retval -EDISBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * + 若接收队列中没有消息，就阻塞接收线程，直到新的消息或被中断或超时。
 * + 接收消息后，会释放消息槽。若有发送线程正在等待消息槽，将唤醒发送线程。
 * + 如果等待被中断，此CAPI将返回 `-EINTR` 。
 * + `to` 表示等待超时的时间点：
 *   + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *     可以使用 `xwtm_ft(delta)` 表示；
 *   + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwmq_dq_to(struct xwmq * mq, xwsq_t * topic, void ** databuf, xwtm_t to);

/**
 * @brief XWMQ API: 等待从消息队列 **首端** 接收一条消息，且等待不可被中断
 * @param[in] mq: 消息队列对象的指针
 * @param[out] :topic 指向缓冲区的指针，通过此缓冲区返回接收消息的标题
 * @param[out] :databuf 指向缓冲区的指针，通过此缓冲区返回接收消息的数据
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @retval -EDISIRQ: 中断被关闭
 * @retval -EDISPMPT: 抢占被关闭
 * @retval -EDISBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * 若接收队列中没有消息，就阻塞接收线程，直到有新的消息。
 * 并且接收线程的等待不可被中断。
 *
 * 接收消息后，会释放消息槽。若有发送线程正在等待消息槽，将唤醒发送线程。
 */
xwer_t xwmq_dq_unintr(struct xwmq * mq, xwsq_t * topic, void ** databuf);

/**
 * @brief XWMQ API: 尝试从消息队列 **首端** 接收一条消息
 * @param[in] mq: 消息队列对象的指针
 * @param[out] :topic 指向缓冲区的指针，通过此缓冲区返回接收消息的标题
 * @param[out] :databuf 指向缓冲区的指针，通过此缓冲区返回接收消息的数据
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENODATA: 接收队列为空
 * @note
 * + 上下文：任意
 * @details
 * 若接收队列中没有新的消息，就立即返回 `-ENODATA` ，此CAPI可在中断中使用。
 *
 * 接收消息后，会释放消息槽。若有发送线程正在等待消息槽，将唤醒发送线程。
 */
xwer_t xwmq_trydq(struct xwmq * mq, xwsq_t * topic, void ** databuf);

/**
 * @brief XWMQ API: 等待从消息队列 **尾端** 接收一条消息
 * @param[in] mq: 消息队列对象的指针
 * @param[out] :topic 指向缓冲区的指针，通过此缓冲区返回接收消息的标题
 * @param[out] :databuf 指向缓冲区的指针，通过此缓冲区返回接收消息的数据
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @retval -EDISIRQ: 中断被关闭
 * @retval -EDISPMPT: 抢占被关闭
 * @retval -EDISBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * 若接收队列中没有消息，就阻塞接收线程，直到有新的消息或被中断。
 *
 * 接收消息后，会释放消息槽。若有发送线程正在等待消息槽，将唤醒发送线程。
 *
 * 如果等待被中断，此CAPI将返回 `-EINTR` 。
 */
xwer_t xwmq_rq(struct xwmq * mq, xwsq_t * topic, void ** databuf);

/**
 * @brief XWMQ API: 限时等待从消息队列 **尾端** 接收一条消息
 * @param[in] mq: 消息队列对象的指针
 * @param[out] :topic 指向缓冲区的指针，通过此缓冲区返回接收消息的标题
 * @param[out] :databuf 指向缓冲区的指针，通过此缓冲区返回接收消息的数据
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ETIMEDOUT: 超时
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @retval -EDISIRQ: 中断被关闭
 * @retval -EDISPMPT: 抢占被关闭
 * @retval -EDISBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * + 若接收队列中没有消息，就阻塞接收线程，直到新的消息或被中断或超时。
 * + 接收消息后，会释放消息槽。若有发送线程正在等待消息槽，将唤醒发送线程。
 * + 如果等待被中断，此CAPI将返回 `-EINTR` 。
 * + `to` 表示等待超时的时间点：
 *   + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *     可以使用 `xwtm_ft(delta)` 表示；
 *   + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwmq_rq_to(struct xwmq * mq, xwsq_t * topic, void ** databuf, xwtm_t to);

/**
 * @brief XWMQ API: 等待从消息队列 **尾端** 接收一条消息，且等待不可被中断
 * @param[in] mq: 消息队列对象的指针
 * @param[out] :topic 指向缓冲区的指针，通过此缓冲区返回接收消息的标题
 * @param[out] :databuf 指向缓冲区的指针，通过此缓冲区返回接收消息的数据
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @retval -EDISIRQ: 中断被关闭
 * @retval -EDISPMPT: 抢占被关闭
 * @retval -EDISBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * 若接收队列中没有消息，就阻塞接收线程，直到有新的消息。
 * 并且接收线程的等待不可被中断。
 *
 * 接收消息后，会释放消息槽。若有发送线程正在等待消息槽，将唤醒发送线程。
 */
xwer_t xwmq_rq_unintr(struct xwmq * mq, xwsq_t * topic, void ** databuf);

/**
 * @brief XWMQ API: 尝试从消息队列 **尾端** 接收一条消息
 * @param[in] mq: 消息队列对象的指针
 * @param[out] :topic 指向缓冲区的指针，通过此缓冲区返回接收消息的标题
 * @param[out] :databuf 指向缓冲区的指针，通过此缓冲区返回接收消息的数据
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENODATA: 接收队列为空
 * @note
 * + 上下文：任意
 * @details
 * 若接收队列中没有新的消息，就立即返回 `-ENODATA` ，此CAPI可在中断中使用。
 *
 * 接收消息后，会释放消息槽。若有发送线程正在等待消息槽，将唤醒发送线程。
 */
xwer_t xwmq_tryrq(struct xwmq * mq, xwsq_t * topic, void ** databuf);

/**
 * @} xwmd_isc_xwmq
 */

#endif /* xwmd/isc/xwmq/mif.h */
