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
#include <xwos/osal/time.h>
#include <xwmd/libc/picolibcac/check.h>
#include <stdlib.h>
#include <time.h>

typedef void (* picolibcac_linkage_f)(void);
typedef void (* picolibcac_init_f)(void);

extern void __libc_init_array(void);
extern void __libc_fini_array(void);

extern void picolibcac_errno_linkage_stub(void);
extern void picolibcac_lock_linkage_stub(void);
extern void picolibcac_sysconf_linkage_stub(void);
extern void picolibcac_time_linkage_stub(void);
extern void picolibcac_string_linkage_stub(void);
extern void picolibcac_mem_linkage_stub(void);
extern void picolibcac_setjmp_linkage_stub(void);
extern void picolibcac_fops_linkage_stub(void);
extern void picolibcac_isatty_linkage_stub(void);
extern void picolibcac_kill_linkage_stub(void);
extern void picolibcac_getpid_linkage_stub(void);
extern void picolibcac_exit_linkage_stub(void);

/**
 * @brief 静态链接表
 * @note
 * + 此函数表作为静态链接占位符，可保证符号重名时优先使用此库中的符号。
 */
const picolibcac_linkage_f picolibcac_linkage_table[] = {
        picolibcac_errno_linkage_stub,
        picolibcac_lock_linkage_stub,
        picolibcac_sysconf_linkage_stub,
        picolibcac_time_linkage_stub,
        picolibcac_string_linkage_stub,
        picolibcac_mem_linkage_stub,
        picolibcac_setjmp_linkage_stub,
        picolibcac_fops_linkage_stub,
        picolibcac_isatty_linkage_stub,
        picolibcac_kill_linkage_stub,
        picolibcac_getpid_linkage_stub,
        picolibcac_exit_linkage_stub,
};

extern void picolibcac_lock_init(void);

/**
 * @brief 初始化列表
 */
const picolibcac_init_f picolibcac_init_table[] = {
        picolibcac_lock_init,
};

xwer_t picolibcac_init(void)
{
        const picolibcac_init_f * f = picolibcac_init_table;
        xwsz_t sz = xw_array_size(picolibcac_init_table);
        xwsz_t i;

        for (i = 0; i < sz; i++) {
                f[i]();
        }
        __libc_init_array();
        atexit(__libc_fini_array);
        return XWOK;
}
