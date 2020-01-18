/**
 * @file
 * @brief S32K14x SOC
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#if ((defined(SOCCFG_CHIP_S32K142)) && (1 == SOCCFG_CHIP_S32K142))
  #define SOC_CHIP_S32K         142
  #include <S32K142.h>
  #include <S32K142_features.h>
#elif ((defined(SOCCFG_CHIP_S32K144)) && (1 == SOCCFG_CHIP_S32K144))
  #define SOC_CHIP_S32K         144
  #include <S32K144.h>
  #include <S32K144_features.h>
#elif ((defined(SOCCFG_CHIP_S32K146)) && (1 == SOCCFG_CHIP_S32K146))
  #define SOC_CHIP_S32K         146
  #include <S32K146.h>
  #include <S32K146_features.h>
#elif ((defined(SOCCFG_CHIP_S32K148)) && (1 == SOCCFG_CHIP_S32K148))
  #define SOC_CHIP_S32K         148
  #include <S32K148.h>
  #include <S32K148_features.h>
#else
  #error "Can't find chip cfg in soc_config.h!"
#endif

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define __flscfg __section(".flash.cfg")

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct soc_sys_cfg {
        struct {
                union {
                        struct {
                                xwu32_t adc_interleave_en:4;
                                xwu32_t clkoutsel:4;
                                xwu32_t clkoutdiv:3;
                                xwu32_t clkouten:1;
                                xwu32_t traceclk_sel:1;
                                xwu32_t pdb_bb_sel:1;
                                xwu32_t Reserved14_15:2;
                                xwu32_t adc_supply:3;
                                xwu32_t adc_supplyen:1;
                                xwu32_t sramu_reten:1;
                                xwu32_t sraml_reten:1;
                                xwu32_t Reserved22_23:2;
                                xwu32_t Reserved24_31:8;
                        } bit;
                        xwu32_t u32;
                } chipctl;
        } sim;
        struct {
                union {
                        struct {
                                xwu32_t stopm:3;
                                xwu32_t reserved3_4:2;
                                xwu32_t runm:2;
                                xwu32_t reserved7_31:25;
                        } bit;
                        xwu32_t u32;
                } pmctrl;
                union {
                        struct {
                                xwu32_t reserved0_5:6;
                                xwu32_t stopo:2;
                                xwu32_t reserved8_31:24;
                        } bit;
                        xwu32_t u32;
                } stopctrl;
        } smc;
        struct {
                union {
                        struct {
                                xwu8_t reserved0_3:4;
                                xwu8_t lvdre:1;
                                xwu8_t lvdie:1;
                                xwu8_t reserved6_7:2;
                        } bit;
                        xwu8_t u8;
                } lvdsc1;
                union {
                        struct {
                                xwu8_t reserved0_4:5;
                                xwu8_t lvwie:1;
                                xwu8_t reserved6_7:2;
                        } bit;
                        xwu8_t u8;
                } lvdsc2;
                union {
                        struct {
                                xwu8_t biasen:1;
                                xwu8_t clkbiasdis:1;
                                xwu8_t reserved2_6:5;
                                xwu8_t lpodis:1;
                        } bit;
                        xwu8_t u8;
                } regsc;
        } pmc;
};

#endif /* soc.h */
