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
include $(XWBS_UTIL_MK_XWMO)

$(eval $(call XwmoReqCfg,XWOS_CFG_LIBC,picolibc))
$(eval $(call XwmoReqCfg,XWEMCFG_fs_fatfs,y))
$(eval $(call XwmoReqCfg,XWMMCFG_MEMPOOL,y))
$(eval $(call XwmoNotReqCfg,XWMDCFG_libc_newlibac,y))

XWMO_CSRCS := mif.c
XWMO_CSRCS += errno.c
XWMO_CSRCS += lock.c
XWMO_CSRCS += sysconf.c
XWMO_CSRCS += time.c
XWMO_CSRCS += string.c
XWMO_CSRCS += mem.c
XWMO_CSRCS += setjmp.c
XWMO_CSRCS += fops.c
XWMO_CSRCS += isatty.c
XWMO_CSRCS += getpid.c
XWMO_CSRCS += kill.c
XWMO_CSRCS += exit.c

XWMO_CFLAGS := -Wno-unused-value -Wno-unused-parameter
XWMO_INCDIRS :=
include xwbs/$(XWOS_CFG_XWMO_MK)
