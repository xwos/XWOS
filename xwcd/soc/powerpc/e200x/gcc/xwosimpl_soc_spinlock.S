/**
 * @file
 * @brief 玄武OS移植实现层：SOC自旋锁
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


        .section        .xwos.text,"ax"
        .global         soc_splk_lock
        .align          4
soc_splk_lock:
/* void soc_splk_lock(void * addr) */
        mbar            0
1:
        lwarx           r4, 0, r3
        e_andi.         r5, r4, 1
        se_beq          1b /* CR0[EQ] == 1, result is zero */
        e_addi          r4, r4, -1
        stwcx.          r4, 0, r3
        se_bne          1b
        msync
        se_blr
soc_splk_lock_end:
        .size           soc_splk_lock, . - soc_splk_lock


        .section        .xwos.text,"ax"
        .global         soc_splk_trylock
        .align          4
soc_splk_trylock:
/* int soc_splk_trylock(void * addr) */
        e_li            r6, EAGAIN /* r6 = EAGAIN */
        neg             r6, r6
        mbar            0
1:
        lwarx           r4, 0, r3
        e_andi.         r5, r4, 1
        se_beq          2f /* if (CR0[EQ] == 1) */
        e_addi          r4, r4, -1
        stwcx.          r4, 0, r3
        se_bne          1b
        se_li           r6, 0
2:
        mr              r3, r6
        msync
        se_blr
soc_splk_trylock_end:
        .size           soc_splk_trylock, . - soc_splk_trylock


        .section        .xwos.text,"ax"
        .global         soc_splk_unlock
        .align          4
soc_splk_unlock:
/* void soc_splk_unlock(void * addr) */
        mbar            0
1:
        lwarx           r4, 0, r3
        e_andi.         r5, r4, 1
        se_bne          1b /* CR0[EQ] == 0, result is non-zero */
        e_addi          r4, r4, 1
        stwcx.          r4, 0, r3
        se_bne          1b
        msync
        se_blr
soc_splk_unlock_end:
        .size           soc_splk_unlock, . - soc_splk_unlock
