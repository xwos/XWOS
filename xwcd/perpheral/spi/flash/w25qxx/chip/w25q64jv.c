/**
 * @file
 * @brief SPI Flash W25Q64JV Driver
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
 */

#include <xwos/standard.h>
#include <xwcd/perpheral/spi/flash/w25qxx/device.h>

struct xwds_w25qxx_cmd xwds_w25q64jv_cmd[XWDS_W25QXX_CMD_NUM] = {
        [XWDS_W25QXX_CMD_WRITE_DISABLE] = {
                .existing = 1,
                .instruction = 0x4U,
                .address = 0x0,
                .address_size = 0,
                .dummy_cycles = 0,
        },
        [XWDS_W25QXX_CMD_WRITE_ENABLE] = {
                .existing = 1,
                .instruction = 0x6U,
                .address = 0x0,
                .address_size = 0,
                .dummy_cycles = 0,
        },

        [XWDS_W25QXX_CMD_READ_STATUS_REG_1] = {
                .existing = 1,
                .instruction = 0x05,
                .address = 0x0,
                .address_size = 0,
                .dummy_cycles = 0,
        },
        [XWDS_W25QXX_CMD_READ_STATUS_REG_2] = {
                .existing = 1,
                .instruction = 0X35,
                .address = 0x0,
                .address_size = 0,
                .dummy_cycles = 0,
        },
        [XWDS_W25QXX_CMD_READ_STATUS_REG_3] = {
                .existing = 1,
                .instruction = 0x15,
                .address = 0x0,
                .address_size = 0,
                .dummy_cycles = 0,
        },
        [XWDS_W25QXX_CMD_WRITE_STATUS_REG_1] = {
                .existing = 1,
                .instruction = 0X01,
                .address = 0x0,
                .address_size = 0,
                .dummy_cycles = 0,
        },
        [XWDS_W25QXX_CMD_WRITE_STATUS_REG_2] = {
                .existing = 1,
                .instruction = 0X31,
                .address = 0x0,
                .address_size = 0,
                .dummy_cycles = 0,
        },
        [XWDS_W25QXX_CMD_WRITE_STATUS_REG_3] = {
                .existing = 1,
                .instruction = 0X11,
                .address = 0x0,
                .address_size = 0,
                .dummy_cycles = 0,
        },

        [XWDS_W25QXX_CMD_READ_DATA] = {
                .existing = 1,
                .instruction = 0X03,
                .address = 0, /* TBD */
                .address_size = 3,
                .dummy_cycles = 0,
        },
        [XWDS_W25QXX_CMD_FAST_READ_DATA] = {
                .existing = 1,
                .instruction = 0X0B,
                .address = 0, /* TBD */
                .address_size = 3,
                .dummy_cycles = 8,
        },

        [XWDS_W25QXX_CMD_PAGE_PROGRAM] = {
                .existing = 1,
                .instruction = 0X02,
                .address = 0x0, /* TBD */
                .address_size = 3,
                .dummy_cycles = 0,
        },

        [XWDS_W25QXX_CMD_32KBLOCK_ERASE] = {
                .existing = 1,
                .instruction = 0X52,
                .address = 0x0, /* TBD */
                .address_size = 3,
                .dummy_cycles = 0,
        },
        [XWDS_W25QXX_CMD_64KBLOCK_ERASE] = {
                .existing = 1,
                .instruction = 0XD8,
                .address = 0x0, /* TBD */
                .address_size = 3,
                .dummy_cycles = 0,
        },
        [XWDS_W25QXX_CMD_SECTOR_ERASE] = {
                .existing = 1,
                .instruction = 0X20,
                .address = 0x0, /* TBD */
                .address_size = 3,
                .dummy_cycles = 0,
        },
        [XWDS_W25QXX_CMD_CHIP_ERASE] = {
                .existing = 1,
                .instruction = 0XC7,
                .address = 0x0,
                .address_size = 0,
                .dummy_cycles = 0,
        },

        [XWDS_W25QXX_CMD_POWER_DOWN] = {
                .existing = 1,
                .instruction = 0XB9,
                .address = 0x0,
                .address_size = 0,
                .dummy_cycles = 0,
        },
        [XWDS_W25QXX_CMD_RELEASE_POWER_DOWN] = {
                .existing = 1,
                .instruction = 0XAB,
                .address = 0x0,
                .address_size = 0,
                .dummy_cycles = 0,
        },

        [XWDS_W25QXX_CMD_UNIQUE_ID] = {
                .existing = 1,
                .instruction = 0X4B,
                .address = 0x0,
                .address_size = 4,
                .dummy_cycles = 0,
        },
        [XWDS_W25QXX_CMD_MANUFACTURER_DEVICE_ID] = {
                .existing = 1,
                .instruction = 0X90,
                .address = 0x0,
                .address_size = 3,
                .dummy_cycles = 0,
        },
        [XWDS_W25QXX_CMD_JEDEC_DEVICE_ID] = {
                .existing = 1,
                .instruction = 0X9F,
                .address = 0x0,
                .address_size = 0,
                .dummy_cycles = 0,
        },

        [XWDS_W25QXX_CMD_ENABLE_RESET] = {
                .existing = 1,
                .instruction = 0x66,
                .address = 0x0,
                .address_size = 0,
                .dummy_cycles = 0,
        },
        [XWDS_W25QXX_CMD_RESET] = {
                .existing = 1,
                .instruction = 0x99,
                .address = 0x0,
                .address_size = 0,
                .dummy_cycles = 0,
        },
};
