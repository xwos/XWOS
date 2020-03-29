/**
 * @file
 * @brief 玄武OS的初始化
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#if defined(XuanWuOS_CFG_CORE__smp)
  #include <xwos/smp/init.h>
#elif defined(XuanWuOS_CFG_CORE__up)
  #include <xwos/up/init.h>
#else
  #error "Can't find the configuration XuanWuOS_CFG_CORE!"
#endif
#include <xwos/mm/kma.h>
#include <xwos/init.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern xwu8_t xwos_data_lma_base[];
extern xwu8_t xwos_data_vma_base[];
extern xwu8_t xwos_data_vma_end[];

extern xwu8_t xwmd_data_lma_base[];
extern xwu8_t xwmd_data_vma_base[];
extern xwu8_t xwmd_data_vma_end[];

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********       function implementations      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 重定向操作系统数据
 */
__xwos_init_code
void xwos_relocate(void)
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

        src = xwmd_data_lma_base;
        dst = xwmd_data_vma_base;
        if (dst != src) {
                cnt = (xwsz_t)xwmd_data_vma_end - (xwsz_t)xwmd_data_vma_base;
                for (i = 0; i < cnt; i++) {
                        dst[i] = src[i];
                }
        }
}

/**
 * @brief 初始化XWOS内核
 */
__xwos_init_code
void xwos_init(void)
{
        /* 将数据(.xwos.data)从flash中重定向到内存 */
        xwos_relocate();

        /* 初始化KMA */
        xwmm_kma_init();

#if defined(XuanWuOS_CFG_CORE__smp)
        xwos_smp_init();
#elif defined(XuanWuOS_CFG_CORE__up)
        xwos_up_init();
#endif
}
