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
class Xwcq
{
  private:
    struct xwcq * mProxy;
    xwu8_t mPort; /**< 端口 */
    xwu8_t mDefaultPriority; /**< 默认发送优先级 */
    xwu8_t mDefaultQos; /**< 默认Qos */

  public:
    Xwcq(struct xwcq * xwcq, xwu8_t port,
                   xwu8_t default_priority, xwu8_t default_qos);
    Xwcq() = delete;
    ~Xwcq() {}

    /**
     * @brief 发送数据，并同步等待发送结果
     * @param[in] data: 数据缓冲区的指针
     * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
     * + (I) 作为输入时，表示数据长度
     * + (O) 作为输出时，返回实际发送的数据长度
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 空指针
     * @retval -E2BIG: 数据太长
     * @retval -ENODEV: 端口号超出范围
     * @retval -EINVAL: qos错误
     * @retval -ENOBUFS: 帧槽被使用完
     * @retval -EPERM: XWCQ未启动
     * @note
     * + 上下文：线程
     */
    xwer_t write(const xwu8_t data[], xwsz_t * size);

    /**
     * @brief 发送数据，并同步等待发送结果
     * @param[in] data: 数据缓冲区的指针
     * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
     * + (I) 作为输入时，表示数据长度
     * + (O) 作为输出时，返回实际发送的数据长度
     * @param[in] pri: 用户数据的优先级
     * @param[in] qos: 服务质量，取值范围： @ref xwcq_msg_qos_em
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 空指针
     * @retval -E2BIG: 数据太长
     * @retval -ENODEV: 端口号超出范围
     * @retval -EINVAL: qos错误
     * @retval -ENOBUFS: 帧槽被使用完
     * @retval -EPERM: XWCQ未启动
     * @note
     * + 上下文：线程
     */
    xwer_t write(const xwu8_t data[], xwsz_t * size, xwu8_t pri, xwu8_t qos);

    /**
     * @brief 发送数据，并等待发送结果
     * @param[in] data: 数据缓冲区的指针
     * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
     * + (I) 作为输入时，表示数据长度
     * + (O) 作为输出时，返回实际发送的数据长度
     * @param[in] to: 期望唤醒的时间点
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 空指针
     * @retval -E2BIG: 数据太长
     * @retval -ENODEV: 端口号超出范围
     * @retval -EINVAL: qos错误
     * @retval -ENOBUFS: 帧槽被使用完
     * @retval -EPERM: XWCQ未启动
     * @note
     * + 上下文：线程
     * @details
     * `to` 表示等待超时的时间点：
     * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
     *   可以使用 `xwtm_ft(delta)` 表示；
     * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
     */
    xwer_t write(const xwu8_t data[], xwsz_t * size, xwtm_t to);

    /**
     * @brief 发送数据，并在限定的时间内等待发送结果
     * @param[in] data: 数据缓冲区的指针
     * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
     * + (I) 作为输入时，表示数据长度
     * + (O) 作为输出时，返回实际发送的数据长度
     * @param[in] pri: 用户数据的优先级
     * @param[in] qos: 服务质量，取值范围： @ref xwcq_msg_qos_em
     * @param[in] to: 期望唤醒的时间点
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 空指针
     * @retval -E2BIG: 数据太长
     * @retval -ENODEV: 端口号超出范围
     * @retval -EINVAL: qos错误
     * @retval -ENOBUFS: 帧槽被使用完
     * @retval -EPERM: XWCQ未启动
     * @note
     * + 上下文：线程
     * @details
     * `to` 表示等待超时的时间点：
     * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
     *   可以使用 `xwtm_ft(delta)` 表示；
     * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
     */
    xwer_t write(const xwu8_t data[], xwsz_t * size, xwu8_t pri, xwu8_t qos, xwtm_t to);

    /**
     * @brief 异步发送数据
     * @param[in] data: 数据缓冲区的指针
     * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
     * + (I) 作为输入时，表示数据长度
     * + (O) 作为输出时，返回实际缓存的数据长度
     * @param[out] txhbuf: 指向缓冲区的指针，通过此缓冲区返回发送句柄
     * @details
     * 发送结果通过虚函数 `onAsyncWrited()` 进行通知。用户需要继承本类，
     * 重新定义此虚函数来获取发送结果。
     */
    xwer_t asyncWrite(const xwu8_t data[], xwsz_t * size, xwcq_txh_t * txhbuf);

