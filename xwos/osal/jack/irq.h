/**
 * @file
 * @brief 操作系统抽象层：中断接口描述
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_osal_jack_irq_h__
#define __xwos_osal_jack_irq_h__

#if defined(XuanWuOS_CFG_CORE__mp)
#  include <xwos/mp/osdl/irq.h>
#elif defined(XuanWuOS_CFG_CORE__up)
#  include <xwos/up/osdl/irq.h>
#endif

#endif /* xwos/osal/jack/irq.h */
