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

include $(XuanWuOS_WKSPC_DIR)/XuanWuOS.cfg
include $(XWBS_UTIL_MK_XWMO)
XWMO_CSRCS := xwtm.c skd.c thd.c mtx.c spinlock.c
XWMO_CFLAGS := -Wno-unused-function
XWMO_INCDIRS :=
include xwbs/$(XuanWuOS_CFG_XWMO_MK)
