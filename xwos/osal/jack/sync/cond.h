/**
 * @file
 * @brief 操作系统抽象层：条件量接口描述
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_osal_jack_sync_cond_h__
#define __xwos_osal_jack_sync_cond_h__

#if defined(XWCFG_CORE__mp)
#  include <xwos/mp/osdl/sync/cond.h>
#elif defined(XWCFG_CORE__up)
#  include <xwos/up/osdl/sync/cond.h>
#endif

#endif /* xwos/osal/jack/sync/cond.h */
