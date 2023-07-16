/**
 * @file
 * @brief 循环队列：模块接口
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_isc_xwcq_mif_h__
#define __xwmd_isc_xwcq_mif_h__

#include <xwos/standard.h>
#include <xwos/lib/object.h>
#include <xwos/lib/bclst.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/sem.h>
#include <xwos/osal/sync/sel.h>

/**
 * @defgroup xwmd_isc_xwcq 循环队列
 * @ingroup xwmd_isc
 * @{
 */

/**
 * @brief 定义循环队列缓冲区
 * @param[in] name: 变量名称
 * @param[in] slotsize: 数据槽的大小
 * @param[in] slotnum: 数据槽的个数
 */
// cppcheck-suppress [misra-c2012-20.7]
#define XWCQ_DEF_MEMPOOL(name, slotsize, slotnum) \
        __xwcc_alignl1cache xwu8_t name[(slotsize) * (slotnum)]

/**
 * @brief 循环队列对象
 */
struct xwcq {
        struct xwos_object xwobj; /**< C语言面向对象：继承struct xwos_object */
        xwsz_t slotsize; /**< 队列中数据槽的大小 */
        xwsz_t slotnum; /**< 队列中数据槽的数量 */
        struct xwos_sem sem; /**< 队列中的可用数据信号量 */
        struct xwos_splk lock; /**< 保护队列的锁 */
        xwssz_t rear; /**< 可用于接收的空白数据槽的位置 */
        xwssz_t front; /**< 包含有效数据的数据槽的位置 */
        xwu8_t * q; /**< 队列 */
};

/**
 * @brief 循环队列对象描述符
 */
typedef struct {
        struct xwcq * cq; /**< 循环队列对象的指针 */
        xwsq_t tik; /**< 标签 */
} xwcq_d;

/**
 * @brief XWCQ API：初始化循环队列
 * @param[in] cq: 循环队列对象的指针
 * @param[in] slotsize: 每个数据槽的大小
 * @param[in] slotnum: 数据槽的数量
 * @param[in] mem: 数据缓冲区的指针，
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ESIZE: `slotsize` 或 `slotnum` 为 `0`
 * @retval -EALIGN: `slotsize` 不是 `XWMMCFG_ALIGNMENT` 的整数倍
 * @note
 * + 上下文：任意
 * @details
 * 循环队列由 `slotnum` 个数据槽构成，每个数据槽能容纳 `slotsize` 字节的数据。
 *
 * 数据缓冲区 `mem` 大小不能小于 `slotsize * slotnum` ，
 * 用户应该使用宏 @ref XWCQ_DEF_MEMPOOL() 来定义数据缓冲区。
 *
 * 单个数据槽的大小 `slotsize` 必须是系统配置 `XWMMCFG_ALIGNMENT` 的整数倍，
 * 错误的大小将返回错误码 `-EALIGN` 。
 *
 */
xwer_t xwcq_init(struct xwcq * cq, xwsz_t slotsize, xwsz_t slotnum, xwu8_t * mem);

/**
 * @brief XWCQ API：销毁循环队列对象
 * @param[in] cq: 循环队列对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：任意
 */
xwer_t xwcq_fini(struct xwcq * cq);

/**
 * @brief XWCQ API：获取循环队列对象的标签
 * @param[in] cq: 循环队列对象的指针
 * @return 循环队列对象的标签
 * @note
 * + 上下文：任意
 */
xwsq_t xwcq_gettik(struct xwcq * cq);

/**
 * @brief XWCQ API：获取循环队列对象描述符
 * @param[in] cq: 循环队列对象的指针
 * @return 循环队列对象描述符
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwcq_d xwcq_getd(struct xwcq * cq)
{
        xwcq_d cqd;

        cqd.cq = cq;
        cqd.tik = xwcq_gettik(cq);
        return cqd;
}

/**
 * @brief XWCQ API：检查循环队列对象的标签并增加引用计数
 * @param[in] cqd: 循环队列对象描述符
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENILOBJD: 空的对象描述符
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * - 同步/异步：同步
 * + 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
xwer_t xwcq_acquire(xwcq_d cqd);

/**
 * @brief XWCQ API：检查循环队列对象的标签并增加引用计数
 * @param[in] cqd: 循环队列对象描述符
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENILOBJD: 空的对象描述符
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * - 同步/异步：同步
 * + 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
xwer_t xwcq_release(xwcq_d cqd);

/**
 * @brief XWCQ API：增加循环队列对象的引用计数
 * @param[in] cq: 循环队列控制块对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EOBJDEAD: 对象无效
 * @note
 * + 上下文：任意
 */
