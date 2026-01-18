/**
 * @file
 * @brief SOC描述层：终端
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

#ifndef __xwcd_soc_arm_v8a_a72_bcm2711_soc_console_h__
#define __xwcd_soc_arm_v8a_a72_bcm2711_soc_console_h__

#include <xwos/standard.h>

void soc_console_init(void);
void soc_console_puts(const char * s);
xwsz_t soc_console_gets(char * buf, xwsz_t bufsz);
void soc_console_tx(const xwu8_t * d, xwsz_t size);
xwsz_t soc_console_rx(char * buf, xwsz_t bufsz);

#endif /* xwcd/soc/arm64/v8a/a72/bcm2711/soc_console.h */
