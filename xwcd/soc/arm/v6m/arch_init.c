/**
 * @file
 * @brief 架构描述层：初始化
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

#include <xwcd/soc/arm/v6m/arch_init.h>
#include <xwcd/soc/arm/v6m/armv6m_isa.h>
#include <xwcd/soc/arm/v6m/arch_irq.h>

/**
 * @brief Init architecture
 */
__xwbsp_init_code
void arch_init(void)
{
        armv6m_scs.scb.ccr.bit.stkalign = 1;
        armv6m_scs.scb.ccr.bit.unalign_trp = 1; /* enable unalign trap */
        arch_init_sysirqs();
}