xwer_t xwcq_grab(struct xwcq * cq);

/**
 * @brief XWCQ API：减少循环队列对象的引用计数
 * @param[in] cq: 循环队列控制块对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EOBJDEAD: 对象无效
 * @note
 * + 上下文：任意
 */
xwer_t xwcq_put(struct xwcq * cq);

/**
 * @brief XWCQ API：将数据发送到循环队列的 **尾端** （入队，EnQueue）
 * @param[in] cq: 循环队列对象的指针
 * @param[in] data: 数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示待入队的数据大小
 * + (O) 作为输出时，返回实际入队的数据大小
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：任意
 * @details
 * 如果传入的数据大小 `*size` 超过初始化循环队列时指定的 `slotsize` ，数据将被截断。
 * 如果循环队列数据已被填满，循环队列会循环回队列 **首端** 的位置，覆盖掉原数据。
 *
 * 此CAPI不会阻塞调用者，因此可以在中断中使用。
 */
xwer_t xwcq_eq(struct xwcq * cq, const xwu8_t * data, xwsz_t * size);

/**
 * @brief XWCQ API：将数据发送循环队列的 **首端** （插队，Jump the Queue）
 * @param[in] cq: 循环队列对象的指针
 * @param[in] data: 数据
 * @param[in] size: 数据的大小
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：任意
 * @details
 * 如果传入的数据大小 `*size` 超过初始化循环队列时指定的 `slotsize` ，数据将被截断。
 * 如果循环队列数据已被填满，循环队列会循环回队列 **尾端** 的位置，覆盖掉原数据。
 *
 * 此CAPI不会阻塞调用者，因此可以在中断中使用。
 */
xwer_t xwcq_jq(struct xwcq * cq, const xwu8_t * data, xwsz_t * size);

/**
 * @brief XWCQ API: 等待从循环队列的 **首端** 接收数据（离队，DeQueue）
 * @param[in] cq: 循环队列对象的指针
 * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示接收缓冲区的大小
 * + (O) 作为输出时，返回实际接收的数据大小
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @retval -EDSPMPT: 抢占被关闭
 * @retval -EDSBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * 若循环队列中数据为空，接收线程会阻塞等待，直到循环队列中有消息可取出，或等待被中断。
 *
 * 如果等待被中断，此CAPI将返回 `-EINTR` 。
 */
xwer_t xwcq_dq(struct xwcq * cq, xwu8_t * data, xwsz_t * size);

/**
 * @brief XWCQ API: 限时等待从循环队列的 **首端** 接收数据（离队，DeQueue）
 * @param[in] cq: 循环队列对象的指针
 * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示接收缓冲区的大小
 * + (O) 作为输出时，返回实际接收的数据大小
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ETIMEDOUT: 超时
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @retval -EDSPMPT: 抢占被关闭
 * @retval -EDSBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * 若循环队列中数据为空，接收线程会阻塞等待，
 * 直到循环队列中有消息可取出，或等待被中断，或等待超时。
 *
 * 如果等待被中断，此CAPI将返回 `-EINTR` 。
 *
 * 如果 `to` 是过去的时间点，此CAPI将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwcq_dq_to(struct xwcq * cq, xwu8_t * data, xwsz_t * size, xwtm_t to);

/**
 * @brief XWCQ API: 等待从循环队列的 **首端** 接收数据（离队，DeQueue），
 *        并且等待不可被中断
 * @param[in] cq: 循环队列对象的指针
 * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示接收缓冲区的大小
 * + (O) 作为输出时，返回实际接收的数据大小
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @retval -EDSPMPT: 抢占被关闭
 * @retval -EDSBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * 若循环队列中数据为空，接收线程会阻塞等待，直到循环队列中有消息可取出。
 *
 * 接收线程的阻塞等待不可被中断。
 */
