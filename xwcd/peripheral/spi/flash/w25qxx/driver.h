/**
 * @file
 * @brief SPI Flash W25Qxx Driver
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
 * @note
 * - 所有API只可被单一线程访问。
 */

#ifndef __xwcd_peripheral_spi_flash_w25qxx_driver_h__
#define __xwcd_peripheral_spi_flash_w25qxx_driver_h__

#include <xwos/standard.h>
#include <xwcd/peripheral/spi/flash/w25qxx/device.h>

/**
 * @ingroup xwcd_peripheral_spi_flash_w25qxx
 * @{
 */

/**
 * @brief W25QXX驱动函数表
 */
struct xwds_w25qxx_driver {
        struct xwds_spip_driver spip; /**< C语言面向对象：继承 `struct xwds_spip_driver` */
        xwer_t (* io)(struct xwds_w25qxx * /*w25qxx*/,
                      xwu8_t * /*txq*/, xwu8_t * /*rxq*/,
                      xwsz_t * /*size*/, xwtm_t /*to*/);
};

extern struct xwds_w25qxx_cmd xwds_w25q64jv_cmd[XWDS_W25QXX_CMD_NUM];
extern struct xwds_w25qxx_cmd xwds_w25q128jv_cmd[XWDS_W25QXX_CMD_NUM];

/******** ******** base driver ******** ********/
/**
 * @brief W25QXX基本驱动：启动设备
 */
xwer_t xwds_w25qxx_drv_start(struct xwds_device * dev);

/**
 * @brief W25QXX基本驱动：停止设备
 */
xwer_t xwds_w25qxx_drv_stop(struct xwds_device * dev);
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)

/**
 * @brief W25QXX基本驱动：暂停设备
 */
xwer_t xwds_w25qxx_drv_resume(struct xwds_device * dev);

/**
 * @brief W25QXX基本驱动：继续设备
 */
xwer_t xwds_w25qxx_drv_suspend(struct xwds_device * dev);
#endif

/******** ******** ******** APIs ******** ******** ********/
/**
 * @brief W25QXX API：配置SPI总线满足W25QXX的时序
 * @param[in] w25qxx: W25QXX对象指针
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * 时序由 `w25qxx->spip.buscfgid` 定义。
 */
xwer_t xwds_w25qxx_cfgbus(struct xwds_w25qxx * w25qxx, xwtm_t to);

/**
 * @brief W25QXX API：向W25QXX发送指令
 * @param[in] w25qxx: W25QXX对象指针
 * @param[in] instruction: 指令
 * @param[in] address_size: 地址信息长度
 * @param[in] address: 地址
 * @param[in] dummy_cycles: SPI通讯过程中的无效时钟周期
 * @param[in] txd: SPI通讯过程中的发送数据的缓冲区
 * @param[out] rxb: SPI通讯过程中的接收数据的缓冲区
 * @param[in] size: SPI通讯过程中的缓冲区大小
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * + 时序由 `w25qxx->spip.buscfgid` 定义。
 * + `to` 表示等待超时的时间点：
 *   + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *     可以使用 `xwtm_ft(delta)` 表示；
 *   + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_w25qxx_ctrl(struct xwds_w25qxx * w25qxx,
                        xwu8_t instruction,
                        xwu8_t address_size, xwu32_t address,
                        xwu32_t dummy_cycles,
                        const xwu8_t txd[], xwu8_t * rxb, xwsz_t size,
                        xwtm_t to);

/**
 * @brief W25QXX API：向W25QXX发送复位命令
 * @param[in] w25qxx: W25QXX对象指针
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_w25qxx_reset(struct xwds_w25qxx * w25qxx, xwtm_t to);

/**
 * @brief W25QXX API：初始化W25QXX的参数
 * @param[in] w25qxx: W25QXX对象指针
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 */
xwer_t xwds_w25qxx_init_parameter(struct xwds_w25qxx * w25qxx, xwtm_t to);

