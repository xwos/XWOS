/**
 * @file
 * @brief SOC描述层：初始化
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

#ifndef __soc_init_h__
#define __soc_init_h__

#include <xwos/standard.h>

struct soc_flash_cfgs {
        xwu8_t backdoor_key[8]; /**< Backdoor Comparison Key 0 ~ 7, offset: 0x0 */
        xwu8_t reserved0[4];
        xwu8_t reserved1[1];
        xwu8_t fprot; /**< Non-volatile P-Flash Protection Register, offset: 0xD */
        xwu8_t fsec; /**< Non-volatile Flash Security Register, offset: 0xE */
        xwu8_t fopt; /**< Non-volatile Flash Option Register, offset: 0xF */
};

void soc_lowlevel_init(void);
void soc_init(void);

#endif /* soc_init.h */
