/**
 * @file
 * @brief XWOS移植实现层：SOC：自旋锁
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
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

#ifndef __xwosimpl_soc_spinlock_h__
#define __xwosimpl_soc_spinlock_h__

#ifndef __xwos_ospl_soc_spinlock_h__
#  error "This file should be included from <xwos/ospl/soc/spinlock.h>."
#endif

#include <xwos/standard.h>
#include <xwcd/soc/arm64/v8a/arch_isa.h>

#define SOC_SPLK_QUEUE_SHIFT 16U

/**
 * @brief ARMv8-A的自旋锁
 * @details
 * 自旋锁的实现，类似于银行的排队系统。每个CPU申请锁时，
 * 可从“排队系统” `soc_splk.v.q.n` 中申请到一个排队号。
 * `soc_splk.v.q.n` 输出一个排队号后自增1。
 * 当“叫号系统” `soc_splk.v.q.c` 叫到排队号时，持有排队号的CPU可以获得锁。
 */
struct soc_splk {
        union {
                xwu32_t raw; /**< 值 */
                struct {
#if defined(ARCHCFG_LITTLE_ENDIAN) && (1 == ARCHCFG_LITTLE_ENDIAN)
                        xwu16_t c; /**< 当前的持锁者 */
                        xwu16_t n; /**< 下一个持锁者 */
#else
                        xwu16_t n; /**< 下一个持锁者 */
                        xwu16_t c; /**< 当前的持锁者 */
#endif
                } q; /**< 队列 */
        } v;
} __xwcc_aligned(4);


#define SOC_SPLK_INITIALIZER { .v.q.n = 0, .v.q.c = 0, }

static __xwbsp_inline
void soc_splk_init(struct soc_splk * splk)
{
        splk->v.q.n = 0;
        splk->v.q.c = 0;
}

#if (CPUCFG_CPU_NUM > 1)
static __xwbsp_inline
void soc_splk_lock(struct soc_splk * splk)
{
        xwer_t rc;
        struct soc_splk lkval;
        xwu16_t queue_number;

        armv8a_prefetch_before_aop(splk->v.raw);
        do {
                lkval.v.raw = armv8a_load_acquire_exclusively_32b((atomic_xwu32_t *)&splk->v.raw);
                queue_number = lkval.v.q.n;
                lkval.v.q.n++;
                rc = armv8a_store_exclusively_32b((atomic_xwu32_t *)&splk->v.raw, lkval.v.raw);
        } while (rc);
        if (queue_number != lkval.v.q.c) {
                /* ARM指令手册中建议在 `WFE` 等待循环之前插入一条 `SEVL` 指令。
                   防止丢失其他CPU的解锁事件。 */
                __asm__ volatile(
                "       sevl\n" : : : "memory"
                );
                do {
                        __asm__ volatile(
                        "       wfe\n" : : : "memory"
                        );
                        lkval.v.q.c = armv8a_load_acquire_exclusively_16b((atomic_xwu16_t *)&splk->v.q.c);
                } while (queue_number != lkval.v.q.c);
        }
}

static __xwbsp_inline
xwer_t soc_splk_trylock(struct soc_splk * splk)
{
        xwer_t rc;
        struct soc_splk lkval;

        armv8a_prefetch_before_aop(splk->v.raw);
        do {
                lkval.v.raw = armv8a_load_acquire_exclusively_32b((atomic_xwu32_t *)&splk->v.raw);
                if (lkval.v.q.n != lkval.v.q.c) {
                        rc = -EAGAIN;
                        break;
                }
                lkval.v.q.n++;
                rc = armv8a_store_exclusively_32b((atomic_xwu32_t *)&splk->v.raw, lkval.v.raw);
        } while (rc);
        return rc;
}

static __xwbsp_inline
void soc_splk_unlock(struct soc_splk * splk)
{
        __xwcc_atomic xwu16_t n;

        n = splk->v.q.c + 1;
        /* 根据ARM架构继续参考手册中的描述，"global monitor"检测到"PE"的
           "Exclusive Access"状态变为可访问时，会向"PE"发送一个"Event"，使得
           正在执行 `WFE` 指令的"PE"唤醒。参考：
           + 《ARMv8-A Architecture RM.pdf》 B2.10.6
           + 《ARMv8-A Architecture RM.pdf》 D1.17 */
        armv8a_store_release_16b((atomic_xwu16_t *)&splk->v.q.c, n);
}
#endif

#endif /* xwosimpl_soc_spinlock.h */
