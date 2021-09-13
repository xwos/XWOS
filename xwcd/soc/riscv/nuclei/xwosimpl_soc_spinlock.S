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

        .equ            EAGAIN, 11


        .section        .xwos.text,"ax"
        .global         soc_splk_lock
        .align          2
        .func           soc_splk_lock
soc_splk_lock:
/* void soc_splk_lock(struct soc_splk * socsplk) */
        li              t0, 0
1:
        amoswap.w.aq    t1, t0, (a0)
        beqz            t1, 1b
        ret
        .endfunc
soc_splk_lock_end:
        .size           soc_splk_lock, . - soc_splk_lock


        .section        .xwos.text,"ax"
        .global         soc_splk_trylock
        .align          2
        .func           soc_splk_trylock
soc_splk_trylock:
/* int soc_splk_trylock(struct soc_splk * socsplk) */
        li              t0, 0
        amoswap.w.aq    t1, t0, (a0)
        beqz            t1, 1f
        li              a0, 0
        j               2f
1:
        li              a0, -EAGAIN
2:
        ret
        .endfunc
soc_splk_trylock_end:
        .size           soc_splk_trylock, . - soc_splk_trylock


        .section        .xwos.text,"ax"
        .global         soc_splk_unlock
        .align          2
        .func           soc_splk_unlock
soc_splk_unlock:
/* void soc_splk_unlock(struct soc_splk * socsplk) */
        li              t0, 1
        amoswap.w.rl    t0, t0, (a0)
        ret
        .endfunc
soc_splk_unlock_end:
        .size           soc_splk_unlock, . - soc_splk_unlock
