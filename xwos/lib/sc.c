/**
 * @file
 * @brief 玄武OS通用库：系统调用与系统特权
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/lib/sc.h>

/**
 * @brief 开启OS特权
 * @note
 * - 开启OS特权后，非特权线程可以调用一些系统功能，例如：开关中断等等。
 */
__xwlib_code
void xwlib_privilege_start(void)
{
        soc_privilege_start();
}

/**
 * @brief 结束OS特权
 */
__xwlib_code
void xwlib_privilege_end(void)
{
        soc_privilege_end();
}
