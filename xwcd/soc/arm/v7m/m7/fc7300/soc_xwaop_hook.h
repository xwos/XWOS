/**
 * @file
 * @brief SOC描述层：XWAOP HOOK
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

#ifndef __xwcd_soc_arm_v7m_m7_fc7300_soc_xwaop_hook_h__
#define __xwcd_soc_arm_v7m_m7_fc7300_soc_xwaop_hook_h__

#include <xwos/standard.h>
#include <xwcd/soc/arm/v7m/m7/fc7300/soc_mb.h>

#define SOC_XWAOP_BEGIN \
        do { \
                xwu32_t __soc_xwaop_mblkcode = soc_mb_lock(SOC_MB_CH_XWAOP); \
                if (__soc_xwaop_mblkcode > 0) {


#define SOC_XWAOP_END \
                        soc_mb_unlock(SOC_MB_CH_XWAOP, __soc_xwaop_mblkcode); \
                        break; \
                } \
        } while (true);

#endif /* xwcd/soc/arm/v7m/m7/fc7300/soc_xwaop_hook.h */
