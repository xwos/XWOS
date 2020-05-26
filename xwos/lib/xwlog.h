/**
 * @file
 * @brief XWOS通用库：日志
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_lib_xwlog_h__
#define __xwos_lib_xwlog_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********               macros                ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if defined(XWLIBCFG_LOG) && (1 == XWLIBCFG_LOG)
__xwlib_code
int xwpf(const char * fmt, ...);

__xwlib_code
int xwisrpf(const char * fmt, ...);

#else /* XWLIBCFG_LOG */
static __xwlib_inline
int xwpf(const char * fmt, ...)
{
        XWOS_UNUSED(fmt);
        return 0;
}

static __xwlib_inline
int xwisrpf(const char * fmt, ...)
{
        XWOS_UNUSED(fmt);
        return 0;
}
#endif /* !XWLIBCFG_LOG */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********           macro functions           ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if (XWLIBCFG_XWLOG_LEVEL <= 1)
  #define XWLOGF_VERBOSE(tag, fmt, ...)                    \
          xwpf("[V.%s] " fmt, tag, ##__VA_ARGS__)
#else
  #define XWLOGF_VERBOSE(tag, fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 2)
  #define XWLOGF_DEBUG(tag, fmt, ...)        \
          xwpf("[D.%s] " fmt, tag, ##__VA_ARGS__)
#else
  #define XWLOGF_DEBUG(tag, fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 3)
  #define XWLOGF_INFO(tag, fmt, ...)         \
          xwpf("[I.%s] " fmt, tag, ##__VA_ARGS__)
#else
  #define XWLOGF_INFO(tag, fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 4)
  #define XWLOGF_NOTICE(tag, fmt, ...)       \
          xwpf("[N.%s] " fmt, tag, ##__VA_ARGS__)
#else
  #define XWLOGF_NOTICE(tag, fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 5)
  #define XWLOGF_WARNING(tag, fmt, ...)      \
          xwpf("[W.%s] " fmt, tag, ##__VA_ARGS__)
#else
  #define XWLOGF_WARNING(tag, fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 6)
  #define XWLOGF_ERR(tag, fmt, ...)          \
          xwpf("[E.%s] " fmt, tag, ##__VA_ARGS__)
#else
  #define XWLOGF_ERR(tag, fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 7)
  #define XWLOGF_CRIT(tag, fmt, ...)         \
          xwpf("[C.%s] " fmt, tag, ##__VA_ARGS__)
#else
  #define XWLOGF_CRIT(tag, fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 8)
  #define XWLOGF_EMERG(tag, fmt, ...)        \
          xwpf("[G.%s] " fmt, tag, ##__VA_ARGS__)
#else
  #define XWLOGF_EMERG(tag, fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 1)
  #define XWISRLOGF_VERBOSE(tag, fmt, ...)        \
          xwisrpf("[V.%s] " fmt, tag, ##__VA_ARGS__)
#else
  #define XWISRLOGF_VERBOSE(tag, fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 2)
  #define XWISRLOGF_DEBUG(tag, fmt, ...)     \
          xwisrpf("[D.%s] " fmt, tag, ##__VA_ARGS__)
#else
  #define XWISRLOGF_DEBUG(tag, fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 3)
  #define XWISRLOGF_INFO(tag, fmt, ...)      \
          xwisrpf("[I.%s] " fmt, tag, ##__VA_ARGS__)
#else
  #define XWISRLOGF_INFO(tag, fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 4)
  #define XWISRLOGF_NOTICE(tag, fmt, ...)    \
          xwisrpf("[N.%s] " fmt, tag, ##__VA_ARGS__)
#else
  #define XWISRLOGF_NOTICE(tag, fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 5)
  #define XWISRLOGF_WARNING(tag, fmt, ...)   \
          xwisrpf("[W.%s] " fmt, tag, ##__VA_ARGS__)
#else
  #define XWISRLOGF_WARNING(tag, fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 6)
  #define XWISRLOGF_ERR(tag, fmt, ...)       \
          xwisrpf("[E.%s] " fmt, tag, ##__VA_ARGS__)
#else
  #define XWISRLOGF_ERR(tag, fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 7)
  #define XWISRLOGF_CRIT(tag, fmt, ...)      \
          xwisrpf("[C.%s] " fmt, tag, ##__VA_ARGS__)
#else
  #define XWISRLOGF_CRIT(tag, fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 8)
  #define XWISRLOGF_EMERG(tag, fmt, ...)     \
          xwisrpf("[G.%s] " fmt, tag, ##__VA_ARGS__)
#else
  #define XWISRLOGF_EMERG(tag, fmt, ...)
#endif

/**
 * @brief 格式化日志，并输出（线程安全）
 * @param lv: (I) 等级，取值
 *                - VERBOSE，等级1
 *                - DEBUG，等级2
 *                - INFO，等级3
 *                - NOTICE，等级4
 *                - WARNING，等级5
 *                - ERR，等级6
 *                - CRIT，等级7
 *                - EMERG，等级8
 * @param tag: (I) 日至标签
 * @param fmt: (I) 格式的字符串
 * @param ...: (I) 需要格式化的参数
 * @note
 * - 同步/异步：同soc_log_isr_write()或board_log_isr_write()的实现
 * - 上下文：线程
 * - 重入性：依据BSP中soc_log_write()或board_log_write()的实现是否具有可重入性
 */
#define xwlogf(lv, tag, fmt, ...)    XWLOGF_##lv(tag, fmt, ##__VA_ARGS__)

/**
 * @brief 格式化日志，并输出（中断安全）
 * @param lv: (I) 等级，取值
 *                - VERBOSE，等级1
 *                - DEBUG，等级2
 *                - INFO，等级3
 *                - NOTICE，等级4
 *                - WARNING，等级5
 *                - ERR，等级6
 *                - CRIT，等级7
 *                - ALERT，等级8
 *                - EMERG，等级9
 * @param tag: (I) 日至标签
 * @param fmt: (I) 格式的字符串
 * @param ...: (I) 需要格式化的参数
 * @note
 * - 同步/异步：同soc_log_isr_write()或board_log_isr_write()的实现
 * - 上下文：线程、中断、中断底半部
 * - 重入性：同soc_log_isr_write()或board_log_isr_write()的实现
 */
#define xwisrlogf(lv, tag, fmt, ...)  XWISRLOGF_##lv(tag, fmt, ##__VA_ARGS__)

#endif /* xwos/lib/xwlog.h */
