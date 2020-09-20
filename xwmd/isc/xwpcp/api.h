/**
 * @file
 * @brief 点对点通讯协议：应用接口层
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_isc_xwpcp_api_h__
#define __xwmd_isc_xwpcp_api_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWPCP_QOS(q)            ((xwu8_t)((q) << 7))
#define XWPCP_QOS_MSK           XWPCP_QOS(1U)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwpcp;
struct xwpcp_hwifal_operations;
struct xwpcp_frmslot;

/**
 * @brief 帧句柄
 */
typedef struct xwpcp_frmslot * xwpcp_fhdl_t;

enum xwpcp_msg_qos_em {
        XWPCP_MSG_QOS_0 = XWPCP_QOS(0), /**< 只传输一次，不管对方是否收到 */
        XWPCP_MSG_QOS_1 = XWPCP_QOS(1), /**< 传输完成后需要等待对方确认收到，
                                             未收到确认应答会重新传输 */
};

/**
 * @brief 异步通知回调函数
 */
typedef void (*xwpcp_ntf_f)(struct xwpcp * /* xwpcp */,
                            xwpcp_fhdl_t /* hdl */,
                            xwer_t /* rc */,
                            void * /* arg */);

/**
 * @brief XWPCP用户报文
 */
struct xwpcp_msg {
        xwu8_t port;
        xwu8_t qos;
        xwsz_t size;
        xwu8_t * text;
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWPCP API: 启动XWPCP
 * @param xwpcp: (I) XWPCP对象的指针
 * @param name: (I) XWPCP实例的名字
 * @param hwifops: (I) 硬件接口抽象层操作函数集合
 * @param hwifcb: (I) 硬件接口控制块指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval --ENOMEM: 内存池台小
 * @retval -EPERM: XWPCP未初始化
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 * - 重入性：不可重入
 */
__xwmd_api
xwer_t xwpcp_start(struct xwpcp * xwpcp, const char * name,
                   const struct xwpcp_hwifal_operations * hwifops,
                   void * hwifcb);

/**
 * @brief XWPCP API: 停止XWPCP
 * @param xwpcp: (I) XWPCP对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EPERM: XWPCP正在被使用中
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 * - 重入性：不可重入
 */
__xwmd_api
xwer_t xwpcp_stop(struct xwpcp * xwpcp);

/**
 * @brief XWPCP API: 在限定的时间内，将一条用户报文加入到XWPCP的发送队列中，
 *                   并等待发送结果
 * @param xwpcp: (I) XWPCP对象的指针
 * @param msg: (I) 描述用户报文的结构体指针
 * @param prio: (I) 用户报文的优先级
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -E2BIG: 数据太长
 * @retval -ENXIO: 端口0不允许发送用户数据
 * @retval -ENODEV: 端口号超出范围
 * @retval -EINVAL: qos错误
 * @retval -ENOBUFS: 帧槽被使用完
 * @retval -EPERM: XWPCP未启动
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwpcp_tx(struct xwpcp * xwpcp,
                const struct xwpcp_msg * msg, xwu8_t prio,
                xwtm_t * xwtm);

/**
 * @brief XWPCP API: 将一条用户报文加入到XWPCP的发送队列中
 * @param xwpcp: (I) XWPCP对象的指针
 * @param msg: (I) 描述用户报文的结构体指针
 * @param prio: (I) 用户报文的优先级
 * @param cbfunc: (I) 异步通知的回调函数
 * @param cbarg: (I) 调用异步通知回调函数时用户自定义的参数
 * @param fhdlbuf: (O) 指向缓冲区的指针，通过此缓冲区返回帧句柄
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -E2BIG: 数据太长
 * @retval -ENXIO: 端口0不允许发送用户数据
 * @retval -ENODEV: 端口号超出范围
 * @retval -EINVAL: qos错误
 * @retval -ENOBUFS: 帧槽被使用完
 * @retval -EPERM: XWPCP未启动
 * @note
 * - 此函数将用户报文加入到XWPCP的发送队列并注册回调函数后就返回。当用户报文被
 *   XWPCP的发送线程成功发送出去（接收到远程端应答）后，注册的回调函数会被调用。
 *   需要注意回调函数是在XWPCP的发送线程的线程上下文中执行，如果在此函数中使用了会
 *   长时间阻塞线程的函数，会导致XWPCP停止发送。
 * @note
 * - 同步/异步：异步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwpcp_eq(struct xwpcp * xwpcp,
                const struct xwpcp_msg * msg, xwu8_t prio,
                xwpcp_ntf_f cbfunc, void * cbarg,
                xwpcp_fhdl_t * fhdlbuf);

/**
 * @brief XWPCP API: 锁定XWPCP的通知锁
 * @param xwpcp: (I) XWPCP对象的指针
 * @note
 * - XWPCP的发送完一帧后是在通知锁内调用异步通知回调函数，锁定通知锁可防止XWPCP的发送
 *   线程调用异步通知回调函数。
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：不可重入
 */
__xwmd_api
void xwpcp_lock_ntflock(struct xwpcp * xwpcp);

/**
 * @brief XWPCP API: 解锁XWPCP的通知锁
 * @param xwpcp: (I) XWPCP对象的指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：不可重入
 */
__xwmd_api
void xwpcp_unlock_ntflock(struct xwpcp * xwpcp);

/**
 * @brief XWPCP API: 接收消息，若接收队列为空，就限时等待
 * @param xwpcp: (I) XWPCP对象的指针
 * @param msgbuf: (O) 指向缓冲区的指针，此缓冲区被用于接收消息
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENXIO: 端口0不允许发送用户数据
 * @retval -ENODEV: 端口号超出范围
 * @retval -EPERM: XWPCP未启动
 * @retval -ETIMEDOUT: 超时
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwpcp_rx(struct xwpcp * xwpcp, struct xwpcp_msg * msgbuf, xwtm_t * xwtm);

/**
 * @brief XWPCP API: 尝试接收消息，若接收队列为空，立即返回错误码
 * @param xwpcp: (I) XWPCP对象的指针
 * @param msgbuf: (O) 指向缓冲区的指针，此缓冲区被用于接收消息
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENXIO: 端口0不允许发送用户数据
 * @retval -ENODEV: 端口号超出范围
 * @retval -ENODATA: 接收队列为空
 * @retval -EPERM: XWPCP未启动
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwpcp_try_rx(struct xwpcp * xwpcp, struct xwpcp_msg * msgbuf);

#endif /* xwmd/isc/xwpcp/api.h */
