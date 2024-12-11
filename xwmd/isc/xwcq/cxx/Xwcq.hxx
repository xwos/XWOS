/**
 * @file
 * @brief xwisc::Xwcq
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_isc_xwcq_cxx_XwcqProxy_hxx__
#define __xwmd_isc_xwcq_cxx_XwcqProxy_hxx__

extern "C" {
#include "xwmd/isc/xwcq/mif.h"
}

namespace xwisc {

/**
 * @defgroup xwmd_isc_xwcq_cxx XWCQ C++ Lib
 * @ingroup xwmd_isc_xwcq
 *
 * @{
 */

/**
 * @brief XWCQ C++ 对象
 */
template <xwsz_t TSlotNum, xwsz_t TSlotSize>
class Xwcq
{
  private:
    struct xwcq mCq;
    __xwcc_alignl1cache xwu8_t mPool[TSlotSize * TSlotNum];

  public:
    Xwcq() { xwcq_init(&mCq, TSlotSize, TSlotNum, mPool); }
    ~Xwcq() {}

    /**
     * @brief 获取循环队列内部对象指针
     */
    struct xwcq * get() { return &mCq; }

    /******** 查询 ********/
    /**
     * @brief XWCQ API: 获取循环队列的容量 `TSlotNum`
     * @return 容量
     * @note
     * + 上下文：任意
     */
    xwsz_t capacity() {
        xwsz_t cap;
        xwcq_get_capacity(&mCq, &cap);
        return cap;
    }

    /**
     * @brief XWCQ API: 获取循环队列的单个数据槽的大小 `TSlotSize`
     * @return 单个数据槽的大小
     * @note
     * + 上下文：任意
     */
    xwsz_t size() {
        xwsz_t size;
        xwcq_get_size(&mCq, &size);
        return size;
    }

    /**
     * @brief XWCQ API: 获取循环队列中有效数据槽的数量
     * @return 有效数据槽的数量
     * @note
     * + 上下文：任意
     */
    xwsz_t availability() {
        xwsz_t availability;
        xwcq_get_availability(&mCq, &availability);
        return availability;
    }

    /******** 维护 ********/
    /**
     * @brief 清空循环队列
     * @note
     * + 上下文：任意
     */
    void flush() { xwcq_flush(&mCq); }

    /******** 发送 ********/
    /**
     * @brief 将数据发送到循环队列的 **尾端**  (入队，EnQueue)
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
     * + 如果传入的数据大小 `*size` 超过初始化循环队列时指定的 `TSlotSize` ，
     *   数据将被截断。
     * + 如果循环队列数据已被填满，循环队列会循环回队列 **首端** 的位置，
     *   覆盖掉原数据。
     * + 此C++API不会阻塞调用者，因此可以在中断中使用。
     */
    xwer_t eq(const xwu8_t * data, xwsz_t * size) { return xwcq_eq(&mCq, data, size); }

    /**
     * @brief 将数据发送循环队列的 **首端**  (插队，Jump the Queue)
     * @param[in] data: 数据
     * @param[in] size: 数据的大小
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 空指针
     * @note
     * + 上下文：任意
     * @details
     * + 如果传入的数据大小 `*size` 超过初始化循环队列时指定的 `TSlotSize` ，
     *   数据将被截断。
     * + 如果循环队列数据已被填满，循环队列会循环回队列 **尾端** 的位置，
     *   覆盖掉原数据。
     * + 此C++API不会阻塞调用者，因此可以在中断中使用。
     */
    xwer_t jq(const xwu8_t * data, xwsz_t * size) { return xwcq_jq(&mCq, data, size); }

    /******** 接收 ********/

    /**
     * @brief 等待从循环队列的 **首端** 接收数据 (离队，DeQueue)
     * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
     * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
     * + (I) 作为输入时，表示接收缓冲区的大小
     * + (O) 作为输出时，返回实际接收的数据大小
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
     * + 若循环队列中数据为空，调用此C++API的线程会阻塞等待，
     *   直到循环队列中有消息可取出，或等待被中断。
     * + 如果等待被中断，此C++API将返回 `-EINTR` 。
     */
    xwer_t dq(xwu8_t * data, xwsz_t * size) { return xwcq_dq(&mCq, data, size); }

    /**
     * @brief 限时等待从循环队列的 **首端** 接收数据 (离队，DeQueue)
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
     * @retval -EDISIRQ: 中断被关闭
     * @retval -EDISPMPT: 抢占被关闭
     * @retval -EDISBH: 中断底半部被关闭
     * @note
     * + 上下文：线程
     * @details
     * + 若循环队列中数据为空，调用此C++API的线程会阻塞等待，
     *   直到循环队列中有消息可取出，或等待被中断，或等待超时。
     * + 如果等待被中断，此C++API将返回 `-EINTR` 。
     * + `to` 表示等待超时的时间点：
     *   + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
     *     可以使用 `xwtm_ft(delta)` 表示；
     *   + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
     */
    xwer_t dq(xwu8_t * data, xwsz_t * size, xwtm_t to) {
        return xwcq_dq_to(&mCq, data, size, to);
    }

