/**
 * @file
 * @brief SOC Internal Clock Source
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

#ifndef __soc_ics_h__
#define __soc_ics_h__

#include <xwos/standard.h>
#include <soc.h>

#define SOC_CLKM_FEI            0
#define SOC_CLKM_FEE            1
#define SOC_CLKM_FBI            2
#define SOC_CLKM_FBILP          3
#define SOC_CLKM_FBE            4
#define SOC_CLKM_FBELP          5

struct soc_ics_reg {
        union {
                struct {
                        volatile xwu8_t irefsten:1;
                        volatile xwu8_t irclken:1;
                        volatile xwu8_t irefs:1;
                        volatile xwu8_t rdiv:3;
                        volatile xwu8_t clks:2;
                } bit;
                volatile xwu8_t byte;
        } c1;
        union {
                struct {
                        volatile xwu8_t reserved0:4;
                        volatile xwu8_t lp:1;
                        volatile xwu8_t bdiv:3;
                } bit;
                volatile xwu8_t byte;
        } c2;
        union {
                struct {
                        volatile xwu8_t sctrim:8;
                } bit;
                volatile xwu8_t byte;
        } c3;
        union {
                struct {
                        volatile xwu8_t scftrim:1;
                        volatile xwu8_t reserved0:4;
                        volatile xwu8_t cme:1;
                        volatile xwu8_t reserved1:1;
                        volatile xwu8_t lolie:1;
                } bit;
                volatile xwu8_t byte;
        } c4;
        union {
                struct {
                        volatile xwu8_t reserved0:2;
                        volatile xwu8_t clkst:2;
                        volatile xwu8_t irefst:1;
                        volatile xwu8_t reserved1:1;
                        volatile xwu8_t lock:1;
                        volatile xwu8_t lols:1;
                } bit;
                volatile xwu8_t byte;
        } s;
};

#define SOC_ICS_BASE    (0x40064000U)
#define soc_ics         (*((volatile struct soc_ics_reg *)SOC_ICS_BASE))

#endif /* soc_ics.h */
