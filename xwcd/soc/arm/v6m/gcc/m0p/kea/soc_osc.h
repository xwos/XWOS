/**
 * @file
 * @brief SOC Oscillator
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

#ifndef __soc_osc_h__
#define __soc_osc_h__

#include <xwos/standard.h>
#include <soc.h>

#define SOC_CLKM_FEI            0
#define SOC_CLKM_FEE            1
#define SOC_CLKM_FBI            2
#define SOC_CLKM_FBILP          3
#define SOC_CLKM_FBE            4
#define SOC_CLKM_FBELP          5

struct soc_osc_reg {
        union {
                struct {
                        volatile const xwu8_t oscinit:1;
                        volatile xwu8_t hgo:1;
                        volatile xwu8_t range:1;
                        volatile xwu8_t reserved0:1;
                        volatile xwu8_t oscos:1;
                        volatile xwu8_t oscsten:1;
                        volatile xwu8_t reserved1:1;
                        volatile xwu8_t oscen:1;
                } bit;
                volatile xwu8_t byte;
        } cr;
};

#define SOC_OSC_BASE    (0x40065000U)
#define soc_osc         (*((volatile struct soc_osc_reg *)SOC_OSC_BASE))

#endif /* soc_osc.h */