xwer_t xwcq_dq_unintr(struct xwcq * cq, xwu8_t * data, xwsz_t * size);

/**
 * @brief XWCQ API: 尝试从循环队列的 **首端** 接收数据（离队，DeQueue）
 * @param[in] cq: 循环队列对象的指针
 * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示接收缓冲区的大小
 * + (O) 作为输出时，返回实际接收的数据大小
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENODATA: 循环队列为空
 * @note
 * + 上下文：任意
 * @details
 * 若循环队列中为空，就立即返回 `-ENODATA` ，此CAPI可在中断中使用。
 */
xwer_t xwcq_trydq(struct xwcq * cq, xwu8_t * data, xwsz_t * size);

/**
 * @brief XWCQ API: 等待从循环队列的 **尾端** 接收数据（反向离队，Reversely deQueue）
 * @param[in] cq: 循环队列对象的指针
 * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示接收缓冲区的大小
 * + (O) 作为输出时，返回实际接收的数据大小
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @retval -EDSPMPT: 抢占被关闭
 * @retval -EDSBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * 若循环队列中数据为空，接收线程会阻塞等待，直到循环队列中有消息可取出，或等待被中断。
 *
 * 如果等待被中断，此CAPI将返回 `-EINTR` 。
 */
xwer_t xwcq_rq(struct xwcq * cq, xwu8_t * data, xwsz_t * size);

/**
 * @brief XWCQ API: 限时等待从循环队列的 **尾端** 接收数据
 *        （反向离队，Reversely deQueue）
 * @param[in] cq: 循环队列对象的指针
 * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示接收缓冲区的大小
 * + (O) 作为输出时，返回实际接收的数据大小
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ETIMEDOUT: 超时
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @retval -EDSPMPT: 抢占被关闭
 * @retval -EDSBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * 若循环队列中数据为空，接收线程会阻塞等待，
 * 直到循环队列中有消息可取出，或等待被中断，或等待超时。
 *
 * 如果等待被中断，此CAPI将返回 `-EINTR` 。
 *
 * 如果 `to` 是过去的时间点，此CAPI将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwcq_rq_to(struct xwcq * cq, xwu8_t * data, xwsz_t * size, xwtm_t to);

/**
 * @brief XWCQ API: 等待从循环队列的 **尾端** 接收数据（反向离队，Reversely deQueue），
 *        并且等待不可被中断
 * @param[in] cq: 循环队列对象的指针
 * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示接收缓冲区的大小
 * + (O) 作为输出时，返回实际接收的数据大小
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @retval -EDSPMPT: 抢占被关闭
 * @retval -EDSBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * 若循环队列中数据为空，接收线程会阻塞等待，直到循环队列中有消息可取出。
 *
 * 接收线程的阻塞等待不可被中断。
 */
xwer_t xwcq_rq_unintr(struct xwcq * cq, xwu8_t * data, xwsz_t * size);

/**
 * @brief XWCQ API: 尝试从循环队列的 **尾端** 接收数据（反向离队，Reversely deQueue）
 * @param[in] cq: 循环队列对象的指针
 * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示接收缓冲区的大小
 * + (O) 作为输出时，返回实际接收的数据大小
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENODATA: 循环队列为空
 * @note
 * + 上下文：任意
 * @details
 * 若循环队列中为空，就立即返回 `-ENODATA` ，此CAPI可在中断中使用。
 */
xwer_t xwcq_tryrq(struct xwcq * cq, xwu8_t * data, xwsz_t * size);

/**
 * @brief XWCQ API: 等待从循环队列 **头端** 拷贝数据（Peek at the Front of Queue）
 * @param[in] cq: 循环队列对象的指针
 * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示接收缓冲区的大小
 * + (O) 作为输出时，返回实际接收的数据大小
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @retval -EDSPMPT: 抢占被关闭
 * @retval -EDSBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * 拷贝数据不会取走数据，也不会释放数据槽，数据可被重复拷贝，也可继续被接收。
 *
 * 若循环队列中数据为空，接收线程会阻塞等待，直到循环队列中有消息可取出，或等待被中断。
 *
 * 如果等待被中断，此CAPI将返回 `-EINTR` 。
 */
