/**
 * @file
 * @brief SPI Flash W25Qxx Device
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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
 * + SPI总线配置：
 *   - `spip->bus` 指明是SOC中的哪条SPI总线；
 *   - 若需要支持API `xwds_w25qxx_cfgbus()` ，需要在配置 `spip->buscfgid` 指明
 *     SPI总线配置的序号，SPI主机配置列表（ `spip->bus->buscfg` 数组）
 *     中必须要有此配置。
 */

#ifndef __xwcd_peripheral_spi_flash_w25qxx_device_h__
#define __xwcd_peripheral_spi_flash_w25qxx_device_h__

#include <xwos/standard.h>
#include <xwcd/ds/spi/peripheral.h>

/**
 * @defgroup xwcd_peripheral_spi_flash_w25qxx SPI Flash W25QXX
 * @ingroup xwcd_peripheral_spi_flash
 * @{
 */

#define XWDS_W25QXX_CMD_MAX_PAYLOAD     256U

/**
 * @brief W25QXX Flash 指令枚举
 */
enum xwds_w25qxx_cmd_em {
        XWDS_W25QXX_CMD_WRITE_DISABLE,
        XWDS_W25QXX_CMD_WRITE_ENABLE,

        XWDS_W25QXX_CMD_READ_STATUS_REG_1,
        XWDS_W25QXX_CMD_READ_STATUS_REG_2,
        XWDS_W25QXX_CMD_READ_STATUS_REG_3,
        XWDS_W25QXX_CMD_WRITE_STATUS_REG_1,
        XWDS_W25QXX_CMD_WRITE_STATUS_REG_2,
        XWDS_W25QXX_CMD_WRITE_STATUS_REG_3,

        XWDS_W25QXX_CMD_READ_DATA,
        XWDS_W25QXX_CMD_FAST_READ_DATA,
        XWDS_W25QXX_CMD_SET_READ_PARAMETERS,

        XWDS_W25QXX_CMD_PAGE_PROGRAM,

        XWDS_W25QXX_CMD_32KBLOCK_ERASE,
        XWDS_W25QXX_CMD_64KBLOCK_ERASE,
        XWDS_W25QXX_CMD_SECTOR_ERASE,
        XWDS_W25QXX_CMD_CHIP_ERASE,

        XWDS_W25QXX_CMD_POWER_DOWN,
        XWDS_W25QXX_CMD_RELEASE_POWER_DOWN,

        XWDS_W25QXX_CMD_UNIQUE_ID,
        XWDS_W25QXX_CMD_MANUFACTURER_DEVICE_ID,
        XWDS_W25QXX_CMD_JEDEC_DEVICE_ID,

        XWDS_W25QXX_CMD_ENTER_4BYTE_ADDR_MODE,
        XWDS_W25QXX_CMD_EXIT_4BYTE_ADDR_MODE,

        XWDS_W25QXX_CMD_ENTER_QPI_MODE,
        XWDS_W25QXX_CMD_EXIT_QPI_MODE,

        XWDS_W25QXX_CMD_ENABLE_RESET,
        XWDS_W25QXX_CMD_RESET,

        XWDS_W25QXX_CMD_NUM,
};

/**
 * @brief W25QXX Flash MID 枚举
 */
enum w25q_mid_em {
        XWDS_W25Q80_MID = 0xEF13,
        XWDS_W25Q16_MID = 0xEF14,
        XWDS_W25Q32_MID = 0xEF15,
        XWDS_W25Q64_MID = 0xEF16,
        XWDS_W25Q128_MID = 0xEF17,
        XWDS_W25Q256_MID = 0xEF18,
};

/**
 * @brief W25QXX Flash SR寄存器序号
 */
enum xwds_w25qxx_sr_idx_em {
        XWDS_W25QXX_SR_1,
        XWDS_W25QXX_SR_2,
        XWDS_W25QXX_SR_3,
        XWDS_W25QXX_SR_NUM,
};

/**
 * @brief W25QXX Flash SR1 位枚举
 */