    /**
     * @brief 等待从循环队列的 **首端** 接收数据 (离队，DeQueue)，
     *        并且等待不可被中断
     * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
     * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
     * + (I) 作为输入时，表示接收缓冲区的大小
     * + (O) 作为输出时，返回实际接收的数据大小
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
     * + 若循环队列中数据为空，调用此C++API的线程会阻塞等待，直到循环队列中有消息可取出。
     * + 调用此C++API的线程的阻塞等待不可被中断。
     */
    xwer_t dqUnintr(xwu8_t * data, xwsz_t * size) {
        return xwcq_dq_unintr(&mCq, data, size);
    }

    /**
     * @brief 尝试从循环队列的 **首端** 接收数据 (离队，DeQueue)
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
     * + 若循环队列中为空，就立即返回 `-ENODATA` 。
     * + 此C++API可在中断中使用。
     */
    xwer_t tryDq(xwu8_t * data, xwsz_t * size) { return xwcq_trydq(&mCq, data, size); }

    /**
     * @brief 等待从循环队列的 **尾端** 接收数据 (反向离队，Reversely deQueue)
     * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
     * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
     * + (I) 作为输入时，表示接收缓冲区的大小
     * + (O) 作为输出时，返回实际接收的数据大小
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
     * + 若循环队列中数据为空，调用此C++API的线程会阻塞等待，
     *   直到循环队列中有消息可取出，或等待被中断。
     * + 如果等待被中断，此C++API将返回 `-EINTR` 。
     */
    xwer_t rq(xwu8_t * data, xwsz_t * size) { return xwcq_rq(&mCq, data, size); }

    /**
     * @brief 限时等待从循环队列的 **尾端** 接收数据  (反向离队，Reversely deQueue)
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
     * @retval -EDISIRQ: 中断被关闭
     * @retval -EDISPMPT: 抢占被关闭
     * @retval -EDISBH: 中断底半部被关闭
     * @note
     * + 上下文：线程
     * @details
     * + 若循环队列中数据为空，调用此C++API的线程会阻塞等待，
     *   直到循环队列中有消息可取出，或等待被中断，或等待超时。
     * + 如果等待被中断，此C++API将返回 `-EINTR` 。
     * + `to` 表示等待超时的时间点：
     *   + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
     *     可以使用 `xwtm_ft(delta)` 表示；
     *   + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
     */
    xwer_t rq(xwu8_t * data, xwsz_t * size, xwtm_t to) {
        return xwcq_rq_to(&mCq, data, size, to);
    }

    /**
     * @brief 等待从循环队列的 **尾端** 接收数据 (反向离队，Reversely deQueue)，
     *        并且等待不可被中断
     * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
     * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
     * + (I) 作为输入时，表示接收缓冲区的大小
     * + (O) 作为输出时，返回实际接收的数据大小
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
     * + 若循环队列中数据为空，调用此C++API的线程会阻塞等待，
     *   直到循环队列中有消息可取出。
     * + 调用此C++API的线程的阻塞等待不可被中断。
     */
    xwer_t rqUnintr(xwu8_t * data, xwsz_t * size) {
        return xwcq_rq_unintr(&mCq, data, size);
    }

    /**
     * @brief 尝试从循环队列的 **尾端** 接收数据 (反向离队，Reversely deQueue)
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
     * + 若循环队列中为空，就立即返回 `-ENODATA` ，此C++API可在中断中使用。
     */
    xwer_t tryRq(xwu8_t * data, xwsz_t * size) { return xwcq_tryrq(&mCq, data, size); }

    /**
     * @brief 等待从循环队列 **头端** 拷贝数据 (Peek at the Front of Queue)
     * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
     * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
     * + (I) 作为输入时，表示接收缓冲区的大小
     * + (O) 作为输出时，返回实际接收的数据大小
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
     * + 拷贝数据不会取走数据，也不会释放数据槽，数据可被重复拷贝，也可继续被接收。
     * + 若循环队列中数据为空，调用此C++API的线程会阻塞等待，
     *   直到循环队列中有消息可取出，或等待被中断。
     * + 如果等待被中断，此C++API将返回 `-EINTR` 。
     */
    xwer_t pfq(xwu8_t * data, xwsz_t * size) {
        return xwcq_pfq(&mCq, data, size);
    }

    /**
     * @brief 限时等待从循环队列 **头端** 拷贝数据 (Peek at the Front of Queue)
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
     * @retval -EDISIRQ: 中断被关闭
     * @retval -EDISPMPT: 抢占被关闭
     * @retval -EDISBH: 中断底半部被关闭
     * @note
     * + 上下文：线程
     * @details
     * + 拷贝数据不会取走数据，也不会释放数据槽，数据可被重复拷贝，也可继续被接收。
     * + 若循环队列中数据为空，调用此C++API的线程会阻塞等待，
     *   直到循环队列中有消息可取出，或等待被中断，或等待超时。
     * + 如果等待被中断，此CAPI将返回 `-EINTR` 。
     * + `to` 表示等待超时的时间点：
     *   + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
     *     可以使用 `xwtm_ft(delta)` 表示；
     *   + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
     */
    xwer_t pfq(xwu8_t * data, xwsz_t * size, xwtm_t to) {
        return xwcq_pfq_to(&mCq, data, size, to);
    }

