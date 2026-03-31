#! /bin/make -f
# @file
# @brief XWOS通用库的编译规则
# @author
# + 隐星曜 (Roy Sun) <xwos@xwos.tech>
# @copyright
# + Copyright © 2015 xwos.tech, All Rights Reserved.
# > This Source Code Form is subject to the terms of the Mozilla Public
# > License, v. 2.0. If a copy of the MPL was not distributed with this
# > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.

######## ######## ######## ######## xwlib ######## ######## ######## ########
XWOS_CSRCS += lib/object.c
XWOS_CSRCS += lib/xwbop.c
XWOS_CSRCS += lib/rbtree.c
XWOS_CSRCS += lib/div64.c

ifeq ($(XWLIBCFG_XWAOP8),y)
  ifeq ($(ARCHCFG_LIB_XWAOP8),y)
    ifeq ($(SOCCFG_LIB_XWAOP8),y)
      $(error "ARCHCFG_LIB_XWAOP8 and SOCCFG_LIB_XWAOP8 cannot be configured 1 simultaneously!")
    endif
  endif
  ifneq ($(ARCHCFG_LIB_XWAOP8),y)
    ifneq ($(SOCCFG_LIB_XWAOP8),y)
      XWOS_CSRCS += lib/xwaop/s8.c
      XWOS_CSRCS += lib/xwaop/u8.c
    endif
  endif
endif

ifeq ($(XWLIBCFG_XWAOP16),y)
  ifeq ($(ARCHCFG_LIB_XWAOP16),y)
    ifeq ($(SOCCFG_LIB_XWAOP16),y)
      $(error "ARCHCFG_LIB_XWAOP16 and SOCCFG_LIB_XWAOP16 cannot be configured 1 simultaneously!")
    endif
  endif
  ifneq ($(ARCHCFG_LIB_XWAOP16),y)
    ifneq ($(SOCCFG_LIB_XWAOP16),y)
      XWOS_CSRCS += lib/xwaop/s16.c
      XWOS_CSRCS += lib/xwaop/u16.c
    endif
  endif
endif

ifeq ($(XWLIBCFG_XWAOP32),y)
  ifeq ($(ARCHCFG_LIB_XWAOP32),y)
    ifeq ($(SOCCFG_LIB_XWAOP32),y)
      $(error "ARCHCFG_LIB_XWAOP32 and SOCCFG_LIB_XWAOP32 cannot be configured 1 simultaneously!")
    endif
  endif
  ifneq ($(ARCHCFG_LIB_XWAOP32),y)
    ifneq ($(SOCCFG_LIB_XWAOP32),y)
      XWOS_CSRCS += lib/xwaop/s32.c
      XWOS_CSRCS += lib/xwaop/u32.c
    endif
  endif
endif

ifeq ($(XWLIBCFG_XWAOP64),y)
  ifeq ($(ARCHCFG_LIB_XWAOP64),y)
    ifeq ($(SOCCFG_LIB_XWAOP64),y)
      $(error "ARCHCFG_LIB_XWAOP64 and SOCCFG_LIB_XWAOP64 cannot be configured 1 simultaneously!")
    endif
  endif
  ifneq ($(ARCHCFG_LIB_XWAOP64),y)
    ifneq ($(SOCCFG_LIB_XWAOP64),y)
      XWOS_CSRCS += lib/xwaop/s64.c
      XWOS_CSRCS += lib/xwaop/u64.c
    endif
  endif
endif

ifeq ($(ARCHCFG_LIB_LFQ),y)
  ifeq ($(SOCCFG_LIB_LFQ),y)
    $(error "ARCHCFG_LIB_LFQ and SOCCFG_LIB_LFQ cannot be configured 1 simultaneously!")
  endif
endif

ifeq ($(XWLIBCFG_MAP),y)
  XWOS_CSRCS += lib/map.c
endif

XWOS_CSRCS += lib/xwspf.c
ifeq ($(XWLIBCFG_LOG),y)
  XWOS_CSRCS += lib/xwlog.c
endif

ifeq ($(XWLIBCFG_CRC32),y)
  XWOS_CSRCS += lib/crc32.c
endif
ifeq ($(XWLIBCFG_CRC8),y)
  XWOS_CSRCS += lib/crc8.c
endif
