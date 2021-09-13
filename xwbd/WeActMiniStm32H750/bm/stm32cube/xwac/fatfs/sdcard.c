/**
 * @file
 * @brief STM32CUBE FATFS：SD卡驱动
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

#include <stdlib.h>
#include <bm/stm32cube/standard.h>
#include <xwem/fs/fatfs/ff.h>
#include <bm/stm32cube/cubemx/Core/Inc/sdmmc.h>
#include <bm/stm32cube/xwac/fatfs/cmif.h>

FATFS sdcard_fatfs;

xwer_t sdcard_fatfs_mount(void)
{
        xwer_t rc;
        FRESULT frc;

        MX_SDMMC1_SD_Init();
        frc = f_mount(&sdcard_fatfs, "sd:", 1);
        if (FR_OK != frc) {
                rc = -EIO;
                goto err_mount;
        }
        return XWOK;

err_mount:
        return rc;
}

xwer_t sdcard_fatfs_unmount(void)
{
        xwer_t rc;
        FRESULT frc;

        frc = f_unmount("sd:");
        if (FR_OK != frc) {
                rc = -EIO;
                goto err_unmount;
        }
        MX_SDMMC1_SD_DeInit();
        return XWOK;

err_unmount:
        return rc;
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
                rc = MX_SDMMC1_SD_TrimClk(10);
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
