/**
 * @file
 * @brief 玄武OS移植层：线程本地存储
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_ospl_tls_h__
#define __xwos_ospl_tls_h__

#include <xwos/standard.h>
#include <xwos/ospl/skd.h>

/**
 * @brief 玄武OS移植层：初始化线程对象的TLS段
 * @param[in] thd: 线程对象的指针
 */
void xwospl_tls_init(struct xwospl_skdobj_stack * stk);

#endif /* xwos/ospl/tls.h */