    /**
     * @brief 等待从循环队列 **头端** 拷贝数据 (Peek at the Front of Queue)，
     *        并且等待不可被中断
     * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
     * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
     * + (I) 作为输入时，表示接收缓冲区的大小
     * + (O) 作为输出时，返回实际接收的数据大小
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
     * + 拷贝数据不会取走数据，也不会释放数据槽，数据可被重复拷贝，也可继续被接收。
     * + 若循环队列中数据为空，调用此C++API的线程会阻塞等待，
     *   直到循环队列中有消息可取出。
     * + 调用此C++API的线程的阻塞等待不可被中断。
     */
    xwer_t pfqUnintr(xwu8_t * data, xwsz_t * size) {
        return xwcq_pfq_unintr(&mCq, data, size);
    }

    /**
     * @brief 尝试从循环队列 **头端** 拷贝数据 (Peek at the Front of Queue)
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
     * + 拷贝数据不会取走数据，也不会释放数据槽，数据可被重复拷贝，也可继续被接收。
     * + 若循环队列中为空，就立即返回 `-ENODATA` ，此CAPI可在中断中使用。
     */
    xwer_t tryPfq(xwu8_t * data, xwsz_t * size) {
        return xwcq_trypfq(&mCq, data, size);
    }

    /**
     * @brief 等待从循环队列 **尾端** 拷贝数据 (Peek at the Rear of Queue)
     * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
     * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
     * + (I) 作为输入时，表示接收缓冲区的大小
     * + (O) 作为输出时，返回实际接收的数据大小
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
     * + 拷贝数据不会取走数据，也不会释放数据槽，数据可被重复拷贝，也可继续被接收。
     * + 若循环队列中数据为空，调用此C++API的线程会阻塞等待，
     *   直到循环队列中有消息可取出，或等待被中断。
     * + 如果等待被中断，此C++API将返回 `-EINTR` 。
     */
    xwer_t prq(xwu8_t * data, xwsz_t * size) { return xwcq_prq(&mCq, data, size); }

    /**
     * @brief 限时等待从循环队列 **尾端** 拷贝数据 (Peek at the Rear of Queue)
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
     * @retval -EDISIRQ: 中断被关闭
     * @retval -EDISPMPT: 抢占被关闭
     * @retval -EDISBH: 中断底半部被关闭
     * @note
     * + 上下文：线程
     * @details
     * + 拷贝数据不会取走数据，也不会释放数据槽，数据可被重复拷贝，也可继续被接收。
     * + 若循环队列中数据为空，调用此C++API的线程会阻塞等待，
     *   直到循环队列中有消息可取出，或等待被中断，或等待超时。
     * + 如果等待被中断，此C++API将返回 `-EINTR` 。
     * + `to` 表示等待超时的时间点：
     *   + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
     *     可以使用 `xwtm_ft(delta)` 表示；
     *   + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
     */
    xwer_t prq(xwu8_t * data, xwsz_t * size, xwtm_t to) {
        return xwcq_prq_to(&mCq, data, size, to);
    }

    /**
     * @brief 等待从循环队列 **尾端** 拷贝数据 (Peek at the Rear of Queue)，
     *        并且等待不可被中断
     * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
     * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
     * + (I) 作为输入时，表示接收缓冲区的大小
     * + (O) 作为输出时，返回实际接收的数据大小
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
     * + 拷贝数据不会取走数据，也不会释放数据槽，数据可被重复拷贝，也可继续被接收。
     * + 若循环队列中数据为空，调用此C++API的线程会阻塞等待，
     *   直到循环队列中有消息可取出。
     * + 调用此C++API的线程的阻塞等待不可被中断。
     */
    xwer_t prqUnintr(xwu8_t * data, xwsz_t * size) {
        return xwcq_prq_unintr(&mCq, data, size);
    }

    /**
     * @brief 尝试从循环队列 **尾端** 拷贝数据 (Peek at the Rear of Queue)
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
     * + 拷贝数据不会取走数据，也不会释放数据槽，数据可被重复拷贝，也可继续被接收。
     * + 若循环队列中为空，就立即返回 `-ENODATA` ，此CAPI可在中断中使用。
     */
    xwer_t tryPrq(xwu8_t * data, xwsz_t * size) {
        return xwcq_tryprq(&mCq, data, size);
    }
};

/**
 * @} xwmd_isc_xwcq_cxx
 */

} // namespace xwisc

#endif /* xwmd/isc/xwcq/cxx/Xwcq.hxx */
