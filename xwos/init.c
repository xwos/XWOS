/**
 * @file
 * @brief 玄武OS内核的初始化
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#if defined(XWOS_CFG_CORE__mp)
#  include <xwos/mp/init.h>
#elif defined(XWOS_CFG_CORE__up)
#  include <xwos/up/init.h>
#else
#  error "Can't find the configuration XWOS_CFG_CORE!"
#endif
#include <xwos/lib/object.h>
#include <xwos/mm/kma.h>
#include <xwos/init.h>

/**
 * @brief 初始化XWOS内核
 */
__xwos_init_code
void xwos_init(void)
{
        /* 初始化对象标签分配器 */
        xwos_objtik_init();

        /* 初始化KMA */
        xwmm_kma_init();

        /* 初始化内核 */
#if defined(XWOS_CFG_CORE__mp)
        xwmp_init();
#elif defined(XWOS_CFG_CORE__up)
        xwup_init();
#endif
}
