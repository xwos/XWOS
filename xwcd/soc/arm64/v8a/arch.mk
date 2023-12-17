#! /bin/make -f
# @file
# @brief ARCH描述层的编译规则
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

ARCH_INCDIRS :=
ARCH_AFLAGS := -ffixed-x18 -mcmodel=large
ARCH_CFLAGS := -ffixed-x18 -mcmodel=large
ARCH_CXXFLAGS := -ffixed-x18 -mcmodel=large
ARCH_LDFLAGS := -mcmodel=large

ARCH_ASRCS :=
ARCH_CSRCS :=
ARCH_CXXSRCS :=
ARCH_EOBJS :=


ARCH_ASRCS += arch_exception.S

ARCH_CSRCS += xwosimpl_soc_xwaop/s8.c
ARCH_CSRCS += xwosimpl_soc_xwaop/u8.c
ARCH_CSRCS += xwosimpl_soc_xwaop/s16.c
ARCH_CSRCS += xwosimpl_soc_xwaop/u16.c
ARCH_CSRCS += xwosimpl_soc_xwaop/s32.c
ARCH_CSRCS += xwosimpl_soc_xwaop/u32.c
ARCH_CSRCS += xwosimpl_soc_xwaop/s64.c
ARCH_CSRCS += xwosimpl_soc_xwaop/u64.c
ARCH_CSRCS += xwosimpl_soc_xwaop/bmp.c
ARCH_CSRCS += xwosimpl_soc_xwbop.c
ARCH_CSRCS += xwosimpl_soc_lfq.c
ARCH_CSRCS += xwosimpl_tls.c
ARCH_CSRCS += arch_init.c
ARCH_CSRCS += arch_isa.c
ARCH_CSRCS += arch_irq.c
ARCH_CSRCS += arch_skd.c
ARCH_CSRCS += arch_trap.c
ifeq ($(ARCHCFG_GICV2),y)
  ARCH_CSRCS += arch_gic2.c
endif
ifeq ($(ARCHCFG_GICV3),y)
  ARCH_CSRCS += arch_gic3.c
endif
ARCH_CSRCS += arch_timer.c
ARCH_CSRCS += arch_mmu.c
ARCH_CSRCS += arch_cache.c
