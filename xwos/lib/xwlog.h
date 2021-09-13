/**
 * @file
 * @brief XWOS通用库：日志
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_lib_xwlog_h__
#define __xwos_lib_xwlog_h__

#include <xwos/standard.h>

/**
 * @defgroup xwlog 日志
 * @{
 */

#if defined(XWLIBCFG_LOG) && (1 == XWLIBCFG_LOG)
__xwlib_code
int xwpf(const char * fmt, ...);

#else
static __xwlib_inline
int xwpf(const char * fmt, ...)
{
        XWOS_UNUSED(fmt);
        return 0;
}
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 1)
#  define XWLOGF_VERBOSE(tag, fmt, ...) xwpf("[V.%s] " fmt, tag, ##__VA_ARGS__)
#else
#  define XWLOGF_VERBOSE(tag, fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 2)
#  define XWLOGF_DEBUG(tag, fmt, ...) xwpf("[D.%s] " fmt, tag, ##__VA_ARGS__)
#else
#  define XWLOGF_DEBUG(tag, fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 3)
#  define XWLOGF_INFO(tag, fmt, ...) xwpf("[I.%s] " fmt, tag, ##__VA_ARGS__)
#else
#  define XWLOGF_INFO(tag, fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 4)
#  define XWLOGF_NOTICE(tag, fmt, ...) xwpf("[N.%s] " fmt, tag, ##__VA_ARGS__)
#else
#  define XWLOGF_NOTICE(tag, fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 5)
#  define XWLOGF_WARNING(tag, fmt, ...) xwpf("[W.%s] " fmt, tag, ##__VA_ARGS__)
#else
#  define XWLOGF_WARNING(tag, fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 6)
#  define XWLOGF_ERR(tag, fmt, ...) xwpf("[E.%s] " fmt, tag, ##__VA_ARGS__)
#else
#  define XWLOGF_ERR(tag, fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 7)
#  define XWLOGF_CRIT(tag, fmt, ...) xwpf("[C.%s] " fmt, tag, ##__VA_ARGS__)
#else
#  define XWLOGF_CRIT(tag, fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 8)
#  define XWLOGF_EMERG(tag, fmt, ...) xwpf("[G.%s] " fmt, tag, ##__VA_ARGS__)
#else
#  define XWLOGF_EMERG(tag, fmt, ...)
#endif

/**
 * @brief 格式化日志，并输出
 * @param[in] lv: 等级，取值
 *   @arg VERBOSE，等级1
 *   @arg DEBUG，等级2
 *   @arg INFO，等级3
 *   @arg NOTICE，等级4
 *   @arg WARNING，等级5
 *   @arg ERR，等级6
 *   @arg CRIT，等级7
 *   @arg EMERG，等级8
 * @param[in] tag: 日至标签
 * @param[in] fmt: 格式的字符串
 * @param[in] ...: 需要格式化的参数
 * @note
 * - 同步/异步：依据BSP中soc_log_write()或board_log_write()的实现
 * - 上下文：依据BSP中soc_log_write()或board_log_write()的实现
 * - 重入性：依据BSP中soc_log_write()或board_log_write()的实现
 */
#define xwlogf(lv, tag, fmt, ...) XWLOGF_##lv(tag, fmt, ##__VA_ARGS__)

/**
 * @} xwlogf
 */

#endif /* xwos/lib/xwlog.h */
