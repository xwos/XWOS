/**
 * @file
 * @brief XWOS内存管理：内存切片分配器
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_mm_memslice_h__
#define __xwos_mm_memslice_h__

#include <xwos/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/lfq.h>
#include <xwos/mm/common.h>

/**
 * @defgroup xwmm_memslice 内存切片分配器
 * @ingroup xwmm
 * 内存切片分配器将内存切割成等量的块，就像 **卡片** 一样，然后将它们组成链表队列，
 * 每次分配时取一块内存，回收时再把内存切片放回到队列中。
 *
 * + 优点：
 *   + 简单，代码量较小；
 *   + 支持释放操作；
 *   + 申请与释放操作的运行时间是稳定的；
 *   + 反复申请与释放不会造成内存碎片。
 * + 缺点：
 *   + 内存块大小固定，如果要求的内存过小，会造成内存浪费；
 *     如果要求的内存超过内存块的大小，无法满足需求；
 *   + 连续申请的两块内存不能保证是连续的。
 * + 适用性：对时间稳定性要求高的RTOS应用场合。
 * + 上下文的安全性：在任何上下文（中断、中断底半部、线程）都是安全的。
 * @{
 */

/**
 * @brief 内存切片分配器
 */
struct xwmm_memslice {
        struct xwmm_zone zone; /**< 内存区域 */
        const char * name; /**< 名字 */
        xwsz_t card_size; /**< “卡片”大小 */
        atomic_xwsz_t num_free; /**< 当前分配器中还剩余多少张“卡片” */
        atomic_xwsz_t num_max; /**< 当前分配器中总共有多少张“卡片” */
        atomic_xwlfq_t free_list; /**< 空闲的内存切片链表 */
        xwptr_t backup; /**< 备份值：
                             - 每个对象的第一段内存字(word)用作无锁队列的
                               链表指针，当分配卡片时，用备份值恢复；
                             - 所有对象初始化后的第一个字(word)必须相同，
                               以保证备份值唯一。
                             */
        ctor_f ctor; /**< 构造函数 */
        dtor_f dtor; /**< 析构函数 */
};

/**
 * @brief XWMM API：静态方式初始化内存切片分配器
 * @param[in] msa: 内存切片分配器对象的指针
 * @param[in] origin: 建立内存切片分配算法的内存区域首地址
 * @param[in] total_size: 建立内存切片分配算法的内存区域大小
 * @param[in] card_size: 切片大小
 * @param[in] name: 名字
 * @param[in] ctor: 切片的构造函数
 * @param[in] dtor: 切片的析构函数
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -E2SMALL: 内存区域太小
 * @note
 * + 同步/异步：同步
 * + 上下文：中断、中断底半部、线程
 * + 重入性：不可重入
 */
xwer_t xwmm_memslice_init(struct xwmm_memslice * msa,
                          xwptr_t origin, xwsz_t total_size,
                          xwsz_t card_size, const char * name,
                          ctor_f ctor, dtor_f dtor);

/**
 * @brief XWMM API：申请内存切片
 * @param[in] msa: 内存切片分配器对象的指针
 * @param[out] membuf: 指向地址缓存的指针，通过此指针缓存返回申请到的内存的首地址
 * @return 错误码
 * @retval -EFAULT: 空指针
 * @retval -ENOMEM: 内存不足
 * @note
 * + 同步/异步：同步
 * + 上下文：中断、中断底半部、线程
 * + 重入性：可重入
 */
xwer_t xwmm_memslice_alloc(struct xwmm_memslice * msa, void ** membuf);

/**
 * @brief XWMM API：释放内存切片
 * @param[in] msa: 内存切片分配器对象的指针
 * @param[in] mem: 内存切片的首地址
 * @return 错误码
 * @note
 * + 同步/异步：同步
 * + 上下文：中断、中断底半部、线程
 * + 重入性：可重入
 */
xwer_t xwmm_memslice_free(struct xwmm_memslice * msa, void * mem);

/**
 * @} xwmm_memslice
 */

#endif /* xwos/mm/memslice.h */
