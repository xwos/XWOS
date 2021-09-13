/**
 * @file
 * @brief XWOS通用库：玄武OS的编译器宏定义
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_lib_compiler_h__
#define __xwos_lib_compiler_h__

#include <cfg/XuanWuOS.h>

/**
 * @defgroup compiler 编译器
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
 *     - ivt: 中断向量表
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
 * @{
 */

#include <xwos/ospl/soc/compiler.h>

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

#ifndef __xwos_ivt
  #define __xwos_ivt
#endif

#ifndef __xwos_isr
  #define __xwos_isr
#endif

#ifndef __xwos_bh
  #define __xwos_bh             __xwos_isr
#endif

#ifndef __xwos_inline
  #define __xwos_inline         __xwcc_inline
#endif

#ifndef __xwos_code
  #define __xwos_code
#endif

#ifndef __xwos_api
  #define __xwos_api            __xwos_code
#endif

#ifndef __xwos_inline_api
  #define __xwos_inline_api     __xwcc_inline
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
  #define __xwlib_inline        __xwcc_inline
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
  #define __xwbsp_inline        __xwcc_inline
#endif

#ifndef __xwbsp_data
  #define __xwbsp_data          __xwos_data
#endif

#ifndef __xwbsp_rodata
  #define __xwbsp_rodata        __xwos_rodata
#endif

#ifndef __xwmp_init_code
  #define __xwmp_init_code      __xwos_init_code
#endif

#ifndef __xwmp_init_rodata
  #define __xwmp_init_rodata    __xwos_init_rodata
#endif

#ifndef __xwmp_exit_code
  #define __xwmp_exit_code      __xwos_exit_code
#endif

#ifndef __xwmp_exit_rodata
  #define __xwmp_exit_rodata    __xwos_exit_rodata
#endif

#ifndef __xwmp_ivt
  #define __xwmp_ivt            __xwos_ivt
#endif

#ifndef __xwmp_isr
  #define __xwmp_isr            __xwos_isr
#endif

#ifndef __xwmp_bh
  #define __xwmp_bh             __xwos_bh
#endif

#ifndef __xwmp_code
  #define __xwmp_code           __xwos_code
#endif

#ifndef __xwmp_api
  #define __xwmp_api            __xwos_api
#endif

#ifndef __xwmp_inline
  #define __xwmp_inline         __xwcc_inline
#endif

#ifndef __xwmp_inline_api
  #define __xwmp_inline_api     __xwos_inline_api
#endif

#ifndef __xwmp_rodata
  #define __xwmp_rodata         __xwos_rodata
#endif

#ifndef __xwmp_data
  #define __xwmp_data           __xwos_data
#endif

#ifndef __xwup_init_code
  #define __xwup_init_code      __xwos_init_code
#endif

#ifndef __xwup_init_rodata
  #define __xwup_init_rodata    __xwos_init_rodata
#endif

#ifndef __xwup_exit_code
  #define __xwup_exit_code      __xwos_exit_code
#endif

#ifndef __xwup_exit_rodata
  #define __xwup_exit_rodata    __xwos_exit_rodata
#endif

#ifndef __xwup_ivt
  #define __xwup_ivt            __xwos_ivt
#endif

#ifndef __xwup_isr
  #define __xwup_isr            __xwos_isr
#endif

#ifndef __xwup_bh
  #define __xwup_bh             __xwos_bh
#endif

#ifndef __xwup_code
  #define __xwup_code           __xwos_code
#endif

#ifndef __xwup_api
  #define __xwup_api            __xwos_api
#endif

#ifndef __xwup_inline
  #define __xwup_inline         __xwcc_inline
#endif

#ifndef __xwup_inline_api
  #define __xwup_inline_api     __xwos_inline_api
#endif

#ifndef __xwup_rodata
  #define __xwup_rodata         __xwos_rodata
#endif

#ifndef __xwup_data
  #define __xwup_data           __xwos_data
#endif

#ifndef __xwmd_code
  #define __xwmd_code           __xwos_code
#endif

#ifndef __xwmd_api
  #define __xwmd_api            __xwmd_code
#endif

#ifndef __xwmd_inline
  #define __xwmd_inline         __xwcc_inline
#endif

#ifndef __xwmd_inline_api
  #define __xwmd_inline_api     __xwos_inline_api
#endif

#ifndef __xwmd_isr
  #define __xwmd_isr            __xwmd_code
#endif

#ifndef __xwmd_bh
  #define __xwmd_bh             __xwmd_isr
#endif

#ifndef __xwmd_rodata
  #define __xwmd_rodata         __xwos_rodata
#endif

#ifndef __xwmd_data
  #define __xwmd_data           __xwos_data
#endif

#ifndef __xwcd_code
  #define __xwcd_code           __xwos_code
#endif

#ifndef __xwcd_api
  #define __xwcd_api            __xwcd_code
#endif

#ifndef __xwcd_inline
  #define __xwcd_inline         __xwcc_inline
#endif

#ifndef __xwcd_inline_api
  #define __xwcd_inline_api     __xwos_inline_api
#endif

#ifndef __xwcd_isr
  #define __xwcd_isr            __xwcd_code
#endif

#ifndef __xwcd_bh
  #define __xwcd_bh             __xwcd_isr
#endif

