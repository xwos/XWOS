/**
 * @file
 * @brief 玄武设备栈：UART控制器
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

#ifndef __xwcd_ds_uart_controller_h__
#define __xwcd_ds_uart_controller_h__

#include <xwcd/ds/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/sem.h>
#include <xwos/osal/lock/mtx.h>
#include <xwcd/ds/device.h>
#include <xwcd/ds/soc/dma.h>

/**
 * @defgroup xwcd_ds_uart 串口控制器
 * @ingroup xwcd_ds
 * ## 接收缓冲区
 * XWOS设备栈串口控制器框架内定义了大小为 `XWCDCFG_ds_UART_RXQ_SIZE`
 * 的 **接收缓冲区** ， `rxq.idx` 用于向 **接收缓冲区** 内填充数据，
 * `rxq.pos` 和 `rxq.tail` 用于向 **接收缓冲区** 获取数据。
 *
 * **接收缓冲区** 是一个循环队列，当 `overflow` 时，最新的数据会将最旧的数据覆盖掉。
 *
 * ### **中断模式** 中使用接收缓冲区
 *
 * 当使用 **中断模式** 的UART硬件控制器时，需在中断函数内
 * 调用 `xwds_uartc_drvcb_rxq_fill()` 填充 **接收缓冲区** ，
 * 然后调用 `xwds_uartc_drvcb_rxq_pub()` 发布数据。
 *
 * 如果UART硬件控制器支持FIFO，需要使用一个 **接收超时定时器** 定期将FIFO内数据填充至
 * **接收缓冲区** 并发布数据。
 *
 * 出现错误时，需要使用 `xwds_uartc_drvcb_rxq_flush()` 重置 **接收缓冲区** 。
 *
 * ### **DMA模式** 中使用接收缓冲区
 *
 * 当使用 **DMA模式** 的UART硬件控制器时，需将DMA的接收地址
 * 设置为 **接收缓冲区** 的首地址，DMA引擎会自动填充 **接收缓冲区** ，
 * DMA完成中断触发时，需要调用 `xwds_uartc_drvcb_rxq_pub()` 发布数据。
 *
 * 此外需要使用一个 **接收超时定时器** 定期发布数据。
 *
 * 出现错误时，需要使用 `xwds_uartc_drvcb_rxq_flush()` 重置 **接收缓冲区** 。
 *
 * @{
 */

#define XWDS_UART_RXQ_SIZE XWCDCFG_ds_UART_RXQ_SIZE

/**
 * @brief UART字长枚举
 */
enum xwds_uart_bits_em {
        XWDS_UART_BITS_5 = 0U,
        XWDS_UART_BITS_6,
        XWDS_UART_BITS_7,
        XWDS_UART_BITS_8,
        XWDS_UART_BITS_9,
};

/**
 * @brief UART停止位枚举
 */
enum xwds_uart_stopbits_em {
        XWDS_UART_STOPBITS_0_5 = 0U,
        XWDS_UART_STOPBITS_1_0,
        XWDS_UART_STOPBITS_1_5,
        XWDS_UART_STOPBITS_2_0,
};

/**
 * @brief UART奇偶校验枚举
 */
enum xwds_uart_parity_em {
        XWDS_UART_PARITY_NONE = 0U,
        XWDS_UART_PARITY_EVEN,
        XWDS_UART_PARITY_ODD,
};

/**
 * @brief UART硬件流控枚举
 */
enum xwds_uart_hardwareflowcontrol_em {
        XWDS_UART_HFC_NONE = 0U,
        XWDS_UART_HFC_RTS = XWBOP_BIT(0),
        XWDS_UART_HFC_CTS = XWBOP_BIT(1),
};

/**
 * @brief UART模式枚举
 */
enum xwds_uart_mode_em {
        XWDS_UART_MODE_CLOSE = 0U,
        XWDS_UART_MODE_TX = XWBOP_BIT(0),
        XWDS_UART_MODE_RX = XWBOP_BIT(1),
};

