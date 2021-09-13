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
#if ((defined(SOCCFG_CHIP_MPC5602D)) && (1 == SOCCFG_CHIP_MPC5602D))
  #include <mpc5602d.h>
  #define MPC5602D
#elif ((defined(SOCCFG_CHIP_MPC5602B)) && (1 == SOCCFG_CHIP_MPC5602B))
  #include <mpc5602b.h>
  #define MPC5602B
#elif ((defined(SOCCFG_CHIP_MPC5604B)) && (1 == SOCCFG_CHIP_MPC5604B))
  #include <mpc5604b.h>
  #define MPC5604B
#elif ((defined(SOCCFG_CHIP_MPC5606B)) && (1 == SOCCFG_CHIP_MPC5606B))
  #include <mpc5606b.h>
  #define MPC5606B
#elif ((defined(SOCCFG_CHIP_MPC5607B)) && (1 == SOCCFG_CHIP_MPC5607B))
  #include <mpc5607b.h>
  #define MPC5607B
#endif

/**
 * @defgroup soc_padsel
 * @brief Pad Selection for Multiplexed Inputs
 * @{
 */
#define SOC_PADSEL_CAN1RX_PCR35         0
#define SOC_PADSEL_CAN1RX_PCR43         1
#define SOC_PADSEL_CAN1RX_PCR95         2

#define SOC_PADSEL_CAN2RX_PCR73         0
#define SOC_PADSEL_CAN2RX_PCR89         1

#define SOC_PADSEL_CAN3RX_PCR36         0
#define SOC_PADSEL_CAN3RX_PCR73         1
#define SOC_PADSEL_CAN3RX_PCR89         2

#define SOC_PADSEL_CAN4RX_PCR35         0
#define SOC_PADSEL_CAN4RX_PCR43         1
#define SOC_PADSEL_CAN4RX_PCR95         2

#define SOC_PADSEL_CAN5RX_PCR64         0
#define SOC_PADSEL_CAN5RX_PCR97         1

#define SOC_PADSEL_SCK_0_PCR14          0
#define SOC_PADSEL_SCK_0_PCR15          1

#define SOC_PADSEL_CS0_0_PCR14          0
#define SOC_PADSEL_CS0_0_PCR15          1
#define SOC_PADSEL_CS0_0_PCR27          2

#define SOC_PADSEL_SCK_1_PCR34          0
#define SOC_PADSEL_SCK_1_PCR68          1
#define SOC_PADSEL_SCK_1_PCR114         2

#define SOC_PADSEL_SIN_1_PCR36          0
#define SOC_PADSEL_SIN_1_PCR66          1
#define SOC_PADSEL_SIN_1_PCR112         2

#define SOC_PADSEL_CS0_1_PCR35          0
#define SOC_PADSEL_CS0_1_PCR61          1
#define SOC_PADSEL_CS0_1_PCR69          2
#define SOC_PADSEL_CS0_1_PCR115         3

#define SOC_PADSEL_SCK_2_PCR46          0
#define SOC_PADSEL_SCK_2_PCR78          1
#define SOC_PADSEL_SCK_2_PCR105         2

#define SOC_PADSEL_SIN_2_PCR44          0
#define SOC_PADSEL_SIN_2_PCR76          1

#define SOC_PADSEL_CS0_2_PCR47          0
#define SOC_PADSEL_CS0_2_PCR79          1
#define SOC_PADSEL_CS0_2_PCR82          2
#define SOC_PADSEL_CS0_2_PCR104         3

#define SOC_PADSEL_E0UC3_PCR3           0
#define SOC_PADSEL_E0UC3_PCR27          1
#define SOC_PADSEL_E0UC3_PCR40          2

#define SOC_PADSEL_E0UC4_PCR4           0
#define SOC_PADSEL_E0UC4_PCR28          1

#define SOC_PADSEL_E0UC5_PCR5           0
#define SOC_PADSEL_E0UC5_PCR29          1

#define SOC_PADSEL_E0UC6_PCR6           0
#define SOC_PADSEL_E0UC6_PCR30          1

#define SOC_PADSEL_E0UC7_PCR7           0
#define SOC_PADSEL_E0UC7_PCR31          1
#define SOC_PADSEL_E0UC7_PCR41          2

#define SOC_PADSEL_E0UC10_PCR10         0
#define SOC_PADSEL_E0UC10_PCR80         1

#define SOC_PADSEL_E0UC11_PCR11         0
#define SOC_PADSEL_E0UC11_PCR81         1

#define SOC_PADSEL_E0UC12_PCR44         0
#define SOC_PADSEL_E0UC12_PCR82         1

#define SOC_PADSEL_E0UC13_PCR45         0
#define SOC_PADSEL_E0UC13_PCR83         1
#define SOC_PADSEL_E0UC13_PCR0          2

#define SOC_PADSEL_E0UC14_PCR46         0
#define SOC_PADSEL_E0UC14_PCR84         1
#define SOC_PADSEL_E0UC14_PCR8          2

