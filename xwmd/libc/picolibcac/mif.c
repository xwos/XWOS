/**
 * @file
 * @brief picolibc适配层：模块接口
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwmd/libc/picolibcac/linkage.h>
#include <xwmd/libc/picolibcac/check.h>
#include <stdlib.h>
#include <xwmd/libc/picolibcac/mif.h>

typedef void (* picolibcac_linkage_f)(void);
typedef void (* picolibcac_init_f)(void);

extern void __libc_init_array(void);
extern void __libc_fini_array(void);

/**
 * @brief 链接选择表
 * @details
 * 用于在符号重定义时，选择链接的符号。
 */
// cppcheck-suppress [misra-c2012-8.4]
const picolibcac_linkage_f picolibcac_linkage_table[] = {
        picolibcac_errno_linkage_stub,
        picolibcac_lock_linkage_stub,
        picolibcac_sysconf_linkage_stub,
        picolibcac_time_linkage_stub,
        picolibcac_string_linkage_stub,
#if defined(XWMDCFG_libc_picolibcac_SPRINTF) && (1 == XWMDCFG_libc_picolibcac_SPRINTF)
        picolibcac_sprintf_linkage_stub,
#endif
#if defined(XWMDCFG_libc_picolibcac_MEM) && (1 == XWMDCFG_libc_picolibcac_MEM)
        picolibcac_mem_linkage_stub,
#endif
#if defined(XWMDCFG_libc_picolibcac_SETJMP) && (1 == XWMDCFG_libc_picolibcac_SETJMP)
        picolibcac_setjmp_linkage_stub,
#endif
#if defined(XWMDCFG_libc_picolibcac_FOPS) && (1 == XWMDCFG_libc_picolibcac_FOPS)
        picolibcac_fops_linkage_stub,
#endif
        picolibcac_isatty_linkage_stub,
        picolibcac_kill_linkage_stub,
        picolibcac_getpid_linkage_stub,
        picolibcac_exit_linkage_stub,
        NULL,
};

extern void picolibcac_lock_init(void);

/**
 * @brief 初始化列表
 */
// cppcheck-suppress [misra-c2012-8.4]
const picolibcac_init_f picolibcac_init_table[] = {
        picolibcac_lock_init,
        NULL,
};

xwer_t picolibcac_init(void)
{
        const picolibcac_init_f * f = picolibcac_init_table;
        xwsz_t i;

        i = 0;
        while (NULL != f[i]) {
                f[i]();
                i++;
        }
        __libc_init_array();
        /* atexit(__libc_fini_array); */ // cppcheck-suppress [misra-c2012-17.7]
        return XWOK;
}
