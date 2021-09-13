/**
 * @file
 * @brief SPI Flash W25Qxx Driver
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
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
 * @note
 * - 所有API只可被单一线程访问。
 */

#ifndef __xwcd_perpheral_spi_flash_w25qxx_driver_h__
#define __xwcd_perpheral_spi_flash_w25qxx_driver_h__

#include <xwos/standard.h>
#include <xwcd/perpheral/spi/flash/w25qxx/device.h>

/**
 * @brief W25QXX驱动函数表
 */
struct xwds_w25qxx_driver {
        struct xwds_spip_driver spip; /**< C语言面向对象：继承struct xwds_spip_driver */
        xwer_t (* io)(struct xwds_w25qxx * /*w25qxx*/,
                      xwu8_t * /*txq*/, xwu8_t * /*rxq*/,
                      xwsz_t * /*size*/, xwtm_t * /*xwtm*/);
};

extern struct xwds_w25qxx_cmd xwds_w25q64jv_cmd[XWDS_W25QXX_CMD_NUM];
extern struct xwds_w25qxx_cmd xwds_w25q128jv_cmd[XWDS_W25QXX_CMD_NUM];

/******** ******** base driver ******** ********/
xwer_t xwds_w25qxx_drv_start(struct xwds_device * dev);
xwer_t xwds_w25qxx_drv_stop(struct xwds_device * dev);
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
xwer_t xwds_w25qxx_drv_resume(struct xwds_device * dev);
xwer_t xwds_w25qxx_drv_suspend(struct xwds_device * dev);
#endif

/******** ******** ******** APIs ******** ******** ********/
xwer_t xwds_w25qxx_cfgbus(struct xwds_w25qxx * w25qxx, xwtm_t * xwtm);
xwer_t xwds_w25qxx_ctrl(struct xwds_w25qxx * w25qxx,
                        xwu8_t instruction,
                        xwu8_t address_size, xwu32_t address,
                        xwu32_t dummy_cycles,
                        const xwu8_t txd[], xwu8_t * rxb, xwsz_t size,
                        xwtm_t * xwtm);
xwer_t xwds_w25qxx_reset(struct xwds_w25qxx * w25qxx, xwtm_t * xwtm);
xwer_t xwds_w25qxx_init_parameter(struct xwds_w25qxx * w25qxx, xwtm_t * xwtm);
xwer_t xwds_w25qxx_write_enable(struct xwds_w25qxx * w25qxx, xwtm_t * xwtm);
xwer_t xwds_w25qxx_write_disable(struct xwds_w25qxx * w25qxx, xwtm_t * xwtm);
xwer_t xwds_w25qxx_read_sr(struct xwds_w25qxx * w25qxx,
                           xwu32_t sridx, xwu8_t * srbuf,
                           xwtm_t * xwtm);
xwer_t xwds_w25qxx_check_idle(struct xwds_w25qxx * w25qxx, xwtm_t * xwtm);
xwer_t xwds_w25qxx_wait_idle(struct xwds_w25qxx * w25qxx, xwtm_t period,
                             xwtm_t * xwtm);
xwer_t xwds_w25qxx_read_uid(struct xwds_w25qxx * w25qxx, xwu64_t * uidbuf,
                            xwtm_t * xwtm);
xwer_t xwds_w25qxx_read_mid(struct xwds_w25qxx * w25qxx, xwu16_t * midbuf,
                            xwtm_t * xwtm);
xwer_t xwds_w25qxx_read_jid(struct xwds_w25qxx * w25qxx, xwu32_t * jidbuf,
                            xwtm_t * xwtm);
xwer_t xwds_w25qxx_read(struct xwds_w25qxx * w25qxx, xwu32_t address,
                        xwu8_t * rxb, xwsz_t * size, xwtm_t * xwtm);
xwer_t xwds_w25qxx_write(struct xwds_w25qxx * w25qxx, xwu32_t address,
                         xwu8_t * txb, xwsz_t * size, xwtm_t * xwtm);
xwer_t xwds_w25qxx_erase_sector(struct xwds_w25qxx * w25qxx, xwu32_t address,
                                xwtm_t * xwtm);
xwer_t xwds_w25qxx_erase_32kblk(struct xwds_w25qxx * w25qxx, xwu32_t address,
                                xwtm_t * xwtm);
xwer_t xwds_w25qxx_erase_64kblk(struct xwds_w25qxx * w25qxx, xwu32_t address,
                                xwtm_t * xwtm);
xwer_t xwds_w25qxx_erase_chip(struct xwds_w25qxx * w25qxx, xwtm_t * xwtm);

#endif /* xwcd/perpheral/spi/flash/w25qxx/driver.h */