/**
 * @brief W25QXX API：开启W25QXX的写操作
 * @param[in] w25qxx: W25QXX对象指针
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_w25qxx_write_enable(struct xwds_w25qxx * w25qxx, xwtm_t to);

/**
 * @brief W25QXX API：关闭W25QXX的写操作
 * @param[in] w25qxx: W25QXX对象指针
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_w25qxx_write_disable(struct xwds_w25qxx * w25qxx, xwtm_t to);

/**
 * @brief W25QXX API：读取W25QXX的SR寄存器
 * @param[in] w25qxx: W25QXX对象指针
 * @param[in] sridx: SR寄存器的序号
 * @param[out] srbuf: 指向缓冲区的指针，通过此缓冲区返回SR寄存器的值
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_w25qxx_read_sr(struct xwds_w25qxx * w25qxx,
                           xwu32_t sridx, xwu8_t * srbuf,
                           xwtm_t to);

/**
 * @brief W25QXX API：检测W25QXX是否空闲
 * @param[in] w25qxx: W25QXX对象指针
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 空闲
 * @retval -EBUSY: 繁忙
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_w25qxx_check_idle(struct xwds_w25qxx * w25qxx, xwtm_t to);

/**
 * @brief W25QXX API：等待W25QXX是否空闲
 * @param[in] w25qxx: W25QXX对象指针
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_w25qxx_wait_idle(struct xwds_w25qxx * w25qxx, xwtm_t period,
                             xwtm_t to);

/**
 * @brief W25QXX API：读取W25QXX的UID
 * @param[in] w25qxx: W25QXX对象指针
 * @param[out] uidbuf: 指向缓冲区的指针，通过此缓冲区返回UID
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_w25qxx_read_uid(struct xwds_w25qxx * w25qxx, xwu64_t * uidbuf,
                            xwtm_t to);

/**
 * @brief W25QXX API：读取W25QXX的MID
 * @param[in] w25qxx: W25QXX对象指针
 * @param[out] midbuf: 指向缓冲区的指针，通过此缓冲区返回MID
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_w25qxx_read_mid(struct xwds_w25qxx * w25qxx, xwu16_t * midbuf,
                            xwtm_t to);

/**
 * @brief W25QXX API：读取W25QXX的JID
 * @param[in] w25qxx: W25QXX对象指针
 * @param[out] jidbuf: 指向缓冲区的指针，通过此缓冲区返回JID
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_w25qxx_read_jid(struct xwds_w25qxx * w25qxx, xwu32_t * jidbuf,
                            xwtm_t to);

/**
 * @brief W25QXX API：向W25QXX读取数据
 * @param[in] w25qxx: W25QXX对象指针
 * @param[in] address: 起始地址
 * @param[out] rxb: 指向缓冲区的指针，通过此缓冲区返回数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示缓冲区长度
 * + (O) 作为输出时，返回实际读取的长度
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_w25qxx_read(struct xwds_w25qxx * w25qxx, xwu32_t address,
                        xwu8_t * rxb, xwsz_t * size, xwtm_t to);

/**
 * @brief W25QXX API：向W25QXX写入数据
 * @param[in] w25qxx: W25QXX对象指针
 * @param[in] address: 起始地址
 * @param[in] txb: 待写入的数据缓冲区
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示缓冲区长度
 * + (O) 作为输出时，返回实际写入的长度
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_w25qxx_write(struct xwds_w25qxx * w25qxx, xwu32_t address,
                         xwu8_t * txb, xwsz_t * size, xwtm_t to);

/**
 * @brief W25QXX API：擦除W25QXX的sector
 * @param[in] w25qxx: W25QXX对象指针
 * @param[in] address: 起始地址
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 */
xwer_t xwds_w25qxx_erase_sector(struct xwds_w25qxx * w25qxx, xwu32_t address,
                                xwtm_t to);

/**
 * @brief W25QXX API：擦除W25QXX的32KiB的块
 * @param[in] w25qxx: W25QXX对象指针
 * @param[in] address: 起始地址
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_w25qxx_erase_32kblk(struct xwds_w25qxx * w25qxx, xwu32_t address,
                                xwtm_t to);

/**
 * @brief W25QXX API：擦除W25QXX的64KiB的块
 * @param[in] w25qxx: W25QXX对象指针
 * @param[in] address: 起始地址
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_w25qxx_erase_64kblk(struct xwds_w25qxx * w25qxx, xwu32_t address,
                                xwtm_t to);

/**
 * @brief W25QXX API：擦除W25QXX的全部数据
 * @param[in] w25qxx: W25QXX对象指针
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
xwer_t xwds_w25qxx_erase_chip(struct xwds_w25qxx * w25qxx, xwtm_t to);

/**
 * @} xwcd_peripheral_spi_flash_w25qxx
 */

#endif /* xwcd/peripheral/spi/flash/w25qxx/driver.h */
