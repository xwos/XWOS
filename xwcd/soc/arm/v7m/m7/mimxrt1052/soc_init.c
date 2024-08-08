/**
 * @file
 * @brief SOC描述层：初始化
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
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

#include <xwcd/soc/arm/v7m/m7/mimxrt1052/soc_init.h>
#include <xwos/ospl/irq.h>
#include <xwos/ospl/skd.h>
#include <xwcd/soc/arm/v7m/armv7m_isa.h>

extern xwu8_t armv7m_ivt_lma_base[];
extern xwu8_t armv7m_ivt_vma_base[];
extern xwu8_t armv7m_ivt_vma_end[];

/**
 * @brief 重定向中断向量表到内存
 */
__xwos_init_code
void soc_relocate_ivt(void)
{
        xwsz_t cnt, i;
        xwu8_t * src;
        xwu8_t * dst;

        src = armv7m_ivt_lma_base;
        dst = armv7m_ivt_vma_base;
        if (dst != src) {
                cnt = (xwsz_t)armv7m_ivt_vma_end - (xwsz_t)armv7m_ivt_vma_base;
                for (i = 0; i < cnt; i++) {
                        dst[i] = src[i];
                }
        }
}

extern xwu8_t xwos_data_lma_base[];
extern xwu8_t xwos_data_vma_base[];
extern xwu8_t xwos_data_vma_end[];

extern xwu8_t data_lma_base[];
extern xwu8_t data_vma_base[];
extern xwu8_t data_vma_end[];
extern xwu8_t bss_vma_base[];
extern xwu8_t bss_vma_end[];

extern xwu8_t itcm_lma_base[];
extern xwu8_t itcm_vma_base[];
extern xwu8_t itcm_vma_end[];
extern xwu8_t itcm_bss_vma_base[];
extern xwu8_t itcm_bss_vma_end[];

extern xwu8_t ocram_lma_base[];
extern xwu8_t ocram_vma_base[];
extern xwu8_t ocram_vma_end[];
extern xwu8_t ocram_bss_vma_base[];
extern xwu8_t ocram_bss_vma_end[];

extern xwu8_t ncache_lma_base[];
extern xwu8_t ncache_vma_base[];
extern xwu8_t ncache_vma_end[];
extern xwu8_t ncache_bss_vma_base[];
extern xwu8_t ncache_bss_vma_end[];

/**
 * @brief 重定向数据区到内存
 */
__xwbsp_init_code
void soc_relocate_data(void)
{
        xwsz_t cnt, i;
        xwu8_t * src;
        xwu8_t * dst;

        src = xwos_data_lma_base;
        dst = xwos_data_vma_base;
        if (dst != src) {
                cnt = (xwsz_t)xwos_data_vma_end - (xwsz_t)xwos_data_vma_base;
                for (i = 0; i < cnt; i++) {
                        dst[i] = src[i];
                }
        }

        src = data_lma_base;
        dst = data_vma_base;
        if (dst != src) {
                cnt = (xwsz_t)data_vma_end - (xwsz_t)data_vma_base;
                for (i = 0; i < cnt; i++) {
                        *dst = *src;
                        dst++;
                        src++;
                }
        }

        dst = bss_vma_base;
        cnt = (xwsz_t)bss_vma_end - (xwsz_t)bss_vma_base;
        for (i = 0; i < cnt; i++) {
                *dst = 0;
                dst++;
        }

        src = itcm_lma_base;
        dst = itcm_vma_base;
        if (dst != src) {
                cnt = (xwsz_t)itcm_vma_end - (xwsz_t)itcm_vma_base;
                for (i = 0; i < cnt; i++) {
                        *dst = *src;
                        dst++;
                        src++;
                }
        }

        dst = itcm_bss_vma_base;
        cnt = (xwsz_t)itcm_bss_vma_end - (xwsz_t)itcm_bss_vma_base;
        for (i = 0; i < cnt; i++) {
                *dst = 0;
                dst++;
        }

        src = ocram_lma_base;
        dst = ocram_vma_base;
        if (dst != src) {
                cnt = (xwsz_t)ocram_vma_end - (xwsz_t)ocram_vma_base;
                for (i = 0; i < cnt; i++) {
                        *dst = *src;
                        dst++;
                        src++;
                }
        }

        dst = ocram_bss_vma_base;
        cnt = (xwsz_t)ocram_bss_vma_end - (xwsz_t)ocram_bss_vma_base;
        for (i = 0; i < cnt; i++) {
                *dst = 0;
                dst++;
        }

        src = ncache_lma_base;
        dst = ncache_vma_base;
        if (dst != src) {
                cnt = (xwsz_t)ncache_vma_end - (xwsz_t)ncache_vma_base;
                for (i = 0; i < cnt; i++) {
                        *dst = *src;
                        dst++;
                        src++;
                }
        }

        dst = ncache_bss_vma_base;
        cnt = (xwsz_t)ncache_bss_vma_end - (xwsz_t)ncache_bss_vma_base;
        for (i = 0; i < cnt; i++) {
                *dst = 0;
                dst++;
        }
}

/**
 * @brief SOC的初始化
 */
__xwbsp_init_code
void soc_init(void)
{
}
