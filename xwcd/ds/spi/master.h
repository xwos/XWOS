/**
 * @file
 * @brief 玄武设备栈：SPI：主机模式控制器
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

#ifndef __xwcd_ds_spi_master_h__
#define __xwcd_ds_spi_master_h__

#include <xwcd/ds/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/osal/lock/mtx.h>
#include <xwcd/ds/device.h>

/**
 * @defgroup xwcd_ds_spi SPI
 * @ingroup xwcd_ds
 */

/**
 * @defgroup xwcd_ds_spi_master SPI主机模式控制器
 * @ingroup xwcd_ds_spi
 * @{
 */

struct xwds_spim;

/**
 * @brief BSP中需要提供的SPI主机模式控制器驱动函数表
 */
struct xwds_spim_driver {
        struct xwds_driver base; /**< C语言面向对象：继承struct xwds_driver */
        xwer_t (* buscfg)(struct xwds_spim * /*spim*/,
                          xwid_t /*cfgid*/,
                          xwtm_t /*to*/); /**< 配置总线参数 */
        xwer_t (* xfer)(struct xwds_spim * /*spim*/,
                        const xwu8_t[] /*txd*/, xwu8_t * /*rxb*/,
                        xwsz_t * /*size*/, xwtm_t /*to*/); /**< 传输消息 */
        xwer_t (* abort)(struct xwds_spim * /*spim*/,
                         xwtm_t /*to*/); /**< 中止传输 */
};

/**
 * @brief SPI主机模式控制器
 */
struct xwds_spim {
        struct xwds_device dev; /**< 继承struct xwds_device */

        /* attributes */
        void * buscfg; /**< BUS配置（具体结构体的定义与平台相关） */
        xwsz_t buscfg_num; /**< BUS配置数量 */

        /* private */
        struct {
                struct xwos_mtx apimtx; /**< API互斥锁 */
        } xfer; /**< 传输API的状态 */
};

/**
 * @brief XWDS API：SPI主机模式控制器对象的构造函数
 * @param[in] spim: SPI主机模式控制器对象指针
 */
void xwds_spim_construct(struct xwds_spim * spim);

/**
 * @brief XWDS API：SPI主机模式控制器对象的析构函数
 * @param[in] spim: SPI主机模式控制器对象指针
 */
void xwds_spim_destruct(struct xwds_spim * spim);

/**
 * @brief XWDS API：增加对象的引用计数
 * @param[in] spim: SPI主机模式控制器对象指针
 */
xwer_t xwds_spim_grab(struct xwds_spim * spim);

/**
 * @brief XWDS API：减少对象的引用计数
 * @param[in] spim: SPI主机模式控制器对象指针
 */
xwer_t xwds_spim_put(struct xwds_spim * spim);

/**
 * @brief XWDS API：配置总线
 * @param[in] spim: SPI主机模式控制器对象指针
 * @param[in] cfgid: 总线配置ID
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENOSYS: 不支持配置总线操作
 * @retval -ECHRNG: 配置ID不在配置表范围内
 * @retval -EFAULT: 无效指针
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_spim_buscfg(struct xwds_spim * spim, xwid_t cfgid, xwtm_t to);

/**
 * @brief XWDS API：启动SPI总线传输
 * @param[in] spim: SPI主机模式控制器对象指针
 * @param[in] txd: 发送数据缓冲区，可为NULL表示不发送数据
 * @param[out] rxb: 接收数据缓冲区，可为NULL表示不接收数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示缓冲区大小（单位：字节）
 * + (O) 作为输出时，返回实际传输的数据大小
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -EINVAL: 参数错误
 * @retval -EBUSY: 总线繁忙
 * @retval -EIO: 传输错误
 * @retval -ETIMEDOUT: 超时
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_spim_xfer(struct xwds_spim * spim,
                      const xwu8_t txd[], xwu8_t * rxb, xwsz_t * size,
                      xwtm_t to);

/**
 * @brief XWDS API：中止SPI总线传输
 * @param[in] spim: SPI主机控制器对象指针
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 设备对象不可引用
 * @retval -ETIMEDOUT: 超时
 * @note
 * + 上下文：线程
 * + 异步函数
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_spim_abort(struct xwds_spim * spim, xwtm_t to);

/**
 * @} xwcd_ds_spi_master
 */

#endif /* xwcd/ds/spi/master.h */
