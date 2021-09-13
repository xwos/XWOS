/**
 * @file
 * @brief 玄武OS移植实现层：SOC系统调用
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

#include <xwos/standard.h>

/**
 * @brief Start supervisor call (change to privileged access temporarily)
 */
__xwbsp_code __xwcc_naked
void soc_privilege_start(void)
{
        __asm__ volatile(".syntax       unified");
        __asm__ volatile("      svc     1");
        __asm__ volatile("      bx      lr");
}

/**
 * @brief End supervisor call (close the privileged access)
 */
__xwbsp_code __xwcc_naked
void soc_privilege_end(void)
{
        __asm__ volatile(".syntax       unified");
        __asm__ volatile("      svc     2");
        __asm__ volatile("      bx      lr");
}

/**
 * @brief 以函数func作为系统调用
 * @param[in] func: 系统调用函数指针
 * @param[in] argnum: 函数参数的数量
 * @param[in] ...: 可变参数
 * @return 64位的有符号整数（返回值的具体意义根据具体的函数而决定）
 */
__xwbsp_code
xws64_t xwsc(xwsc_f func, xwreg_t argnum, ...)
{
        xws64_t rc;
        va_list args;

        va_start(args, argnum);
        __asm__ volatile(".syntax       unified");
        __asm__ volatile(
        "       mov     r0, %[__func]\n" /* sc function */
        "       mov     r1, %[__argnum]\n" /* number of args */
        "       mov     r2, %[__args]\n" /* args address */
        "       svc     8\n"
        "       mov     lr, r3\n" /* When returning here, r3 is old lr */
        :
        : [__func] "r" (func),
          [__argnum] "r" (argnum),
          [__args] "r" (args)
        : "memory", "r0", "r1", "r2"
        );

        __asm__ volatile(
        "       strd    r0, r1, [%[__rc]]\n" /* r0, r1 is result */
        :
        : [__rc] "r" (&rc)
        : "memory", "r0", "r1"
        );
        va_end(args);
        return rc;
}

/**
 * @brief 系统调用入口
 * @param[in] func: 系统调用函数的指针
 * @param[in] argnum: 函数参数的数量
 * @param[in] args: 可变参数
 * @param[in] old_lr: LR寄存器的原始值
 * @note
 * - 此函数返回时，会将r0,r1作为结果，并恢复LR寄存器的原始值。
 * - 此函数扩展了C语言的语法，在C语言中，可变参函数的参数个数在编译时就已经确定，
 *   也就是运行时参数的数量就已经确定。此函数用汇编语言写成，可以做到运行时参数
 *   可变。也即是可以用任意数量的参数调用任意函数。遗憾的是，这种运行时的调用并不具备
 *   检查功能。假设一个函数的原型是void foo(int a); 可以使用xwsc(foo, 2, 1 ,2)传递两个
 *   参数给foo运行，但运行结果未知。
 * - 设计这个接口的目的是为了将系统调用的接口统一化，而不必像Linux定义系统调用时还需要
 *   定义一堆指定参数个数的宏(SYSCALL_DEFINE1, SYSCALL_DEFINE2, SYSCALL_DEFINE3, ...)。
 * - 对于系统调用，XWOS会使用xwsc()直接给出参数具体化的封装，不建议直接使用xwsc()，
 *   除非用户对编译器产生的汇编代码十分了解。
 */
