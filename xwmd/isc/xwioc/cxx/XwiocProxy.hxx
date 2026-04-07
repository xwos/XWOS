/**
 * @file
 * @brief xwmd::isc::xwioc::cxx::XwiocProxy
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_isc_xwioc_cxx_XwiocProxy_hxx__
#define __xwmd_isc_xwioc_cxx_XwiocProxy_hxx__

extern "C" {
#include "xwmd/isc/xwioc/mi.h"
}

namespace xwisc {

/**
 * @defgroup xwmd_isc_xwioc_cxx XWIOC C++ Proxy Lib
 * @ingroup xwmd_isc_xwioc
 *
 * @{
 */

/**
 * @brief XWIOC端口代理
 */
class XwiocProxy
{
  private:
    struct xwioc * mXwioc;

  public:
    explicit XwiocProxy(struct xwioc * xwioc) : mXwioc(xwioc) {}
    explicit XwiocProxy(XwiocProxy * other) : mXwioc(other->mXwioc) {}
    explicit XwiocProxy(XwiocProxy & other) : mXwioc(other.mXwioc) {}
    ~XwiocProxy() {}

    /**
     * @brief 获取循环队列内部对象指针
     */
    struct xwioc * get() { return mXwioc; }

    /**
     * @brief 设置发送队列的额外内存
     * @param[in] port: 端口
     * @param[in] origin: 额外内存地址
     * @param[in] size: 额外内存地址的大小
     * @note
     * + 上下文：任意
     */
    void setTxcqExtraMemory(xwu64_t port, void * origin, xwu64_t size) {
        xwioc_set_txcq_extra_memory(mXwioc, port, origin, size);
    }

    /**
     * @brief 设置接收队列的额外内存
     * @param[in] port: 端口
     * @param[in] origin: 额外内存地址
     * @param[in] size: 额外内存地址的大小
     * @note
     * + 上下文：任意
     */
    void setRxcqExtraMemory(xwu64_t port, void * origin, xwu64_t size) {
        xwioc_set_rxcq_extra_memory(mXwioc, port, origin, size);
    }

    /**
     * @brief 发送数据
     * @param[in] port: 端口
     * @param[in] sdu: 数据
     * @param[in,out] dlc: 指向缓冲区的指针，此缓冲区：
     * + (I) 作为输入时，表示待发送的数据大小
     * + (O) 作为输出时，返回实际发送的数据大小
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 空指针
     * @retval -ECHRNG: 端口超出范围
     * @retval -EMSGSIZE: 数据太长
     * @retval -ENOSPC: 没有足够的空间发送数据
     * @note
     * + 上下文：任意
     */
    xwer_t write(xwu64_t port, const xwu8_t sdu[], xwsz_t * dlc) {
        return xwioc_write(mXwioc, port, sdu, dlc);
    }

    /**
     * @brief 通知接收队列中有新的消息
     * @note
     * + 上下文：任意
     */
    void notify() {
        xwioc_notify(mXwioc);
    }

    /**
     * @brief 接收数据
     * @param[in] port: 端口
     * @param[in] sdu: 数据
     * @param[in,out] dlc: 指向缓冲区的指针，此缓冲区：
     * + (I) 作为输入时，表示缓冲区的大小
     * + (O) 作为输出时，返回实际接收的数据大小
     * @param[in] to: 期望唤醒的时间点
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 空指针
     * @retval -ECHRNG: 端口超出范围
     * @retval -EINTR: 等待被中断
     * @retval -ETIMEDOUT: 超时
     * @retval -ENODATA: 队列中没有数据
     * @retval -EPROTO: 协议错误（存在BUG）
     * @retval -ENOBUFS: 缓冲区不够接收数据
     * @note
     * + 上下文：线程
     * @details
     * + 如果等待被中断，此CAPI将返回 `-EINTR` 。
     * + `to` 表示等待超时的时间点：
     *   + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
     *     可以使用 `xwtm_ft(delta)` 表示；
     *   + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
     */
    xwer_t read(xwu64_t port, xwu8_t sdu[], xwsz_t * dlc, xwtm_t to) {
        return xwioc_read(mXwioc, port, sdu, dlc, to);
    }

    /**
     * @brief 接收数据
     * @param[in] port: 端口
     * @param[in] sdu: 数据
     * @param[in,out] dlc: 指向缓冲区的指针，此缓冲区：
     * + (I) 作为输入时，表示缓冲区的大小
     * + (O) 作为输出时，返回实际接收的数据大小
     * @return 错误码
     * @retval XWOK: 没有错误
     * @retval -EFAULT: 空指针
     * @retval -ECHRNG: 端口超出范围
     * @retval -EINTR: 等待被中断
     * @retval -ENODATA: 队列中没有数据
     * @retval -EPROTO: 协议错误（存在BUG）
     * @retval -ENOBUFS: 缓冲区不够接收数据
     * @note
     * + 上下文：线程
     * @details
     * + 如果等待被中断，此CAPI将返回 `-EINTR` 。
     */
    xwer_t read(xwu64_t port, xwu8_t sdu[], xwsz_t * dlc) {
        return xwioc_read(mXwioc, port, sdu, dlc, XWTM_MAX);
    }
};

/**
 * @} xwmd_isc_xwioc_cxx
 */

} // namespace xwisc

#endif /* xwmd/isc/xwioc/cxx/XwiocProxy.hxx */
