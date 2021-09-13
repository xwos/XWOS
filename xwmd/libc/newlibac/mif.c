/**
 * @file
 * @brief newlib适配代码：模块接口
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
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
extern void newlibac_mem_init(void);
extern void newlibac_fops_init(void);
extern void newlibac_string_init(void);
extern void newlibac_lock_init(void);

/**
 * @brief 初始化列表
 * @note
 * + 此函数表同时作为静态链接占位符，可保证符号重名时优先使用此库中的符号。
 */
const newlibac_init_f newlibac_init_table[] = {
        newlibac_errno_init,
        newlibac_mem_init,
        newlibac_fops_init,
        newlibac_string_init,
        newlibac_lock_init,
};

xwer_t newlibac_init(void)
{
        const newlibac_init_f * f = newlibac_init_table;
        xwsz_t sz = xw_array_size(newlibac_init_table);
        xwsz_t i;

        atexit(__libc_fini_array);
        __libc_init_array();
        for (i = 0; i < sz; i++) {
                f[i]();
        }
        return XWOK;
}
