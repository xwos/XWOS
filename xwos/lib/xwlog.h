/**
 * @file
 * @brief XWOS通用库：日志
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_lib_xwlog_h__
#define __xwos_lib_xwlog_h__

#include <xwos/standard.h>

/**
 * @defgroup xwos_lib_xwlog 日志
 * @ingroup xwos_lib
 * @{
 */

int xwpf(const char * fmt, ...);

#if defined(XWLIBCFG_LOG) && (1 == XWLIBCFG_LOG) && (XWLIBCFG_XWLOG_LEVEL <= 0)
#  define XWLOGF_VERBOSE(tag, fmt, ...) xwpf("V|" tag " " fmt, ##__VA_ARGS__)
#  define XWLOGF_V(tag, fmt, ...) xwpf("V|" tag " " fmt, ##__VA_ARGS__)
#else
#  define XWLOGF_VERBOSE(tag, fmt, ...)
#  define XWLOGF_V(tag, fmt, ...)
#endif

#if defined(XWLIBCFG_LOG) && (1 == XWLIBCFG_LOG) && (XWLIBCFG_XWLOG_LEVEL <= 1)
#  define XWLOGF_DEBUG(tag, fmt, ...) xwpf("D|" tag " " fmt, ##__VA_ARGS__)
#  define XWLOGF_D(tag, fmt, ...) xwpf("D|" tag " " fmt, ##__VA_ARGS__)
#else
#  define XWLOGF_DEBUG(tag, fmt, ...)
#  define XWLOGF_D(tag, fmt, ...)
#endif

#if defined(XWLIBCFG_LOG) && (1 == XWLIBCFG_LOG) && (XWLIBCFG_XWLOG_LEVEL <= 2)
#  define XWLOGF_INFO(tag, fmt, ...) xwpf("I|" tag " " fmt, ##__VA_ARGS__)
#  define XWLOGF_I(tag, fmt, ...) xwpf("I|" tag " " fmt, ##__VA_ARGS__)
#else
#  define XWLOGF_INFO(tag, fmt, ...)
#  define XWLOGF_I(tag, fmt, ...)
#endif

#if defined(XWLIBCFG_LOG) && (1 == XWLIBCFG_LOG) && (XWLIBCFG_XWLOG_LEVEL <= 3)
#  define XWLOGF_NOTICE(tag, fmt, ...) xwpf("N|" tag " " fmt, ##__VA_ARGS__)
#  define XWLOGF_N(tag, fmt, ...) xwpf("N|" tag " " fmt, ##__VA_ARGS__)
#else
#  define XWLOGF_NOTICE(tag, fmt, ...)
#  define XWLOGF_N(tag, fmt, ...)
#endif

#if defined(XWLIBCFG_LOG) && (1 == XWLIBCFG_LOG) && (XWLIBCFG_XWLOG_LEVEL <= 4)
#  define XWLOGF_WARNING(tag, fmt, ...) xwpf("W|" tag " " fmt, ##__VA_ARGS__)
#  define XWLOGF_W(tag, fmt, ...) xwpf("W|" tag " " fmt, ##__VA_ARGS__)
#else
#  define XWLOGF_WARNING(tag, fmt, ...)
#  define XWLOGF_W(tag, fmt, ...)
#endif

#if defined(XWLIBCFG_LOG) && (1 == XWLIBCFG_LOG) && (XWLIBCFG_XWLOG_LEVEL <= 5)
#  define XWLOGF_ERR(tag, fmt, ...) xwpf("E|" tag " " fmt, ##__VA_ARGS__)
#  define XWLOGF_E(tag, fmt, ...) xwpf("E|" tag " " fmt, ##__VA_ARGS__)
#else
#  define XWLOGF_ERR(tag, fmt, ...)
#  define XWLOGF_E(tag, fmt, ...)
#endif

#if defined(XWLIBCFG_LOG) && (1 == XWLIBCFG_LOG) && (XWLIBCFG_XWLOG_LEVEL <= 6)
#  define XWLOGF_CRIT(tag, fmt, ...) xwpf("C|" tag " " fmt, ##__VA_ARGS__)
#  define XWLOGF_C(tag, fmt, ...) xwpf("C|" tag " " fmt, ##__VA_ARGS__)
#else
#  define XWLOGF_CRIT(tag, fmt, ...)
#  define XWLOGF_C(tag, fmt, ...)
#endif

#if defined(XWLIBCFG_LOG) && (1 == XWLIBCFG_LOG) && (XWLIBCFG_XWLOG_LEVEL <= 7)
#  define XWLOGF_ALERT(tag, fmt, ...) xwpf("A|" tag " " fmt, ##__VA_ARGS__)
#  define XWLOGF_A(tag, fmt, ...) xwpf("A|" tag " " fmt, ##__VA_ARGS__)
#else
#  define XWLOGF_ALERT(tag, fmt, ...)
#  define XWLOGF_A(tag, fmt, ...)
#endif

#if defined(XWLIBCFG_LOG) && (1 == XWLIBCFG_LOG) && (XWLIBCFG_XWLOG_LEVEL <= 8)
#  define XWLOGF_EMERG(tag, fmt, ...) xwpf("G|" tag " " fmt, ##__VA_ARGS__)
#  define XWLOGF_G(tag, fmt, ...) xwpf("G|" tag " " fmt, ##__VA_ARGS__)
#else
#  define XWLOGF_EMERG(tag, fmt, ...)
#  define XWLOGF_G(tag, fmt, ...)
#endif

/**
 * @brief 格式化日志，并输出
 * @param[in] lv: 等级，取值
 *   @arg VERBOSE | V，等级0
 *   @arg DEBUG | D，等级1
 *   @arg INFO | I，等级2
 *   @arg NOTICE | N，等级3
 *   @arg WARNING | W，等级4
 *   @arg ERR | E，等级5
 *   @arg CRIT | C，等级6
 *   @arg ALERT | A，等级7
 *   @arg EMERG | G，等级8
 * @param[in] tag: 日志标签
 * @param[in] fmt: 格式的字符串
 * @param[in] ...: 需要格式化的参数
 * @note
 * + 上下文：依赖 `soc_log_write()` 或 `board_log_write()` 实现
 * @details
 * 如果底层 `soc_log_write()` 或 `board_log_write()` 的实现会导致线程睡眠，
 * 不可在除线程外的其他上下文使用。
 */
#define xwlogf(lv, tag, fmt, ...) XWLOGF_##lv(tag, fmt, ##__VA_ARGS__)

/**
 * @} xwos_lib_xwlog
 */

#endif /* xwos/lib/xwlog.h */
