/**
 * @file
 * @brief 架构描述层：ARMv8A异常
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

#ifndef __xwcd_soc_arm_v8a_arch_exception_h__
#define __xwcd_soc_arm_v8a_arch_exception_h__

#include <xwos/standard.h>

/**
 * @brief ARMV8A的寄存器结构
 */
struct armv8a_reg_frame {
        xwu64_t r[30]; /**< 异常发生前的X0~X29 */
        xwu64_t lr; /**< 异常发生前的LR */
        xwu64_t sp; /**< 异常发生前的SP */
        xwu64_t elr; /**< 异常发生前的PC */
        xwu64_t spsr; /**< 异常发生前的PSTATE */
};

/**
 * @brief ARMV8A的FPU寄存器结构
 */
struct armv8a_fpreg_frame {
        xwu128_t q[32]; /**< 异常发生前的Q0~Q31 */
        xwu32_t fpsr; /**< 异常发生前的FPSR */
        xwu32_t reserved[3]; /**< 预留空间 */
};

/******** ESR ********/
#define ARMV8A_ESR_ELx_EC_UNKNOWN       (0x00)
#define ARMV8A_ESR_ELx_EC_WFx           (0x01)
/* Unallocated EC: 0x02 */
#define ARMV8A_ESR_ELx_EC_CP15_32       (0x03)
#define ARMV8A_ESR_ELx_EC_CP15_64       (0x04)
#define ARMV8A_ESR_ELx_EC_CP14_MR       (0x05)
#define ARMV8A_ESR_ELx_EC_CP14_LS       (0x06)
#define ARMV8A_ESR_ELx_EC_FP_ASIMD      (0x07)
#define ARMV8A_ESR_ELx_EC_CP10_ID       (0x08)/* EL2 only */
#define ARMV8A_ESR_ELx_EC_PAC           (0x09)/* EL2 and EL3 */
/* Unallocated EC: 0x0A - 0x0B */
#define ARMV8A_ESR_ELx_EC_CP14_64       (0x0C)
/* Unallocated EC: 0x0d */
#define ARMV8A_ESR_ELx_EC_ILL           (0x0E)
/* Unallocated EC: 0x0F - 0x10 */
#define ARMV8A_ESR_ELx_EC_SVC32         (0x11) /* EL1 and EL2 */
#define ARMV8A_ESR_ELx_EC_HVC32         (0x12) /* EL2 only */
#define ARMV8A_ESR_ELx_EC_SMC32         (0x13) /* EL2 and EL3 */
/* Unallocated EC: 0x14 */
#define ARMV8A_ESR_ELx_EC_SVC64         (0x15) /* EL1, EL2, EL3 */
#define ARMV8A_ESR_ELx_EC_HVC64         (0x16) /* EL2 and EL3 */
#define ARMV8A_ESR_ELx_EC_SMC64         (0x17) /* EL2 and EL3 */
#define ARMV8A_ESR_ELx_EC_SYS64         (0x18)
#define ARMV8A_ESR_ELx_EC_SVE           (0x19)
#define ARMV8A_ESR_ELx_EC_ERET          (0x1a) /* EL2 only */
/* Unallocated EC: 0x1b - 0x1E */
#define ARMV8A_ESR_ELx_EC_IMP_DEF       (0x1f) /* EL3 only */
#define ARMV8A_ESR_ELx_EC_IABT_LOW      (0x20)
#define ARMV8A_ESR_ELx_EC_IABT_CUR      (0x21)
#define ARMV8A_ESR_ELx_EC_PC_ALIGN      (0x22)
/* Unallocated EC: 0x23 */
#define ARMV8A_ESR_ELx_EC_DABT_LOW      (0x24)
#define ARMV8A_ESR_ELx_EC_DABT_CUR      (0x25)
#define ARMV8A_ESR_ELx_EC_SP_ALIGN      (0x26)
/* Unallocated EC: 0x27 */
#define ARMV8A_ESR_ELx_EC_FP_EXC32      (0x28)
/* Unallocated EC: 0x29 - 0x2B */
#define ARMV8A_ESR_ELx_EC_FP_EXC64      (0x2C)
/* Unallocated EC: 0x2D - 0x2E */
#define ARMV8A_ESR_ELx_EC_SERROR        (0x2F)
#define ARMV8A_ESR_ELx_EC_BREAKPT_LOW   (0x30)
#define ARMV8A_ESR_ELx_EC_BREAKPT_CUR   (0x31)
#define ARMV8A_ESR_ELx_EC_SOFTSTP_LOW   (0x32)
#define ARMV8A_ESR_ELx_EC_SOFTSTP_CUR   (0x33)
#define ARMV8A_ESR_ELx_EC_WATCHPT_LOW   (0x34)
#define ARMV8A_ESR_ELx_EC_WATCHPT_CUR   (0x35)
/* Unallocated EC: 0x36 - 0x37 */
#define ARMV8A_ESR_ELx_EC_BKPT32        (0x38)
/* Unallocated EC: 0x39 */
#define ARMV8A_ESR_ELx_EC_VECTOR32      (0x3A) /* EL2 only */
/* Unallocted EC: 0x3B */
#define ARMV8A_ESR_ELx_EC_BRK64         (0x3C)
/* Unallocated EC: 0x3D - 0x3F */
#define ARMV8A_ESR_ELx_EC_MAX           (0x3F)

void armv8a_dump(struct armv8a_reg_frame  * frame);
void armv8a_backtrace_frame(struct armv8a_reg_frame  * frame, const char * name);
void armv8a_backtrace(xwu64_t fp);

void armv8a_init_vector(void);

/******** exception vector ********/
extern xwu32_t armv8a_exception_vector_el1[];
extern xwu32_t armv8a_exception_vector_el2[];
extern xwu32_t armv8a_exception_vector_el3[];

/******** IRQs ********/
void armv8a_irq_el1(struct armv8a_reg_frame * regs, xwu64_t routine);
void armv8a_irq_el2(struct armv8a_reg_frame * regs, xwu64_t routine);
void armv8a_irq_el3(struct armv8a_reg_frame * regs, xwu64_t routine);

/******** FIQs ********/
void armv8a_fiq_el1(struct armv8a_reg_frame * regs, xwu64_t routine);
void armv8a_fiq_el2(struct armv8a_reg_frame * regs, xwu64_t routine);
void armv8a_fiq_el3(struct armv8a_reg_frame * regs, xwu64_t routine);

/******** Traps ********/
void armv8a_trap_sync_el1(struct armv8a_reg_frame * regs, xwu64_t routine);
void armv8a_trap_serror_el1(struct armv8a_reg_frame * regs, xwu64_t routine);
void armv8a_trap_sync_el2(struct armv8a_reg_frame * regs, xwu64_t routine);
void armv8a_trap_serror_el2(struct armv8a_reg_frame * regs, xwu64_t routine);
void armv8a_trap_sync_el3(struct armv8a_reg_frame * regs, xwu64_t routine);
void armv8a_trap_serror_el3(struct armv8a_reg_frame * regs, xwu64_t routine);

#endif /* xwcd/soc/arm64/v8a/arch_exception.h */
