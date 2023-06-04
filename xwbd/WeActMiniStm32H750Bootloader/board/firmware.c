/**
 * @file
 * @brief 板级描述层：固件描述信息
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

#include <xwos/standard.h>
#include <xwcd/soc/arm/v7m/arch_firmware.h>

extern const xwu8_t firmware_tail_lma_base[];
extern const xwu8_t firmware_tail_lma_end[];
extern const xwu8_t firmware_head_lma[];
extern void arch_isr_reset(void);
extern xwstk_t xwos_stk_top[];

__firmware_info
const struct firmware_info firmware_info = {
        .head = (void *)firmware_head_lma,
        .tail_flag_addr = (void *)firmware_tail_lma_base,
        .end_addr = (void *)firmware_tail_lma_end,
        .entry = arch_isr_reset,
        .sp = xwos_stk_top,
};

__firmware_tail
const struct firmware_tail firmware_tail = {
        .flag = BRDCFG_FIRMWARE_TAILFLAG,
};