#define SOC_PADSEL_E0UC22_PCR70         0
#define SOC_PADSEL_E0UC22_PCR72         1
#define SOC_PADSEL_E0UC22_PCR85         2

#define SOC_PADSEL_E0UC23_PCR71         0
#define SOC_PADSEL_E0UC23_PCR73         1
#define SOC_PADSEL_E0UC23_PCR86         2

#define SOC_PADSEL_E0UC24_PCR60         0
#define SOC_PADSEL_E0UC24_PCR106        1
#define SOC_PADSEL_E0UC24_PCR75         2

#define SOC_PADSEL_E0UC25_PCR61         0
#define SOC_PADSEL_E0UC25_PCR107        1

#define SOC_PADSEL_E0UC26_PCR62         0
#define SOC_PADSEL_E0UC26_PCR108        1

#define SOC_PADSEL_E0UC27_PCR63         0
#define SOC_PADSEL_E0UC27_PCR109        1

#define SOC_PADSEL_SCL_PCR11            0
#define SOC_PADSEL_SCL_PCR19            1

#define SOC_PADSEL_SDA_PCR10            0
#define SOC_PADSEL_SDA_PCR18            1

#define SOC_PADSEL_LIN3RX_PCR8          0
#define SOC_PADSEL_LIN3RX_PCR75         1

#define SOC_PADSEL_SCK3_PCR100          0
#define SOC_PADSEL_SCK3_PCR124          0

#define SOC_PADSEL_SIN3_PCR101          0
#define SOC_PADSEL_SIN3_PCR139          1

#define SOC_PADSEL_CS0_3_PCR99          0
#define SOC_PADSEL_CS0_3_PCR125         1
#define SOC_PADSEL_CS0_3_PCR140         2

#define SOC_PADSEL_SCK_4_PCR109         0
#define SOC_PADSEL_SCK_4_PCR126         1
#define SOC_PADSEL_SCK_4_PCR133         2

#define SOC_PADSEL_SIN_4_PCR106         0
#define SOC_PADSEL_SIN_4_PCR142         1

#define SOC_PADSEL_CS0_4_PCR107         0
#define SOC_PADSEL_CS0_4_PCR123         1
#define SOC_PADSEL_CS0_4_PCR134         2
#define SOC_PADSEL_CS0_4_PCR143         3

#define SOC_PADSEL_E0UC0_PCR0           0
#define SOC_PADSEL_E0UC0_PCR14          1

#define SOC_PADSEL_E0UC1_PCR1           0
#define SOC_PADSEL_E0UC1_PCR15          1

#define SOC_PADSEL_E0UC28_PCR2          0
#define SOC_PADSEL_E0UC28_PCR128        1

#define SOC_PADSEL_E0UC29_PCR13         0
#define SOC_PADSEL_E0UC29_PCR129        1

#define SOC_PADSEL_E0UC30_PCR16         0
#define SOC_PADSEL_E0UC30_PCR18         1
#define SOC_PADSEL_E0UC30_PCR130        2

#define SOC_PADSEL_E0UC31_PCR17         0
#define SOC_PADSEL_E0UC31_PCR19         1
#define SOC_PADSEL_E0UC31_PCR131        2

#define SOC_PADSEL_E1UC1_PCR111         0
#define SOC_PADSEL_E1UC1_PCR89          1

#define SOC_PADSEL_E1UC2_PCR112         0
#define SOC_PADSEL_E1UC2_PCR90          1

#define SOC_PADSEL_E1UC3_PCR113         0
#define SOC_PADSEL_E1UC3_PCR91          1

#define SOC_PADSEL_E1UC4_PCR114         0
#define SOC_PADSEL_E1UC4_PCR95          1

#define SOC_PADSEL_E1UC5_PCR115         0
#define SOC_PADSEL_E1UC5_PCR123         1

#define SOC_PADSEL_E1UC17_PCR104        0
#define SOC_PADSEL_E1UC17_PCR127        1

#define SOC_PADSEL_E1UC18_PCR105        0
#define SOC_PADSEL_E1UC18_PCR148        1

#define SOC_PADSEL_E1UC25_PCR92         0
#define SOC_PADSEL_E1UC25_PCR124        1

#define SOC_PADSEL_E1UC26_PCR93         0
#define SOC_PADSEL_E1UC26_PCR125        1

#define SOC_PADSEL_E1UC27_PCR94         0
#define SOC_PADSEL_E1UC27_PCR126        1

#define SOC_PADSEL_E1UC28_PCR38         0
#define SOC_PADSEL_E1UC28_PCR132        1

#define SOC_PADSEL_E1UC29_PCR39         0
#define SOC_PADSEL_E1UC29_PCR133        1

#define SOC_PADSEL_E1UC30_PCR74         0
#define SOC_PADSEL_E1UC30_PCR103        1
#define SOC_PADSEL_E1UC30_PCR134        2

