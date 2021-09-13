/**
 * @file
 * @brief 玄武OS移植层：SOC中断
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_ospl_soc_irq_h__
#define __xwos_ospl_soc_irq_h__

#include <xwos/standard.h>

/**
 * @brief 中断向量表限定词
 */
#if defined(SOCCFG_RO_IVT) && (1 == SOCCFG_RO_IVT)
#  define __xwos_ivt_qualifier const
#else
#  define __xwos_ivt_qualifier
#endif

struct soc_irq_cfg;
struct soc_irq_data;
struct soc_isr_table;
struct soc_isr_data_table;

#include <xwosimpl_soc_irq.h>

#endif /* xwos/ospl/soc/irq.h */