xwer_t xwcq_pfq(struct xwcq * cq, xwu8_t * data, xwsz_t * size);

/**
 * @brief XWCQ API: 限时等待从循环队列 **头端** 拷贝数据
 *        （Peek at the Front of Queue）
 * @param[in] cq: 循环队列对象的指针
 * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示接收缓冲区的大小
 * + (O) 作为输出时，返回实际接收的数据大小
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ETIMEDOUT: 超时
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @retval -EDSPMPT: 抢占被关闭
 * @retval -EDSBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * 拷贝数据不会取走数据，也不会释放数据槽，数据可被重复拷贝，也可继续被接收。
 *
 * 若循环队列中数据为空，接收线程会阻塞等待，
 * 直到循环队列中有消息可取出，或等待被中断，或等待超时。
 *
 * 如果等待被中断，此CAPI将返回 `-EINTR` 。
 *
 * 如果 `to` 是过去的时间点，此CAPI将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwcq_pfq_to(struct xwcq * cq, xwu8_t * data, xwsz_t * size, xwtm_t to);

/**
 * @brief XWCQ API: 等待从循环队列 **头端** 拷贝数据（Peek at the Front of Queue），
 *        并且等待不可被中断
 * @param[in] cq: 循环队列对象的指针
 * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示接收缓冲区的大小
 * + (O) 作为输出时，返回实际接收的数据大小
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @retval -EDSPMPT: 抢占被关闭
 * @retval -EDSBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * 拷贝数据不会取走数据，也不会释放数据槽，数据可被重复拷贝，也可继续被接收。
 *
 * 若循环队列中数据为空，接收线程会阻塞等待，直到循环队列中有消息可取出。
 *
 * 接收线程的阻塞等待不可被中断。
 */
xwer_t xwcq_pfq_unintr(struct xwcq * cq, xwu8_t * data, xwsz_t * size);

/**
 * @brief XWCQ API: 尝试从循环队列 **头端** 拷贝数据（Peek at the Front of Queue）
 * @param[in] cq: 循环队列对象的指针
 * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示接收缓冲区的大小
 * + (O) 作为输出时，返回实际接收的数据大小
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENODATA: 循环队列为空
 * @note
 * + 上下文：任意
 * @details
 * 拷贝数据不会取走数据，也不会释放数据槽，数据可被重复拷贝，也可继续被接收。
 *
 * 若循环队列中为空，就立即返回 `-ENODATA` ，此CAPI可在中断中使用。
 */
xwer_t xwcq_trypfq(struct xwcq * cq, xwu8_t * data, xwsz_t * size);

/**
 * @brief XWCQ API: 等待从循环队列 **尾端** 拷贝数据（Peek at the Rear of Queue）
 * @param[in] cq: 循环队列对象的指针
 * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示接收缓冲区的大小
 * + (O) 作为输出时，返回实际接收的数据大小
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @retval -EDSPMPT: 抢占被关闭
 * @retval -EDSBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * 拷贝数据不会取走数据，也不会释放数据槽，数据可被重复拷贝，也可继续被接收。
 *
 * 若循环队列中数据为空，接收线程会阻塞等待，直到循环队列中有消息可取出，或等待被中断。
 *
 * 如果等待被中断，此CAPI将返回 `-EINTR` 。
 */
xwer_t xwcq_prq(struct xwcq * cq, xwu8_t * data, xwsz_t * size);

/**
 * @brief XWCQ API: 限时等待从循环队列 **尾端** 拷贝数据（Peek at the Rear of Queue）
 * @param[in] cq: 循环队列对象的指针
 * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示接收缓冲区的大小
 * + (O) 作为输出时，返回实际接收的数据大小
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ETIMEDOUT: 超时
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @retval -EDSPMPT: 抢占被关闭
 * @retval -EDSBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * 拷贝数据不会取走数据，也不会释放数据槽，数据可被重复拷贝，也可继续被接收。
 *
 * 若循环队列中数据为空，接收线程会阻塞等待，
 * 直到循环队列中有消息可取出，或等待被中断，或等待超时。
 *
 * 如果等待被中断，此CAPI将返回 `-EINTR` 。
 *
 * 如果 `to` 是过去的时间点，此CAPI将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwcq_prq_to(struct xwcq * cq, xwu8_t * data, xwsz_t * size, xwtm_t to);

/**
 * @brief XWCQ API: 等待从循环队列 **尾端** 拷贝数据（Peek at the Rear of Queue），
 *        并且等待不可被中断
 * @param[in] cq: 循环队列对象的指针
 * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示接收缓冲区的大小
 * + (O) 作为输出时，返回实际接收的数据大小
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @retval -EDSPMPT: 抢占被关闭
 * @retval -EDSBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * 拷贝数据不会取走数据，也不会释放数据槽，数据可被重复拷贝，也可继续被接收。
 *
 * 若循环队列中数据为空，接收线程会阻塞等待，直到循环队列中有消息可取出。
 *
 * 接收线程的阻塞等待不可被中断。
 */
