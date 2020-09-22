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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwlog.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern xwer_t soc_log_write(const char * s, xwsz_t * n);
extern xwer_t board_log_write(const char * s, xwsz_t * n);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 格式化字符串并输出到日志，同printf()函数
 * @param fmt: (I) 格式的字符串
 * @param ...: (I) 需要转换位字符串的其他参数
 * @return 同vsnprintf()函数
 * @note
 * - 同步/异步：同@ref soc_log_write()的实现
 * - 上下文：线程
 */
__xwlib_code
int xwpf(const char * fmt, ...)
{
        va_list args;
        xwer_t rc;
        int i;
        xwsz_t sz;
        char buf[XWLIBCFG_XWLOG_BUFSIZE];

        va_start(args, fmt);
        i = vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);
        if (i < 0) {
                goto err_vsnprintf;
        }
        sz = (xwsz_t)i;

        /* Print the string */
#if defined(SOCCFG_LOG) && (1 == SOCCFG_LOG)
        rc = soc_log_write(buf, &sz);
        if (rc < 0) {
                i = (int)rc;
        } else {
                i = (int)sz;
        }
#elif defined(BRDCFG_LOG) && (1 == BRDCFG_LOG)
        rc = board_log_write(buf, &sz);
        if (rc < 0) {
                i = (int)rc;
        } else {
                i = (int)sz;
        }
#else
  #warning "Can't find the log configurations!"
        i = 0;
#endif

err_vsnprintf:
        return i;
}
