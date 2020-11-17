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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwmd/libc/newlibac/mif.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
typedef void (*newlibac_linkage_f)(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern void newlibac_mem_linkage_placeholder(void);
extern void newlibac_fops_linkage_placeholder(void);
extern void newlibac_string_linkage_placeholder(void);
extern void newlibac_lock_linkage_placeholder(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 连接占位符
 * @note
 * + 静态连接时，若符号存在多重定义，优先选择包含占位符的文件里面的符号。
 */
const newlibac_linkage_f newlibac_linkage_table[] = {
        newlibac_mem_linkage_placeholder,
        newlibac_fops_linkage_placeholder,
        newlibac_string_linkage_placeholder,
        newlibac_lock_linkage_placeholder,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t newlibac_init(void)
{
        const newlibac_linkage_f * f = newlibac_linkage_table;
        xwsz_t sz = xw_array_size(newlibac_linkage_table);
        xwsz_t i;

        for (i = 0; i < sz; i++) {
                f[i]();
        }
        return XWOK;
}
