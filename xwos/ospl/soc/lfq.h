/**
 * @file
 * @brief 玄武OS移植层：SOC无锁队列
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_ospl_soc_lfq_h__
#define __xwos_ospl_soc_lfq_h__

#include <xwos/standard.h>

void xwlib_lfq_push(xwlfq_a * h, xwlfq_a * n);
xwlfq_t * xwlib_lfq_pop(xwlfq_a * h);

#include <xwosimpl_soc_lfq.h>

#endif /* xwos/ospl/soc/lfq.h */
