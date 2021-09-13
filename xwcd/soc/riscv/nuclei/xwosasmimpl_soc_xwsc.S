/**
 * @file
 * @brief SOC描述层：SOC系统调用汇编代码
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * Copyright (c) 2019 Nuclei Limited. All rights reserved.
 * @copyright
 * + (c) 2021 隐星魂 (Roy.Sun) <https://xwos.tech>
 * + (c) 2019 Nuclei Limited. All rights reserved.
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

#include "NMSIS/Core/Include/riscv_encoding.h"

/**
 * @brief XWOS System Call Exception Service Routine Entry
 * @param[in] func: function
 * @param[in] argnum: number of arguments
 * @param[in] args: address of arguments
 */
        .section        .xwos.isr.text,"ax"
        .align          2 /* 1 << 2 */
        .global         soc_esr_xwsc
        .func           soc_esr_xwsc
soc_esr_xwsc:
/* xws64_t soc_esr_xwsc(xwsc_f func, xwptr_t argnum, va_list args) */
        addi	        sp, sp, -32
        sw              s2, 0 (sp)
        sw              s3, 4 (sp)
        sw              s4, 8 (sp)
        sw              s5, 12 (sp)
        sw              ra, 16 (sp)
        mv              s2, a0 /* s2 = func; */
        mv              s3, a1 /* s3 = argnum; */
        mv              s4, a2 /* s4 = args; */
        li              s5, 0 /* s5 = stack frame size; */
        li              t0, 8
        ble             s3, t0, __get_args /* if (argnum <= 8) goto __get_args; */
        /* Calculate arg stack frame size to s5 */
        addi            s5, s3, -8
        /* roundup s5 to 16 bytes */
        addi            s5, s5, 3
        srli            s5, s5, 2
        slli            s5, s5, 4
        /* alloc stack frame */
        sub             sp, sp, s5
__get_args: /* pre-loop */
        li              t0, 0 /* t0 = 0; */
        mv              t1, s3 /* t1 = argnum; */
        li              t2, 8 /* t2 = 8 */
        /* t3 is branch reg */
        /* t4 is src pointer */
        /* t5 is dst pointer */
        /* t6 is tmp reg */
__loop: /* do { */
        bge             t0, t1, __call  /* if (t0 >= argnum) break; */
        slli            t4, t0, 2 /* t4 = t0 * 4 */
        bge             t0, t2, __arg_x /* if (t0 >= 8) goto __arg_x; */
        /* switch (__btable + (t4)) { */
        la              t3, __btable
        add             t3, t3, t4
        lw              t3, 0 (t3)
        jr              t3
__btable:
        .word           __arg_0
        .word           __arg_1
        .word           __arg_2
        .word           __arg_3
        .word           __arg_4
        .word           __arg_5
        .word           __arg_6
        .word           __arg_7
__arg_0:
        add             t4, s4, t4 /* t4 = args + t4 */
        lw              a0, 0 (t4)
        j               __btable_end
__arg_1:
        add             t4, s4, t4 /* t4 = args + t4 */
        lw              a1, 0 (t4)
        j               __btable_end
__arg_2:
        add             t4, s4, t4 /* t4 = args + t4 */
        lw              a2, 0 (t4)
        j               __btable_end
__arg_3:
        add             t4, s4, t4 /* t4 = args + t4 */
        lw              a3, 0 (t4)
        j               __btable_end
__arg_4:
        add             t4, s4, t4 /* t4 = args + t4 */
        lw              a4, 0 (t4)
        j               __btable_end
__arg_5:
        add             t4, s4, t4 /* t4 = args + t4 */
        lw              a5, 0 (t4)
        j               __btable_end
__arg_6:
        add             t4, s4, t4 /* t4 = args + t4 */
        lw              a6, 0 (t4)
        j               __btable_end
__arg_7:
        add             t4, s4, t4 /* t4 = args + t4 */
        lw              a7, 0 (t4)
        j               __btable_end
__arg_x:
        sub             t5, t0, t2 /* t5 = t0 - t2 */
        slli            t5, t5, 2 /* t5 = t5 * 4 */
        add             t5, sp, t5 /* t5 = sp + t5 */
        add             t4, s4, t4 /* t4 = args + t4 */
        lw              t6, 0 (t4)
        sw              t6, 0 (t5)
        j               __btable_end
__btable_end: /* } end of switch (__btable + (t4)) */
        addi            t0, t0, 1 /* t0++; */
        j               __loop /* } while (1); */
__call:
        jalr            s2 /* call func with args */
        /* free stack frame */
        add             sp, sp, s5
        lw              ra, 16 (sp)
        lw              s5, 12 (sp)
        lw              s4, 8 (sp)
        lw              s3, 4 (sp)
        lw              s2, 0 (sp)
        addi	        sp, sp, 32
        ret
        .endfunc
soc_esr_xwsc_end:
        .size           soc_esr_xwsc, soc_esr_xwsc_end - soc_esr_xwsc
