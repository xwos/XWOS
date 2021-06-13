/**
 * @file
 * @brief 玄武OS移植实现层：SOC调试断点
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

#ifndef __xwosimpl_soc_bkpt_h__
#define __xwosimpl_soc_bkpt_h__

#ifndef __xwos_ospl_soc_bkpt_h__
  #error "This file should be included from <xwos/ospl/soc/bkpt.h>."
#endif

#include <compiler/gcc.h>

static __xwbsp_inline
void soc_bkpt(void)
{
        __asm__ volatile("bkpt  0\n");
}

#endif /* xwosimpl_soc_bkpt.h */
