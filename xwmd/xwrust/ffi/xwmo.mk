#! /bin/make -f
# @file
# @brief XWOS模块的编译规则
# @author
# + 隐星魂 (Roy Sun) <xwos@xwos.tech>
# @copyright
# + Copyright © 2015 xwos.tech, All Rights Reserved.
# > This Source Code Form is subject to the terms of the Mozilla Public
# > License, v. 2.0. If a copy of the MPL was not distributed with this
# > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.

include $(XWOS_WKSPC_DIR)/XWOS.cfg
include xwbs/util/mk/xwmo.mk
XWMO_CSRCS := xwtm.c skd.c pm.c thd.c swt.c mtx.c spinlock.c seqlock.c sem.c cond.c flg.c br.c sel.c
XWMO_CFLAGS := -Wno-unused-function
XWMO_INCDIRS :=
include xwbs/xwmo.mk
