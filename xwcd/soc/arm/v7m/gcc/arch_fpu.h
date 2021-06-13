/**
 * @file
 * @brief 架构描述层：FPU
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

#ifndef __arch_fpu_h__
#define __arch_fpu_h__

#include <xwos/standard.h>

#define FPU_FPCCR_ASPEN_POS     31
#define FPU_FPCCR_ASPEN_MSK     (1UL << FPU_FPCCR_ASPEN_POS)

#define FPU_FPCCR_LSPEN_POS     30
#define FPU_FPCCR_LSPEN_MSK     (1UL << FPU_FPCCR_LSPEN_POS)

#define FPU_FPCCR_MONRDY_POS    8
#define FPU_FPCCR_MONRDY_MSK    (1UL << FPU_FPCCR_MONRDY_POS)

#define FPU_FPCCR_BFRDY_POS     6
#define FPU_FPCCR_BFRDY_MSK     (1UL << FPU_FPCCR_BFRDY_POS)

#define FPU_FPCCR_MMRDY_POS     5
#define FPU_FPCCR_MMRDY_MSK     (1UL << FPU_FPCCR_MMRDY_POS)

#define FPU_FPCCR_HFRDY_POS     4
#define FPU_FPCCR_HFRDY_MSK     (1UL << FPU_FPCCR_HFRDY_POS)

#define FPU_FPCCR_THREAD_POS    3
#define FPU_FPCCR_THREAD_MSK    (1UL << FPU_FPCCR_THREAD_POS)

#define FPU_FPCCR_USER_POS      1
#define FPU_FPCCR_USER_MSK      (1UL << FPU_FPCCR_USER_POS)

#define FPU_FPCCR_LSPACT_POS    0
#define FPU_FPCCR_LSPACT_MSK    (1UL << FPU_FPCCR_LSPACT_POS)

/* Floating-Point Context Address Register */
#define FPU_FPCAR_ADDRESS_POS   3
#define FPU_FPCAR_ADDRESS_MSK   (0X1FFFFFFFUL << FPU_FPCAR_ADDRESS_POS)

/* Floating-Point Default Status Control Register */
#define FPU_FPDSCR_AHP_POS      26
#define FPU_FPDSCR_AHP_MSK      (1UL << FPU_FPDSCR_AHP_POS)

#define FPU_FPDSCR_DN_POS       25
#define FPU_FPDSCR_DN_MSK       (1UL << FPU_FPDSCR_DN_POS)

#define FPU_FPDSCR_FZ_POS       24
#define FPU_FPDSCR_FZ_MSK       (1UL << FPU_FPDSCR_FZ_POS)

#define FPU_FPDSCR_RMODE_POS    22
#define FPU_FPDSCR_RMODE_MSK    (3UL << FPU_FPDSCR_RMODE_POS)

/* Media and FP Feature Register 0 */
#define FPU_MVFR0_FP_ROUNDING_MODES_POS 28
#define FPU_MVFR0_FP_ROUNDING_MODES_MSK (0XFUL << FPU_MVFR0_FP_ROUNDING_MODES_POS)

#define FPU_MVFR0_SHORT_VECTORS_POS     24
#define FPU_MVFR0_SHORT_VECTORS_MSK     (0XFUL << FPU_MVFR0_SHORT_VECTORS_POS)

#define FPU_MVFR0_SQUARE_ROOT_POS       20
#define FPU_MVFR0_SQUARE_ROOT_MSK       (0XFUL << FPU_MVFR0_SQUARE_ROOT_POS)

#define FPU_MVFR0_DIVIDE_POS            16
#define FPU_MVFR0_DIVIDE_MSK            (0XFUL << FPU_MVFR0_DIVIDE_POS)

#define FPU_MVFR0_FP_EXCEP_TRAPPING_POS 12
#define FPU_MVFR0_FP_EXCEP_TRAPPING_MSK (0XFUL << FPU_MVFR0_FP_EXCEP_TRAPPING_POS)

#define FPU_MVFR0_DOUBLE_PRECISION_POS  8
#define FPU_MVFR0_DOUBLE_PRECISION_MSK  (0XFUL << FPU_MVFR0_DOUBLE_PRECISION_POS)

#define FPU_MVFR0_SINGLE_PRECISION_POS  4
#define FPU_MVFR0_SINGLE_PRECISION_MSK  (0XFUL << FPU_MVFR0_SINGLE_PRECISION_POS)

#define FPU_MVFR0_A_SIMD_REGISTERS_POS  0
#define FPU_MVFR0_A_SIMD_REGISTERS_MSK  (0XFUL << FPU_MVFR0_A_SIMD_REGISTERS_POS)

/* Media and FP Feature Register 1 */
#define FPU_MVFR1_FP_FUSED_MAC_POS      28
#define FPU_MVFR1_FP_FUSED_MAC_MSK      (0XFUL << FPU_MVFR1_FP_FUSED_MAC_POS)

#define FPU_MVFR1_FP_HPFP_POS           24
#define FPU_MVFR1_FP_HPFP_MSK           (0XFUL << FPU_MVFR1_FP_HPFP_POS)

#define FPU_MVFR1_D_NAN_MODE_POS        4
#define FPU_MVFR1_D_NAN_MODE_MSK        (0XFUL << FPU_MVFR1_D_NAN_MODE_POS)

#define FPU_MVFR1_FTZ_MODE_POS          0
#define FPU_MVFR1_FTZ_MODE_MSK          (0XFUL << FPU_MVFR1_FTZ_MODE_POS)

void arch_fpu_init(void);

#endif /* arch_fpu.h */
