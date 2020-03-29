/**
 * @file
 * @brief 玄武OS的编译器宏定义
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * @note
 * + 段说明
 *   - __xwos_init_*
 *     - code: XWOS初始化阶段的代码
 *     - rodata: XWOS初始化阶段的const数据
 *     - （初始化阶段可能数据还未重定向至内存，因此不存在__xwos_init_data）
 *   - __xwos_exit_*
 *     - code: XWOS退出阶段的代码
 *     - rodata: XWOS退出阶段的const数据
 *   - __xwos_*
 *     - code: XWOS内核的代码
 *     - vctbl: 中断向量表
 *     - isr: 中断函数
 *     - bh: 底半部函数
 *     - api: XWOS的内核API函数
 *     - rodata: XWOS内核的const数据
 *     - data: XWOS内核的数据
 *   - __xwlib_*
 *     - code: XWOS的C库函数
 *     - rodata: XWOS的C库const数据
 *     - data: XWOS的C库数据
 *   - __xwbsp_*
 *     - init_code: BSP的初始化阶段的代码
 *     - init_rodata: BSP的初始化阶段的const数据
 *     - exit_code: BSP的退出阶段的代码
 *     - exit_rodata: BSP的退出阶段的const数据
 *     - code: BSP中的代码
 *     - api: BSP中的API函数
 *     - isr: BSP中的中断函数
 *     - rodata: BSP中的const数据
 *     - data: BSP中的数据
 *   - __xwmd_*
 *     - code: XWOS中间件的代码
 *     - isr: XWOS中间件中断函数
 *     - api: XWOS中间件的API函数
 *     - rodata: XWOS中间件的const数据
 *     - data: XWOS中间件的数据
 */

#ifndef __xwos_compiler_h__
#define __xwos_compiler_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <cfg/XuanWuOS.h>
#include <arch_compiler.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      section      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#ifndef __xwos_init_code
  #define __xwos_init_code
#endif

#ifndef __xwos_init_rodata
  #define __xwos_init_rodata
#endif

#ifndef __xwos_exit_code
  #define __xwos_exit_code
#endif

#ifndef __xwos_exit_rodata
  #define __xwos_exit_rodata
#endif

#ifndef __xwos_vctbl
  #define __xwos_vctbl
#endif

#ifndef __xwos_isr
  #define __xwos_isr
#endif

#ifndef __xwos_bh
  #define __xwos_bh             __xwos_isr
#endif

#ifndef __xwos_code
  #define __xwos_code
#endif

#ifndef __xwos_api
  #define __xwos_api            __xwos_code
#endif

#ifndef __xwos_inline_api
  #define __xwos_inline_api     __xw_inline
#endif

#ifndef __xwos_rodata
  #define __xwos_rodata
#endif

#ifndef __xwos_data
  #define __xwos_data
#endif

#ifndef __xwlib_code
  #define __xwlib_code          __xwos_code
#endif

#ifndef __xwlib_inline
  #define __xwlib_inline        __xw_inline
#endif

#ifndef __xwlib_data
  #define __xwlib_data          __xwos_data
#endif

#ifndef __xwlib_rodata
  #define __xwlib_rodata        __xwos_rodata
#endif

#ifndef __xwbsp_init_code
  #define __xwbsp_init_code     __xwos_init_code
#endif

#ifndef __xwbsp_init_rodata
  #define __xwbsp_init_rodata   __xwos_init_rodata
#endif

#ifndef __xwbsp_exit_code
  #define __xwbsp_exit_code     __xwos_exit_code
#endif

#ifndef __xwbsp_exit_rodata
  #define __xwbsp_exit_rodata   __xwos_exit_rodata
#endif

#ifndef __xwbsp_code
  #define __xwbsp_code          __xwos_code
#endif

#ifndef __xwbsp_api
  #define __xwbsp_api           __xwos_api
#endif

#ifndef __xwbsp_isr
  #define __xwbsp_isr           __xwos_isr
#endif

#ifndef __xwbsp_inline
  #define __xwbsp_inline        __xw_inline
#endif

#ifndef __xwbsp_data
  #define __xwbsp_data          __xwos_data
#endif

#ifndef __xwbsp_rodata
  #define __xwbsp_rodata        __xwos_rodata
#endif

#ifndef __xwmd_code
  #define __xwmd_code
#endif

#ifndef __xwmd_api
  #define __xwmd_api            __xwmd_code
#endif

#ifndef __xwmd_inline
  #define __xwmd_inline         __xw_inline
#endif

#ifndef __xwmd_inline_api
  #define __xwmd_inline_api     __xwos_inline_api
#endif

#ifndef __xwmd_isr
  #define __xwmd_isr
#endif

#ifndef __xwmd_bh
  #define __xwmd_bh             __xwmd_isr
#endif

#ifndef __xwmd_rodata
  #define __xwmd_rodata
#endif

#ifndef __xwmd_data
  #define __xwmd_data
#endif

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#ifndef __XWOS_COMPILER__
  #error "Unknown compiler!"
#endif

/******** ******** SMP primitives ******** ********/
#ifndef xwmb_smp_load_acquire
/**
 * @brief: macro to load a pointer safely for later dereference
 * @param v: (I) lvalue
 * @param p: (I) the address to load
 */
#define xwmb_smp_load_acquire(v, p)             \
        do {                                    \
                v = xwmb_fcrd(*p);              \
                xwmb_smp_mb();                  \
        } while (0)
#endif

#ifndef xwmb_smp_store_release
/**
 * @brief Store a value to a pointer and prevents the compiler/HW from
 *        reordering the code that initializes the structure after the pointer
 *        assignment.
 * @param p: (O) pointer to the address that is assigned to
 * @param v: (I) value to assign (publish)
 */
