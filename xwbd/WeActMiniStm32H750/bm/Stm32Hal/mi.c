/**
 * @file
 * @brief Stm32Hal::mi 模块集成接口
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

#include <board/std.h>
#include <time.h>
#include "bm/Stm32Hal/CubeMX/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_ll_cortex.h"
#include "bm/Stm32Hal/CubeMX/Drivers/STM32H7xx_HAL_Driver/Inc/stm32h7xx_ll_pwr.h"
#include "bm/Stm32Hal/CubeMX/Core/Inc/crc.h"
#include "bm/Stm32Hal/CubeMX/Core/Inc/sdmmc.h"
#include "bm/Stm32Hal/CubeMX/Core/Inc/rtc.h"
#include "bm/Stm32Hal/CubeMX/Core/Inc/main.h"
#include "bm/Stm32Hal/CubeMX/integration.h"
#include "bm/Stm32Hal/xwds/device.h"
#include "bm/Stm32Hal/mi.h"

#include <xwos/lib/xwlog.h>
#define LOGTAG "stm32hal"

void stm32hal_init_devices(void);
void stm32hal_fini_devices(void);

__xwbsp_init_code
void stm32hal_preinit(void)
{
        stm32cubemx_preinit();
}

__xwbsp_init_code
void stm32hal_init(void)
{
        stm32cubemx_init();
        stm32xwds_init();
        stm32xwds_soc_init();
}

__xwbsp_init_code
void stm32hal_postinit(void)
{
        stm32hal_init_devices();
}

void stm32hal_resume(void)
{
        xwds_pm_resume(&stm32xwds); /* 恢复所有设备 */
}

void stm32hal_suspend(void)
{
        xwds_pm_suspend(&stm32xwds); /* 暂停所有设备 */

        /* 设置休眠方式为STOP模式：
           STOP模式下寄存器与内部RAM数据不丢失，
           因此休眠方式为SuspendToRAM，唤醒后运行状态可恢复。*/
        LL_PWR_SetRegulModeDS(LL_PWR_REGU_DSMODE_LOW_POWER);
        LL_PWR_EnableFlashPowerDown();
        LL_PWR_CPU_SetD1PowerMode(LL_PWR_CPU_MODE_D1STOP);
        LL_PWR_CPU_SetD2PowerMode(LL_PWR_CPU_MODE_D2STOP);
        LL_PWR_CPU_SetD3PowerMode(LL_PWR_CPU_MODE_D3STOP);
        LL_LPM_EnableDeepSleep();
}

extern void SystemClock_Config(void);
void stm32hal_wakeup(void)
{
        LL_LPM_EnableSleep(); /* 清除DEEPSLEEP位 */
        SystemClock_Config(); /* 从STOP模式恢复后，需要重新配置时钟 */
}

void stm32hal_sleep(void)
{
}

__xwbsp_init_code
void stm32hal_init_devices(void)
{
        xwer_t rc;

        rc = stm32xwds_uart_init();
        XWOS_BUG_ON(rc < 0);
        /* Can print log from here */
        rc = stm32xwds_spi_init();
        if (rc < 0) {
                xwlogf(ERR, LOGTAG, "Init SPI ... <rc:%d>\n", rc);
        }
        rc = stm32xwds_w25q64jv_init();
        if (rc < 0) {
                xwlogf(ERR, LOGTAG, "Init W25Q64JV ... <rc:%d>\n", rc);
        }
        rc = stm32xwds_st7735_init();
        if (rc < 0) {
                xwlogf(ERR, LOGTAG, "Init ST7735 ... <rc:%d>\n", rc);
        }
        rc = stm32xwds_i2c_init();
        if (rc < 0) {
                xwlogf(ERR, LOGTAG, "Init I2C ... <rc:%d>\n", rc);
        }
        rc = stm32xwds_eeprom_init();
        if (rc < 0) {
                xwlogf(ERR, LOGTAG, "Init EEPROM ... <rc:%d>\n", rc);
        }
}

__xwbsp_init_code
void stm32hal_fini_devices(void)
{
        xwer_t rc;

        rc = stm32xwds_eeprom_fini();
        if (rc < 0) {
                xwlogf(ERR, LOGTAG, "Fini EEPROM ... <rc:%d>\n", rc);
        }
        rc = stm32xwds_i2c_fini();
        if (rc < 0) {
                xwlogf(ERR, LOGTAG, "Fini I2C ... <rc:%d>\n", rc);
        }
        rc = stm32xwds_st7735_fini();
        if (rc < 0) {
                xwlogf(ERR, LOGTAG, "Fini ST7735 ... <rc:%d>\n", rc);
        }
        rc = stm32xwds_w25q64jv_fini();
        if (rc < 0) {
                xwlogf(ERR, LOGTAG, "Fini W25Q64JV ... <rc:%d>\n", rc);
        }
        rc = stm32xwds_spi_fini();
        if (rc < 0) {
                xwlogf(ERR, LOGTAG, "Fini SPI ... <rc:%d>\n", rc);
        }
        stm32xwds_uart_fini();
}

void stm32hal_inc_tick(void)
{
        HAL_IncTick();
}

xwer_t stm32hal_crc32_cal(xwu32_t * crc32,
                          bool refin, xwu32_t polynomial, xwu32_t direction,
                          const xwu8_t stream[], xwsz_t * size)
{
        return MX_CRC32_Cal(crc32, refin, polynomial, direction, stream, size);
}

xwer_t stm32hal_crc8_cal(xwu8_t * crc8,
                         bool refin, xwu8_t polynomial,
                         const xwu8_t stream[], xwsz_t * size)
{
        return MX_CRC8_Cal(crc8, refin, polynomial, stream, size);
}

xwer_t stm32hal_sdmmc1_init(void)
{
        xwer_t rc;

        MX_SDMMC1_SD_Init();
        rc = MX_SDMMC1_SD_TrimClk(10);
        if (rc < 0) {
                xwlogf(ERR, LOGTAG, "MX_SDMMC1_SD_TrimClk() ... <rc:%d>\n", rc);
        }
        return rc;
}

void stm32hal_sdmmc1_fini(void)
{
        MX_SDMMC1_SD_DeInit();
}

xwer_t stm32hal_sdmmc1_status(void)
{
        return MX_SDMMC1_SD_GetState();
}

xwer_t stm32hal_sdmmc1_read(uint8_t * buff, uint64_t sector, uint32_t count)
{
        return (xwer_t)MX_SDMMC1_SD_Read(buff, (uint32_t)sector, count);
}

xwer_t stm32hal_sdmmc1_write(uint8_t * buff, uint64_t sector, uint32_t count)
{
        return (xwer_t)MX_SDMMC1_SD_Write(buff, (uint32_t)sector, count);
}

uint32_t stm32hal_sdmmc1_get_sector_count(void)
{
        return (hsd1.SdCard.BlockSize * hsd1.SdCard.BlockNbr) / MX_SD_SECTOR_SIZE;
}

uint16_t stm32hal_sdmmc1_get_sector_size(void)
{
        return MX_SD_SECTOR_SIZE;
}

uint32_t stm32hal_sdmmc1_get_block_size(void)
{
        return hsd1.SdCard.BlockSize;
}

xwer_t stm32hal_rtc_set_datetime(struct tm * tm, suseconds_t ms)
{
        return MX_RTC_Set_DateTime(tm, ms);
}

xwer_t stm32hal_rtc_get_datetime(struct tm * tm, suseconds_t * ms)
{
        return MX_RTC_Get_DateTime(tm, ms);
}