/**
 * @brief UART配置
 */
struct xwds_uart_cfg {
        xwu32_t baudrate; /**< 波特率 */
        struct {
                xwu8_t bits; /**< 字长 */
                xwu8_t stopbits; /**< 停止位长度 */
                xwu8_t parity; /**< 奇偶校验 */
                xwu8_t hfc; /**< 硬件流控 */
                xwu8_t mode; /**< 模式 */
        } bus;
        void * soccfg; /**< SOC私有配置 */
};

struct xwds_uartc;

/**
 * @brief BSP中需要提供的UART控制器驱动函数表
 */
struct xwds_uartc_driver {
        struct xwds_driver base; /**< C语言面向对象：继承struct xwds_driver */
        xwer_t (* cfg)(struct xwds_uartc * /*uartc*/,
                       const struct xwds_uart_cfg * /*cfg*/); /**< 配置UART控制器 */
        xwer_t (* tx)(struct xwds_uartc * /*uartc*/,
                      const xwu8_t * /*data*/, xwsz_t * /*size*/,
                      xwtm_t /*to*/); /**< 配置DMA通道并发送 */
        xwer_t (* putc)(struct xwds_uartc * /*uartc*/,
                        const xwu8_t /*byte*/); /**< 发送一个字节 */
};

/**
 * @brief UART控制器
 */
struct xwds_uartc {
        struct xwds_device dev; /**< 继承struct xwds_device */

        /* attributes */
        const struct xwds_uart_cfg * cfg; /**< 配置 */
        const struct xwds_resource_dma * rxdmarsc; /**< 用于接收UART数据的DMA资源 */
        const struct xwds_resource_dma * txdmarsc; /**< 用于发送UART数据的DMA资源 */

        /* TX */
        struct xwos_mtx txmtx; /**< 发送互斥锁 */

        /* RX */
        struct {
                struct xwos_sem sem; /**< 接收队列中数据可用的信号量 */
                struct xwos_splk lock; /**< 保护接收队列的锁 */
                xwsq_t pos; /**< 当前有效数据的起始位置 */
                xwsz_t tail; /**< 当前有效数据的结束位置 + 1 */
                xwsq_t idx; /**< 从UART硬件接收数据的位置，如果使用DMA，不使用此成员 */
                xwu8_t mem[XWDS_UART_RXQ_SIZE] __xwcc_alignl1cache; /**< 缓冲区 */
        } rxq; /**< 循环接收队列 */
};

/******** ******** APIs ******** ********/
void xwds_uartc_construct(struct xwds_uartc * uartc);
void xwds_uartc_destruct(struct xwds_uartc * uartc);
xwer_t xwds_uartc_grab(struct xwds_uartc * uartc);
xwer_t xwds_uartc_put(struct xwds_uartc * uartc);

/**
 * @brief XWDS API：从接收队列中获取数据
 * @param[in] uartc: UART控制器对象指针
 * @param[out] buf: 指向缓冲区的指针，通过此缓冲区返回数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示缓冲区大小（单位：字节）
 * + (O) 作为输出时，返回实际读取的数据大小
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ETIMEDOUT: 超时
 * @note
 * + 上下文：线程
 * @details
 * 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_uartc_rx(struct xwds_uartc * uartc,
                     xwu8_t * buf, xwsz_t * size,
                     xwtm_t to);

/**
 * @brief XWDS API：尝试从接收队列中获取数据
 * @param[in] uartc: UART控制器对象指针
 * @param[out] buf: 指向缓冲区的指针，通过此缓冲区返回数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示缓冲区大小（单位：字节）
 * + (O) 作为输出时，返回实际读取的数据大小
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ENODATA: 没有数据
 * @note
 * + 上下文：中断、中断底半部、线程
 */
xwer_t xwds_uartc_try_rx(struct xwds_uartc * uartc,
                         xwu8_t * buf, xwsz_t * size);

