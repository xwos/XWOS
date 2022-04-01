/**
 * @file
 * @brief newlib适配代码：模块接口
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwmd/libc/newlibac/mif.h>
#include <stdlib.h>

typedef void (* newlibac_init_f)(void);

extern void __libc_init_array (void);
extern void __libc_fini_array (void);

extern void newlibac_errno_init(void);
#if defined(XWMDCFG_libc_newlibac_string) && (1 == XWMDCFG_libc_newlibac_string)
extern void newlibac_string_init(void);
#endif
#if defined(XWMDCFG_libc_newlibac_mem) && (1 == XWMDCFG_libc_newlibac_mem)
extern void newlibac_mem_init(void);
#endif
#if defined(XWMDCFG_libc_newlibac_fops) && (1 == XWMDCFG_libc_newlibac_fops)
extern void newlibac_fops_init(void);
#endif
extern void newlibac_lock_init(void);

/**
 * @brief 初始化列表
 * @note
 * + 此函数表同时作为静态链接占位符，可保证符号重名时优先使用此库中的符号。
 */
const newlibac_init_f newlibac_init_table[] = {
        newlibac_errno_init,
#if defined(XWMDCFG_libc_newlibac_string) && (1 == XWMDCFG_libc_newlibac_string)
        newlibac_string_init,
#endif
#if defined(XWMDCFG_libc_newlibac_mem) && (1 == XWMDCFG_libc_newlibac_mem)
        newlibac_mem_init,
#endif
#if defined(XWMDCFG_libc_newlibac_fops) && (1 == XWMDCFG_libc_newlibac_fops)
        newlibac_fops_init,
#endif
        newlibac_lock_init,
};

xwer_t newlibac_init(void)
{
        const newlibac_init_f * f = newlibac_init_table;
        xwsz_t sz = xw_array_size(newlibac_init_table);
        xwsz_t i;

        for (i = 0; i < sz; i++) {
                f[i]();
        }
        __libc_init_array();
        atexit(__libc_fini_array);
        return XWOK;
}
