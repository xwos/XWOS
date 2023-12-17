/**
 * @file
 * @brief SOC描述层：调试
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
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

#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_debug.h>
#include <xwos/ospl/skd.h>
#include <stdio.h> // cppcheck-suppress [misra-c2012-21.6]
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_console.h>

/**
 * @brief 格式化字符串并输出日志到SOC的终端
 * @param[in] fmt: 格式的字符串
 * @param[in] ...: 需要转换位字符串的其他参数
 * @return 同 `vsnprintf()` 函数
 * @note
 * + 同步/异步：同步
 * + 上下文：任意
 * + 重入性：可重入，但日志可能会混乱
 */
__xwbsp_code
void soc_logf(const char * fmt, ...)
{
        va_list args;
        char buf[SOCCFG_DBGLOGBUF_SIZE];
        int i;

        va_start(args, fmt);
        i = vsnprintf(buf, sizeof(buf), fmt, args);
        va_end(args);
        if (i > 0) {
               soc_console_puts(buf);
        }
}

__xwbsp_code
void soc_panic(const char * fmt, ...)
{
        va_list args;
        char msg[SOCCFG_DBGLOGBUF_SIZE];
        xwid_t cpuid;

        va_start(args, fmt);
        vsnprintf(msg, sizeof(msg), fmt, args);
        va_end(args);
        cpuid = xwospl_skd_get_cpuid_lc();
        soc_logf("[panic CPU%d @0x%016lX] %s",
                 cpuid, (xwu64_t)__xwcc_caller(0), msg);
        while (true) {
                __asm__ volatile("wfi");
        }
}
