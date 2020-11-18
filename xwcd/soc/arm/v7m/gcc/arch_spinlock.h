/**
 * @file
 * @brief 架构描述层：自旋锁
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

#ifndef __arch_spinlock_h__
#define __arch_spinlock_h__

#include <xwos/standard.h>
#include <armv7m_core.h>

#define ARCH_SPLK_TICKET_SHIFT          16
#define ARCH_SPLK_USING_BITSPLK         0

struct arch_splk {
        union {
                xwu32_t total;
                struct {
#if defined(ARCHCFG_LITTLE_ENDIAN) && (1 == ARCHCFG_LITTLE_ENDIAN)
                        xwu16_t curr; /**< current owner */
                        xwu16_t next; /**< next owner */
#else /* ARCHCFG_LITTLE_ENDIAN */
                        xwu16_t next; /**< next owner */
                        xwu16_t curr; /**< current owner */
#endif /* !ARCHCFG_LITTLE_ENDIAN */
                } tickets;
        } v;
};

/**
 * @brief 在指针中初始化位自旋锁。
 * @param ptr: (I) 待初始化位自旋锁的指针
 */
static __xwbsp_inline
void arch_bitsplk_init(xwptr_t * ptr)
{
        *ptr |= 1U;
}

#if (CPUCFG_CPU_NUM > 1)
/**
 * @brief 对位自旋锁上锁。
 * @param ptr: (I) 携带位自旋锁的指针
 */
static __xwbsp_inline
void arch_bitsplk_lock(xwptr_t * ptr)
{
        prefetch(ptr);
        __asm__ volatile(
        "1:\n"
        "       ldrex   r0, [%[__lock]]\n"
        "       tst     r0, #1\n"       /* Z = ~(r0 & 1); */
        "       ittte   ne\n"
        "       bicne   r0, #1\n"
        "       strexne r1, r0, [%[__lock]]\n"
        "       teqne   r1, #1\n"
        "       beq     1b\n"
        :
        : [__lock] "r" (ptr)
        : "r0", "r1", "cc", "memory"
        );
        xwmb_mp_mb();
}

#endif /* (CPUCFG_CPU_NUM > 1) */

#if (CPUCFG_CPU_NUM > 1)
/**
 * @brief 试图对位自旋锁上锁。
 * @param ptr: (I) 携带位自旋锁的指针
 * @retval 0: OK
 * @retval -EAGAIN: 上锁失败
 */
static __xwbsp_inline
xwer_t arch_bitsplk_trylock(xwptr_t * ptr)
{
        int rc;

        prefetch(ptr);
        __asm__ volatile(
        "1:\n"
        "       ldrex   r1, [%[__lock]]\n"
        "       tst     r1, #1\n"       /* Z = ~(r0 & 1); */
        "       itte    ne\n"           /* if (Z != 1) */
        "       bicne   r1, #1\n"
        "       strexne r0, r1, [%[__lock]]\n"
        "       mvneq   r0, #10\n" /* -EAGAIN */
        "       teq     r0, #1\n"
        "       beq     1b\n"
        "       mov     %[rc], r0\n"
        : [rc] "=&r" (rc)
        : [__lock] "r" (ptr)
        : "r0", "r1", "cc", "memory"
        );
        xwmb_mp_mb();
        return rc;
}
#endif /* (CPUCFG_CPU_NUM > 1) */

#if (CPUCFG_CPU_NUM > 1)
/**
 * @brief 解锁位自旋锁。
 * @param ptr: ptr: (I) 携带位自旋锁的指针
 */
static __xwbsp_inline
void arch_bitsplk_unlock(xwptr_t * ptr)
{
        xwmb_mp_mb();
        __asm__ volatile(
        "1:\n"
        "       ldrex   r0, [%[__lock]]\n"
        "       tst     r0, #1\n"       /* Z = ~(r0 & 1); */
        "       ittte   eq\n"           /* if (Z == 1) */
        "       orreq   r0, #1\n"
        "       strexeq r1, r0, [%[__lock]]\n"
        "       teqeq   r1, #0\n"
        "       bne     1b\n"
        :
        : [__lock] "r" (ptr)
        : "r0", "r1", "cc", "memory"
        );
        armv7m_dsb();
}
#endif /* (CPUCFG_CPU_NUM > 1) */

