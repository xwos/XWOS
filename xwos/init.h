/**
 * @file
 * @brief XWOS的初始化
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_init_h__
#define __xwos_init_h__

#include <xwos/standard.h>

/**
 * @brief 预初始化阶段函数
 * @details
 * 此函数由用户定义，表示 @ref xwos_init() 之前的初始化工作。
 */
void xwos_preinit(void);

/**
 * @brief 初始化XWOS内核
 * @details
 * 在多核系统中， `xwos_init()` 在每个CPU的初始化流程中都会被调用一次。
 */
void xwos_init(void);

/**
 * @brief 后初始化阶段函数
 * @details
 * 此函数由用户定义，表示 @ref xwos_init() 之后的初始化工作。
 */
void xwos_postinit(void);

/**
 * @brief 主函数
 * @details
 * 此函数由用户定义，用户的应用代码由此开始，类似于 `main()` 。
 */
xwer_t xwos_main(void);

#endif /* xwos/init.h */
