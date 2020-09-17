/**
 * @file
 * @brief xwmd设备栈：SPI主机模式控制器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwds_spim;

/**
 * @breif BSP中需要提供的SPI主机模式控制器驱动函数表
 */
struct xwds_spim_driver {
        struct xwds_driver base; /**< C语言面向对象：继承struct xwds_driver */
        xwer_t (* buscfg)(struct xwds_spim * /*spim*/,
                          xwid_t /*cfgid*/,
                          xwtm_t * /*xwtm*/); /**< 配置总线参数 */
        xwer_t (* xfer)(struct xwds_spim * /*spim*/,
                        const xwu8_t[] /*txd*/, xwu8_t * /*rxb*/,
                        xwsz_t /*size*/, xwtm_t * /*xwtm*/); /**< 发送消息 */
};

/**
 * @breif SPI主机模式控制器
 */
struct xwds_spim {
        struct xwds_device dev; /**< 继承struct xwds_device */

        /* attributes */
        void * buscfg; /**< BUS配置（具体结构体的定义与平台相关） */
        xwsz_t buscfg_num; /**< BUS配置数量 */

        /* private */
        struct xwosal_mtx apilock; /**< API互斥锁 */
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
 * @brief XWDS API：配置总线
 * @param spim: (I) SPI主机模式控制器对象指针
 * @param cfgid: (I) 总线配置ID
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
xwer_t xwds_spim_buscfg(struct xwds_spim * spim, xwid_t cfgid, xwtm_t * xwtm);

/**
 * @brief XWDS API：启动传输
 * @param spim: (I) SPI主机模式控制器对象指针
 * @param txd: (I) 发送数据缓冲区，不可为NULL
 * @param rxb: (O) 接收数据缓冲区，可为NULL表示不接收数据
 * @param size: (I) 传输数据的大小
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ENOSYS: 不支持主机模式传输
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_spim_xfer(struct xwds_spim * spim,
                      const xwu8_t txd[], xwu8_t * rxb,
                      xwsz_t size, xwtm_t * xwtm);

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
