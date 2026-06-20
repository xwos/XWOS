/**
 * @file
 * @brief XWOS移植实现层：线程本地存储
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

#include <xwos/standard.h>
#include <xwos/ospl/skd.h>

extern xwu8_t tdata_lma_base[];
extern xwu8_t tdata_vma_base[];
extern xwu8_t tdata_vma_end[];

extern xwu8_t tbss_vma_base[];
extern xwu8_t tbss_vma_end[];
extern xwu8_t tbss_vma_offset[];

extern xwu8_t tls_offset[];

__xwbsp_code
void soc_tls_relocate(void * tls)
{
        xwsz_t cnt, i;
        xwu8_t * src;
        xwu8_t * dst;

        src = tdata_lma_base;
        dst = tls;
        if (dst != src) {
                cnt =  (xwsz_t)tdata_vma_end - (xwsz_t)tdata_vma_base;
                for (i = 0; i < cnt; i++) {
                        *dst = *src;
                        dst++;
                        src++;
                }
        }

        dst = tls + (xwptr_t)tbss_vma_offset;
        cnt = (xwsz_t)tbss_vma_end - (xwsz_t)tbss_vma_base;
        for (i = 0; i < cnt; i++) {
                *dst = 0;
                dst++;
        }
}

__xwbsp_code
void xwospl_tls_init(struct xwospl_skdobj_stack * stk)
{
        xwsz_t tls_size;

        tls_size = (xwsz_t)tbss_vma_end - (xwsz_t)tdata_vma_base;
        stk->guard_base = (xwstk_t *)((xwptr_t)stk->base +
                                      XWBOP_ALIGN(tls_size, (xwsz_t)tls_offset));
        soc_tls_relocate(stk->tls);
        stk->tls = (void *)((xwptr_t)stk->tls - (xwptr_t)tls_offset);
}

/**
 * @brief XWOS移植层：返回TLS段的基地址
 * @return TLS段的基地址
 * @details
 * ARM EABI中定义的函数，用于返回TLS段的基地址。
 * 当使用C11（含）以后标准中的 `_Thread_local` 变量时，会链接到此函数。
 *
 * 编译器会假定此函数内只会使用 `r0` 寄存器，
 * 因此，若此函数内需要使用 `volatile` 寄存器，需要保护 `r1`, `r2`, `r3`, `lr` 。
 */
__xwbsp_code __xwcc_naked
void * __aeabi_read_tp(void)
{
        __asm__ volatile("      push    {r1, r2, r3, lr}");
        __asm__ volatile("      bl      xwosplcb_skd_get_cthd_lc");
        __asm__ volatile("      ldr     r0, [r0, %[__tls]]"
                         :
                         :[__tls] "I" (xwcc_offsetof(struct xwospl_thd, stack.tls))
                         :);
        __asm__ volatile("      pop     {r1, r2, r3, pc}");
}
