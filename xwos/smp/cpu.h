/**
 * @file
 * @brief XuanWuOS内核：CPU
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_smp_cpu_h__
#define __xwos_smp_cpu_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <arch_cpuid.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macors      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWOS_CPU_NUM            ((xwid_t)CPUCFG_CPU_NUM)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOS API：获取当前CPU的ID
 * @return 当前CPU的ID
 */
static __xwos_inline_api
xwid_t xwos_cpu_get_id(void)
{
        return arch_cpu_get_id();
}

#endif /* xwos/smp/cpu.h */
