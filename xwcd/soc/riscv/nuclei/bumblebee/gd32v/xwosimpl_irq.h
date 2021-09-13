/**
 * @file
 * @brief 玄武OS移植实现层：中断
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

#ifndef __xwosimpl_irq_h__
#define __xwosimpl_irq_h__

#ifndef __xwos_ospl_irq_h__
  #error "This file should be included from <xwos/ospl/irq.h>."
#endif

static __xwbsp_inline
void xwospl_cpuirq_enable_lc(void)
{
        soc_cpuirq_enable_lc();
}

static __xwbsp_inline
void xwospl_cpuirq_disable_lc(void)
{
        soc_cpuirq_disable_lc();
}

static __xwbsp_inline
void xwospl_cpuirq_restore_lc(xwreg_t cpuirq)
{
        soc_cpuirq_restore_lc(cpuirq);
}

static __xwbsp_inline
void xwospl_cpuirq_save_lc(xwreg_t * cpuirq)
{
        soc_cpuirq_save_lc(cpuirq);
}

#endif /* xwosimpl_irq.h */
