/**
 * @file
 * @brief XWOS通用库：玄武OS内核的编译规则
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_lib_rule_h__
#define __xwos_lib_rule_h__

#include <cfg/XuanWuOS.h>

/**
 * @mainpage XWOS通用库
 * + @subpage XuanWuOS_Version
 * + @subpage compiler
 * + @subpage error
 * + @subpage type
 * + @subpage bclst
 * + @subpage rbtree
 * + @subpage map
 * + @subpage CRC8
 * + @subpage CRC32
 * + @subpage xwlog
 * + @subpage ctype
 * + @subpage div64
 * + @subpage xwaop
 * + @subpage lfq
 * + @subpage xwbop
 * + @subpage xwbmpaop
 * + @subpage xwobj
 * + @subpage setjmp
 * + @subpage xwsc
 */

#if defined(XuanWuOS_CFG_CORE__mp)
#  include <xwos/mp/rule.h>
#elif defined(XuanWuOS_CFG_CORE__up)
#  include <xwos/up/rule.h>
#else
#  error "Can't find the configuration XuanWuOS_CFG_CORE!"
#endif

#endif /* xwos/lib/rule.h */
