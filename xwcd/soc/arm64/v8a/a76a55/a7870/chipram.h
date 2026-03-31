/**
 * @file
 * @brief SOC描述层：A7870 chipram evn
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

#ifndef __xwcd_soc_arm_v8a_a76a55_a7870_chipram_h__
#define __xwcd_soc_arm_v8a_a76a55_a7870_chipram_h__

#include <xwos/standard.h>

struct chipram_env {
        xwu32_t magic; //0x43454e56
        xwu32_t mode;
        xwu64_t dram_size;
        xwu64_t vol_para_addr;
        xwu32_t keep_charge;
        xwu32_t channel_sel;
        xwu32_t cs_number;
        xwu64_t cs0_size;
        xwu64_t cs1_size;
        xwu32_t tspl_s;
        xwu32_t tspl_e;
        xwu32_t spl_adjust_flag;
        xwu32_t reserved;
};

#define SOC_CHIPRAM_ENV_ADDR 0x82000000UL
#define soc_chipram_env (*((volatile struct chipram_env *)SOC_CHIPRAM_ENV_ADDR))

#endif /* xwcd/soc/arm64/v8a/a76a55/a7870/chipram.h */
