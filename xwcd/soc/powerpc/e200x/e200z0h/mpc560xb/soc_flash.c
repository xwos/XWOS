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
 * @note
 * - When erasing & programming the flash, we can't read it. So we must relocate the
 *   code to the RAM and run it in RAM.
 */

#include <xwos/standard.h>
#include <soc_flash.h>

extern xwu8_t flsopc_lma_base[];
extern xwu8_t flsopc_vma_base[];
extern xwu8_t flsopc_vma_end[];

/**
 * @brief Select block of code flash
 * @param[in] lmsmsk: Low/Mid space block bit mask
 * @param[in] lmsmsk: High space block bit mask
 */
__flsopc
void soc_cflash_select(xwsq_t lmsmsk, xwsq_t hsmsk)
{
        xwu32_t unlkval;

        lmsmsk &= SOC_FLASH_LMS_BLK_ALL_MSK;
        if (lmsmsk) {
                unlkval = 0x80000000 | (lmsmsk ^ SOC_FLASH_LMS_BLK_ALL_MSK);
                CFLASH.LML.R = 0xA1A11111; /* password for enable LML register */
                CFLASH.LML.R = unlkval; /* unlock */
                CFLASH.SLL.R = 0xC3C33333; /* password for enable SLL register */
                CFLASH.SLL.R = unlkval; /* unlock */
                CFLASH.LMS.R = (lmsmsk & ~(SOC_FLASH_LMS_BLK_TS_MSK)); /* select */
        }

        hsmsk &= SOC_FLASH_HS_BLK_ALL_MSK;
        if (hsmsk) {
                unlkval = 0x80000000 | (hsmsk ^ SOC_FLASH_HS_BLK_ALL_MSK);
                CFLASH.HBL.R = 0xB2B22222; /* password for enable HBL register */
                CFLASH.HBL.R = unlkval; /* unlock */
                CFLASH.HBS.R = hsmsk; /* select */
        }
}

/**
 * @brief Erase blocks of code flash
 * @param[in] lmsmsk: Low/Mid space block bit mask
 * @param[in] lmsmsk: High space block bit mask
 */
__flsopc
void soc_cflash_erase(xwsq_t lmsmsk, xwsq_t hsmsk)
{
        xwu32_t * any_addr = (xwu32_t *)0x8;

        /* step 1 prepare */
        if (CFLASH.MCR.B.EER)
                CFLASH.MCR.B.EER = 1; /* clear ecc event error if have */
        if (CFLASH.MCR.B.RWE)
                CFLASH.MCR.B.RWE = 1; /* clear read-while-write error if have */
        while (CFLASH.MCR.B.PGM) { /* programming? */
        }
        while (CFLASH.MCR.B.ERS) {/* erasing? */
        }

        /* step 2 erase */
        CFLASH.MCR.B.PEAS = 0;
        CFLASH.MCR.B.ERS = 1;
        soc_cflash_select(lmsmsk, hsmsk);
        *any_addr = 0xffffffff; /* write to any address in flash. ref to p706@refman */
        CFLASH.MCR.B.EHV = 1; /* start erase */
        while (!CFLASH.MCR.B.DONE) { /* wait erase finish */
        }
        while (!CFLASH.MCR.B.PEG) { /* check PEG flag */
        }
        CFLASH.MCR.B.EHV = 0; /* clear EHV bit */
        CFLASH.MCR.B.ERS = 0; /* terminate erase operation */
}

/**
 * @brief Program code flash
 * @param[in] addr: address to program
 * @param[in] data0: data 0
 * @param[in] data1: data 1
 */
__flsopc
void soc_cflash_program(xwu32_t * addr, xwu32_t data0, xwu32_t data1)
{
        while (CFLASH.MCR.B.PGM)
                ;
        CFLASH.MCR.B.PEAS = 0;
        CFLASH.MCR.B.PGM = 1;
        *addr = data0;
        addr ++;
        *addr = data1;
        CFLASH.MCR.B.EHV = 1;
        while (!CFLASH.MCR.B.DONE) { /* wait DONE */
        }
        while (!CFLASH.MCR.B.PEG) { /* confirm PEG */
        }
        CFLASH.MCR.B.EHV = 0;
        CFLASH.MCR.B.PGM = 0; /* terminate program */
}

