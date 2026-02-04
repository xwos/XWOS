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
#include "bm/Stm32Hal/CubeMX/Core/Inc/sdmmc.h"

#include <xwos/lib/xwlog.h>
#define LOGTAG "sdcard"

FATFS sdcard_fatfs;

void sdcard_fatfs_mount(void)
{
        xwer_t rc;
        FRESULT frc;

        MX_SDMMC1_SD_Init();
        rc = MX_SDMMC1_SD_TrimClk(10);
        if (rc < 0) {
                xwlogf(ERR, LOGTAG, "MX_SDMMC1_SD_TrimClk() ... <rc:%d>\n", rc);
        } else {
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
        MX_SDMMC1_SD_DeInit();
}

void sdcard_fatfs_tst(void)
{
        FRESULT fsrc;
        FIL fp;
        void * buf;
        UINT fsrd;

        buf = malloc(4096);
        fsrc = f_open(&fp, "sd:/xw.lua", FA_READ);
        if (FR_OK == fsrc) {
                fsrc = f_read(&fp, buf, 4096, &fsrd);
                fsrc = f_close(&fp);
        }
        free(buf);
}

int MMC_disk_status(void)
{
        return (int)MX_SDMMC1_SD_GetState();
}

int MMC_disk_initialize(void)
{
        int rc;

        if (HAL_SD_STATE_READY == hsd1.State) {
                rc = XWOK;
        } else {
                rc = -EIO;
        }
        return (int)rc;
}

int MMC_disk_read(uint8_t * buff, uint64_t sector, uint32_t count)
{
        xwer_t rc;

        rc = MX_SDMMC1_SD_Read(buff, (uint32_t)sector, count);
        return (int)rc;
}

int MMC_disk_write(uint8_t * buff, uint64_t sector, uint32_t count)
{
        xwer_t rc;

        rc = MX_SDMMC1_SD_Write(buff, (uint32_t)sector, count);
        return (int)rc;
}

int MMC_disk_ioctl(uint8_t cmd, void * buff)
{
        int rc;

        switch (cmd) {
        case CTRL_SYNC:
                rc = XWOK;
                break;
        case GET_SECTOR_COUNT:
                *(uint32_t *)buff = (hsd1.SdCard.BlockSize * hsd1.SdCard.BlockNbr) /
                                    MX_SD_SECTOR_SIZE;
                rc = XWOK;
                break;
        case GET_SECTOR_SIZE:
                *(uint16_t *)buff = MX_SD_SECTOR_SIZE;
                rc = XWOK;
                break;
        case GET_BLOCK_SIZE:
                *(uint32_t *)buff = hsd1.SdCard.BlockSize;
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
