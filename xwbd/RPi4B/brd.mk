#! /bin/make -f
# @file
# @brief 板级描述层的编译规则
# @author
# + 隐星魂 (Roy Sun) <xwos@xwos.tech>
# @copyright
# + Copyright © 2015 xwos.tech, All Rights Reserved.
# > Licensed under the Apache License, Version 2.0 (the "License");
# > you may not use this file except in compliance with the License.
# > You may obtain a copy of the License at
# >
# >         http://www.apache.org/licenses/LICENSE-2.0
# >
# > Unless required by applicable law or agreed to in writing, software
# > distributed under the License is distributed on an "AS IS" BASIS,
# > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# > See the License for the specific language governing permissions and
# > limitations under the License.
#

BRD_INCDIRS :=
BRD_AFLAGS :=
BRD_CFLAGS :=
BRD_CXXFLAGS :=
BRD_LDFLAGS :=
BRD_CPPCHECK_FLAGS :=

BRD_ASRCS :=
BRD_CSRCS :=
BRD_CXXSRCS :=
BRD_LDFLAGS :=
BRD_CXXFLAGS += -Wno-sign-conversion
BRD_CFLAGS += -Wno-sign-conversion
BRD_LDFLAGS_gcc := -Wl,--no-warn-rwx-segment

BRD_EOBJS :=

BRD_CSRCS := board/init.c
BRD_CSRCS += board/main.c
BRD_CSRCS += $(call BrdWildcard,*.c,board/xwac/xwos)
ifeq ($(XWCFG_LIBC),newlib)
  BRD_CSRCS += $(call BrdWildcard,*.c,board/xwac/newlib)
endif
ifeq ($(XWCFG_LIBC),picolibc)
  BRD_CSRCS += $(call BrdWildcard,*.c,board/xwac/picolibc)
endif
BRD_CSRCS += $(call BrdWildcard,*.c,board/xwac/xwds)
