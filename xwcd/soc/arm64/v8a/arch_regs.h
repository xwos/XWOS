/**
 * @file
 * @brief 架构描述层：寄存器
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

#ifndef __xwcd_soc_arm_v8a_arch_regs_h__
#define __xwcd_soc_arm_v8a_arch_regs_h__

#include <xwos/standard.h>

#define ARMV8A_SCR_TWE          (1U << 13U)
#define ARMV8A_SCR_TWI          (1U << 12U)
#define ARMV8A_SCR_ST           (1U << 11U)
#define ARMV8A_SCR_RW           (1U << 10U)
#define ARMV8A_SCR_SIF          (1U << 9U)
#define ARMV8A_SCR_HCE          (1U << 8U)
#define ARMV8A_SCR_SMD          (1U << 7U)
#define ARMV8A_SCR_RES1_5       (1U << 5U)
#define ARMV8A_SCR_RES1_4       (1U << 4U)
#define ARMV8A_SCR_EA           (1U << 3U)
#define ARMV8A_SCR_FIQ          (1U << 2U)
#define ARMV8A_SCR_IRQ          (1U << 1U)
#define ARMV8A_SCR_NS           (1U << 0U)

#define ARMV8A_SCTLR_EL3_RES    (0x30C50830U)
#define ARMV8A_SCTLR_EL2_RES    (0x30C50830U)
#define ARMV8A_SCTLR_EL1_RES    ((3U << 28U) | (3U << 22U) | (1U << 20U) | (1U << 11U))
#define ARMV8A_SCTLR_EE         (1U << 25U)
#define ARMV8A_SCTLR_WXN        (1U << 19U)
#define ARMV8A_SCTLR_I          (1U << 12U)
#define ARMV8A_SCTLR_SA         (1U << 3U)
#define ARMV8A_SCTLR_C          (1U << 2U)
#define ARMV8A_SCTLR_A          (1U << 1U)
#define ARMV8A_SCTLR_M          (1U << 0U)

#define ARMV8A_SPSR_D           (1U << 9U)
#define ARMV8A_SPSR_A           (1U << 8U)
#define ARMV8A_SPSR_I           (1U << 7U)
#define ARMV8A_SPSR_F           (1U << 6U)
#define ARMV8A_SPSR_M4          (1U << 4U)
#define ARMV8A_SPSR_EL3H        0xDU
#define ARMV8A_SPSR_EL3T        0xCU
#define ARMV8A_SPSR_EL2H        0x9U
#define ARMV8A_SPSR_EL2T        0x8U
#define ARMV8A_SPSR_EL1H        0x5U
#define ARMV8A_SPSR_EL1T        0x4U
#define ARMV8A_SPSR_EL0T        0x0U
#define ARMV8A_SPSR_EL(spsr)    (((spsr) & 0xCU) >> 2U)
#define ARMV8A_SPSR_SPSEL(spsr) ((spsr) & 0x1U)

#define ARMV8A_HCR_VM           (1U << 0U)
#define ARMV8A_HCR_SWIO         (1U << 1U)
#define ARMV8A_HCR_PTW          (1U << 2U)
#define ARMV8A_HCR_FMO          (1U << 3U)
#define ARMV8A_HCR_IMO          (1U << 4U)
#define ARMV8A_HCR_AMO          (1U << 5U)
#define ARMV8A_HCR_DC           (1U << 12U)
#define ARMV8A_HCR_TWI          (1U << 13U)
#define ARMV8A_HCR_TWE          (1U << 14U)
#define ARMV8A_HCR_TID0         (1U << 15U)
#define ARMV8A_HCR_TID1         (1U << 16U)
#define ARMV8A_HCR_TID2         (1U << 17U)
#define ARMV8A_HCR_TID3         (1U << 18U)
#define ARMV8A_HCR_TSC          (1U << 19U)
#define ARMV8A_HCR_TIDCP        (1U << 20U)
#define ARMV8A_HCR_TACR         (1U << 21U)
#define ARMV8A_HCR_TSW          (1U << 22U)
#define ARMV8A_HCR_TPC          (1U << 23U)
#define ARMV8A_HCR_TPU          (1U << 24U)
#define ARMV8A_HCR_TTLB         (1U << 25U)
#define ARMV8A_HCR_TVM          (1U << 26U)
#define ARMV8A_HCR_TGE          (1U << 27U)
#define ARMV8A_HCR_TDZ          (1U << 28U)
#define ARMV8A_HCR_HCD          (1U << 29U)
#define ARMV8A_HCR_TRVM         (1U << 30U)
#define ARMV8A_HCR_RW           (1U << 31U)
#define ARMV8A_HCR_CD           (1U << 32U)
#define ARMV8A_HCR_ID           (1U << 33U)
#define ARMV8A_HCR_MIOCNCE      (1U << 38U)

#define ARMV8A_CPACR_EL1_SMEN   ((1U << 25U) | (1U << 24U))
#define ARMV8A_CPACR_EL1_FPEN   ((1U << 21U) | (1U << 20U))
#define ARMV8A_CPACR_EL1_ZEN    ((1U << 17U) | (1U << 16U))

#endif /* xwcd/soc/arm64/v8a/arch_regs.h */
