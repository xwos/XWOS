#! /bin/make -f
# @file
# @brief XWOS AUTOSAR CP OS适配层编译规则
# @author
# + 隐星魂 (Roy Sun) <xwos@xwos.tech>
# @copyright
# + Copyright © 2015 xwos.tech, All Rights Reserved.
# > This Source Code Form is subject to the terms of the Mozilla Public
# > License, v. 2.0. If a copy of the MPL was not distributed with this
# > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.

XWOS_CSRCS += arcos/irq.c
XWOS_CSRCS += arcos/counter.c
XWOS_CSRCS += arcos/resource.c
XWOS_CSRCS += arcos/spinlock.c
XWOS_CSRCS += arcos/task.c
XWOS_CSRCS += arcos/schedtbl.c
