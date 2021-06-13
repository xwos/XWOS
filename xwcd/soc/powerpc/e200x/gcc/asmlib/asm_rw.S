/**
 * @file
 * @brief 架构描述层：读写库
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
#include <xwos/lib/errno.h>

/**
 * @brief load exclusively
 * @param[in] addr: 地址
 */
        .section        .xwos.text,"ax"
        .global         lwarx
        .align          4
lwarx:
/* xwu32_t lwarx(volatile void * addr) */
        lwarx           r4, 0, r3
        se_mr           r3, r4
        se_blr
lwarx_end:
        .size           lwarx, . - lwarx


/**
 * @brief store exclusively
 * @param[in] addr: 地址
 * @param[in] value: 数值
 * @return boolean
 * @retval 0: OK
 * @retval !0: FAILED
 */
        .section        .xwos.text,"ax"
        .global         stwcx
        .align          4
stwcx:
/* xwer_t stwcx(volatile void * addr, xwu32_t value) */
        stwcx.          r4, 0, r3
        se_bne          1f
        se_li           r3, 0
        se_b            2f
1:
        se_li           r3, 1
2:
        se_blr
stwcx_end:
        .size           stwcx, . - stwcx


/**
 * @brief 反转字节序
 * @param[in] addr: 需要反转字节序的地址
 */
        .section        .xwos.text,"ax"
        .global         lwbrx
        .align          4
lwbrx:
/* xwu32_t lwbrx(volatile xwu32_t * addr) */
        lwbrx           r4, 0, r3
        se_mr           r3, r4
        se_blr
lwbrx_end:
        .size           lwbrx, . - lwbrx


/**
 * @brief 反转半字的字节序
 * @param[in] addr: 需要反转字节序的地址
 */
        .section        .xwos.text,"ax"
        .global         lhbrx
        .align          4
lhbrx:
/* xwu16_t lhbrx(volatile xwu16_t * addr) */
        lhbrx           r4, 0, r3
        se_mr           r3, r4
        se_blr
lhbrx_end:
        .size           lhbrx, . - lhbrx
