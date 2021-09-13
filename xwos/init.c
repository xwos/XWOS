/**
 * @file
 * @brief 玄武OS的初始化
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#if defined(XuanWuOS_CFG_CORE__mp)
#  include <xwos/mp/init.h>
#elif defined(XuanWuOS_CFG_CORE__up)
#  include <xwos/up/init.h>
#else
#  error "Can't find the configuration XuanWuOS_CFG_CORE!"
#endif
#include <xwos/lib/object.h>
#include <xwos/mm/kma.h>
#include <xwos/init.h>

#if defined(XWKNCFG_RELOCATE_DATA) && (1 == XWKNCFG_RELOCATE_DATA)
extern xwu8_t xwos_data_lma_base[];
extern xwu8_t xwos_data_vma_base[];
extern xwu8_t xwos_data_vma_end[];

/**
 * @brief 重定向操作系统数据
 */
static __xwos_init_code
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
}
#endif

/**
 * @brief 初始化XWOS内核
 */
__xwos_init_code
void xwos_init(void)
{
#if defined(XWKNCFG_RELOCATE_DATA) && (1 == XWKNCFG_RELOCATE_DATA)
        /* 将数据(.xwos.data)从flash中重定向到内存 */
        xwos_relocate();
#endif

        /* 初始化对象标签分配器 */
        xwos_objtik_init();

        /* 初始化KMA */
        xwmm_kma_init();

#if defined(XuanWuOS_CFG_CORE__mp)
        xwmp_init();
#elif defined(XuanWuOS_CFG_CORE__up)
        xwup_init();
#endif
}
