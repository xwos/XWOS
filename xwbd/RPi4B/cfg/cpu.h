/**
 * @file
 * @brief CPU描述层(CDL)配置
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

#ifndef __cfg_cpu_h__
#define __cfg_cpu_h__

#define CPUCFG_SVE                              0
#define CPUCFG_SME                              0
#define CPUCFG_CPU_NUM                          (4U)
#define CPUCFG_MAIN_CPU                         (0U)
#define CPUCFG_CPU_STACK_SIZE                   (8192U)
#define CPUCFG_L1_CACHELINE_SIZE                (64U)

#endif /* cfg/cpu.h */
