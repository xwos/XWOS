/**
 * @file
 * @brief 操作系统抽象层：线程接口描述
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_osal_jack_thd_h__
#define __xwos_osal_jack_thd_h__

#if defined(XuanWuOS_CFG_CORE__mp)
#  include <xwos/mp/osdl/thd.h>
#elif defined(XuanWuOS_CFG_CORE__up)
#  include <xwos/up/osdl/thd.h>
#endif

#endif /* xwos/osal/jack/thd.h */
