/**
 * @file
 * @brief SOC间点对点通讯协议：用户应用接口层
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_isc_xwssc_mif_h__
#define __xwmd_isc_xwssc_mif_h__

#include <xwos/standard.h>

/**
 * @ingroup xwmd_isc_xwssc_mif
 * @{
 */

#if (XWMDCFG_isc_xwssc_MEMBLK_SIZE & (XWMDCFG_isc_xwssc_MEMBLK_SIZE - 1))
#  error "XWMDCFG_isc_xwssc_MEMBLK_SIZE must be the power of 2!"
#endif
#if ((1 << XWMDCFG_isc_xwssc_MEMBLK_ODR) > 32768U)
#  error "power(2, XWMDCFG_isc_xwssc_MEMBLK_ODR) must be <= 32768!"
#endif
#if (XWMDCFG_isc_xwssc_PORT_NUM > 256)
#  error "XWSSC Only supports 256 ports (0 ~ 255)!"
#endif
#if (XWMDCFG_isc_xwssc_PRI_NUM > 64)
#  error "XWSSC Only supports 64 priorities!"
#endif

#define XWSSC_VERSION           ("1.0.0") /**< XWSSC的版本号字符串 */
#define XWSSC_VERSION_MAJOR     1U /**< XWSSC的大版本号 */
#define XWSSC_VERSION_MINOR     0U /**< XWSSC的小版本号 */
#define XWSSC_VERSION_REVISION  0U /**< XWSSC的修订版本号 */

#define XWSSC_PRI_NUM (XWMDCFG_isc_xwssc_PRI_NUM) /**< 优先级数量 */
#define XWSSC_INVALID_PRI (0xFFU) /**< 无效优先级 */
#define XWSSC_MAX_PRI (XWSSC_PRI_NUM - 1U) /**< XWSSC的大版本号 */
#define XWSSC_PORT_NUM (XWMDCFG_isc_xwssc_PORT_NUM) /**< 端口号 */
#define XWSSC_RETRY_PERIOD (XWMDCFG_isc_xwssc_RETRY_PERIOD) /**< 重试发送的周期 */
#define XWSSC_RETRY_NUM (XWMDCFG_isc_xwssc_RETRY_NUM) /**< 重试发送的次数 */
#define XWSSC_MEMBLK_SIZE (XWMDCFG_isc_xwssc_MEMBLK_SIZE) /**< 单位内存块的大小 */
#define XWSSC_MEMBLK_ODR (XWMDCFG_isc_xwssc_MEMBLK_ODR) /**< 内存块数量的阶，
                                                             阶是指2的n次方中的n */
#define XWSSC_MEMBLK_NUM (1U << XWSSC_MEMBLK_ODR) /**< 内存块的数量 */
#define XWSSC_MEMPOOL_SIZE (XWSSC_MEMBLK_SIZE * XWSSC_MEMBLK_NUM) /**< 内存池的总大小 */

/**
 * @brief 定义内存池
 * @param[in] name: 数值名
 */
// cppcheck-suppress [misra-c2012-20.7]
#define XWSSC_DEF_MEMPOOL(name) __xwcc_alignl1cache xwu8_t name[XWSSC_MEMPOOL_SIZE]

struct xwssc;
struct xwssc_hwifal_operation;
struct xwssc_carrier;

/**
 * @brief 发送句柄
 */
typedef struct xwssc_carrier * xwssc_txh_t;

/**
 * @brief 发送状态枚举
 */
enum xwssc_carrier_state_em {
        XWSSC_CRS_IDLE = 0U,
        XWSSC_CRS_READY = 1U,
        XWSSC_CRS_INPROGRESS = 2U,
        XWSSC_CRS_ABORT = 3U,
        XWSSC_CRS_FINISH = 4U,
};

/**
 * @brief 服务质量枚举
 */