xwer_t xwcq_prq_unintr(struct xwcq * cq, xwu8_t * data, xwsz_t * size);

/**
 * @brief XWCQ API: 尝试从循环队列 **尾端** 拷贝数据（Peek at the Rear of Queue）
 * @param[in] cq: 循环队列对象的指针
 * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示接收缓冲区的大小
 * + (O) 作为输出时，返回实际接收的数据大小
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENODATA: 循环队列为空
 * @note
 * + 上下文：任意
 * @details
 * 拷贝数据不会取走数据，也不会释放数据槽，数据可被重复拷贝，也可继续被接收。
 *
 * 若循环队列中为空，就立即返回 `-ENODATA` ，此CAPI可在中断中使用。
 */
xwer_t xwcq_tryprq(struct xwcq * cq, xwu8_t * data, xwsz_t * size);

/**
 * @brief XWCQ API: 清空循环队列
 * @param[in] cq: 循环队列对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：任意
 */
xwer_t xwcq_flush(struct xwcq * cq);

/**
 * @brief XWCQ API: 获取循环队列的容量
 * @param[in] cq: 循环队列对象的指针
 * @param[out] capbuf: 指向缓冲区的指针，此缓冲区用于返回容量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：任意
 * @details
 * 循环队列的容量也即是数据槽的数量，是在调用 @ref xwcq_init() 时的参数 `slotnum` 。
 */
xwer_t xwcq_get_capacity(struct xwcq * cq, xwsz_t * capbuf);

/**
 * @brief XWCQ API: 获取循环队列的单个数据槽的大小
 * @param[in] cq: 循环队列对象的指针
 * @param[out] szbuf: 指向缓冲区的指针，此缓冲区用于返回单个数据槽的大小
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：任意
 * @details
 * 数据槽的大小是在调用 @ref xwcq_init() 时的参数 `slotsize` 。
 */
xwer_t xwcq_get_size(struct xwcq * cq, xwsz_t * szbuf);

/**
 * @brief XWCQ API: 获取循环队列中有效数据的数量
 * @param[in] cq: 循环队列对象的指针
 * @param[out] szbuf: 指向缓冲区的指针，此缓冲区用于返回数量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：任意
 * @details
 * 有效数据的数量是指可被取出的数据的数量。
 */
xwer_t xwcq_get_availability(struct xwcq * cq, xwsz_t * avbbuf);

/**
 * @brief XWOS API：绑定循环队列对象到信号选择器
 * @param[in] cq: 循环队列对象的指针
 * @param[in] sel: 信号选择器的指针
 * @param[in] pos: 循环队列对象对象映射到位图中的位置
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ECHRNG: 位置超出范围
 * @retval -EALREADY: 同步对象已经绑定到事件对象
 * @retval -EBUSY: 通道已经被其他同步对象独占
 * @note
 * + 上下文：任意
 * - 绑定方式：独占绑定
 */
xwer_t xwcq_bind(struct xwcq * cq, struct xwos_sel * sel, xwsq_t pos);

/**
 * @brief XWOS API：从信号选择器上解绑循环队列对象
 * @param[in] cq: 循环队列对象的指针
 * @param[in] sel: 信号选择器的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
 * @note
 * + 上下文：任意
 */
xwer_t xwcq_unbind(struct xwcq * cq, struct xwos_sel * sel);

/**
 * @} xwmd_isc_xwcq
 */

#endif /* xwmd/isc/xwcq/mif.h */
