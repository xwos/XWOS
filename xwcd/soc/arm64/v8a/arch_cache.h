/**
 * @file
 * @brief 架构描述层：ARMv8A CACHE
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

#ifndef __xwcd_soc_arm_v8a_arch_cache_h__
#define __xwcd_soc_arm_v8a_arch_cache_h__

#include <xwos/standard.h>

/**
 * @brief 开启ICache
 */
void armv8a_icache_enable(void);

/**
 * @brief 关闭ICache
 */
void armv8a_icache_disable(void);

/**
 * @brief 完全无效ICache
 */
void armv8a_invalidate_icache_all(void);

/**
 * @brief 开启DCache
 */
void armv8a_dcache_enable(void);

/**
 * @brief 关闭DCache
 */
void armv8a_dcache_disable(void);

/**
 * @brief 完全冲洗DCache
 */
void armv8a_flush_dcache_all(void);

/**
 * @brief 冲洗DCache
 * @param[in] origin: 清理的起始地址
 * @param[in] size: 清理的大小
 */
void armv8a_flush_dcache(xwptr_t origin, xwsz_t size);

/**
 * @brief 清理DCache到一致点
 * @param[in] origin: 清理的起始地址
 * @param[in] size: 清理的大小
 * @details
 * + **一致点** 是指所有CPU、DMA、GPU等总线的主设备都能看到的点。在ARM64架构文档中被称为Outer Shareable。
 */
void armv8a_clean_dcache_poc(xwptr_t origin, xwsz_t size);

/**
 * @brief 清理DCache到统一点
 * @param[in] origin: 清理的起始地址
 * @param[in] size: 清理的大小
 * @details
 * + **统一点** 是指所有CPU都能看到的点。在ARM64架构文档中被称为Inner Shareable。
 */
void armv8a_clean_dcache_pou(xwptr_t origin, xwsz_t size);

#if defined(ARCHCFG_FEAT_DPB) && (1 == ARCHCFG_FEAT_DPB)
/**
 * @brief 清理DCache到持久化点
 * @param[in] origin: 清理的起始地址
 * @param[in] size: 清理的大小
 * @details
 * + **持久化点** 是指数据断电后依然存在的点。
 */
void armv8a_clean_dcache_pop(xwptr_t origin, xwsz_t size);
#endif

/**
 * @brief 无效DCache
 * @param[in] origin: 清理的起始地址
 * @param[in] size: 清理的大小
 */
void armv8a_invalidate_dcache(xwptr_t origin, xwsz_t size);

#endif /* xwcd/soc/arm64/v8a/arch_cache.h */
