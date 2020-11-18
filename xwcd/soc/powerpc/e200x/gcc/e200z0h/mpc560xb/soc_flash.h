/**
 * @file
 * @brief SOC描述层：MPC560xB Flash
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

#ifndef __soc_flash_h__
#define __soc_flash_h__

#include <xwos/standard.h>
#include <soc.h>

#define SOC_FLASH_LMS_BLK_TS_MSK        0x00100000U
#define SOC_FLASH_LMS_BLK_7_MSK         0x00020000U
#define SOC_FLASH_LMS_BLK_6_MSK         0x00010000U
#define SOC_FLASH_LMS_BLK_5_MSK         0x00000020U
#define SOC_FLASH_LMS_BLK_4_MSK         0x00000010U
#define SOC_FLASH_LMS_BLK_3_MSK         0x00000008U
#define SOC_FLASH_LMS_BLK_2_MSK         0x00000004U
#define SOC_FLASH_LMS_BLK_1_MSK         0x00000002U
#define SOC_FLASH_LMS_BLK_0_MSK         0x00000001U
#define SOC_FLASH_LMS_BLK_ALL_MSK       0x0013003FU

#define SOC_FLASH_HS_BLK_7_MSK          0x00000080U
#define SOC_FLASH_HS_BLK_6_MSK          0x00000040U
#define SOC_FLASH_HS_BLK_5_MSK          0x00000020U
#define SOC_FLASH_HS_BLK_4_MSK          0x00000010U
#define SOC_FLASH_HS_BLK_3_MSK          0x00000008U
#define SOC_FLASH_HS_BLK_2_MSK          0x00000004U
#define SOC_FLASH_HS_BLK_1_MSK          0x00000002U
#define SOC_FLASH_HS_BLK_0_MSK          0x00000001U
#define SOC_FLASH_HS_BLK_ALL_MSK        0x000000FFU

void soc_cflash_select(xwsq_t lmsmsk, xwsq_t hsmsk);
void soc_cflash_erase(xwsq_t lmsmsk, xwsq_t hsmsk);
void soc_cflash_program(xwu32_t * addr, xwu32_t data0, xwu32_t data1);

#if defined(SOCCFG_FLASH_SHADOW_BLOCK) && (1 == SOCCFG_FLASH_SHADOW_BLOCK)
void soc_cflash_erase_shadow_block(void);
void soc_cflash_program_shadow_test_block(xwu32_t * addr, xwu32_t data0, xwu32_t data1);
#endif
/* #if defined(SOCCFG_FLASH_SHADOW_BLOCK) && (1 == SOCCFG_FLASH_SHADOW_BLOCK) */

void soc_cflash_lock(void);
void soc_flsopc_relocate(void);

#endif /* soc_flash.h */
