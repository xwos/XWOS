/**
 * @file
 * @brief 架构描述层：寄存器
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

#ifndef __xwcd_soc_arm_v8a_arch_regs_h__
#define __xwcd_soc_arm_v8a_arch_regs_h__

#include <xwos/standard.h>

#define ARMV8A_SCR_TWE          (1UL << 13UL)
#define ARMV8A_SCR_TWI          (1UL << 12UL)
#define ARMV8A_SCR_ST           (1UL << 11UL)
#define ARMV8A_SCR_RW           (1UL << 10UL)
#define ARMV8A_SCR_SIF          (1UL << 9UL)
#define ARMV8A_SCR_HCE          (1UL << 8UL)
#define ARMV8A_SCR_SMD          (1UL << 7UL)
#define ARMV8A_SCR_RES1_5       (1UL << 5UL)
#define ARMV8A_SCR_RES1_4       (1UL << 4UL)
#define ARMV8A_SCR_EA           (1UL << 3UL)
#define ARMV8A_SCR_FIQ          (1UL << 2UL)
#define ARMV8A_SCR_IRQ          (1UL << 1UL)
#define ARMV8A_SCR_NS           (1UL << 0UL)

#define ARMV8A_SCTLR_EL3_RES1   (0x30850030UL)
#define ARMV8A_SCTLR_EE         (1UL << 25UL)
#define ARMV8A_SCTLR_WXN        (1UL << 19UL)
#define ARMV8A_SCTLR_I          (1UL << 12UL)
#define ARMV8A_SCTLR_SA         (1UL << 3UL)
#define ARMV8A_SCTLR_C          (1UL << 2UL)
#define ARMV8A_SCTLR_A          (1UL << 1UL)
#define ARMV8A_SCTLR_M          (1UL << 0UL)

#define ARMV8A_SPSR_D           (1UL << 9UL)
#define ARMV8A_SPSR_A           (1UL << 8UL)
#define ARMV8A_SPSR_I           (1UL << 7UL)
#define ARMV8A_SPSR_F           (1UL << 6UL)
#define ARMV8A_SPSR_M4          (1UL << 4UL)
#define ARMV8A_SPSR_EL3H        0xDUL
#define ARMV8A_SPSR_EL3T        0xCUL
#define ARMV8A_SPSR_EL2H        0x9UL
#define ARMV8A_SPSR_EL2T        0x8UL
#define ARMV8A_SPSR_EL1H        0x5UL
#define ARMV8A_SPSR_EL1T        0x4UL
#define ARMV8A_SPSR_EL0T        0x0UL
#define ARMV8A_SPSR_EL(spsr)    (((spsr) & 0xCUL) >> 2UL)
#define ARMV8A_SPSR_SPSEL(spsr) ((spsr) & 0x1UL)

#define ARMV8A_HCR_VM           (1UL << 0UL)
#define ARMV8A_HCR_SWIO         (1UL << 1UL)
#define ARMV8A_HCR_PTW          (1UL << 2UL)
#define ARMV8A_HCR_FMO          (1UL << 3UL)
#define ARMV8A_HCR_IMO          (1UL << 4UL)
#define ARMV8A_HCR_AMO          (1UL << 5UL)
#define ARMV8A_HCR_DC           (1UL << 12UL)
#define ARMV8A_HCR_TWI          (1UL << 13UL)
#define ARMV8A_HCR_TWE          (1UL << 14UL)
#define ARMV8A_HCR_TID0         (1UL << 15UL)
#define ARMV8A_HCR_TID1         (1UL << 16UL)
#define ARMV8A_HCR_TID2         (1UL << 17UL)
#define ARMV8A_HCR_TID3         (1UL << 18UL)
#define ARMV8A_HCR_TSC          (1UL << 19UL)
#define ARMV8A_HCR_TIDCP        (1UL << 20UL)
#define ARMV8A_HCR_TACR         (1UL << 21UL)
#define ARMV8A_HCR_TSW          (1UL << 22UL)
#define ARMV8A_HCR_TPC          (1UL << 23UL)
#define ARMV8A_HCR_TPU          (1UL << 24UL)
#define ARMV8A_HCR_TTLB         (1UL << 25UL)
#define ARMV8A_HCR_TVM          (1UL << 26UL)
#define ARMV8A_HCR_TGE          (1UL << 27UL)
#define ARMV8A_HCR_TDZ          (1UL << 28UL)
#define ARMV8A_HCR_HCD          (1UL << 29UL)
#define ARMV8A_HCR_TRVM         (1UL << 30UL)
#define ARMV8A_HCR_RW           (1UL << 31UL)
#define ARMV8A_HCR_CD           (1UL << 32UL)
#define ARMV8A_HCR_ID           (1UL << 33UL)
#define ARMV8A_HCR_E2H          (1UL << 34UL)
#define ARMV8A_HCR_TLOR         (1UL << 35UL)
#define ARMV8A_HCR_TERR         (1UL << 36UL)
#define ARMV8A_HCR_TEA          (1UL << 37UL)
#define ARMV8A_HCR_MIOCNCE      (1UL << 38UL)
#define ARMV8A_HCR_TME          (1UL << 39UL)
#define ARMV8A_HCR_APK          (1UL << 40UL)
#define ARMV8A_HCR_API          (1UL << 41UL)
#define ARMV8A_HCR_NV           (1UL << 42UL)
#define ARMV8A_HCR_NV1          (1UL << 43UL)
#define ARMV8A_HCR_AT           (1UL << 44UL)
#define ARMV8A_HCR_NV2          (1UL << 45UL)
#define ARMV8A_HCR_FWB          (1UL << 46UL)
#define ARMV8A_HCR_FIEN         (1UL << 47UL)
#define ARMV8A_HCR_GPF          (1UL << 48UL)
#define ARMV8A_HCR_TID4         (1UL << 49UL)
#define ARMV8A_HCR_TICAB        (1UL << 50UL)
#define ARMV8A_HCR_AMVOFFEN     (1UL << 51UL)
#define ARMV8A_HCR_TOCU         (1UL << 52UL)
#define ARMV8A_HCR_ENSCXT       (1UL << 53UL)
#define ARMV8A_HCR_TTLBIS       (1UL << 54UL)
#define ARMV8A_HCR_TTLBOS       (1UL << 55UL)
#define ARMV8A_HCR_ATA          (1UL << 56UL)
#define ARMV8A_HCR_DCT          (1UL << 57UL)
#define ARMV8A_HCR_TID5         (1UL << 58UL)
#define ARMV8A_HCR_TWEDEN       (1UL << 59UL)
#define ARMV8A_HCR_TWEDEL(x)    (((x) & 0xFUL) << 60UL)

#define ARMV8A_CPACR_EL1_SMEN   ((1UL << 25UL) | (1UL << 24UL))
#define ARMV8A_CPACR_EL1_FPEN   ((1UL << 21UL) | (1UL << 20UL))
#define ARMV8A_CPACR_EL1_ZEN    ((1UL << 17UL) | (1UL << 16UL))

#define ARMV8A_MPIDR_UP_BITMASK         (0x1UL << 30UL)
#define ARMV8A_MPIDR_MT_BITMASK         (0x1UL << 24UL)
#define ARMV8A_MPIDR_HWID_BITMASK	(0xFF00FFFFFFUL)
#define ARMV8A_MPIDR_LEVEL_BITS_SHIFT   (3UL)
#define ARMV8A_MPIDR_LEVEL_BITS         (1UL << ARMV8A_MPIDR_LEVEL_BITS_SHIFT)
#define ARMV8A_MPIDR_LEVEL_MASK         ((1UL << ARMV8A_MPIDR_LEVEL_BITS) - 1UL)
#define ARMV8A_MPIDR_LEVEL_SHIFT(level) \
        (((1UL << level) >> 1UL) << ARMV8A_MPIDR_LEVEL_BITS_SHIFT)
#define ARMV8A_MPIDR_AFFINITY_LEVEL(mpidr, level) \
	((mpidr >> ARMV8A_MPIDR_LEVEL_SHIFT(level)) & ARMV8A_MPIDR_LEVEL_MASK)

#endif /* xwcd/soc/arm64/v8a/arch_regs.h */