__xwbsp_code __xwcc_naked
xws64_t soc_xwsc_entry(__xwcc_unused xwsc_f func, __xwcc_unused xwptr_t argnum,
                       __xwcc_unused xwptr_t * args, __xwcc_unused xwreg_t old_lr)
{
        __asm__ volatile(".syntax       unified");
        __asm__ volatile("      push    {r3, r4, r5, r6, r7, r8, r9, sl, ip, lr}");
        __asm__ volatile("      mov     r4, r0");                       /* r4 = func;           */
        __asm__ volatile("      mov     r5, r1");                       /* r5 = argnum;         */
        __asm__ volatile("      mov     r6, r2");                       /* r6 = args;           */

        /* prepare stack (due to AAPCS, stack must be aligned to 8) */
        __asm__ volatile("      cmp     r5, #4");                       /* if (argnum <= 4)     */
        __asm__ volatile("      it      le\n"                           /* {                    */
                         "      movle   r7, #0");                       /*     r7 = 0;          */
        __asm__ volatile("      ble     get_args");                     /*     goto get_args;   */
                                                                        /* }                    */
        __asm__ volatile("      sub     r7, r5, #4");                   /* r7 = argnum - 4;     */
        __asm__ volatile("      tst     r7, #1");                       /* if (r7 & 1) (is odd) */
        __asm__ volatile("      it      ne\n"                           /* {                    */
                         "      addne   r7, #1");                       /*     r7++;            */
                                                                        /* }                    */
        __asm__ volatile("      lsl     r7, #2");                       /* r7 *= 4;             */
        __asm__ volatile("      sub     sp, sp, r7");                   /* sp = sp - r7;        */
        /* get arguments */
        __asm__ volatile("get_args:");
        __asm__ volatile("      mov     r9, #0");                       /* r9 = 0               */
        __asm__ volatile("      mov     r8, #0");                       /* r8 = 0               */
        __asm__ volatile("1:");
        __asm__ volatile("      cmp     r5, #0");                       /* if (0 == argnum)     */
        __asm__ volatile("      beq     call");                         /*     goto call        */
        __asm__ volatile("      sub     r5, #1");                       /* argnum--;            */
        __asm__ volatile("      ldr     sl, [r6], #4");                 /* sl = *args++;        */
        __asm__ volatile("      tbb     [pc, r9]");                     /* switch (r9) {        */
        __asm__ volatile("tbb_table:\n"
                         ".byte         (arg_0 - tbb_table) / 2\n"
                         ".byte         (arg_1 - tbb_table) / 2\n"
                         ".byte         (arg_2 - tbb_table) / 2\n"
                         ".byte         (arg_3 - tbb_table) / 2\n"
                         ".byte         (arg_x - tbb_table) / 2\n"
                         ".byte         0\n");                          /* align to 2           */
        __asm__ volatile("arg_0:");                                     /* case 0:              */
        __asm__ volatile("      add     r9, #1");                       /*     r9++;            */
        __asm__ volatile("      mov     r0, sl");                       /*     r0 = sl;         */
        __asm__ volatile("      b       1b");                           /*     goto 1b;         */
        __asm__ volatile("arg_1:");                                     /* case 1:              */
        __asm__ volatile("      add     r9, #1");                       /*     r9++;            */
        __asm__ volatile("      mov     r1, sl");                       /*     r1 = sl;         */
        __asm__ volatile("      b       1b");                           /*     goto 1b;         */
        __asm__ volatile("arg_2:");                                     /* case 2:              */
        __asm__ volatile("      add     r9, #1");                       /*     r9++;            */
        __asm__ volatile("      mov     r2, sl");                       /*     r2 = sl;         */
        __asm__ volatile("      b       1b");                           /*     goto 1b;         */
        __asm__ volatile("arg_3:");                                     /* case 3:              */
        __asm__ volatile("      add     r9, #1");                       /*     r9++;            */
        __asm__ volatile("      mov     r3, sl");                       /*     r3 = sl;         */
        __asm__ volatile("      b       1b");                           /*     goto 1b;         */
        __asm__ volatile("arg_x:");                                     /* default:             */
        __asm__ volatile("      str     sl, [sp, r8]");                 /* *(sp + r8) = sl      */
        __asm__ volatile("      add     r8, #4");                       /*     r8 += 4;         */
        __asm__ volatile("      b       1b");                           /*     goto 1b;         */
                                                                        /* }                    */
        /* call func */
        __asm__ volatile("call:");
        __asm__ volatile("      orr     r4, #1");                       /* r4 |= 1;(Thumb mode) */
        __asm__ volatile("      blx     r4");                           /* call func with args  */
        __asm__ volatile("      add     sp, r7");                       /* delete arg stack     */
        /* close privileged access */
        __asm__ volatile("      mrs     r2, control");                  /* r2 = control;        */
        __asm__ volatile("      orr     r2, #1");                       /* r2 |= 1;             */
        __asm__ volatile("      msr     control, r2");                  /* control = r2;        */
        __asm__ volatile("      pop     {r3, r4, r5, r6, r7, r8, r9, sl, ip, pc}");
}
