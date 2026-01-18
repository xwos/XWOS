/**
 * @file
 * @brief SOC描述层：调试
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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

#ifndef __xwcd_soc_arm_v8a_a72_bcm2711_soc_debug_h__
#define __xwcd_soc_arm_v8a_a72_bcm2711_soc_debug_h__

#include <xwos/standard.h>
#include <xwcd/soc/arm64/v8a/arch_isa.h>

void soc_logf(const char * fmt, ...);
void soc_panic(const char * fmt, ...);

#if defined(SOCCFG_DEBUG_LV) && (SOCCFG_DEBUG_LV >= 3U) && defined(SOC_DBGF)
#  define soc_dbgf(tag, fmt, ...) soc_logf("[" tag "] " fmt, ##__VA_ARGS__)
#else
#  define soc_dbgf(tag, fmt, ...)
#endif

#if defined(SOCCFG_DEBUG_LV) && (SOCCFG_DEBUG_LV >= 2U)
#  define soc_infof(tag, fmt, ...) soc_logf("[" tag "] " fmt, ##__VA_ARGS__)
#else
#  define soc_infof(tag, fmt, ...)
#endif

#if defined(SOCCFG_DEBUG_LV) && (SOCCFG_DEBUG_LV >= 1U)
#  define soc_warnf(tag, fmt, ...) soc_logf("[" tag "] " fmt, ##__VA_ARGS__)
#else
#  define soc_warnf(tag, fmt, ...)
#endif

#define soc_errf(tag, fmt, ...) soc_logf("[" tag "] " fmt, ##__VA_ARGS__)


#define soc_show_sysreg(reg) \
        ({                                                                        \
                __xw_io xwu64_t reg;                                              \
                armv8a_sysreg_read(&reg, reg);                                    \
                soc_dbgf("%s", "%s:%ld|0x%lX\r\n", __FUNCTION__, #reg, reg, reg); \
        })

#define soc_show_reg(reg) \
        ({                                                                        \
                __xw_io xwu64_t reg;                                              \
                armv8a_reg_read(&reg, reg);                                       \
                soc_dbgf("%s", "%s:%ld|0x%lX\r\n", __FUNCTION__, #reg, reg, reg); \
        })

#endif /* xwcd/soc/arm64/v8a/a72/bcm2711/soc_debug.h */