enum xwssc_msg_qos_em {
        XWSSC_MSG_QOS_0 = 0U, /**< 不可靠消息，无CRC校验码 */
        XWSSC_MSG_QOS_1 = 1U, /**< 不可靠消息，有CRC校验码 */
        XWSSC_MSG_QOS_2 = 2U, /**< 可靠消息，无CRC校验码 */
        XWSSC_MSG_QOS_3 = 3U, /**< 可靠消息，有CRC校验码 */
        XWSSC_MSG_QOS_NUM, /**< QOS数量 */
        XWSSC_MSG_QOS_CHKSUM_MSK = (1U), /**< CHKSUM标志的掩码，BIT(0) */
        XWSSC_MSG_QOS_RELIABLE_MSK = (2U), /**< RELIABLE标志的掩码，BIT(1) */
        XWSSC_MSG_QOS_MSK = (XWSSC_MSG_QOS_CHKSUM_MSK |
                             XWSSC_MSG_QOS_RELIABLE_MSK), /**< 掩码，BIT(0) | BIT(1) */
};


/**
 * @brief 通知发送结果的回调函数
 */
typedef void (* xwssc_ntf_f)(struct xwssc * /* xwssc */,
                             xwssc_txh_t /* txh */,
                             xwer_t /* rc */,
                             void * /* arg */);

/**
 * @brief XWSSC API: 启动XWSSC
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] name: XWSSC实例的名字
 * @param[in] hwifops: 硬件接口抽象层操作函数集合
 * @param[in] hwifcb: 硬件接口控制块指针
 * @param[in] mem: 连续的内存块
 * @param[in] memsize: 连续的内存块大小，值必须为 @ref XWSSC_MEMPOOL_SIZE
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOMEM: 内存池太小
 * @retval -EPERM: XWSSC未初始化
 * @note
 * + 参数 `mem` 作为xwssc发送和接收缓冲区，用户可以使用 @ref XWSSC_DEF_MEMPOOL 定义，
 *   `XWSSC_DEF_MEMPOOL` 增加修饰符 `__xwcc_alignl1cache` ，可提高内存的访问的效率。
 * + 参数 `memsize` 作用是提醒用户 `mem` 的大小必须为 @ref XWSSC_MEMPOOL_SIZE ，
 *   API内部也会做检查。
 * @note
 * + 上下文：线程
 * + 重入性：不可重入
 */
xwer_t xwssc_start(struct xwssc * xwssc, const char * name,
                   const struct xwssc_hwifal_operation * hwifops, void * hwifcb,
                   xwu8_t * mem, xwsz_t memsize);

/**
 * @brief XWSSC API: 停止XWSSC
 * @param[in] xwssc: XWSSC对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：中断、中断底半部、线程
 * + 重入性：不可重入
 */
xwer_t xwssc_stop(struct xwssc * xwssc);

/**
 * @brief XWSSC API: 测试XWSSC是否已经连接
 * @param[in] xwssc: XWSSC对象的指针
 * @return 布尔值
 * @retval true: 已链接
 * @retval false: 未链接
 * @note
 * + 上下文：中断、中断底半部、线程
 */
bool xwssc_tst_connected(struct xwssc * xwssc);

