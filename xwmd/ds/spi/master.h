/**
 * @file
 * @brief xwmd设备栈：SPI主机模式控制器
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_ds_spi_master_h__
#define __xwmd_ds_spi_master_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwmd/ds/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/irq.h>
#include <xwos/osal/lock/mutex.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/spi/common.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief SPI帧格式标志
 */
enum xwds_spim_frame_flag_em {
        XWDS_SPIM_FF_WL_MSK = BIT(0) | BIT(1), /**< 字长 */
        XWDS_SPIM_FF_WL_8 = 0, /**< 字长：8位 */
        XWDS_SPIM_FF_WL_16 = 1, /**< 字长：16位 */
        XWDS_SPIM_FF_WL_32 = 2, /**< 字长：32位 */

        XWDS_SPIM_FF_CPOL_MSK = BIT(2), /**< 时钟极性 */
        XWDS_SPIM_FF_CPOL_LOW = 0, /**< 时钟极性：低 */
        XWDS_SPIM_FF_CPOL_HIGH = BIT(2), /**< 时钟极性：高 */
        XWDS_SPIM_FF_CPHA_MSK = BIT(3), /**< 时钟相位 */
        XWDS_SPIM_FF_CPHA_LCTS = 0, /**< 起始沿采集，结束沿设置 */
        XWDS_SPIM_FF_CPHA_LSTC = BIT(3), /**< 起始沿设置，结束沿采集 */
        XWDS_SPIM_FF_MODE_1 = XWDS_SPIM_FF_CPOL_LOW | XWDS_SPIM_FF_CPHA_LCTS,
        XWDS_SPIM_FF_MODE_2 = XWDS_SPIM_FF_CPOL_LOW | XWDS_SPIM_FF_CPHA_LSTC,
        XWDS_SPIM_FF_MODE_3 = XWDS_SPIM_FF_CPOL_HIGH | XWDS_SPIM_FF_CPHA_LCTS,
        XWDS_SPIM_FF_MODE_4 = XWDS_SPIM_FF_CPOL_HIGH | XWDS_SPIM_FF_CPHA_LSTC,

        XWDS_SPIM_FF_FO_MSK = BIT(4), /**< 帧顺序 */
        XWDS_SPIM_FF_FO_MSB = 0, /**< MSB优先 */
        XWDS_SPIM_FF_FO_LSB = BIT(4), /**< LSB优先 */

        XWDS_SPIM_FF_CS_MSK = BIT(5), /**< 片选 */
        XWDS_SPIM_FF_CS_NONCONTINUOUS = 0, /**< 非连续 */
        XWDS_SPIM_FF_CS_CONTINUOUS = BIT(5), /**< 连续 */
};

/**
 * @brief SPI主机模式消息
 */
struct xwds_spim_msg {
        xwsq_t cs_pin_msk; /**< 片选信号掩码 */
        xwsq_t flags; /**< 消息标志 */
        union {
                xwu8_t * d8;
                xwu16_t * d16;
                xwu32_t * d32;
                void * anon;
        } txq; /**< 发送队列 */
        union {
                xwu8_t * d8;
                xwu16_t * d16;
                xwu32_t * d32;
                void * anon;
        } rxq; /**< 接收队列 */
        xwsz_t size; /**< 数据大小（单位：字节） */
};

/**
 * @brief SPI主机模式片选资源
 */
struct xwds_spim_cspin {
        const struct xwds_resource_gpio * csres; /**< 片选GPIO资源 */
        xwsq_t inactive; /**< 片选无效电平 */
};

/**
 * @breif SPI主机模式配置
 */
struct xwds_spim_cfg {
        union {
                struct {
                        xwu32_t wire:1; /**< 3/4线模式 */
                        xwu32_t direction:2; /**< 方向 */
                } bit;
                xwu32_t u32;
        } bus;
        const struct xwds_spim_cspin * cspins; /**< CS GPIO资源数组 */
        xwsz_t cspins_num; /**< CS GPIO资源数组大小 */
        void * xwccfg; /**< SOC私有配置 */
};

struct xwds_spim;

/**
 * @breif BSP中需要提供的SPI主机模式控制器驱动函数表
 */
struct xwds_spim_driver {
        struct xwds_driver base; /**< C语言面向对象：继承struct xwds_driver */
        xwer_t (* xfer)(struct xwds_spim *,
                        struct xwds_spim_msg *,
                        xwtm_t *); /**< 发送消息 */
};

/**
 * @breif SPI主机模式控制器
 */
struct xwds_spim {
        struct xwds_device dev; /**< 继承struct xwds_device */

        /* attributes */
        const struct xwds_spim_cfg * cfg; /**< 配置 */

        /* private */
        struct xwosal_mtx xfermtx; /**< 发送互斥锁 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** constructor & destructor ******** ******** ********/
__xwds_api
void xwds_spim_construct(struct xwds_spim * spim);

__xwds_api
void xwds_spim_destruct(struct xwds_spim * spim);

/******** ******** ******** APIs ******** ******** ********/
/**
 * @brief XWDS API：启动传输
 * @param spim: (I) SPI主机模式控制器对象指针
 * @param msg: (I) 消息
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -ENOSYS: 不支持主机模式传输
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_spim_xfer(struct xwds_spim * spim,
                      struct xwds_spim_msg * msg,
                      xwtm_t * xwtm);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********   inline function implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 增加对象的引用计数
 * @param spim: (I) SPI主机模式控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_grab()
 */
static __xwds_inline
xwer_t xwds_spim_grab(struct xwds_spim * spim)
{
        return xwds_device_grab(&spim->dev);
}

/**
 * @brief 减少对象的引用计数
 * @param spim: (I) SPI主机模式控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_put()
 */
static __xwds_inline
xwer_t xwds_spim_put(struct xwds_spim * spim)
{
        return xwds_device_put(&spim->dev);
}

/**
 * @brief 增加设备运行状态计数器
 * @param spim: (I) SPI主机模式控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_request()
 */
static __xwds_inline
xwer_t xwds_spim_request(struct xwds_spim * spim)
{
        return xwds_device_request(&spim->dev);
}

/**
 * @brief 减少设备运行状态计数器
 * @param spim: (I) SPI主机模式控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_release()
 */
static __xwds_inline
xwer_t xwds_spim_release(struct xwds_spim * spim)
{
        return xwds_device_release(&spim->dev);
}

#endif /* xwmd/ds/spi/master.h */
