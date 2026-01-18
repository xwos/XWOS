/**
 * @file
 * @brief XWOS通用库：编译规则
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_lib_rule_h__
#define __xwos_lib_rule_h__

#include <cfg/project.h>

#if defined(XWCFG_CORE__mp)
#  include <xwos/mp/rule.h>
#elif defined(XWCFG_CORE__up)
#  include <xwos/up/rule.h>
#else
#  error "Can't find the configuration XWCFG_CORE!"
#endif

#endif /* xwos/lib/rule.h */
