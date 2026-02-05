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

#ifndef bm_Stm32Hal_mi_h__
#define bm_Stm32Hal_mi_h__

#include <board/std.h>
#include <time.h>
#include "bm/Stm32Hal/xwds/device.h"

/**
 * @brief 预初始化
 * @details
 * + 此函数在 `xwos_preinit()` 中被调用。
 */
void stm32hal_preinit(void);

/**
 * @brief 初始化
 * @details
 * + 此函数在 `xwos_postinit()` 中被调用。
 */
void stm32hal_init(void);

/**
 * @brief 后初始化
 * @details
 * + 此函数在主线程中被调用。
 */
void stm32hal_postinit(void);

/**
 * @brief 恢复
 * @details
 * + 此函数在电源管理的 `resume()` 回调函数中被调用。
 */
void stm32hal_resume(void);

/**
 * @brief 暂停
 * @details
 * + 此函数在电源管理的 `suspend()` 回调函数中被调用。
 */
void stm32hal_suspend(void);

/**
 * @brief 唤醒
 * @details
 * + 此函数在电源管理的 `wakeup()` 回调函数中被调用。
 */
void stm32hal_wakeup(void);

/**
 * @brief 休眠
 * @details
 * + 此函数在电源管理的 `sleep()` 回调函数中被调用。
 */
void stm32hal_sleep(void);

/**
 * @brief 增加STM23HAL库的Tick时钟
 * @details
 * + 此函数在Systick中断Hook中被调用。
 */
void stm32hal_inc_tick(void);

/******** ******** CRC ******** ********/
xwer_t stm32hal_crc32_cal(xwu32_t * crc32,
                          bool refin, xwu32_t polynomial, xwu32_t direction,
                          const xwu8_t stream[], xwsz_t * size); /**< 计算CRC32 */
xwer_t stm32hal_crc8_cal(xwu8_t * crc8,
                         bool refin, xwu8_t polynomial,
                         const xwu8_t stream[], xwsz_t * size); /**< 计算CRC8 */

/******** ******** SDMMC ******** ********/
xwer_t stm32hal_sdmmc1_init(void);
void stm32hal_sdmmc1_fini(void);
xwer_t stm32hal_sdmmc1_status(void);
xwer_t stm32hal_sdmmc1_read(uint8_t * buff, uint64_t sector, uint32_t count);
xwer_t stm32hal_sdmmc1_write(uint8_t * buff, uint64_t sector, uint32_t count);
uint32_t stm32hal_sdmmc1_get_sector_count(void);
uint16_t stm32hal_sdmmc1_get_sector_size(void);
uint32_t stm32hal_sdmmc1_get_block_size(void);

/******** ******** RTC ******** ********/
xwer_t stm32hal_rtc_set_datetime(struct tm * tm, suseconds_t ms);
xwer_t stm32hal_rtc_get_datetime(struct tm * tm, suseconds_t * ms);

#endif /* bm/Stm32Hal/mi.h */