#define xwmb_smp_store_release(p, v)            \
        do {                                    \
                xwmb_smp_mb();                  \
                xwmb_fcwr(*(p), v);             \
        } while (0)
#endif

#ifndef xwmb_smp_store_mb
/**
 * @brief Store a value to a pointer and prevents the compiler/HW from
 *        reordering the code that initializes the structure before the pointer
 *        assignment.
 * @param p: (O) pointer to the address that is assigned to
 * @param v: (I) value to assign
 */
#define xwmb_smp_store_mb(p, v)                 \
        do {                                    \
                xwmb_fcwr((*(p)), v);           \
                xwmb_smp_mb();                  \
        } while (0)
#endif

#ifndef xwmb_smp_ddb
/**
 * @brief Flush all pending reads that subsequents reads depend on.
 * @detail
 * - same as smp_read_barrier_depends() in Linux.
 * - (From Linux kernel):
 *   No data-dependent reads from memory-like regions are ever reordered
 *   over this barrier.  All reads preceding this primitive are guaranteed
 *   to access memory (but not necessarily other CPUs' caches) before any
 *   reads following this primitive that depend on the data return by
 *   any of the preceding reads.  This primitive is much lighter weight than
 *   rmb() on most CPUs, and is never heavier weight than is
 *   rmb().
 *
 *   These ordering constraints are respected by both the local CPU
 *   and the compiler.
 *
 *   Ordering is not guaranteed by anything other than these primitives,
 *   not even by data dependencies.  See the documentation for
 *   memory_barrier() for examples and URLs to more information.
 *
 *   For example, the following code would force ordering (the initial
 *   value of "a" is zero, "b" is one, and "p" is "&a"):
 *
 *   ```C
 *      CPU 0                           CPU 1
 *
 *      b = 2;
 *      memory_barrier();
 *      p = &b;                         q = p;
 *                                      read_barrier_depends();
 *                                      d = *q;
 *   ```
 *
 *   because the read of "*q" depends on the read of "p" and these
 *   two reads are separated by a read_barrier_depends().  However,
 *   the following code, with the same initial values for "a" and "b":
 *
 *   ```C
 *      CPU 0                           CPU 1
 *
 *      a = 2;
 *      memory_barrier();
 *      b = 3;                          y = b;
 *                                      read_barrier_depends();
 *                                      x = a;
 *   ```
 *
 *   does not enforce ordering, since there is no data dependency between
 *   the read of "a" and the read of "b".  Therefore, on some CPUs, such
 *   as Alpha, "y" could be set to 3 and "x" to 0.  Use rmb()
 *   in cases like this where there are no data dependencies.
 * - C/C++中的内存顺序 —— 消费序(memory_order_consume)所表达的意思也即是读依赖屏障。
 *   + X86/X86_64这种强内存模型的CPU，自带数据依赖(Data dependency)，
 *     不需要任何读依赖屏障
 *   + ARM/PowerPC这种弱内存模型的CPU，也带有数据依赖(Data dependency)，
 *     不需要任何读依赖屏障
 *   + DEC Alpha这种弱内存模型的CPU，不带有数据依赖(Data dependency)，
 *     需要使用读依赖屏障
 */
  #define xwmb_smp_ddb()                xwccmb()
#endif

/******** ******** compiletime check ******** ********/
#ifndef xwcc_native_word
/**
 * @brief Is this type a native word size
 * @param t: something to be checked
 */
  #define xwcc_native_word(t)                   \
          (sizeof(t) == sizeof(char) ||         \
           sizeof(t) == sizeof(short) ||        \
           sizeof(t) == sizeof(int) ||          \
           sizeof(t) == sizeof(long))
#endif

/******** ******** macros to string ******** ********/
#ifndef stringify
/**
 * @brief 定义stringify的辅助宏
 */
  #define __stringify(m)                  #m

/**
 * @brief 字符串化
 * @param x: (I) 将x变为字符串
 * @note
 * C语言宏展开的规则：遇到#和##就停止展开宏。
 * 因此如果直接定义stringify(m)为#m，
 * 遇到下面的代码将不能正常工作：
 * ```C
 * #define stringify(m)       #m
 * #define NAME               Roy
 * const char my_name[] = stringify(NAME);
 * // 结果是 my_name[] = "NAME";
 * ```
 * 正确的定义方法应该是：
 * ```C
 * __stringify(m)             #m
 * #define stringify(m)       __stringify(m)
 * #define NAME               Roy
 * const char my_name[] = stringify(NAME);
 * // 结果是 my_name[] = "Roy";
 * ```
 * 因为NAME在stringify(m)中展开，此时没有遇到#或##。
 */
  #define stringify(m)                    __stringify(m)
#endif /* stringify */

/**
 * @brief 显示宏的值
 * @param m: (I) 宏
 */
#define __show_macro(m)                 #m ":" stringify(m)

/******** ******** struct & member ******** ********/
#ifndef offsetof
/**
 * @brief 计算某个成员在结构体中偏移量
 * @param type: (I) 结构体类型
 * @param member: (I) 成员在结构体中符号名
 */
  #define offsetof(type, member)  ((xwptr_t)&(((type *)0)->member))
#endif

#ifndef container_of
/**
 * @brief 从结构体的某个成员的地址计算出结构体的首地址
 * @param ptr: (I) 结构体某个成员的地址
 * @param type: (I) 结构体类型
 * @param member: (I) 成员在结构体中符号名
 */
  #define container_of(ptr, type, member) \
          ((type *)(((xwptr_t)(ptr)) - offsetof(type, member)))
#endif

#endif /* xwos/compiler.h */
