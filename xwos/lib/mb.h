/**
 * @file
 * @brief XWOS通用库：内存屏障
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_lib_mb_h__
#define __xwos_lib_mb_h__

#include <cfg/project.h>

/**
 * @defgroup xwos_lib_mb 内存屏障
 * @ingroup xwos_lib
 * @{
 */

#include <xwos/ospl/soc/isa.h>

/**
 * @brief: 强制从内存中访问指定类型的变量
 * @param[in] t: 类型，只能为基本类型
 * @param[out] v: 类型为t左值，读取的值放在这个左值中
 * @note
 * + 类型t的位宽只能是8位、16位、32位、64位。
 */
// cppcheck-suppress [misra-c2012-20.7]
#define xwmb_access(t, v)       (*((volatile t *)&(v)))

/**
 * @brief: 读取地址中的指定类型的值
 * @param[in] t: 类型，只能为基本类型及其typedef后的类型
 * @param[out] v: 类型为t左值，读取的值放在这个左值中
 * @param[in] p: 类型为t *的指针，指向需要读取的地址
 * @note
 * + 类型t的位宽只能是8位、16位、32位、64位。
 */
// cppcheck-suppress [misra-c2012-20.7]
#define xwmb_read(t, v, p)      xwmb_access(t, v) = (*(volatile t * )(p))

/**
 * @brief 存储指定类型的值到地址中
 * @param[in] t: 类型，只能为基本类型及其typedef后的类型
 * @param[in] p: 类型为t *的指针，指向需要存储的地址
 * @param[in] v: 待存储的类型为t右值
 * @note
 * + 类型t的位宽只能是8位、16位、32位、64位。
 */
// cppcheck-suppress [misra-c2012-20.7]
#define xwmb_write(t, p, v)     (*(volatile t * )(p)) = (v)

#if !defined(xwmb_compiler) || defined(__DOXYGEN__)
/**
 * @brief 编译器内存屏障
 */
#  define xwmb_compiler()
#endif

#if !defined(xwmb_mp_mb) || defined(__DOXYGEN__)
/**
 * @brief 多核系统内存屏障
 */
#  define xwmb_mp_mb()
#endif

#if !defined(xwmb_mp_rmb) || defined(__DOXYGEN__)
/**
 * @brief 多核系统读内存屏障
 */
#  define xwmb_mp_rmb()
#endif

#if !defined(xwmb_mp_wmb) || defined(__DOXYGEN__)
/**
 * @brief 多核写内存屏障
 */
#  define xwmb_mp_wmb()
#endif

#if !defined(xwmb_ddb) || defined(__DOXYGEN__)
/**
 * @brief 数据依赖屏障
 * @detail
 * + Flush all pending reads that subsequents reads depend on.
 * + same as `mp_read_barrier_depends()` in Linux.
 *   (From Linux kernel):
 *   No data-dependent reads from memory-like regions are ever reordered
 *   over this barrier. All reads preceding this primitive are guaranteed
 *   to access memory (but not necessarily other CPUs' caches) before any
 *   reads following this primitive that depend on the data return by
 *   any of the preceding reads.  This primitive is much lighter weight than
 *   `rmb()` on most CPUs, and is never heavier weight than is
 *   `rmb()` .
 *
 *   These ordering constraints are respected by both the local CPU
 *   and the compiler.
 *
 *   Ordering is not guaranteed by anything other than these primitives,
 *   not even by data dependencies.  See the documentation for
 *   `memory_barrier()` for examples and URLs to more information.
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
 * + 上文描述简单理解就是两次读之间的依赖关系，比较常见的场景：
 *   + 通过 **指针** 访问 **变量** 时，若 **指针** 也是从内存中读取的，
 *     **指针** 的 **读** 和 **变量** 的 **读** 之间产生读依赖。
 *     如果 **指针** 内存的 **写** 是在其他CPU完成的，就要考虑数据依赖屏障。
 *   + 读数组中 **元素** 时，若数组 **指针** 是从内存中读取的，
 *     **指针** 的 **读** 和 **元素** 的 **读** 之间产生读依赖。
 *     如果 **指针** 内存的 **写** 是在其他CPU完成的，就要考虑数据依赖屏障。
 * + C/C++中的内存顺序 —— 消费序( `memory_order_consume` )所表达的意思也即是读依赖屏障。
 *   + X86/X86_64这种强内存模型的CPU，带有数据依赖(Data dependency)，
 *     不需要任何读依赖屏障
 *   + ARM/PowerPC这种弱内存模型的CPU，带有数据依赖(Data dependency)，
 *     不需要任何读依赖屏障
 *   + DEC Alpha这种弱内存模型的CPU，不带有数据依赖(Data dependency)，
 *     需要使用读依赖屏障
 */
#  define xwmb_ddb()    xwmb_compiler()
#endif

#if !defined(xwmb_mp_consume) || defined(__DOXYGEN__)
/**
 * @brief 多核系统 **消费** 屏障，数据依赖屏障
 */
#  define xwmb_mp_consume()     xwmb_ddb()
#endif

#if !defined(xwmb_mp_acquire) || defined(__DOXYGEN__)
/**
 * @brief 多核系统 **获取** 屏障
 * @details
 * 通常用于 **加载** 操作之后。
 */
#  define xwmb_mp_acquire()     xwmb_mp_mb()
#endif

#if !defined(xwmb_mp_release) || defined(__DOXYGEN__)
/**
 * @brief 多核系统 **释放** 屏障
 * @details
 * 通常用于 **存储** 操作之前。
 */
#  define xwmb_mp_release()     xwmb_mp_mb()
#endif

#if !defined(xwmb_mp_load_acquire) || defined(__DOXYGEN__)
/**
 * @brief: 读取地址中的指定类型的值，并保证此处的“读”操作一定发生在之后的“读写”操作之前
 * @param[in] t: 类型，只能为基本类型及其typedef后的类型
 * @param[out] v: 类型为t左值，读取的值放在这个左值中
 * @param[in] p: 类型为t *的指针，指向需要读取的地址
 * @note
 * + 类型t的位宽只能是8位、16位、32位、64位。
 */
#define xwmb_mp_load_acquire(t, v, p)           \
        do {                                    \
                xwmb_read(t, (v), (p));         \
                xwmb_mp_acquire();              \
        } while (0)
#endif

#if !defined(xwmb_mp_store_release) || defined(__DOXYGEN__)
/**
 * @brief 存储指定类型的值到地址中，并保证此处的“写”操作一定发生在之前的“读写”操作之后
 * @param[in] t: 类型，只能为基本类型及其typedef后的类型
 * @param[in] p: 类型为t *的指针，指向需要存储的地址
 * @param[in] v: 待存储的类型为t右值
 * @note
 * + 类型t的位宽只能是8位、16位、32位、64位。
 */
#define xwmb_mp_store_release(t, p, v)          \
        do {                                    \
                xwmb_mp_release();              \
                xwmb_write(t, (p), (v));        \
        } while (0)
#endif

/**
 * @} xwos_lib_mb
 */

#endif /* xwos/lib/mb.h */
