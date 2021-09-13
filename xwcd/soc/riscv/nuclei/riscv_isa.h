/**
 * @file
 * @brief 架构描述层：RISC-V指令架构
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

#ifndef __riscv_isa_h__
#define __riscv_isa_h__

#include <xwos/standard.h>

/**
 * @brief Load Reserved (32-bit)
 * @param[in] addr: 内存地址
 * @return 内存中的32位数据
 */
static __xwbsp_inline
xwu32_t rv_lrw(volatile void * addr)
{
        xwu32_t tmp;

        __asm__ volatile(
        "lr.w  %[__tmp], (%[__addr])\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return tmp;
}

/**
 * @brief Store Conditional (32-bit)
 * @param[in] addr: 内存地址
 * @param[in] value: 待写入的数据
 * @return 数据是否成功写入内存
 * @retval ~0: 失败
 * @retval 0: 成功
 */
static __xwbsp_inline
xwer_t rv_scw(volatile void * addr, xwu32_t value)
{
        xwer_t rc;

        __asm__ volatile(
        "sc.w  %[__rc], %[__value], (%[__addr])\n"
        : [__rc] "=&r" (rc)
        : [__value] "r" (value),
          [__addr] "r" (addr)
        : "memory"
        );
        return rc;
}

#if __riscv_xlen == 64
/**
 * @brief Load Reserved (64-bit)
 * @param[in] addr: 内存地址
 * @return 内存中的64位数据
 */
static __xwbsp_inline
xwu64_t rv_lrd(volatile void * addr)
{
        xwu64_t tmp;

        __asm__ volatile(
        "lr.d  %[__tmp], (%[__addr])\n"
        : [__tmp] "=&r" (tmp)
        : [__addr] "r" (addr)
        : "memory"
        );
        return tmp;
}

/**
 * @brief Store Conditional (64-bit)
 * @param[in] addr: 内存地址
 * @param[in] value: 待写入的数据
 * @return 数据是否成功写入内存
 * @retval ~0: 失败
 * @retval 0: 成功
 */
static __xwbsp_inline
xwer_t rv_scd(volatile void * addr, xwu64_t value)
{
        xwer_t rc;

        __asm__ volatile(
        "sc.d  %[__rc], %[__value], (%[__addr])\n"
        : [__rc] "=&r" (rc)
        : [__value] "r" (value),
          [__addr] "r" (addr)
        : "memory"
        );
        return rc;
}
#endif

/**
 * @brief 等待中断
 */
static __xwbsp_inline
void rv_wfi(void)
{
        __asm__ volatile("wfi");
}

#endif /* riscv_isa.h */