    /**
     * @brief 异步发送数据
     * @param[in] data: 数据缓冲区的指针
     * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
     * + (I) 作为输入时，表示数据长度
     * + (O) 作为输出时，返回实际缓存的数据长度
     * @param[in] pri: 用户数据的优先级
     * @param[in] qos: 服务质量，取值范围： @ref xwcq_msg_qos_em
     * @param[out] txhbuf: 指向缓冲区的指针，通过此缓冲区返回发送句柄
     * @details
     * 发送结果通过虚函数 `onAsyncWrited()` 进行通知。用户需要继承本类，
     * 重新定义此虚函数来获取发送结果。
     */
    xwer_t asyncWrite(const xwu8_t data[], xwsz_t * size,
                      xwu8_t pri, xwu8_t qos, xwcq_txh_t * txhbuf);

    /**
     * @brief 中断异步发送
     * @param[in] txh: 发送句柄
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 空指针
     * @retval -EACCES: 消息帧已经正在发送
     * @note
     * + 上下文：中断、中断底半部、线程
     * + 异步函数
     * @details
     * 如果消息已经被 XWCQ TX 线程选中，发送不可被中断；
     * 仅当消息还在就绪队列中，未被选中发送时才可中断。
     */
    xwer_t abortAsyncWrite(xwcq_txh_t txh);

    /**
     * @brief 获取异步发送状态
     * @param[in] txh: 发送句柄
     * @return 发送状态，取值： @ref xwcq_carrier_state_em
     * @note
     * + 上下文：中断、中断底半部、线程
     */
    enum xwcq_carrier_state_em getAsyncWriteState(xwcq_txh_t txh) {
        return (enum xwcq_carrier_state_em)xwcq_get_txstate(txh);
    }

    /**
     * @brief 接收数据，若数据为空，就无限等待
     * @param[out] rxbuf: 指向缓冲区的指针，此缓冲区用于接收消息
     * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
     * + (I) 作为输入时，表示接收缓冲区的大小
     * + (O) 作为输出时，返回实际接收的消息大小
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 空指针
     * @retval -ENODEV: 端口号超出范围
     * @retval -EPERM: XWCQ未启动
     * @retval -ETIMEDOUT: 超时
     * @note
     * + 上下文：线程
     */
    xwer_t read(xwu8_t rxbuf[], xwsz_t * size);

    /**
     * @brief 接收数据，若数据为空，就限时等待
     * @param[out] rxbuf: 指向缓冲区的指针，此缓冲区用于接收消息
     * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
     * + (I) 作为输入时，表示接收缓冲区的大小
     * + (O) 作为输出时，返回实际接收的消息大小
     * @param[out] qos: 返回消息的QoS的缓冲区，可为 `nullptr` 表示不关心QoS
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 空指针
     * @retval -ENODEV: 端口号超出范围
     * @retval -EPERM: XWCQ未启动
     * @retval -ETIMEDOUT: 超时
     * @note
     * + 上下文：线程
     * @details
     * `to` 表示等待超时的时间点：
     * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
     *   可以使用 `xwtm_ft(delta)` 表示；
     * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
     */
    xwer_t read(xwu8_t rxbuf[], xwsz_t * size, xwu8_t * qos, xwtm_t to);

    /**
     * @brief 尝试接收消息，若接收队列为空，立即返回错误码
     * @param[out] rxbuf: 指向缓冲区的指针，此缓冲区用于接收消息
     * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
     * + (I) 作为输入时，表示接收缓冲区的大小
     * + (O) 作为输出时，返回实际接收的消息大小
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 空指针
     * @retval -ENODEV: 端口号超出范围
     * @retval -ENODATA: 接收队列为空
     * @retval -EPERM: XWCQ未启动
     * @note
     * + 上下文：中断、中断底半部、线程
     */
    xwer_t tryRead(xwu8_t rxbuf[], xwsz_t * size);

    /**
     * @brief 尝试接收消息，若接收队列为空，立即返回错误码
     * @param[out] rxbuf: 指向缓冲区的指针，此缓冲区用于接收消息
     * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
     * + (I) 作为输入时，表示接收缓冲区的大小
     * + (O) 作为输出时，返回实际接收的消息大小
     * @param[out] qos: 返回消息的QoS的缓冲区，可为 `nullptr` 表示不关心QoS
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 空指针
     * @retval -ENODEV: 端口号超出范围
     * @retval -ENODATA: 接收队列为空
     * @retval -EPERM: XWCQ未启动
     * @note
     * + 上下文：中断、中断底半部、线程
     */
    xwer_t tryRead(xwu8_t rxbuf[], xwsz_t * size, xwu8_t * qos);

  protected:
    virtual void onAsyncWrited(xwcq_txh_t txh, xwer_t rc);


  private:
    static void sOnAsyncWrited(struct xwcq * xwcq, xwcq_txh_t txh,
                               xwer_t rc, void * arg);
};

/**
 * @} xwmd_isc_xwcq_cxx
 */

} // namespace xwisc

#endif /* xwmd/isc/xwcq/cxx/Xwcq.hxx */
