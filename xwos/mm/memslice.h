/**
 * @file
 * @brief 玄武OS内存管理：内存切片分配器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * @note
 * - 此算法是在所有上下文（线程、中断、中断底半部）都是安全的。
 */

#ifndef __xwos_mm_memslice_h__
#define __xwos_mm_memslice_h__

#include <xwos/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/lfq.h>
#include <xwos/mm/common.h>

/**
 * @brief 内存切片分配器
 */
struct xwmm_memslice {
        struct xwmm_zone zone; /**< 内存区域 */
        const char * name; /**< 名字 */
        xwsz_t card_size; /**< “卡片”大小 */
        xwsz_a num_free; /**< 当前分配器中还剩余多少张“卡片” */
        xwsz_a num_max; /**< 当前分配器中总共有多少张“卡片” */
        xwlfq_a free_list; /**< 空闲的内存切片链表 */
        xwptr_t backup; /**< 备份值：
                             - 每个对象的第一段内存字(word)用作无锁队列的
                               链表指针，当分配卡片时，用备份值恢复；
                             - 所有对象初始化后的第一个字(word)必须相同，
                               以保证备份值唯一。
                             */
        ctor_f ctor; /**< 构造函数 */
        dtor_f dtor; /**< 析构函数 */
};

xwer_t xwmm_memslice_init(struct xwmm_memslice * msa,
                          xwptr_t origin, xwsz_t total_size,
                          xwsz_t card_size, const char * name,
                          ctor_f ctor, dtor_f dtor);
xwer_t xwmm_memslice_alloc(struct xwmm_memslice * msa, void ** membuf);
xwer_t xwmm_memslice_free(struct xwmm_memslice * msa, void * mem);

#endif /* xwos/mm/memslice.h */
