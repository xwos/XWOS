/**
 * @file
 * @brief newlib适配层：模块接口
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwmd/libc/newlibac/linkage.h>
#include <xwmd/libc/newlibac/check.h>
#include <stdlib.h>
#include <xwmd/libc/newlibac/mif.h>

typedef void (* newlibac_linkage_f)(void);
typedef void (* newlibac_init_f)(void);

extern void __libc_init_array (void);
extern void __libc_fini_array (void);

/**
 * @brief 静态链接表
 * @note
 * + 此函数表作为静态链接占位符，可保证符号重名时优先使用此库中的符号。
 */
// cppcheck-suppress [misra-c2012-8.4]
const newlibac_linkage_f newlibac_linkage_table[] = {
        newlibac_errno_linkage_stub,
        newlibac_lock_linkage_stub,
        newlibac_sysconf_linkage_stub,
        newlibac_time_linkage_stub,
        newlibac_string_linkage_stub,
#if defined(XWMDCFG_libc_newlibac_SPRINTF) && (1 == XWMDCFG_libc_newlibac_SPRINTF)
        newlibac_sprintf_linkage_stub,
#endif
#if defined(XWMDCFG_libc_newlibac_MEM) && (1 == XWMDCFG_libc_newlibac_MEM)
        newlibac_mem_linkage_stub,
#endif
#if defined(XWMDCFG_libc_newlibac_SETJMP) && (1 == XWMDCFG_libc_newlibac_SETJMP)
        newlibac_setjmp_linkage_stub,
#endif
#if defined(XWMDCFG_libc_newlibac_FOPS) && (1 == XWMDCFG_libc_newlibac_FOPS)
        newlibac_fops_linkage_stub,
#endif
        newlibac_isatty_linkage_stub,
        newlibac_kill_linkage_stub,
        newlibac_getpid_linkage_stub,
        newlibac_exit_linkage_stub,
        NULL,
};

extern void newlibac_lock_init(void);

/**
 * @brief 初始化列表
 * @note
 * + 此函数表同时作为静态链接占位符，可保证符号重名时优先使用此库中的符号。
 */
// cppcheck-suppress [misra-c2012-8.4]
const newlibac_init_f newlibac_init_table[] = {
        newlibac_lock_init,
        NULL,
};

xwer_t newlibac_init(void)
{
        const newlibac_init_f * f = newlibac_init_table;
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
