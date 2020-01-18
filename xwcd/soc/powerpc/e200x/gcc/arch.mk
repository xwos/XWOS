#! /bin/make -f
# @file
# @brief ARCH描述层的编译规则
# @author
# + 隐星魂 (Roy.Sun) <www.starsoul.tech>
# @copyright
# + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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

ARCH_CFLAGS :=
ARCH_AFLAGS :=
ARCH_LDFLAGS :=
ARCH_EOBJS :=

ARCH_CSRCS :=
ARCH_ASRCS :=

ifeq ($(ARCHCFG_LIB_XWAOP32),y)
    ARCH_CSRCS += asmlib/xwaop/xws32_t.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t.c
endif

ifeq ($(ARCHCFG_LIB_XWBMPAOP),y)
    ARCH_CSRCS += asmlib/xwbmpaop.c
    ARCH_ASRCS += asmlib/asm_xwbmpaop.S
endif

ARCH_CSRCS += asmlib/lfq.c
ARCH_CSRCS += asmlib/xwbop.c
ARCH_ASRCS += asmlib/asm_xwbop.S

ARCH_CSRCS += arch_init.c arch_sc_trap.c arch_xwsc.c
ARCH_ASRCS += startup.S
ARCH_ASRCS += arch_isr.S
ARCH_ASRCS += arch_spinlock.S
