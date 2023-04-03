#! /bin/make -f
# @file
# @brief 玄武OS通用库的编译规则
# @author
# + 隐星魂 (Roy Sun) <xwos@xwos.tech>
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
    ifneq ($(ARCHCFG_LIB_XWAOP8),y)
        XWOS_CSRCS += lib/xwaops8.c
        XWOS_CSRCS += lib/xwaopu8.c
    endif
endif

ifeq ($(XWLIBCFG_XWAOP16),y)
    ifneq ($(ARCHCFG_LIB_XWAOP16),y)
        XWOS_CSRCS += lib/xwaops16.c
        XWOS_CSRCS += lib/xwaopu16.c
    endif
endif

ifeq ($(XWLIBCFG_XWAOP32),y)
    ifneq ($(ARCHCFG_LIB_XWAOP32),y)
        XWOS_CSRCS += lib/xwaops32.c
        XWOS_CSRCS += lib/xwaopu32.c
    endif
endif

ifeq ($(XWLIBCFG_XWAOP64),y)
    ifneq ($(ARCHCFG_LIB_XWAOP64),y)
        XWOS_CSRCS += lib/xwaops64.c
        XWOS_CSRCS += lib/xwaopu64.c
    endif
endif

ifeq ($(XWLIBCFG_XWBMPAOP),y)
    ifneq ($(ARCHCFG_LIB_XWBMPAOP),y)
        XWOS_CSRCS += lib/xwbmpaop.c
    endif
endif

ifeq ($(XWLIBCFG_MAP),y)
    XWOS_CSRCS += lib/map.c
endif

ifeq ($(XWLIBCFG_LOG),y)
    XWOS_CSRCS += lib/xwlog.c
endif

ifeq ($(XWLIBCFG_CRC32),y)
    XWOS_CSRCS += lib/crc32.c
endif
ifeq ($(XWLIBCFG_CRC8),y)
    XWOS_CSRCS += lib/crc8.c
endif