/**
 * @brief XWSSC API: 将用户数据加入到XWSSC的发送队列中，并在限定的时间内等待发送结果
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] data: 数据缓冲区的指针
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示数据长度
 * + (O) 作为输出时，返回实际发送的数据长度
 * @param[in] pri: 用户数据的优先级
 * @param[in] port: 端口
 * @param[in] qos: 服务质量，取值范围： @ref xwssc_msg_qos_em
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -E2BIG: 数据太长
 * @retval -ENODEV: 端口号超出范围
 * @retval -EINVAL: qos错误
 * @retval -ENOBUFS: 帧槽被使用完
 * @retval -EPERM: XWSSC未启动
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwssc_tx(struct xwssc * xwssc,
                const xwu8_t data[], xwsz_t * size,
                xwu8_t pri, xwu8_t port, xwu8_t qos,
                xwtm_t to);

/**
 * @brief XWSSC API: 将一条用户数据加入到XWSSC的发送队列中
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] data: 数据缓冲区的指针
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示数据长度
 * + (O) 作为输出时，返回实际入队的数据长度
 * @param[in] pri: 优先级
 * @param[in] port: 端口
 * @param[in] qos: 服务质量
 * @param[in] ntfcb: 通知发送结果的回调函数
 * @param[in] cbarg: 调用回调函数时的用户数据
 * @param[out] txhbuf: 指向缓冲区的指针，通过此缓冲区返回发送句柄
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -E2BIG: 数据太长
 * @retval -ENODEV: 端口号超出范围
 * @retval -EINVAL: qos错误
 * @retval -ENOBUFS: 帧槽被使用完
 * @retval -EPERM: XWSSC未启动
 * @note
 * + 上下文：中断、中断底半部、线程
 * + 异步函数
 * @details
 * + 此函数将用户数据放如发送队列就返回。当用户数据被XWSSC的发送线程成功发送
 *   （接收到远程端应答），注册的回调函数会被调用；
 * + 回调函数在XWSSC的发送线程的线程上下文中执行，如果在此函数中使用了会
 *   长时间阻塞线程的函数，会导致XWSSC停止发送。
 * + `to` 表示等待超时的时间点：
 *   + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *     可以使用 `xwtm_ft(delta)` 表示；
 *   + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwssc_eq(struct xwssc * xwssc,
                const xwu8_t data[], xwsz_t * size,
                xwu8_t pri, xwu8_t port, xwu8_t qos,
                xwssc_ntf_f ntfcb, void * cbarg,
                xwssc_txh_t * txhbuf);

/**
 * @brief XWSSC API: 中断发送
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] txh: 发送句柄
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EACCES: 消息帧已经正在发送
 * @note
 * + 上下文：中断、中断底半部、线程
 * + 异步函数
 * @details
 * 如果消息已经被 XWSSC TX 线程选中，发送不可被中断；
 * 仅当消息还在就绪队列中，未被选中发送时才可中断。
 */
xwer_t xwssc_abort(struct xwssc * xwssc, xwssc_txh_t txh);

/**
 * @brief XWSSC API: 获取发送状态
 * @param[in] txh: 发送句柄
 * @return 发送状态，取值： @ref xwssc_carrier_state_em
 * @note
 * + 上下文：中断、中断底半部、线程
 */
xwsq_t xwssc_get_txstate(xwssc_txh_t txh);

/**
 * @brief XWSSC API: 接收消息，若接收队列为空，就限时等待
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] port: 接收消息的端口
 * @param[out] rxbuf: 指向缓冲区的指针，此缓冲区用于接收消息
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示接收缓冲区的大小
 * + (O) 作为输出时，返回实际接收的消息大小
 * @param[out] qos: 指向缓冲区的指针，此缓冲区用于返回消息的QoS，可为NULL表示不关心QoS
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENODEV: 端口号超出范围
 * @retval -EPERM: XWSSC未启动
 * @retval -ETIMEDOUT: 超时
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwssc_rx(struct xwssc * xwssc, xwu8_t port,
                xwu8_t rxbuf[], xwsz_t * size, xwu8_t * qos,
                xwtm_t to);

/**
 * @brief XWSSC API: 尝试接收消息，若接收队列为空，立即返回错误码
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] port: 接收消息的端口
 * @param[out] rxbuf: 指向缓冲区的指针，此缓冲区用于接收消息
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示接收缓冲区的大小
 * + (O) 作为输出时，返回实际接收的消息大小
 * @param[out] qos: 返回消息的QoS的缓冲区，可为 `NULL` 表示不关心QoS
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENODEV: 端口号超出范围
 * @retval -ENODATA: 接收队列为空
 * @retval -EPERM: XWSSC未启动
 * @note
 * + 上下文：中断、中断底半部、线程
 */
xwer_t xwssc_try_rx(struct xwssc * xwssc, xwu8_t port,
                    xwu8_t rxbuf[], xwsz_t * size, xwu8_t * qos);

/**
 * @} xwmd_isc_xwssc_mif
 */

#endif /* xwmd/isc/xwssc/mif.h */
