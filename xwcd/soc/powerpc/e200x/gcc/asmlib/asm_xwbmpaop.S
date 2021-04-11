/**
 * @file
 * @brief 架构描述层：位图原子操作汇编库
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

#include <cfg/XuanWuOS.h>

        .section        .xwos.text,"ax"
        .global         xwbmpaop_c0i
/* void xwbmpaop_c0i(xwbmp_a * bmp, xwsq_t n) */
xwbmpaop_c0i:
        e_andi          r6, r4, 31      /* r6 = r4 & 0x1F; */
        se_li           r5, 1           /* r5 = 1; */
        se_slw          r5, r6          /* r5 <<= r6; */
        se_srwi         r4, 5           /* r4 = n / 32; (BIC low 5 bits) */
        se_slwi         r4, 2           /* r4 *= 4; */
1:
        lwarx           r6, r4, r3
        se_andc         r6, r5          /* r6 &= ~r5; */
        stwcx.          r6, r4, r3
        se_bne          1b
        se_blr
        .size           xwbmpaop_c0i, . - xwbmpaop_c0i


        .section        .xwos.text,"ax"
        .global         xwbmpaop_s1i
/* void xwbmpaop_s1i(xwbmp_a * bmp, xwsq_t n) */
xwbmpaop_s1i:
        e_andi          r6, r4, 31      /* r6 = r4 & 0x1F; */
        se_li           r5, 1           /* r5 = 1; */
        se_slw          r5, r6          /* r5 <<= r6; */
        se_srwi         r4, 5           /* r4 = n / 32; (BIC low 5 bits) */
        se_slwi         r4, 2           /* r4 *= 4; */
1:
        lwarx           r6, r4, r3
        se_or           r6, r5          /* r6 |= r5; */
        stwcx.          r6, r4, r3
        se_bne          1b
        se_blr
        .size           xwbmpaop_s1i, . - xwbmpaop_s1i


        .section        .xwos.text,"ax"
        .global         xwbmpaop_x1i
/* void xwbmpaop_x1i(xwbmp_a * bmp, xwsq_t n) */
xwbmpaop_x1i:
        e_andi          r6, r4, 31      /* r6 = r4 & 0x1F; */
        se_li           r5, 1           /* r5 = 1; */
        se_slw          r5, r6          /* r5 <<= r6; */
        se_srwi         r4, 5           /* r4 = n / 32; (BIC low 5 bits) */
        se_slwi         r4, 2           /* r4 *= 4; */
1:
        lwarx           r6, r4, r3
        xor             r6, r6, r5      /* r6 ^= r5; */
        stwcx.          r6, r4, r3
        se_bne          1b
        se_blr
        .size           xwbmpaop_x1i, . - xwbmpaop_x1i


        .section        .xwos.text,"ax"
        .global         xwbmpaop_t0i_then_s1i
/* xwssq_t xwbmpaop_t0i_then_s1i(xwbmp_a * bmp, xwsq_t n) */
xwbmpaop_t0i_then_s1i:
        e_andi          r6, r4, 31      /* r6 = r4 & 0x1F; */
        se_li           r5, 1           /* r5 = 1; */
        se_slw          r5, r6          /* r5 <<= r6; */
        se_srwi         r4, 5           /* r4 = n / 32; (BIC low 5 bits) */
        se_slwi         r4, 2           /* r4 *= 4; */
        msync
1:
        lwarx           r6, r4, r3      /* r6 = r4(r3); */
        and             r7, r6, r5      /* r7 = r6 & r5 */
        e_cmpi          cr1, r7, 0      /* if (r7 == 0) { */
        e_bne           cr1, 2f
        or              r6, r6, r5      /*      r6 = r6 | r5; */
        stwcx.          r6, r4, r3      /*      Z flag = (stwcx.(r6, r4(r3))); */
        se_bne          1b              /*      if ((Z flag != 1)) goto retry; */
        se_b            3f              /*      goto s2; */
2:                                      /* } else { */
        e_li            r3, -13         /*      r3 = -EACCESS */
        se_blr                          /*      return r3; */
3:                                      /* } */
        se_li           r3, 0           /* r3 = OK; */
        msync                           /* mp_mb(); */
        se_blr                          /* return OK; */
        .size           xwbmpaop_t0i_then_s1i, . - xwbmpaop_t0i_then_s1i


        .section        .xwos.text,"ax"
        .global         xwbmpaop_t1i_then_c0i
/* xwssq_t xwbmpaop_t1i_then_c0i(xwbmp_a * bmp, xwsq_t n) */
xwbmpaop_t1i_then_c0i:
        e_andi          r6, r4, 31      /* r6 = r4 & 0x1F; */
        se_li           r5, 1           /* r5 = 1; */
        se_slw          r5, r6          /* r5 <<= r6; */
        se_srwi         r4, 5           /* r4 = n / 32; (BIC low 5 bits) */
        se_slwi         r4, 2           /* r4 *= 4; */
        msync
1:
        lwarx           r6, r4, r3      /* r6 = r4(r3); */
        and             r7, r6, r5      /* r7 = r6 & r5 */
        cmpl            cr1, 0, r7, r5  /* if (r7 == r5) { */
        e_bne           cr1, 2f
        se_andc         r6, r5          /*      r6 = r6 & ~r5; */
        stwcx.          r6, r4, r3      /*      Z flag = (stwcx.(r6, r4(r3))); */
        se_bne          1b              /*      if ((Z flag != 1)) goto retry; */
        se_b            3f              /*      goto s2; */
2:                                      /* } else { */
        e_li            r3, -13         /*      r3 = -EACCESS */
        se_blr                          /*      return r3; */
3:                                      /* } */
        se_li           r3, 0           /* r3 = OK; */
        msync                           /* mp_mb(); */
        se_blr                          /* return OK; */
        .size           xwbmpaop_t1i_then_c0i, . - xwbmpaop_t1i_then_c0i