/**
 * @brief Initialize a spinlock
 * @parem asl: arch spin lock
 */
static __xwbsp_inline
void arch_splk_init(struct arch_splk * asl)
{
        asl->v.tickets.next = 1;
        asl->v.tickets.curr = 1;
}

#if (CPUCFG_CPU_NUM > 1)
/**
 * @brief Lock a spinlock
 * @parem asl: arch spin lock
 */
static __xwbsp_inline
void arch_splk_lock(struct arch_splk * asl)
{
#if (1 == ARCH_SPLK_USING_BITSPLK)
        arch_bitsplk_lock(asl);
#else /* (1 == ARCH_SPLK_USING_BITSPLK) */
        xwer_t rc;
        xwu32_t newval;
        struct arch_splk tmp;

        prefetch(asl);
        __asm__ volatile(
        "1:\n"
        "       ldrex   %[__tmp], [%[__lock]]\n"
        "       add     %[__newval], %[__tmp], %[__ticket]\n"
        "       strex   %[__rc], %[__newval], [%[__lock]]\n"
        "       teq     %[__rc], #0\n"
        "       bne     1b\n"
        : [__tmp] "=&r" (tmp.v.total),
          [__newval] "=&r" (newval),
          [__rc] "=&r" (rc)
        : [__lock] "r" (asl),
          [__ticket] "I" (1 << ARCH_SPLK_TICKET_SHIFT)
        : "cc", "memory"
        );

        while (tmp.v.tickets.next != tmp.v.tickets.curr) {
                wfe();
                tmp.v.tickets.curr = xwmb_access(xwu16_t, asl->v.tickets.curr);
        }
        xwmb_mp_mb();
#endif /* !(1 == ARCH_SPLK_USING_BITSPLK) */
}

/**
 * @brief Try to lock a spinlock
 * @parem asl: arch spin lock
 * @retval 0: OK
 * @retval -EAGAIN: Can't acquire the lock and try again
 */
static __xwbsp_inline
xwer_t arch_splk_trylock(struct arch_splk * asl)
{
#if (1 == ARCH_SPLK_USING_BITSPLK)
        return arch_bitsplk_trylock(asl);
#else /* (1 == ARCH_SPLK_USING_BITSPLK) */
        xwer_t rc;
        xwu32_t contended;
        struct arch_splk tmp;

        prefetch(asl);
        do {
                __asm__ volatile(
                "       ldrex   %[__tmp], [%[__lock]]\n"
                "       mov     %[__rc], #0\n"
                "       ror     %[__contended], %[__tmp], #16\n"
                "       subs    %[__contended], %[__tmp]\n"
                "       itt     eq\n"
                "       addeq   %[__tmp], %[__tmp], %[__ticket]\n"
                "       strexeq %[__rc], %[__tmp], [%[__lock]]\n"
                : [__tmp] "=&r" (tmp.v.total),
                  [__contended] "=&r" (contended),
                  [__rc] "=&r" (rc)
                : [__lock] "r" (asl),
                  [__ticket] "I" (1 << ARCH_SPLK_TICKET_SHIFT)
                : "cc"
                );
        } while (rc);

        if (0 == contended) {
                xwmb_mp_mb();
                return XWOK;
        } else {
                return -EAGAIN;
        }
#endif /* !(1 == ARCH_SPLK_USING_BITSPLK) */
}

/**
 * @brief Unlock a spinlock
 * @parem asl: arch spin lock
 */
static __xwbsp_inline
void arch_splk_unlock(struct arch_splk * asl)
{
#if (1 == ARCH_SPLK_USING_BITSPLK)
        arch_bitsplk_unlock(asl);
#else /* (1 == ARCH_SPLK_USING_BITSPLK) */
        xwmb_mp_mb();
        asl->v.tickets.curr++;
        armv7m_dsb();
        sev();
#endif /* !(1 == ARCH_SPLK_USING_BITSPLK) */
}
#endif /* (CPUCFG_CPU_NUM > 1) */

#endif /* arch_spinlock.h */
