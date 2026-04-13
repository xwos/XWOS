/**
 * @file
 * @brief xwmd::isc::xwioc 模块集成接口
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_isc_xwioc_mi_h__
#define __xwmd_isc_xwioc_mi_h__

#include <xwos/standard.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/sem.h>

/**
 * @defgroup xwmd_isc_xwioc 操作系统间通讯协议
 * @ingroup xwmd_isc
 *
 * 当SOC中同时跑两个OS，其中一些CPU跑XWOS，另一些CPU跑Linux Kernel，
 * 两个OS间可通过xwioc进行通讯。
 *
 * xwioc是为ARM64的多核SOC设计，Linux端需要安装xwioc的驱动。
 *
 *
 * ## 工作流程
 *
 * 在OS间的共享内存区域，构建 **循环队列** 。通讯时将数据写入 **循环队列** ，
 * 然后通过CPU核间软中断通知运行在不同CPU上的不同OS取出数据。
 *
 *
 * ## 软中断
 *
 * 目前使用ARM64的SGI7，需要修改Linux Kernel源码 `arch/arm64/kernel/smp.c` 增加新IPI。
 *
 *
 * ## 循环队列
 *
 *
 * ```
 * |________++++++++++++++++++++++++++++++++++++++++++++++++________|........|
 *          ^                                               ^
 *          pos                                             tail
 *
 *
 * |++++++++________________________________________________++++++++|........|
 *          ^                                               ^
 *          tail                                            pos
 * ```
 *
 * + 符号含义
 *   + `+` : 包含有效数据的内存块
 *   + `_` : 空内存块
 *   + `.` : 隐藏内存，用于防止数据抵达内存边界时“转弯”
 * + 循环队列的Block大小为8字节，当数据大小不能整除以8时，最后一块Block写不满8字节。
 *
 * ## 协议
 *
 * + Head
 *   + sof: 帧首定界符, 0xAA5555AA
 *   + size: 数据大小
 * + sdu: 变长数组，大小由Head.size确定
 *
 * ## 额外内存
 *
 * 额外内存主要用于大数据通讯，通常在Linux端配合mmap使用。
 * 可通过 @ref xwioc_set_txcq_extra_memory() 和
 * @ref xwioc_set_rxcq_extra_memory() 设置
 *
 * @{
 */

#define XWIOC_PROTOCOL_HEAD_SOF         ((xwu32_t)0xAA5555AA)
#define XWIOC_PROTOCOL_HEAD_SOF_SIZE    (4UL)
#define XWIOC_BLOCK_SIZE                (8UL)
#define XWIOC_SHADOW_SIZE               (4096UL)
#define XWIOC_SOCKET_TX_POS             (1UL)
#define XWIOC_SOCKET_RX_POS             (2UL)
#define XWIOC_SOCKET_CONNECTED          ((XWIOC_SOCKET_TX_POS) | (XWIOC_SOCKET_RX_POS))

#if (XWMDCFG_isc_xwioc_PORT_NUM >= 64UL)
#  error "XWMDCFG_isc_xwioc_PORT_NUM must be less than 64!"
#endif

#if ((XWIOC_SHADOW_SIZE & (XWIOC_SHADOW_SIZE -1UL)) != 0UL)
#  error "XWIOC_SHADOW_SIZE must be power of 2!"
#endif

#if ((XWIOC_SHADOW_SIZE & (XWIOC_SHADOW_SIZE -1UL)) != 0UL)
#  error "XWIOC_SHADOW_SIZE must be power of 2!"
#endif

/**
 * @brief 通讯协议头
 */
struct xwioc_cq_protocol_head {
        xwu32_t sof; /**< 帧首定界符, 0xAA5555AA */
        xwu32_t dlc; /**< sdu的大小 */
};

/**
 * @brief 通讯协议
 */
struct xwioc_cq_protocol {
        struct xwioc_cq_protocol_head head; /**< 通讯协议头 */
        xwu8_t sdu[0]; /**< 变长数组，大小由 `head.size` 确定*/
};

/**
 * @brief 循环队列内存块
 */
typedef xwu8_t xwioc_block_t[XWIOC_BLOCK_SIZE];

/**
 * @brief 循环队列
 */
struct xwioc_cq {
        struct xwos_splk splk;
        xwu32_t port_num;
        xwu64_t portmap;
        struct {
                struct {
                        xwu64_t origin;
                        xwu64_t size;
                } extra_memory;
                struct {
                        xwu64_t size; /**< 内存大小 */
                        xwu64_t shadow; /**< 隐藏内存大小 */
                        xwu64_t num; /**< 内存块数量 */
                        xwu64_t pos; /**< 当前有效数据的起始位置 */
                        xwu64_t tail; /**< 当前有效数据的结束位置 */
                        xwu64_t m; /**< 内存地址偏移 */
                } cq; /**< 循环队列 */
                xwu64_t connection; /**< 通道连接标志 */
        } port[XWMDCFG_isc_xwioc_PORT_NUM];
};

struct xwioc;

/**
 * @brief 向Linux产生SGI的函数
 */
typedef void (* xwioc_sgi_f)(void);