#ifndef __xwcd_rodata
  #define __xwcd_rodata         __xwos_rodata
#endif

#ifndef __xwcd_data
  #define __xwcd_data           __xwos_data
#endif

/**
 * @brief: 强制从内存中访问指定类型的变量
 * @param[in] t: 类型，只能为基本类型及其typedef后的类型
 * @param[out] v: 类型为t左值，读取的值放在这个左值中
 * @note
 * + 类型t的位宽只能是8位、16位、32位、64位。
 */
#define xwmb_access(t, v)       (*((volatile t *)&(v)))

/**
 * @brief: 读取地址中的指定类型的值
 * @param[in] t: 类型，只能为基本类型及其typedef后的类型
 * @param[out] v: 类型为t左值，读取的值放在这个左值中
 * @param[in] p: 类型为t *的指针，指向需要读取的地址
 * @note
 * + 类型t的位宽只能是8位、16位、32位、64位。
 */
#define xwmb_read(t, v, p)      xwmb_access(t, v) = (*(volatile t * )(p))

/**
 * @brief 存储指定类型的值到地址中
 * @param[in] t: 类型，只能为基本类型及其typedef后的类型
 * @param[in] p: 类型为t *的指针，指向需要存储的地址
 * @param[in] v: 待存储的类型为t右值
 * @note
 * + 类型t的位宽只能是8位、16位、32位、64位。
 */
#define xwmb_write(t, p, v)     (*(volatile t * )(p)) = (v)

/******** ******** MP primitives ******** ********/
#ifndef xwmb_mp_load_acquire
/**
 * @brief: 读取地址中的指定类型的值，并保证此处的“读”操作
 *         一定发生在之后的“读写”操作之前
 * @param[in] t: 类型，只能为基本类型及其typedef后的类型
 * @param[out] v: 类型为t左值，读取的值放在这个左值中
 * @param[in] p: 类型为t *的指针，指向需要读取的地址
 * @note
 * + 类型t的位宽只能是8位、16位、32位、64位。
 */
#define xwmb_mp_load_acquire(t, v, p)           \
        do {                                    \
                xwmb_read(t, (v), (p));         \
                xwmb_mp_load_acquire_mb();      \
        } while (0)
#endif

#ifndef xwmb_mp_store_release
/**
 * @brief 存储指定类型的值到地址中，并保证此处的“写”操作
 *        一定发生在之前的“读写”操作之后
 * @param[in] t: 类型，只能为基本类型及其typedef后的类型
 * @param[in] p: 类型为t *的指针，指向需要存储的地址
 * @param[in] v: 待存储的类型为t右值
 * @note
 * + 类型t的位宽只能是8位、16位、32位、64位。
 */
#define xwmb_mp_store_release(t, p, v)          \
        do {                                    \
                xwmb_mp_store_release_mb();     \
                xwmb_write(t, (p), (v));        \
        } while (0)
#endif

#ifndef xwmb_mp_ddb
/**
 * @brief Flush all pending reads that subsequents reads depend on.
 * @detail
 * - same as mp_read_barrier_depends() in Linux.
 *   (From Linux kernel):
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
 *   + X86/X86_64这种强内存模型的CPU，带有数据依赖(Data dependency)，
 *     不需要任何读依赖屏障
 *   + ARM/PowerPC这种弱内存模型的CPU，带有数据依赖(Data dependency)，
 *     不需要任何读依赖屏障
 *   + DEC Alpha这种弱内存模型的CPU，不带有数据依赖(Data dependency)，
 *     需要使用读依赖屏障
 */
  #define xwmb_mp_ddb()                 xwccmb()
#endif

/******** ******** compiletime check ******** ********/
#ifndef xwcc_native_word
/**
 * @brief Is this type a native word size
 * @param t: something to be checked
 */
  #define xwcc_native_word(t)                   \
          (sizeof(t) == sizeof(xwu8_t) ||       \
           sizeof(t) == sizeof(xwu16_t) ||      \
           sizeof(t) == sizeof(xwu32_t) ||      \
           sizeof(t) == sizeof(xwu64_t))
#endif

/******** ******** macros to string ******** ********/
#ifndef stringify
/**
 * @brief 定义stringify的辅助宏
 */
  #define __stringify(m)                  #m

/**
 * @brief 字符串化
 * @param[in] x: 将x变为字符串
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
 * @param[in] m: 宏
 */
#define __show_macro(m)                 #m ":" stringify(m)

/******** ******** struct & member ******** ********/
#ifndef xwcc_offsetof
/**
 * @brief 计算某个成员在结构体中偏移量
 * @param[in] type: 结构体类型
 * @param[in] member: 成员在结构体中符号名
 */
  #define xwcc_offsetof(type, member)  ((xwptr_t)&(((type *)0)->member))
#endif

#ifndef xwcc_baseof
/**
 * @brief 从结构体的某个成员的地址计算出结构体的首地址
 * @param[in] ptr: 结构体某个成员的地址
 * @param[in] type: 结构体类型
 * @param[in] member: 成员在结构体中符号名
 */
  #define xwcc_baseof(ptr, type, member) \
          ((type *)(((xwptr_t)(ptr)) - xwcc_offsetof(type, member)))
#endif

/**
 * @} compiler
 */

#endif /* xwos/lib/compiler.h */
