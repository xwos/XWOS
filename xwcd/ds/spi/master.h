/**
 * @file
 * @brief 玄武设备栈：SPI主机模式控制器
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

void xwds_spim_construct(struct xwds_spim * spim);
void xwds_spim_destruct(struct xwds_spim * spim);
xwer_t xwds_spim_grab(struct xwds_spim * spim);
xwer_t xwds_spim_put(struct xwds_spim * spim);

xwer_t xwds_spim_buscfg(struct xwds_spim * spim, xwid_t cfgid, xwtm_t to);
xwer_t xwds_spim_xfer(struct xwds_spim * spim,
                      const xwu8_t txd[], xwu8_t * rxb, xwsz_t * size,
                      xwtm_t to);
xwer_t xwds_spim_abort(struct xwds_spim * spim, xwtm_t to);

#endif /* xwcd/ds/spi/master.h */
