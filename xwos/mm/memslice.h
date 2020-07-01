/**
 * @file
 * @brief XuanWuOS的内存管理机制：内存切片分配器
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/object.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/lfq.h>
#include <xwos/mm/common.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 内存切片分配器
 */
struct xwmm_memslice {
        struct xwmm_zone zone; /**< 内存区域 */
        const char * name; /**< 名字 */
        xwsz_t card_size; /**< “卡片”大小 */
        __atomic xwsz_t num_free; /**< 当前分配器中还剩余多少张“卡片” */
        __atomic xwsz_t num_max; /**< 当前分配器中总共有多少张“卡片” */
        __atomic xwlfq_t free_list; /**< 空闲的内存切片链表 */
        xwptr_t backup; /**< 备份值：
                             - 每个对象的第一段内存字(word)用作无锁队列的
                               链表指针，当分配卡片时，用备份值恢复；
                             - 所有对象初始化后的第一个字(word)必须相同，
                               以保证备份值唯一。
                             */
        ctor_f ctor; /**< 构造函数 */
        dtor_f dtor; /**< 析构函数 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     internal function prototypes    ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ********       internal inline function implementations        ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********       API function prototypes       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWMM API：静态方式初始化内存切片分配器
 * @param msa: (I) 内存切片分配器对象的指针
 * @param origin: (I) 建立内存切片分配算法的内存区域首地址
 * @param total_size: (I) 建立内存切片分配算法的内存区域大小
 * @param card_size: (I) 切片大小
 * @param name: (I) 名字
 * @param ctor: (I) 切片的构造函数
 * @param dtor: (I) 切片的析构函数
 * @return 错误码
 * @retval OK: OK
 * @retval -E2SMALL: 内存区域太小
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwmm_memslice_init(struct xwmm_memslice * msa,
                          xwptr_t origin, xwsz_t total_size,
                          xwsz_t card_size, const char * name,
                          ctor_f ctor, dtor_f dtor);

/**
 * @brief XWMM API：销毁静态方式初始化的内存切片分配器
 * @param msa: (I) 内存切片分配器对象的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwmm_memslice_destroy(struct xwmm_memslice * msa);

/**
 * @brief XWMM API：动态方式创建内存切片分配器
 * @param ptrbuf: (O) 用于返回内存切片分配器对象指针的缓存
 * @param origin: (I) 建立内存切片分配算法的内存区域首地址
 * @param total_size: (I) 建立内存切片分配算法的内存区域大小
 * @param card_size: (I) 切片大小
 * @param ctor: (I) 切片的构造函数
 * @param dtor: (I) 切片的析构函数
 * @param name: (I) 名字
 * @return 错误码
 * @retval OK: OK
 * @retval -E2SMALL: 内存区域太小
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwmm_memslice_create(struct xwmm_memslice ** ptrbuf,
                            xwptr_t origin, xwsz_t total_size, xwsz_t card_size,
                            const char * name, ctor_f ctor, dtor_f dtor);

/**
 * @brief XWMM API：删除动态方式创建的内存切片分配器
 * @param msa: (I) 内存切片分配器对象的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwmm_memslice_delete(struct xwmm_memslice * msa);

/**
 * @brief XWMM API：申请一片内存切片
 * @param msa: (I) 内存切片分配器对象的指针
 * @param membuf: (O) 指向地址缓存的指针，通过此指针缓存返回申请到的内存的首地址
 * @return 错误码
 * @retval -EFAULT: 空指针
 * @retval -ENOMEM: 内存不足
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwmm_memslice_alloc(struct xwmm_memslice * msa, void ** membuf);

/**
 * @brief XWMM API：释放一片内存切片
 * @param msa: (I) 内存切片分配器对象的指针
 * @param mem: (I) 内存切片的首地址
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwmm_memslice_free(struct xwmm_memslice * msa, void * mem);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      inline API implementations     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwos/mm/memslice.h */
