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
/******** ******** Thread Context Log ******** ********/
#if (XWLIBCFG_XWLOG_LEVEL <= 1)
  #define XWLOGF_DEBUG(fmt, ...)        \
          xwpf("[DEBUG]<%s> " fmt, __func__, ##__VA_ARGS__)
#else
  #define XWLOGF_DEBUG(fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 2)
  #define XWLOGF_INFO(fmt, ...)         \
          xwpf("[INFO]<%s> " fmt, __func__, ##__VA_ARGS__)
#else
  #define XWLOGF_INFO(fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 3)
  #define XWLOGF_NOTICE(fmt, ...)       \
          xwpf("[NOTICE]<%s> " fmt, __func__, ##__VA_ARGS__)
#else
  #define XWLOGF_NOTICE(fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 4)
  #define XWLOGF_WARNING(fmt, ...)      \
          xwpf("[WARNING]<%s> " fmt, __func__, ##__VA_ARGS__)
#else
  #define XWLOGF_WARNING(fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 5)
  #define XWLOGF_ERR(fmt, ...)          \
          xwpf("[ERR]<%s> " fmt, __func__, ##__VA_ARGS__)
#else
  #define XWLOGF_ERR(fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 6)
  #define XWLOGF_CRIT(fmt, ...)         \
          xwpf("[CRIT]<%s> " fmt, __func__, ##__VA_ARGS__)
#else
  #define XWLOGF_CRIT(fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 7)
  #define XWLOGF_ALERT(fmt, ...)        \
          xwpf("[ALERT]<%s> " fmt, __func__, ##__VA_ARGS__)
#else
  #define XWLOGF_ALERT(fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 8)
  #define XWLOGF_EMERG(fmt, ...)        \
          xwpf("[EMERG]<%s> " fmt, __func__, ##__VA_ARGS__)
#else
  #define XWLOGF_EMERG(fmt, ...)
#endif

#define xwlogf(lv, fmt, ...)    XWLOGF_##lv(fmt, ##__VA_ARGS__)

/******** ******** Interrupt Context Log ******** ********/
#if (XWLIBCFG_XWLOG_LEVEL <= 1)
  #define XWISRLOGF_DEBUG(fmt, ...)     \
          xwisrpf("[DEBUG]<%s> " fmt, __func__, ##__VA_ARGS__)
#else
  #define XWISRLOGF_DEBUG(fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 2)
  #define XWISRLOGF_INFO(fmt, ...)      \
          xwisrpf("[INFO]<%s> " fmt, __func__, ##__VA_ARGS__)
#else
  #define XWISRLOGF_INFO(fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 3)
  #define XWISRLOGF_NOTICE(fmt, ...)    \
          xwisrpf("[NOTICE]<%s> " fmt, __func__, ##__VA_ARGS__)
#else
  #define XWISRLOGF_NOTICE(fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 4)
  #define XWISRLOGF_WARNING(fmt, ...)   \
          xwisrpf("[WARNING]<%s> " fmt, __func__, ##__VA_ARGS__)
#else
  #define XWISRLOGF_WARNING(fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 5)
  #define XWISRLOGF_ERR(fmt, ...)       \
          xwisrpf("[ERR]<%s> " fmt, __func__, ##__VA_ARGS__)
#else
  #define XWISRLOGF_ERR(fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 6)
  #define XWISRLOGF_CRIT(fmt, ...)      \
          xwisrpf("[CRIT]<%s> " fmt, __func__, ##__VA_ARGS__)
#else
  #define XWISRLOGF_CRIT(fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 7)
  #define XWISRLOGF_ALERT(fmt, ...)     \
          xwisrpf("[ALERT]<%s> " fmt, __func__, ##__VA_ARGS__)
#else
  #define XWISRLOGF_ALERT(fmt, ...)
#endif

#if (XWLIBCFG_XWLOG_LEVEL <= 8)
  #define XWISRLOGF_EMERG(fmt, ...)     \
          xwisrpf("[EMERG]<%s> " fmt, __func__, ##__VA_ARGS__)
#else
  #define XWISRLOGF_EMERG(fmt, ...)
#endif

#define xwisrlogf(lv, fmt, ...)  XWISRLOGF_##lv(fmt, ##__VA_ARGS__)

#endif /* xwos/lib/xwlog.h */
