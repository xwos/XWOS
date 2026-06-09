/**
 * @file
 * @brief XWOS移植实现层：ARCH自旋锁
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

#ifndef __xwosimpl_arch_spinlock_h__
#define __xwosimpl_arch_spinlock_h__

#ifndef __xwos_ospl_spinlock_h__
#  error "This file should be included from <xwos/ospl/spinlock.h>."
#endif

#include <xwos/standard.h>
#include <xwcd/soc/arm64/v8a/arch_isa.h>

#define XWOSPL_SPLK_QUEUE_SHIFT 16U

/**
 * @brief ARMv8-A的自旋锁
 * @details
 * 自旋锁的实现，类似于银行的排队系统。每个CPU申请锁时，
 * 可从“排队系统” `xwospl_splk.v.q.n` 中申请到一个排队号。
 * `xwospl_splk.v.q.n` 输出一个排队号后自增1。
 * 当“叫号系统” `xwospl_splk.v.q.c` 叫到排队号时，持有排队号的CPU可以获得锁。
 */
struct xwospl_splk {
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


#define XWOSPL_SPLK_INITIALIZER { .v.q.n = 0, .v.q.c = 0, }

static __xwbsp_inline
void xwospl_splk_init(struct xwospl_splk * osplsplk)
{
        osplsplk->v.q.n = 0;
        osplsplk->v.q.c = 0;
}

#if (CPUCFG_CPU_NUM > 1)
static __xwbsp_inline
void xwospl_splk_lock(struct xwospl_splk * osplsplk)
{
        xwer_t rc;
        struct xwospl_splk lkval;
        xwu16_t queue_number;

        armv8a_prefetch_before_aop(osplsplk->v.raw);
        do {
                lkval.v.raw = armv8a_load_acquire_exclusively_32b(
                        (atomic_xwu32_t *)&osplsplk->v.raw);
                queue_number = lkval.v.q.n;
                lkval.v.q.n++;
                rc = armv8a_store_exclusively_32b(
                        (atomic_xwu32_t *)&osplsplk->v.raw, lkval.v.raw);
        } while (rc);
        if (queue_number != lkval.v.q.c) {
                /* ARM指令手册中建议在 `WFE` 等待循环之前插入一条 `SEVL` 指令。
                   防止丢失其他CPU的解锁事件。 */
                __asm__ volatile("sevl" : : : "memory");
                do {
                        __asm__ volatile("wfe" : : : "memory"
                        );
                        lkval.v.q.c = armv8a_load_acquire_exclusively_16b(
                                (atomic_xwu16_t *)&osplsplk->v.q.c);
                } while (queue_number != lkval.v.q.c);
        }
}

static __xwbsp_inline
xwer_t xwospl_splk_trylock(struct xwospl_splk * osplsplk)
{
        xwer_t rc;
        struct xwospl_splk lkval;

        armv8a_prefetch_before_aop(osplsplk->v.raw);
        do {
                lkval.v.raw = armv8a_load_acquire_exclusively_32b(
                        (atomic_xwu32_t *)&osplsplk->v.raw);
                if (lkval.v.q.n != lkval.v.q.c) {
                        rc = -EAGAIN;
                        break;
                }
                lkval.v.q.n++;
                rc = armv8a_store_exclusively_32b(
                        (atomic_xwu32_t *)&osplsplk->v.raw, lkval.v.raw);
        } while (rc);
        return rc;
}

static __xwbsp_inline
void xwospl_splk_unlock(struct xwospl_splk * osplsplk)
{
        __xwcc_atomic xwu16_t n;

        n = osplsplk->v.q.c + 1;
        /* 根据ARM架构参考手册中的描述，"global monitor" 检测到 "PE" 的
           "Exclusive Access" 状态变为可访问时，会向 "PE" 发送一个 "Event" ，使得
           正在执行 `WFE` 指令的 "PE" 唤醒。*/
        armv8a_store_release_16b((atomic_xwu16_t *)&osplsplk->v.q.c, n);
}
#endif

#endif /* xwosimpl_arch_spinlock.h */
