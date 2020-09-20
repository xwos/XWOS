/**
 * @file
 * @brief 精简的点对点通讯协议：应用接口层
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_isc_xwscp_api_h__
#define __xwmd_isc_xwscp_api_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwscp;
struct xwscp_hwifal_operations;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWSCP API: 启动XWSCP
 * @param xwscp: (I) XWSCP对象的指针
 * @param name: (I) XWSCP实例的名字
 * @param hwifops: (I) 数据链路层操作函数集合
 * @param hwifcb: (I) 硬件接口控制块指针
 * @return 错误码
 * @retval XWOK: 没有错误
 */
__xwmd_api
xwer_t xwscp_start(struct xwscp * xwscp, const char * name,
                   const struct xwscp_hwifal_operations * hwifops,
                   void * hwifcb);

/**
 * @brief XWSCP API: 停止XWSCP
 * @param xwscp: (I) XWSCP对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 */
__xwmd_api
xwer_t xwscp_stop(struct xwscp * xwscp);

/**
 * @brief XWSCP API: 连接远程端
 * @param xwscp: (I) XWSCP对象的指针
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOTCONN: 远程端无回应
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 */
__xwmd_api
xwer_t xwscp_connect(struct xwscp * xwscp, xwtm_t * xwtm);

/**
 * @brief XWSCP API: 发送一条报文，并在限定的时间等待回应
 * @param xwscp: (I) XWSCP对象的指针
 * @param msg: (I) 报文
 * @param size: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示报文的字节数
 *              (O) 作为输出时，返回实际发送的字节数
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -E2BIG: 数据太长
 * @retval -ENOTCONN: 远程端无回应
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 */
__xwmd_api
xwer_t xwscp_tx(struct xwscp * xwscp,
                const xwu8_t msg[], xwsz_t * size,
                xwtm_t * xwtm);

/**
 * @brief XWSCP API: 接收一条报文，若接收队列为空，就限时等待
 * @param xwscp: (I) XWSCP对象的指针
 * @param buf: (I) 接收报文数据的缓冲区指针
 * @param size: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示报文的字节数
 *              (O) 作为输出时，返回实际接收的字节数
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 */
__xwmd_api
xwer_t xwscp_rx(struct xwscp * xwscp,
                xwu8_t buf[], xwsz_t * size,
                xwtm_t * xwtm);

#endif /* xwmd/isc/xwscp/api.h */
