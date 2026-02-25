#! /bin/make -f
# @file
# @brief SOC描述层的编译规则
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

SOC_INCDIRS :=
SOC_AFLAGS := -mfpu=fpv5-d16
SOC_CFLAGS := -mfpu=fpv5-d16
SOC_CXXFLAGS := -mfpu=fpv5-d16
SOC_LDFLAGS := -mfpu=fpv5-d16

SOC_ASRCS :=
SOC_CSRCS :=
SOC_CXXSRCS :=

SOC_EOBJS :=

SOC_CSRCS += soc_init.c
ifeq ($(SOCCFG_CHIP_FC7300F4MDS),y)
  SOC_CSRCS += fc7300f4mds_init.c
endif
ifeq ($(SOCCFG_CHIP_FC7300F4MDD),y)
  SOC_CSRCS += fc7300f4mdd_init.c
endif
ifeq ($(SOCCFG_CHIP_FC7300F8MDT),y)
  SOC_CSRCS += fc7300f8mdt_init.c
endif
ifeq ($(SOCCFG_CHIP_FC7300F8MDQ),y)
  SOC_CSRCS += fc7300f8mdq_init.c
endif
SOC_CSRCS += xwosimpl_irq.c
SOC_CSRCS += xwosimpl_skd.c
SOC_CSRCS += xwosimpl_syshwt.c
