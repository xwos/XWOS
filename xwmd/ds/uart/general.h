/**
 * @file
 * @brief xwmd设备栈：UART
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_ds_uart_general_h__
#define __xwmd_ds_uart_general_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwmd/ds/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/osal/lock/seqlock.h>
#include <xwos/osal/lock/mutex.h>
#include <xwos/osal/sync/semaphore.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/uart/common.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwds_uartc;

/**
 * @brief UART控制器驱动层操作接口
 */
struct xwds_uartc_driver {
        struct xwds_driver base; /**< C语言面向对象：继承struct xwds_driver */
        xwer_t (* cfg)(struct xwds_uartc *,
                       const struct xwds_uart_cfg *); /**< 配置 */
        xwer_t (* tx)(struct xwds_uartc *, xwu8_t); /**< 发送一个字节 */
        xwer_t (* rx)(struct xwds_uartc *, xwu8_t *); /**< 接收一个字节 */
};

/**
 * @brief UART控制器
 */
struct xwds_uartc {
        struct xwds_device dev; /**< 继承struct xwds_device */

        /* attributes */
        const struct xwds_uart_cfg * cfg; /**< uart配置 */

        /* private */
        /* rx */
        struct xwosal_smr rxsmr; /**< 接收队列的数据信号量 */
        struct xwosal_sqlk rxseqlock; /**< 保护接收队列的锁 */
        xwu8_t rxq[XWMDCFG_ds_UART_GNR_RXQ_SIZE]; /**< 接收队列（循环数组） */
        xwssq_t rxpos; /**< 当前可以接收数据的位置 */
        xwssz_t rxnum; /**< 缓冲区中有效数据的数量 */

        /* tx */
        struct xwosal_mtx txmtx; /**< 多线程安全的互斥锁 */
        xwu32_t txport_available; /**< 硬件发送端口可用标志 */
        struct xwosal_sqlk txseqlock; /**< 保护发送队列的锁 */
        xwu8_t txq[XWMDCFG_ds_UART_GNR_TXQ_SIZE]; /**< 发送队列（循环数组）*/
        xwssq_t txpos; /**< 当前可以加入数据的位置 */
        xwssz_t txnum; /**< 缓冲区中待发送的数据的数量 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** constructor & destructor ******** ******** ********/
__xwds_api
void xwds_uartc_construct(struct xwds_uartc * uartc);

__xwds_api
void xwds_uartc_destruct(struct xwds_uartc * uartc);

/******** ******** ******** APIs ******** ******** ********/
/**
 * @brief XWDS API：清空接收队列
 * @param uartc: (I) UART控制器对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_uartc_clear_rxq(struct xwds_uartc * uartc);

/**
 * @brief XWDS API：获取接收队列有效数据的大小
 * @param uartc: (I) UART控制器对象指针
 * @param ret: (O) 指向缓冲区的指针，通过此缓冲区返回接收队列中有效数据的大小
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_uartc_get_rxq_datasize(struct xwds_uartc * uartc, xwsz_t *ret);

/**
 * @brief XWDS API：从接收队列中获取一个字节的数据
 * @param uartc: (I) UART控制器对象指针
 * @param buf: (O) 指向缓冲区的指针，此缓冲区被用于返回数据
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_uartc_getc(struct xwds_uartc * uartc,
                      xwu8_t * buf, xwtm_t * xwtm);

/**
 * @brief XWDS API：尝试从接收队列中获取一个字节的数据
 * @param uartc: (I) UART控制器对象指针
 * @param buf: (O) 指向缓冲区的指针，此缓冲区被用于返回数据
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_uartc_try_getc(struct xwds_uartc * uartc, xwu8_t * buf);

/**
 * @brief XWDS API：从接收队列中获取多个数据
 * @param uartc: (I) UART控制器对象指针
 * @param buf: (O) 指向缓冲区的指针，此缓冲区被用于返回数据
 * @param size: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示缓冲区大小（单位：字节）
 *              (O) 作为输出时，返回实际读取的数据大小
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_uartc_rx(struct xwds_uartc * uartc,
                     void * buf, xwsz_t * size,
                     xwtm_t * xwtm);

/**
 * @brief XWDS API：尝试从接收队列中获取多个字节的数据
 * @param uartc: (I) UART控制器对象指针
 * @param buf: (O) 指向缓冲区的指针，此缓冲区被用于返回数据
 * @param size: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示缓冲区大小（单位：字节）
 *              (O) 作为输出时，返回实际读取的数据大小
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_uartc_try_rx(struct xwds_uartc * uartc, void * buf, xwsz_t * size);

/**
 * @brief XWDS API：清空发送队列
 * @param uartc: (I) UART控制器对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_uartc_clear_txq(struct xwds_uartc * uartc);

/**
 * @brief XWDS API：发送一个字节
 * @param uartc: (I) UART控制器对象指针
 * @param byte: (I) 待发送的数据
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -EOVERFLOW: 发送队列已满
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_uartc_putc(struct xwds_uartc * uartc,
                       const xwu8_t byte,
                       xwtm_t * xwtm);

/**
 * @brief XWDS API：发送一些数据
 * @param uartc: (I) UART控制器对象指针
 * @param data: (I) 待发送的数据的缓冲区
 * @param size: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示待发送的数据的大小
 *              (O) 作为输出时，返回实际放入发送队列的数据的大小
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_uartc_tx(struct xwds_uartc * uartc,
                     const xwu8_t * data, xwsz_t * size,
                     xwtm_t * xwtm);

/**
 * @brief XWDS API：配置UART
 * @param uartc: (I) UART控制器对象指针
 * @param cfg: (I) 新的配置
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_uartc_cfg(struct xwds_uartc * uartc,
                      const struct xwds_uart_cfg * cfg);

/******** ******** Libraries for BSP driver ******** ********/
__xwds_lib_code
void xwds_uartc_lib_rx_isr(struct xwds_uartc * uartc);

__xwds_lib_code
void xwds_uartc_lib_tx_isr(struct xwds_uartc * uartc);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********   inline function implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 增加对象的引用计数
 * @param uartc: (I) UART控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_grab()
 */
static __xwds_inline
xwer_t xwds_uartc_grab(struct xwds_uartc * uartc)
{
        return xwds_device_grab(&uartc->dev);
}

/**
 * @brief 减少对象的引用计数
 * @param uartc: (I) UART控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_put()
 */
static __xwds_inline
xwer_t xwds_uartc_put(struct xwds_uartc * uartc)
{
        return xwds_device_put(&uartc->dev);
}

/**
 * @brief 增加设备运行状态计数器
 * @param uartc: (I) UART控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_request()
 */
static __xwds_inline
xwer_t xwds_uartc_request(struct xwds_uartc * uartc)
{
        return xwds_device_request(&uartc->dev);
}

/**
 * @brief 减少设备运行状态计数器
 * @param uartc: (I) UART控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_release()
 */
static __xwds_inline
xwer_t xwds_uartc_release(struct xwds_uartc * uartc)
{
        return xwds_device_release(&uartc->dev);
}

#endif /* xwmd/ds/uart/general.h */