/**
 * @brief 操作系统间通讯协议对象
 * @details
 * + 每个OS中需要定义至少一个struct xwioc实例，struct xwioc实例是OS的私有数据，
 *   其中包含共享内存的地址信息。
 * + 不同OS中的struct xwioc实例，txcq与rxcq需要交叉链接。
 *   即，一个OS中的txcq与另一个OS中的rxcq在共享内存中地址相同。
 */
struct xwioc {
        xwu64_t shm_origin; /**< 共享内存的起始地址 */
        xwu64_t cq_mr_offset; /**< 循环队列在共享内存中，基于起始地址的偏移 */
        xwu64_t cq_mr_size; /**< 循环队列区域的大小 */
        xwu64_t block_mr_offset; /**< 内存块在共享内存中，基于起始地址的偏移 */
        xwu64_t block_mr_size; /**< 内存块区域的大小 */
        xwioc_sgi_f sgi; /**< 向Linux产生SGI的函数 */
        struct xwioc_cq * txcq; /**< 指向共享内存中的发送队列 */
        struct xwioc_cq * rxcq; /**< 指向共享内存中的接收队列 */
        struct xwos_sem rxsem[XWMDCFG_isc_xwioc_PORT_NUM]; /**< 接收队列的信号量 */
};

/**
 * @brief 初始化操作系统间通讯协议对象
 * @param[in] xwioc: 对象指针
 * @param[in] shm_origin: 共享内存的起始地址
 * @param[in] cq_mr_origin: 循环队列在共享内存中，基于起始地址的偏移
 * @param[in] cq_mr_size: 循环队列区域的大小
 * @param[in] block_mr_origin: 内存块在共享内存中，基于起始地址的偏移
 * @param[in] block_mr_size: 内存块区域的大小
 * @param[in] order: 发送队列与接收队列在共享内存中的排列顺序，
 * 两个OS之间需要使用不同顺序，使得发送队列与接收队列交叉链接
 * + `0` 发送队列在前，接收队列在后
 * + `1` 接收队列在前，发送队列在后
 * @param[in] sgi: 向Linux产生SGI的函数
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENOMEM: 共享内存空间不足
 * @note
 * + 上下文：任意
 */
xwer_t xwioc_init(struct xwioc * xwioc, xwu64_t shm_origin,
                  xwu64_t cq_mr_offset, xwu64_t cq_mr_size,
                  xwu64_t block_mr_offset, xwu64_t block_mr_size,
                  xwu64_t order, xwioc_sgi_f sgi);

/**
 * @brief 设置发送队列的额外内存
 * @param[in] xwioc: 对象指针
 * @param[in] port: 端口
 * @param[in] origin: 额外内存地址
 * @param[in] size: 额外内存地址的大小
 * @note
 * + 上下文：任意
 */
void xwioc_set_txcq_extra_memory(struct xwioc * xwioc, xwu64_t port,
                                 void * origin, xwu64_t size);

/**
 * @brief 设置接收队列的额外内存
 * @param[in] xwioc: 对象指针
 * @param[in] port: 端口
 * @param[in] origin: 额外内存地址
 * @param[in] size: 额外内存地址的大小
 * @note
 * + 上下文：任意
 */
void xwioc_set_rxcq_extra_memory(struct xwioc * xwioc, xwu64_t port,
                                 void * origin, xwu64_t size);

/**
 * @brief 连接端口
 * @param[in] xwioc: 对象指针
 * @param[in] port: 端口
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ECHRNG: 端口超出范围
 * @note
 * + 上下文：任意
 */
xwer_t xwioc_connect(struct xwioc * xwioc, xwu64_t port);

/**
 * @brief 断开端口
 * @param[in] xwioc: 对象指针
 * @param[in] port: 端口
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ECHRNG: 端口超出范围
 * @note
 * + 上下文：任意
 */
xwer_t xwioc_disconnect(struct xwioc * xwioc, xwu64_t port);

/**
 * @brief 测试发送队列的连接状态
 * @param[in] xwioc: 对象指针
 * @param[in] port: 端口
 * @return 布尔值
 * @retval true: 已连接
 * @retval false: 连接断开
 * @note
 * + 上下文：任意
 */
bool xwioc_tst_txcq_connected(struct xwioc * xwioc, xwu64_t port);

/**
 * @brief 测试接收队列的连接状态
 * @param[in] xwioc: 对象指针
 * @param[in] port: 端口
 * @return 布尔值
 * @retval true: 已连接
 * @retval false: 连接断开
 * @note
 * + 上下文：任意
 */
bool xwioc_tst_rxcq_connected(struct xwioc * xwioc, xwu64_t port);

/**
 * @brief 发送数据
 * @param[in] xwioc: 对象指针
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
xwer_t xwioc_write(struct xwioc * xwioc, xwu64_t port,
                   const xwu8_t sdu[], xwsz_t * dlc);

/**
 * @brief 通知接收队列中有新的消息
 * @param[in] xwioc: 对象指针
 * @note
 * + 上下文：任意
 */
void xwioc_notify(struct xwioc * xwioc);

/**
 * @brief 接收数据
 * @param[in] xwioc: 对象指针
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
xwer_t xwioc_read(struct xwioc * xwioc, xwu64_t port,
                  xwu8_t sdu[], xwsz_t * dlc, xwtm_t to);

/**
 * @} xwmd_isc_xwioc
 */

#endif /* xwmd/isc/xwioc/mi.h */
