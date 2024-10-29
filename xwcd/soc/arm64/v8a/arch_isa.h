/**
 * @file
 * @brief 架构描述层：ARMv8A的架构指令
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

#ifndef __xwcd_soc_arm_v8a_arch_isa_h__
#define __xwcd_soc_arm_v8a_arch_isa_h__

#include <xwos/standard.h>

/* Read/Write */
#define armv8a_prefetch_before_aop(v) \
        __asm__ volatile( \
        "       prfm    pstl1strm, %[__v]\n" \
        : \
        : [__v] "Q" (v) \
        : "memory");

static inline
xwu8_t armv8a_load_acquire_8b(atomic_xwu8_t * addr)
{
        register xwu32_t tmp;

        __asm__ volatile(
        "       ldarb   %w[__tmp], [%[__addr]]\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return (xwu8_t)tmp;
}

static inline
void armv8a_store_release_8b(atomic_xwu8_t * addr, xwu8_t value)
{
        __asm__ volatile(
        "       stlrb  %w[__value], [%[__addr]]\n"
        :
        : [__value] "r" ((xwu32_t)value),
          [__addr] "r" (addr)
        : "memory"
        );
}

static inline
xwu16_t armv8a_load_acquire_16b(atomic_xwu16_t * addr)
{
        register xwu32_t tmp;

        __asm__ volatile(
        "       ldarh   %w[__tmp], [%[__addr]]\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return (xwu16_t)tmp;
}

static inline
void armv8a_store_release_16b(atomic_xwu16_t * addr, xwu16_t value)
{
        __asm__ volatile(
        "       stlrh  %w[__value], [%[__addr]]\n"
        :
        : [__value] "r" ((xwu32_t)value),
          [__addr] "r" (addr)
        : "memory"
        );
}

static inline
xwu32_t armv8a_load_acquire_32b(atomic_xwu32_t * addr)
{
        register xwu32_t tmp;

        __asm__ volatile(
        "       ldar    %w[__tmp], [%[__addr]]\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return tmp;
}

static inline
void armv8a_store_release_32b(atomic_xwu32_t * addr, xwu32_t value)
{
        __asm__ volatile(
        "       stlr    %w[__value], [%[__addr]]\n"
        :
        : [__value] "r" (value),
          [__addr] "r" (addr)
        : "memory"
        );
}

static inline
xwu64_t armv8a_load_acquire_64b(atomic_xwu64_t * addr)
{
        register xwu64_t tmp;

        __asm__ volatile(
        "       ldar    %[__tmp], [%[__addr]]\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return tmp;
}

static inline
void armv8a_store_release_64b(atomic_xwu64_t * addr, xwu64_t value)
{
        __asm__ volatile(
        "       stlr    %[__value], [%[__addr]]\n"
        :
        : [__value] "r" (value),
          [__addr] "r" (addr)
        : "memory"
        );
}

static inline
void armv8a_clear_exclusive(void)
{
        __asm__ volatile(
        "       clrex\n"
        :
        :
        : "memory"
        );
}

static inline
xwu8_t armv8a_load_exclusively_8b(atomic_xwu8_t * addr)
{
        register xwu32_t tmp;

        __asm__ volatile(
        "       ldxrb   %w[__tmp], [%[__addr]]\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return (xwu8_t)tmp;
}

static inline
xwer_t armv8a_store_exclusively_8b(atomic_xwu8_t * addr, xwu8_t value)
{
        register int32_t rc;

        __asm__ volatile(
        "       stxrb   %w[__rc], %w[__value], [%[__addr]]\n"
        : [__rc] "=&r" (rc)
        : [__value] "r" ((xwu32_t)value),
          [__addr] "r" (addr)
        : "memory"
        );
        return (xwer_t)rc;
}

static inline
xwu8_t armv8a_load_acquire_exclusively_8b(atomic_xwu8_t * addr)
{
        register xwu32_t tmp;

        __asm__ volatile(
        "       ldaxrb  %w[__tmp], [%[__addr]]\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return (xwu8_t)tmp;
}

static inline
xwer_t armv8a_store_release_exclusively_8b(atomic_xwu8_t * addr, xwu8_t value)
{
        register int32_t rc;

        __asm__ volatile(
        "       stlxrb  %w[__rc], %w[__value], [%[__addr]]\n"
        : [__rc] "=&r" (rc)
        : [__value] "r" ((xwu32_t)value),
          [__addr] "r" (addr)
        : "memory"
        );
        return (xwer_t)rc;
}

static inline
xwu16_t armv8a_load_exclusively_16b(atomic_xwu16_t * addr)
{
        register xwu32_t tmp;

        __asm__ volatile(
        "       ldxrh   %w[__tmp], [%[__addr]]\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return (xwu16_t)tmp;
}

static inline
xwer_t armv8a_store_exclusively_16b(atomic_xwu16_t * addr, xwu16_t value)
{
        register int32_t rc;

        __asm__ volatile(
        "       stxrh   %w[__rc], %w[__value], [%[__addr]]\n"
        : [__rc] "=&r" (rc)
        : [__value] "r" ((xwu32_t)value),
          [__addr] "r" (addr)
        : "memory"
        );
        return (xwer_t)rc;
}

static inline
xwu16_t armv8a_load_acquire_exclusively_16b(atomic_xwu16_t * addr)
{
        register xwu32_t tmp;

        __asm__ volatile(
        "       ldaxrh  %w[__tmp], [%[__addr]]\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return (xwu16_t)tmp;
}

static inline
xwer_t armv8a_store_release_exclusively_16b(atomic_xwu16_t * addr, xwu16_t value)
{
        register int32_t rc;

        __asm__ volatile(
        "       stlxrh  %w[__rc], %w[__value], [%[__addr]]\n"
        : [__rc] "=&r" (rc)
        : [__value] "r" ((xwu32_t)value),
          [__addr] "r" (addr)
        : "memory"
        );
        return (xwer_t)rc;
}

static inline
xwu32_t armv8a_load_exclusively_32b(atomic_xwu32_t * addr)
{
        register xwu32_t tmp;

        __asm__ volatile(
        "       ldxr    %w[__tmp], [%[__addr]]\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return tmp;
}

static inline
xwer_t armv8a_store_exclusively_32b(atomic_xwu32_t * addr, xwu32_t value)
{
        register int32_t rc;

        __asm__ volatile(
        "       stxr    %w[__rc], %w[__value], [%[__addr]]\n"
        : [__rc] "=&r" (rc)
        : [__value] "r" (value),
          [__addr] "r" (addr)
        : "memory"
        );
        return (xwer_t)rc;
}

static inline
xwu32_t armv8a_load_acquire_exclusively_32b(atomic_xwu32_t * addr)
{
        register xwu32_t tmp;

        __asm__ volatile(
        "       ldaxr   %w[__tmp], [%[__addr]]\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return tmp;
}

static inline
xwer_t armv8a_store_release_exclusively_32b(atomic_xwu32_t * addr, xwu32_t value)
{
        register int32_t rc;

        __asm__ volatile(
        "       stlxr   %w[__rc], %w[__value], [%[__addr]]\n"
        : [__rc] "=&r" (rc)
        : [__value] "r" (value),
          [__addr] "r" (addr)
        : "memory"
        );
        return (xwer_t)rc;
}

static inline
xwu64_t armv8a_load_exclusively_64b(atomic_xwu64_t * addr)
{
        register xwu64_t tmp;

        __asm__ volatile(
        "       ldxr    %[__tmp], [%[__addr]]\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return tmp;
}

static inline
xwer_t armv8a_store_exclusively_64b(atomic_xwu64_t * addr, xwu64_t value)
{
        register int32_t rc;

        __asm__ volatile(
        "       stxr    %w[__rc], %[__value], [%[__addr]]\n"
        : [__rc] "=&r" (rc)
        : [__value] "r" (value),
          [__addr] "r" (addr)
        : "memory"
        );
        return (xwer_t)rc;
}

static inline
xwu64_t armv8a_load_acquire_exclusively_64b(atomic_xwu64_t * addr)
{
        register xwu64_t tmp;

        __asm__ volatile(
        "       ldaxr   %[__tmp], [%[__addr]]\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return tmp;
}

static inline
xwer_t armv8a_store_release_exclusively_64b(atomic_xwu64_t * addr, xwu64_t value)
{
        register int32_t rc;

        __asm__ volatile(
        "       stlxr   %w[__rc], %[__value], [%[__addr]]\n"
        : [__rc] "=&r" (rc)
        : [__value] "r" (value),
          [__addr] "r" (addr)
        : "memory"
        );
        return (xwer_t)rc;
}

static inline
xwu128_t armv8a_load_exclusively_128b(atomic_xwu128_t * addr)
{
        union {
                xwu128_t u128;
                struct {
                        xwu64_t l;
                        xwu64_t h;
                } u64;
        } value;

        __asm__ volatile(
        "       ldxp    %[__vl], %[__vh], [%[__addr]]\n"
        : [__vl] "=&r" (value.u64.l),
          [__vh] "=&r" (value.u64.h)
        : [__addr] "r" (addr)
        : "memory"
        );
        return value.u128;
}

static inline
xwer_t armv8a_store_exclusively_128b(atomic_xwu128_t * addr, xwu128_t value)
{
        register int32_t rc;

        __asm__ volatile(
        "       stxp    %w[__rc], %[__vl], %[__vh], [%[__addr]]\n"
        : [__rc] "=&r" (rc)
        : [__vl] "r" (*((xwu64_t *)&value)),
          [__vh] "r" (*(((xwu64_t *)&value) + 1)),
          [__addr] "r" (addr)
        : "memory"
        );
        return (xwer_t)rc;
}

static inline
xwu128_t armv8a_load_acquire_exclusively_128b(atomic_xwu128_t * addr)
{
        union {
                xwu128_t u128;
                struct {
                        xwu64_t l;
                        xwu64_t h;
                } u64;
        } value;

        __asm__ volatile(
        "       ldaxp   %[__vl], %[__vh], [%[__addr]]\n"
        : [__vl] "=&r" (value.u64.l),
          [__vh] "=&r" (value.u64.h)
        : [__addr] "r" (addr)
        : "memory"
        );
        return value.u128;
}

static inline
xwer_t armv8a_store_release_exclusively_128b(atomic_xwu128_t * addr, xwu128_t value)
{
        register int32_t rc;

        __asm__ volatile(
        "       stlxp   %w[__rc], %[__vl], %[__vh], [%[__addr]]\n"
        : [__rc] "=&r" (rc)
        : [__vl] "r" (*((xwu64_t *)&value)),
          [__vh] "r" (*(((xwu64_t *)&value) + 1)),
          [__addr] "r" (addr)
        : "memory"
        );
        return (xwer_t)rc;
}

#define armv8a_sysreg_read(val, reg) \
        __asm__ volatile("mrs %[__val], " #reg : [__val] "=&r" (*val) : : "memory")

#define armv8a_sysreg_write(reg, val) \
        __asm__ volatile("msr " #reg ", %[__val]" : : [__val] "r" (val) : "memory"); \
        armv8a_isb()

#define armv8a_reg_read(val, reg) \
        __asm__ volatile("mov %[__val], " #reg : [__val] "=&r" (*val) : : "memory")

#define armv8a_reg_write(reg, val) \
        __asm__ volatile("mov " #reg ", %[__val]" : : [__val] "r" (val) : "memory"); \
        armv8a_isb()


/* Exception */
/**
 * @brief Enable DAIF.F
 */
static __xwbsp_inline
void armv8a_fiqs_enable_lc(void)
{
        __asm__ volatile("msr daifclr, #1" : : : "memory");
}

/**
 * @brief Disable DAIF.F
 */
static __xwbsp_inline
void armv8a_fiqs_disable_lc(void)
{
        __asm__ volatile("msr daifset, #1" : : : "memory");
}

/**
 * @brief Enable DAIF.I
 */
static __xwbsp_inline
void armv8a_irqs_enable_lc(void)
{
        __asm__ volatile("msr daifclr, #2" : : : "memory");
}

/**
 * @brief Disable DAIF.I
 */
static __xwbsp_inline
void armv8a_irqs_disable_lc(void)
{
        __asm__ volatile("msr daifset, #2" : : : "memory");
}

/**
 * @brief Enable DAIF.A
 */
static __xwbsp_inline
void armv8a_serror_enable_lc(void)
{
        __asm__ volatile("msr daifclr, #4" : : : "memory");
}

/**
 * @brief Disable DAIF.A
 */
static __xwbsp_inline
void armv8a_serror_disable_lc(void)
{
        __asm__ volatile("msr daifset, #4" : : : "memory");
}

/**
 * @brief Enable DAIF.D
 */
static __xwbsp_inline
void armv8a_dbg_enable_lc(void)
{
        __asm__ volatile("msr daifclr, #8" : : : "memory");
}

/**
 * @brief Disable DAIF.D
 */
static __xwbsp_inline
void armv8a_dbg_disable_lc(void)
{
        __asm__ volatile("msr daifset, #8" : : : "memory");
}

/**
 * @brief Enable IRQs & FIQs of Local CPU
  */
static __xwbsp_inline
void armv8a_cpuirq_enable_lc(void)
{
        __asm__ volatile("msr daifclr, #3" : : : "memory");
}

/**
 * @brief Disable IRQs & FIQs of Local CPU
 */
static __xwbsp_inline
void armv8a_cpuirq_disable_lc(void)
{
        __asm__ volatile("msr daifset, #3" : : : "memory");
}

/**
 * @brief Restore Exceptions of Local CPU
 */
static __xwbsp_inline
void armv8a_cpuirq_restore_lc(xwreg_t cpuirq)
{
        armv8a_sysreg_write(daif, cpuirq);
}

/**
 * @brief Save Exceptions and disable them of Local CPU
 */
static __xwbsp_inline
void armv8a_cpuirq_save_lc(xwreg_t * cpuirq)
{
        armv8a_sysreg_read(&*cpuirq, daif);
        armv8a_cpuirq_disable_lc();
}

/**
 * @brief Test local CPU IRQ flag
 */
static __xwbsp_inline
bool armv8a_cpuirq_test_lc(void)
{
        xwreg_t cpuirq;

        armv8a_sysreg_read(&cpuirq, daif);
        return !(cpuirq & ((xwreg_t)3 << (xwreg_t)6));
}

/* EL switch */
void armv8a_switch_el3_to_el2_aarch64(void);
void armv8a_switch_el2_to_el1_aarch64(void);

static __xwbsp_inline
void armv8a_wfi(void)
{
        __asm__ volatile("wfi" : : :);
}

static __xwbsp_inline
void armv8a_wfe(void)
{
        __asm__ volatile("wfe" : : :);
}

#endif /* xwcd/soc/arm64/v8a/arch_isa.h */
