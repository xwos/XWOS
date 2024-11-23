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

XWMO_CSRCS := mif.c
XWMO_CSRCS += errno.c
XWMO_CSRCS += lock.c
XWMO_CSRCS += sysconf.c
XWMO_CSRCS += time.c
XWMO_CSRCS += string.c
ifeq ($(XWMDCFG_libc_newlibac_SPRINTF),y)
  XWMO_CSRCS += sprintf.c
endif
ifeq ($(XWMDCFG_libc_newlibac_MEM),y)
  XWMO_CSRCS += mem.c
endif
ifeq ($(XWMDCFG_libc_newlibac_SETJMP),y)
  XWMO_CSRCS += setjmp.c
endif
ifeq ($(XWMDCFG_libc_newlibac_FOPS),y)
  XWMO_CSRCS += fops.c
endif
ifeq ($(XWMDCFG_libc_newlibac_FATFS),y)
  XWMO_CSRCS += fatfs.c
endif
XWMO_CSRCS += isatty.c
XWMO_CSRCS += getpid.c
XWMO_CSRCS += kill.c
XWMO_CSRCS += exit.c

XWMO_CFLAGS := -Wno-unused-value -Wno-sign-conversion -Wno-error=cpp
XWMO_INCDIRS :=