#if defined(SOCCFG_FLASH_SHADOW_BLOCK) && (1 == SOCCFG_FLASH_SHADOW_BLOCK)
/**
 * @brief Erase shadow block of code flash
 */
__flsopc
void soc_cflash_erase_shadow_block(void)
{
        xwu32_t * any_addr = (xwu32_t *)0x200000;

        if (CFLASH.MCR.B.EER) {
                CFLASH.MCR.B.EER = 1; /* clear ecc event error if have */
        }
	if (CFLASH.MCR.B.RWE) {
                CFLASH.MCR.B.RWE = 1; /* clear read-while-write error if have */
        }
        while (CFLASH.MCR.B.PGM) { /* programming? */
        }
        while (CFLASH.MCR.B.ERS) { /* erasing? */
        }

        CFLASH.MCR.B.PEAS = 1; /* Shadow/Test address */
        soc_cflash_select(SOC_FLASH_BLK_TS_MSK);
        CFLASH.MCR.B.ERS = 1;
        *any_addr = 0xffffffff; /* write to any address in flash. ref to refman p706 */
        CFLASH.MCR.B.EHV = 1; /* start erase */
        while (!CFLASH.MCR.B.DONE) { /* wait erase finish */
        }
        while (!CFLASH.MCR.B.PEG) { /* check PEG flag */
        }
        CFLASH.MCR.B.EHV = 0; /* clear EHV bit */
        CFLASH.MCR.B.ERS = 0; /* terminate erase operation */
        CFLASH.MCR.B.PEAS = 0; /* normal address */
}

/**
 * @brief Program shadow/test block of code flash
 * @param[in] addr: address to program
 * @param[in] data0: data 0
 * @param[in] data1: data 1
 */
__flsopc
void soc_cflash_program_shadow_test_block(xwu32_t * addr, xwu32_t data0, xwu32_t data1)
{
        while (CFLASH.MCR.B.PGM) {
        }
        CFLASH.MCR.B.PEAS = 1;
        CFLASH.MCR.B.PGM = 1;
        *addr = data0;
        addr ++;
        *addr = data1;
        CFLASH.MCR.B.EHV = 1;
        while (!CFLASH.MCR.B.DONE) { /* wait DONE */
        }
        while (!CFLASH.MCR.B.PEG) { /* confirm PEG */
        }
        CFLASH.MCR.B.EHV = 0;
        CFLASH.MCR.B.PGM = 0; /* terminate program */
        CFLASH.MCR.B.PEAS = 0;
}
#endif

/**
 * @brief Lock code flash
 */
__flsopc
void soc_cflash_lock(void)
{
        xwu32_t lkval;

        while (CFLASH.MCR.B.PGM) { /* programming? */
        }
        while (CFLASH.MCR.B.ERS) { /* erasing? */
        }
        CFLASH.HBS.R = 0x00000000; /* unselect all high space */
        lkval = 0x80000000 | SOC_FLASH_HS_BLK_ALL_MSK;
        CFLASH.HBL.R = 0xB2B22222; /* password for enable HBL register */
        CFLASH.HBL.R = lkval; /* unlock */

        CFLASH.LMS.R = 0x00000000; /* unselect all low/mid space */
        lkval = 0x80000000 | SOC_FLASH_LMS_BLK_ALL_MSK;
        CFLASH.LML.R = 0xA1A11111; /* password for enable LML register */
        CFLASH.LML.R = lkval; /* lock all */
        CFLASH.SLL.R = 0xC3C33333; /* password for enable SLL register */
        CFLASH.SLL.R = lkval; /* lock all */
}

/**
 * @brief SOC flash operation code relocate
 */
__xwbsp_code
void soc_flsopc_relocate(void)
{
        xwsz_t count, i;
        xwu8_t * src;
        xwu8_t * dst;

        src = flsopc_lma_base;
        dst = flsopc_vma_base;
        if (dst != src) {
                count = (xwsz_t)flsopc_vma_end - (xwsz_t)flsopc_vma_base;
                for (i = 0; i < count; i ++) {
                        *dst = *src;
                        dst ++;
                        src ++;
                }
        }
}
