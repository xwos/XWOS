/**
 * @file
 * @brief architecture atomic operations
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
#include <xwos/lib/errno.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief Lock a bit spin lock
 * @param addr: bit0 is lock.
 */
        .section        .xwos.text,text_vle
        .global         arch_bitsplk_lock
        .align          4
        .function       "arch_bitsplk_lock", arch_bitsplk_lock,         \
                        arch_bitsplk_lock_end - arch_bitsplk_lock
arch_bitsplk_lock:
/* void arch_bitsplk_lock(void * addr) */
        mbar            0
@retry:
        lwarx           r4, 0, r3
        e_andi.         r5, r4, 1
        se_beq          @retry /* CR0[EQ] == 1, result is zero */
        e_addi          r4, r4, -1
        stwcx.          r4, 0, r3
        se_bne          @retry
        msync
        se_blr
arch_bitsplk_lock_end:
        .size           arch_bitsplk_lock, . - arch_bitsplk_lock

/**
 * @brief Try to lock a spinlock
 * @param addr: bit0 is lock.
 * @retval -EAGAIN: Can't acquire the lock and try again
 * @retval 0: OK
 */
        .section        .xwos.text,text_vle
        .global         arch_bitsplk_trylock
        .align          4
        .function       "arch_bitsplk_trylock", arch_bitsplk_trylock,   \
                        arch_bitsplk_trylock_end - arch_bitsplk_trylock
arch_bitsplk_trylock:
/* int arch_bitsplk_trylock(void * addr) */
        e_li            r6, EAGAIN /* r6 = EAGAIN */
        neg             r6, r6
        mbar            0
@retry:
        lwarx           r4, 0, r3
        e_andi.         r5, r4, 1
        se_beq          @skip /* if (CR0[EQ] == 1) */
        e_addi          r4, r4, -1
        stwcx.          r4, 0, r3
        se_bne          @retry
        se_li           r6, 0
@skip:
        mr              r3, r6
        msync
        se_blr
arch_bitsplk_trylock_end:
        .size           arch_bitsplk_trylock, . - arch_bitsplk_trylock

/**
 * @brief Unlock a spinlock
 * @param addr: bit0 is lock.
 */
        .section        .xwos.text,text_vle
        .global         arch_bitsplk_unlock
        .align          4
        .function       "arch_bitsplk_unlock", arch_bitsplk_unlock,     \
                        arch_bitsplk_unlock_end - arch_bitsplk_unlock
arch_bitsplk_unlock:
/* void arch_bitsplk_unlock(void * addr) */
        mbar            0
@retry:
        lwarx           r4, 0, r3
        e_andi.         r5, r4, 1
        se_bne          @retry /* CR0[EQ] == 0, result is non-zero */
        e_addi          r4, r4, 1
        stwcx.          r4, 0, r3
        se_bne          @retry
        msync
        se_blr
arch_bitsplk_unlock_end:
        .size           arch_bitsplk_unlock, . - arch_bitsplk_unlock