#define SOC_PADSEL_E1UC31_PCR36         0
#define SOC_PADSEL_E1UC31_PCR106        1
#define SOC_PADSEL_E1UC31_PCR135        2

#define SOC_PADSEL_LIN2RX_PCR41         0
#define SOC_PADSEL_LIN2RX_PCR11         1

#define SOC_PADSEL_LIN4RX_PCR6          0
#define SOC_PADSEL_LIN4RX_PCR91         1

#define SOC_PADSEL_LIN5RX_PCR4          0
#define SOC_PADSEL_LIN5RX_PCR93         1

#define SOC_PADSEL_LIN8RX_PCR111        0
#define SOC_PADSEL_LIN8RX_PCR129        1

#define SOC_PADSEL_LIN0RX_PCR19         0
#define SOC_PADSEL_LIN0RX_PCR17         1
/**
 * @} end of soc_padsel
 */

#define SDL_BUG()       XWOS_BUG()
#define SDL_BUG_ON(x)   XWOS_BUG_ON(x)

/**
 * @addtogroup soc_padsel
 * @{
 */
enum soc_padsel_em {
        SOC_PADSEL0_CAN1RX = 0,
        SOC_PADSEL1_CAN2RX,
        SOC_PADSEL2_CAN3RX,
        SOC_PADSEL3_CAN4RX,
        SOC_PADSEL4_CAN5RX,
        SOC_PADSEL5_SCK_0,
        SOC_PADSEL6_CS0_0,
        SOC_PADSEL7_SCK_1,
        SOC_PADSEL8_SIN_1,
        SOC_PADSEL9_CS0_1,
        SOC_PADSEL10_SCK_2,
        SOC_PADSEL11_SIN_2,
        SOC_PADSEL12_CS0_2,
        SOC_PADSEL13_E0UC3,
        SOC_PADSEL14_E0UC4,
        SOC_PADSEL15_E0UC5,
        SOC_PADSEL16_E0UC6,
        SOC_PADSEL17_E0UC7,
        SOC_PADSEL18_E0UC10,
        SOC_PADSEL19_E0UC11,
        SOC_PADSEL20_E0UC12,
        SOC_PADSEL21_E0UC13,
        SOC_PADSEL22_E0UC14,
        SOC_PADSEL23_E0UC22,
        SOC_PADSEL24_E0UC23,
        SOC_PADSEL25_E0UC24,
        SOC_PADSEL26_E0UC25,
        SOC_PADSEL27_E0UC26,
        SOC_PADSEL28_E0UC27,
        SOC_PADSEL29_SCL,
        SOC_PADSEL30_SDA,
        SOC_PADSEL31_LIN3RX,
#ifdef MPC5607B
        SOC_PADSEL32_SCK_3,
        SOC_PADSEL33_SIN_3,
        SOC_PADSEL34_CS0_3,
        SOC_PADSEL35_SCK_4,
        SOC_PADSEL36_SIN_4,
        SOC_PADSEL37_CS0_4,
        SOC_PADSEL38_E0UC0,
        SOC_PADSEL39_E0UC1,
        SOC_PADSEL40_E0UC28,
        SOC_PADSEL41_E0UC29,
        SOC_PADSEL42_E0UC30,
        SOC_PADSEL43_E0UC31,
        SOC_PADSEL44_E1UC1,
        SOC_PADSEL45_E1UC2,
        SOC_PADSEL46_E1UC3,
        SOC_PADSEL47_E1UC4,
        SOC_PADSEL48_E1UC5,
        SOC_PADSEL49_E1UC17,
        SOC_PADSEL50_E1UC18,
        SOC_PADSEL51_E1UC25,
        SOC_PADSEL52_E1UC26,
        SOC_PADSEL53_E1UC27,
        SOC_PADSEL54_E1UC28,
        SOC_PADSEL55_E1UC29,
        SOC_PADSEL56_E1UC30,
        SOC_PADSEL57_E1UC31,
        SOC_PADSEL58_LIN2RX,
        SOC_PADSEL59_LIN4RX,
        SOC_PADSEL60_LIN5RX,
        SOC_PADSEL61_LIN8RX,
        SOC_PADSEL62_LIN0RX,
#endif
        SOC_PADSEL_NUM,
};

/**
 * @} end of soc_padsel
 */

/**
 * @brief pad functions selection
 */
struct soc_sys_cfg {
        xwu8_t padsel[SOC_PADSEL_NUM];
};

/**
 * @brief SOC reset reason
 */
struct soc_reset_flags {
        xwu16_t fes; /**< Functional Event Status */
        xwu16_t des; /**< Destructive Event Status */
};

/**
 * @brief SOC wakeup flag
 */
struct soc_wkup_flags {
        xwu32_t nsr; /**< non-maskable IRQ */
        xwu32_t wisr; /**< wakeup status flags */
};

extern __xwbsp_data struct soc_reset_flags soc_reset_flags;
extern __xwbsp_data struct soc_wkup_flags soc_wkup_flags;

#endif /* soc.h */
