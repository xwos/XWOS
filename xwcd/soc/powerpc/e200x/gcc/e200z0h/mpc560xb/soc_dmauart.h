/**
 * @file
 * @brief SOC描述层：DMAUART
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

#ifndef __soc_dmauart_h__
#define __soc_dmauart_h__

#include <xwos/standard.h>
#include <soc.h>

/**
 * @brief mpc560xb DMA UART private configurations
 */
struct soc_dmauart_private_cfg {
        xwu32_t pto; /**< preset timeout */
};


#endif /* soc_dmauart.h */
