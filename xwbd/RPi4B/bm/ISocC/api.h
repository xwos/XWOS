/**
 * @file
 * @brief Board Module：ISocC：API
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > Licensed under the Apache License, Version 2.0 (the "License");
 * > you may not use this file except in compliance with the License.
 * > You may obtain a copy of the License at
 * >
 * >         http://www.apache.org/licenses/LICENSE-2.0
 * >
 * > Unless required by applicable law or agreed to in writing, software
 * > distributed under the License is distributed on an "AS IS" BASIS,
 * > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * > See the License for the specific language governing permissions and
 * > limitations under the License.
 */

#ifndef __bm_ISocC_api_h__
#define __bm_ISocC_api_h__

#include "board/std.h"

/**
 * @defgroup bm_ISocC SOC之间的通讯
 * @ingroup bm
 * @{
 */

/**
 * @brief ISocC API：初始化并启动协议层XWSSC
 * @note
 * + 上下文：线程
 */
xwer_t isocc_init(void);

/**
 * @brief ISocC API：停止协议层XWSSC
 * @note
 * + 上下文：线程
 */
xwer_t isocc_fini(void);

/**
 * @brief ISocC API: 通过协议层发送数据并等待发送结果
 * @param[in] data: 数据缓冲区的指针
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示数据长度
 * + (O) 作为输出时，返回实际发送的数据长度
 * @param[in] pri: 优先级
 * @param[in] port: 端口
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -E2BIG: 数据太长
 * @retval -ENODEV: 端口号超出范围
 * @retval -ENOBUFS: 帧槽被使用完
 * @retval -EPERM: XWSSC未启动
 * @note
 * + 同步/异步：同步
 * + 上下文：线程
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t isocc_tx(const xwu8_t data[], xwsz_t * size,
                    xwu8_t pri, xwu8_t port,
                    xwtm_t to);

/**
 * @brief ISocC API: 通过协议层接收消息，若接收队列为空，就限时等待
 * @param[out] rxbuf: 指向缓冲区的指针，此缓冲区用于接收消息
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示接收缓冲区的大小
 * + (O) 作为输出时，返回实际接收的消息大小
 * @param[in] port: 接收消息的端口
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENODEV: 端口号超出范围
 * @retval -EPERM: XWSSC未启动
 * @retval -ETIMEDOUT: 超时
 * @note
 * + 同步/异步：同步
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t isocc_rx(xwu8_t rxbuf[], xwsz_t * size, xwu8_t port, xwtm_t to);

/**
 * @brief XWSSC API: 尝试接收消息，若接收队列为空，立即返回错误码
 * @param[out] rxbuf: 指向缓冲区的指针，此缓冲区用于接收消息
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示接收缓冲区的大小
 * + (O) 作为输出时，返回实际接收的消息大小
 * @param[in] port: 接收消息的端口
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENODEV: 端口号超出范围
 * @retval -ENODATA: 接收队列为空
 * @retval -EPERM: XWSSC未启动
 * @note
 * + 同步/异步：同步
 * + 上下文：中断、中断底半部、线程
 */
xwer_t isocc_try_rx(xwu8_t rxbuf[], xwsz_t * size, xwu8_t port);

/**
 * @brief ISocC API: 测试协议层是否已经连接
 * @return 布尔值
 * @retval true: 已链接
 * @retval false: 未链接
 * @note
 * + 同步/异步：同步
 * + 上下文：中断、中断底半部、线程
 */
bool isocc_tst_connected(void);

/**
 * @} bm_ISocC
 */

#endif /* bm/ISocC/api.h */
