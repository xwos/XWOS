/**
 * @file
 * @brief SOC描述层：SOC
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

#ifndef __soc_h__
#define __soc_h__

#include <xwos/standard.h>

#define __flscfg        __xwcc_section(".flash.cfg")

struct soc_flash_cfgs {
        xwu8_t backdoor_key[8];
        xwu8_t fprot[4];
        xwu8_t fsec;
        xwu8_t fopt;
        xwu8_t feprot;
        xwu8_t fdprot;
};

#endif /* soc.h */
