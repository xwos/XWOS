/**
 * @file
 * @brief 示例：CRC
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
#include <xwos/lib/xwlog.h>
#include <xwos/lib/crc8.h>
#include <xwos/lib/crc32.h>

#define LOGTAG "crc"
#define crclogf(lv, fmt, ...) xwlogf(lv, LOGTAG, fmt, ##__VA_ARGS__)

/**
 * @brief 模块的加载函数
 */
xwer_t example_crc_start(void)
{
        xwu8_t data[] = {'X', 'u', 'a', 'n', 'W', 'u', 'O', 'S', '\0',};
        xwu32_t crc32;
        xwu8_t crc8;
        xwsz_t size;

        crclogf(INFO, "Data:%s\n", data);

        size = sizeof(data) - 1;
        crc32 = xwlib_crc32_calms(data, &size);
        crclogf(INFO, "CRC32:0x%X\n", crc32);

        crc32 = 0xFFFFFFFF;
        size = sizeof(data) - 1;
        xwlib_crc32_cal(&crc32, 0x0, false, false,
                        0x04C11DB7, XWLIB_CRC32_LEFT_SHIFT,
                        data, &size);
        crclogf(INFO, "CRC32(MPEG-2):0x%X\n", crc32);

        crc32 = 0x0;
        size = sizeof(data) - 1;
        xwlib_crc32_cal(&crc32, 0xFFFFFFFF, false, false,
                        0x04C11DB7, XWLIB_CRC32_LEFT_SHIFT,
                        data, &size);
        crclogf(INFO, "CRC32(POSIX):0x%X\n", crc32);

        crc32 = 0xFFFFFFFF;
        size = sizeof(data) - 1;
        xwlib_crc32_cal(&crc32, 0x0, true, true,
                        0x04C11DB7, XWLIB_CRC32_LEFT_SHIFT,
                        data, &size);
        crclogf(INFO, "CRC32(JAMCRC):0x%X\n", crc32);

        size = sizeof(data) - 1;
        crc8 = xwlib_crc8_calms(data, &size);
        crclogf(INFO, "CRC8:0x%X\n", crc8);

        crc8 = 0x0;
        size = sizeof(data) - 1;
        xwlib_crc8_cal(&crc8, 0x55, false, false, 0x07,
                       data, &size);
        crclogf(INFO, "CRC8(ITU):0x%X\n", crc8);

        crc8 = 0xFF;
        size = sizeof(data) - 1;
        xwlib_crc8_cal(&crc8, 0x0, true, true, 0x07,
                       data, &size);
        crclogf(INFO, "CRC8(ROHC):0x%X\n", crc8);

        crc8 = 0x0;
        size = sizeof(data) - 1;
        xwlib_crc8_cal(&crc8, 0x0, true, true, 0x31,
                       data, &size);
        crclogf(INFO, "CRC8(MAXIM):0x%X\n", crc8);

        crc8 = 0x0;
        size = sizeof(data) - 1;
        xwlib_crc8_cal(&crc8, 0x0, true, true, 0x9B,
                       data, &size);
        crclogf(INFO, "CRC8(WCDMA):0x%X\n", crc8);

        return XWOK;
}
