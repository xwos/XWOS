/**
 * @file
 * @brief 架构描述层：Firmware信息
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

#ifndef __xwcd_soc_arm_v6m_arch_firmware_h__
#define __xwcd_soc_arm_v6m_arch_firmware_h__

#include <xwos/standard.h>

struct firmware_info {
        void * head;
        void * tail_flag_addr;
        void * end_addr;
        void (* entry)(void);
        xwstk_t * sp;
};

struct __xwcc_aligned(32) firmware_tail {
        char flag[32];
};

xwer_t arch_boot_firmware(void * firmware, xwsz_t info_offset, const char * tailflag);

#endif /* xwcd/soc/arm/v6m/arch_firmware.h */
