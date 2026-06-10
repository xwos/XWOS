#! /bin/make -f
# @file
# @brief ARCH描述层的编译规则
# @author
# + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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

ARCH_INCDIRS :=
ARCH_AFLAGS :=
ARCH_CFLAGS :=
ARCH_CXXFLAGS :=
ARCH_LDFLAGS :=

ARCH_ASRCS :=
ARCH_CSRCS :=
ARCH_CXXSRCS :=

ARCH_EOBJS :=

ARCH_CSRCS += xwosimpl_arch_xwbop/xwbop.c
ARCH_ASRCS += xwosimpl_arch_xwbop/asm_xwbop.S

ifeq ($(ARCHCFG_LIB_XWAOP32),y)
  ARCH_CSRCS += xwosimpl_arch_xwaop/s32.c
  ARCH_CSRCS += xwosimpl_arch_xwaop/u32.c
endif

ifeq ($(ARCHCFG_LIB_LFQ),y)
  ARCH_CSRCS += xwosimpl_arch_lfq.c
endif

ifeq ($(ARCHCFG_LIB_SPINLOCK),y)
  ARCH_ASRCS += xwosimpl_arch_spinlock.S
endif

ARCH_CSRCS += arch_init.c arch_sc_trap.c
ARCH_CSRCS += xwosimpl_arch_xwsc.c xwosimpl_tls.c

ARCH_ASRCS += e200x_isa.S
ARCH_ASRCS += startup.S arch_isr.S
