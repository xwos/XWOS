#! /bin/make -f
# @file
# @brief SOC描述层的编译规则
# @author
# + 隐星魂 (Roy.Sun) <https://xwos.tech>
# @copyright
# + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
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
SOC_AFLAGS :=
SOC_CFLAGS :=
SOC_CXXFLAGS :=
SOC_LDFLAGS :=

SOC_ASRCS :=
SOC_CSRCS :=
SOC_CXXSRCS :=

SOC_EOBJS :=

SOC_ASRCS += soc_eii_isr.S xwosasmimpl_skd.S

SOC_CSRCS += soc_init.c
SOC_CSRCS += soc_me.c
ifeq ($(SOCCFG_FLASH_OPCODE),y)
  SOC_CSRCS += soc_flash.c
endif
SOC_CSRCS += soc_wdg.c
SOC_CSRCS += soc_reset.c
SOC_CSRCS += xwosimpl_irq.c
SOC_CSRCS += xwosimpl_skd.c
SOC_CSRCS += xwosimpl_syshwt.c
ifeq ($(XuanWuOS_CFG_CORE),mp)
  SOC_CSRCS += soc_mp_irqc.c
  SOC_CSRCS += soc_mp_irqc_drv.c
endif