enum xwds_w25qxx_sr1_bit_em {
        XWDS_W25QXX_SR1_BUSY = 0x1U,
        XWDS_W25QXX_SR1_WEL = 0x2U,
        XWDS_W25QXX_SR1_BP0 = 0x4U,
        XWDS_W25QXX_SR1_BP1 = 0x8U,
        XWDS_W25QXX_SR1_BP2 = 0x10U,
        XWDS_W25QXX_SR1_TB = 0x20U,
        XWDS_W25QXX_SR1_SEC = 0x40U,
        XWDS_W25QXX_SR1_SRP = 0x80U,
};
/**
 * @brief W25QXX Flash SR2 位枚举
 */
enum xwds_w25qxx_sr2_bit_em {
        XWDS_W25QXX_SR2_SRL = 0x1U,
        XWDS_W25QXX_SR2_QE = 0x2U,
        XWDS_W25QXX_SR2_R2 = 0x4U,
        XWDS_W25QXX_SR2_LB1 = 0x8U,
        XWDS_W25QXX_SR2_LB2 = 0x10U,
        XWDS_W25QXX_SR2_LB3 = 0x20U,
        XWDS_W25QXX_SR2_CMP = 0x40U,
        XWDS_W25QXX_SR2_SUS = 0x80U,
};

/**
 * @brief W25QXX Flash SR3 位枚举
 */
enum xwds_w25qxx_sr3_bit_em {
        XWDS_W25QXX_SR3_R0 = 0x1U,
        XWDS_W25QXX_SR3_R1 = 0x2U,
        XWDS_W25QXX_SR3_WPS = 0x4U,
        XWDS_W25QXX_SR3_R3 = 0x8U,
        XWDS_W25QXX_SR3_R4 = 0x10U,
        XWDS_W25QXX_SR3_DRV0 = 0x20U,
        XWDS_W25QXX_SR3_DRV1 = 0x40U,
        XWDS_W25QXX_SR3_HR = 0x80U,
};

/**
 * @brief W25QXX Flash 命令
 */
struct xwds_w25qxx_cmd {
        xwu8_t existing; /**< 指令存在 */
        xwu8_t instruction; /**< 指令 */
        xwu8_t address_size; /**< 地址的大小，取值0 ~ 4，0表示没有地址 */
        xwu32_t address; /**< 地址，有效位由@ref address_size决定 */
        xwu32_t dummy_cycles; /**< dummy周期 */
};

/**
 * @brief W25QXX Flash 参数
 */
struct xwds_w25qxx_parameter {
        xwu32_t size;
        xwu64_t uid;
        xwu16_t mid;
        xwu32_t jid;
};

/**
 * @brief W25QXX Flash 设备
 */
struct xwds_w25qxx {
        struct xwds_spip spip; /**< C语言面向对象：继承 `struct xwds_spip` */

        /* attributes */
        struct xwds_w25qxx_parameter parameter; /**< 参数 */
        const struct xwds_w25qxx_cmd * cmdtbl; /**< 参数列表 */

        /* private */
        xwu8_t txq[XWDS_W25QXX_CMD_MAX_PAYLOAD + 16]; /**< 发送缓冲区 */
        xwu8_t rxq[XWDS_W25QXX_CMD_MAX_PAYLOAD + 16]; /**< 接收缓冲区 */
};

void xwds_w25qxx_construct(struct xwds_w25qxx * w25qxx);
void xwds_w25qxx_destruct(struct xwds_w25qxx * w25qxx);

/**
 * @brief W25QXX API：增加对象的引用计数
 * @param[in] w25qxx: W25QXX Flash 设备指针
 * @return 错误码
 * @retval @ref xwds_spip_grab()
 */
static __xwds_inline
xwer_t xwds_w25qxx_grab(struct xwds_w25qxx * w25qxx)
{
        return xwds_spip_grab(&w25qxx->spip);
}

/**
 * @brief W25QXX API：减少对象的引用计数
 * @param[in] w25qxx: W25QXX Flash 设备指针
 * @return 错误码
 * @retval @ref xwds_spip_put()
 */
static __xwds_inline
xwer_t xwds_w25qxx_put(struct xwds_w25qxx * w25qxx)
{
        return xwds_spip_put(&w25qxx->spip);
}

/**
 * @} xwcd_peripheral_spi_flash_w25qxx
 */

#endif /* xwcd/peripheral/spi/flash/w25qxx/device.h */
