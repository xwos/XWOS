/**
 * @file
 * @brief 板级描述层：XWOS适配层：FATFS：SDCard
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
 */

#include "board/std.h"
#include <stdlib.h>
#include <xwem/fs/fatfs/ff.h>
#include <xwem/fs/fatfs/diskio.h>
#include "bm/Stm32Hal/mi.h"

#include <xwos/lib/xwlog.h>
#define LOGTAG "sdcard"

FATFS sdcard_fatfs;

void sdcard_fatfs_mount(void)
{
        xwer_t rc;
        FRESULT frc;

        rc = stm32hal_sdmmc1_init();
        if (XWOK == rc) {
                frc = f_mount(&sdcard_fatfs, "sd:", 1);
                if (FR_OK != frc) {
                        xwlogf(ERR, LOGTAG, "f_mount() ... <rc:%d>\n", rc);
                }
        }
}

void sdcard_fatfs_unmount(void)
{
        FRESULT frc;

        frc = f_unmount("sd:");
        if (FR_OK != frc) {
                xwlogf(ERR, LOGTAG, "f_unmount() ... <rc:%d>\n", frc);
        }
        stm32hal_sdmmc1_fini();
}

int MMC_disk_status(void)
{
        return (int)stm32hal_sdmmc1_status();
}

int MMC_disk_initialize(void)
{
        return XWOK;
}

int MMC_disk_read(uint8_t * buff, uint64_t sector, uint32_t count)
{
        return (int)stm32hal_sdmmc1_read(buff, sector, count);
}

int MMC_disk_write(uint8_t * buff, uint64_t sector, uint32_t count)
{
        return (int)stm32hal_sdmmc1_write(buff, sector, count);
}

int MMC_disk_ioctl(uint8_t cmd, void * buff)
{
        int rc;

        switch (cmd) {
        case CTRL_SYNC:
                rc = XWOK;
                break;
        case GET_SECTOR_COUNT:
                *(uint32_t *)buff = stm32hal_sdmmc1_get_sector_count();
                rc = XWOK;
                break;
        case GET_SECTOR_SIZE:
                *(uint16_t *)buff = stm32hal_sdmmc1_get_sector_size();
                rc = XWOK;
                break;
        case GET_BLOCK_SIZE:
                *(uint32_t *)buff = stm32hal_sdmmc1_get_block_size();
                rc = XWOK;
                break;
        case CTRL_TRIM:
                rc = XWOK;
                break;
        default:
                rc = -EINVAL;
                break;
        }
        return (int)rc;
}
