/**
 * @file
 * @brief 汇编库：位操作
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <cfg/XuanWuOS.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
        .section        .xwos.text,text_vle
        .global         arch_xwbop_fls32
/* xwssq_t arch_xwbop_fls32(xwu32_t x) */
arch_xwbop_fls32:
        cntlzw          r3, r3
        e_subfic        r3, r3, 31
        se_blr
        .size           arch_xwbop_fls32, . - arch_xwbop_fls32


        .section        .xwos.text,text_vle
        .global         arch_xwbop_ffs32
/* xwssq_t arch_xwbop_ffs32(xwu32_t x) */
arch_xwbop_ffs32:
        neg             r4, r3
        se_and          r4, r3
        cntlzw          r3, r4
        e_subfic        r3, r3, 31
        se_blr
        .size           arch_xwbop_ffs32, . - arch_xwbop_ffs32


#if (defined(ARCHCFG_LIB_XWBOP_FLS64)) && (1 == ARCHCFG_LIB_XWBOP_FLS64)
        .section        .xwos.text,text_vle
        .global         arch_xwbop_fls64
/* xwssq_t arch_xwbop_fls64(xwu64_t x) */
arch_xwbop_fls64:
        se_cmpi         r3, 0           /* if (high_word(x) != 0)       */
        se_beq          @s1             /* {                            */
        cntlzw          r3, r3          /*      r3 = cntlzw(r4);        */
        e_subfic        r3, r3, 63      /*      r3 = 63 - r3;           */
        se_blr                          /*      return r3;              */
                                        /* }                            */
@s1:
        cntlzw          r3, r4          /* r3 = cntlzw(r3);             */
        e_subfic        r3, r3, 31      /* r3 = 31 - r3;                */
        se_blr                          /* return r3;                   */
        .size           arch_xwbop_fls64, . - arch_xwbop_fls64
#endif


#if (defined(ARCHCFG_LIB_XWBOP_FFS64)) && (1 == ARCHCFG_LIB_XWBOP_FFS64)
        .section        .xwos.text,text_vle
        .global         arch_xwbop_ffs64
/* xwssq_t arch_xwbop_ffs64(xwu64_t x) */
arch_xwbop_ffs64:
        se_cmpi         r4, 0           /* if (low_word(x) != 0)        */
        se_beq          @s1             /* {                            */
        neg             r5, r3          /*      r5 = ~r3 + 1;           */
        se_and          r5, r3          /*      r5 = r5 & r3;           */
        cntlzw          r3, r5          /*      r3 = cntlzw(r5);        */
        e_subfic        r3, r3, 63      /*      r3 = 63 - r3;           */
        se_blr                          /*      return r3;              */
                                        /* }                            */
@s1:
        neg             r5, r4
        se_and          r5, r4
        cntlzw          r3, r5          /* r3 = cntlzw(r5);             */
        e_subfic        r3, r3, 31      /* r3 = 31 - r3;                */
        se_blr                          /* return r3;                   */
        .size           arch_xwbop_ffs64, . - arch_xwbop_ffs64
#endif