/**
 * @brief XWDS API：配置UART的DMA通道发送数据
 * @param[in] uartc: UART控制器对象指针
 * @param[in] data: 待发送的数据的缓冲区
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示期望发送的数据的大小（单位：字节）
 * + (O) 作为输出时，返回实际发送的数据大小
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ENOSYS: 不支持此操作
 * @retval -ECANCELED: 发送被取消
 * @retval -ETIMEDOUT: 超时
 * @note
 * + 上下文：线程
 * @details
 * 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_uartc_tx(struct xwds_uartc * uartc,
                     const xwu8_t * data, xwsz_t * size,
                     xwtm_t to);

/**
 * @brief XWDS API：直接发送一个字节（非DMA模式）
 * @param[in] uartc: UART控制器对象指针
 * @param[in] byte: 待发送的字节
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ENOSYS: 不支持此操作
 * @note
 * + 上下文：线程
 * @details
 * 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_uartc_putc(struct xwds_uartc * uartc,
                       const xwu8_t byte,
                       xwtm_t to);

/**
 * @brief XWDS API：配置UART
 * @param[in] uartc: UART控制器对象指针
 * @param[in] cfg: 新的配置
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ENOSYS: 不支持此操作
 * @note
 * + 上下文：中断、中断底半部、线程
 */
xwer_t xwds_uartc_cfg(struct xwds_uartc * uartc,
                      const struct xwds_uart_cfg * cfg);

/**
 * @brief 测试UART波特率是否合法
 * @param[in] baudrate: 波特率
 */
static __xwcc_inline
bool xwds_uart_tst_baudrate(xwu32_t baudrate)
{
        return ((50U == baudrate) || (75U == baudrate) || (110U == baudrate) ||
                (134U == baudrate) || (150U == baudrate) || (200U == baudrate) ||
                (300U == baudrate) || (600U == baudrate) || (1200U == baudrate) ||
                (1800U == baudrate) || (2400U == baudrate) || (4800U == baudrate) ||
                (9600U == baudrate) || (19200U == baudrate) || (38400U == baudrate) ||
                (57600U == baudrate) || (115200U == baudrate) ||
                (230400U == baudrate) || (460800U == baudrate) ||
                (576000U == baudrate) || (921600U == baudrate) ||
                (1000000U == baudrate) || (1152000U == baudrate) ||
                (1500000U == baudrate) || (2000000U == baudrate) ||
                (2500000U == baudrate) || (3000000U == baudrate) ||
                (3500000U == baudrate) || (4000000U == baudrate));
}

/******** ******** Callbacks for driver ******** ********/
/**
 * @brief XWDS Driver Callback：清空接收队列
 * @param[in] uartc: UART控制器对象指针
 */
void xwds_uartc_drvcb_rxq_flush(struct xwds_uartc * uartc);

/**
 * @brief XWDS Driver Callback：从UART硬件接收数据到缓冲区
 * @param[in] uartc: UART控制器对象指针
 * @return 下一次接收数据的位置
 * @details
 * 如果使用 `uartc->rxq.mem` 作为DMA内存，缓冲区将自动被填充，不需要使用此函数。
 * 此函数仅当DMA内存与缓冲区不一致时，或使用中断模式时使用。
 */
xwsq_t xwds_uartc_drvcb_rxq_fill(struct xwds_uartc * uartc,
                                 xwu8_t bytes[], xwsz_t size);

/**
 * @brief XWDS Driver Callback：发布数据到接收队列
 * @param[in] uartc: UART控制器对象指针
 * @param[in] pub: 新的数据接收位置（有效数据结尾 + 1）
 */
void xwds_uartc_drvcb_rxq_pub(struct xwds_uartc * uartc, xwsq_t tail);

/**
 * @} xwcd_ds_uart
 */

#endif /* xwcd/ds/uart/controller.h */
