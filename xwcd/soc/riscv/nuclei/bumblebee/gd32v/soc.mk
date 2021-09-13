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
SOC_AFLAGS := -DDOWNLOAD_MODE=DOWNLOAD_MODE_FLASHXIP
SOC_CFLAGS := -DDOWNLOAD_MODE=DOWNLOAD_MODE_FLASHXIP
SOC_CXXFLAGS := -DDOWNLOAD_MODE=DOWNLOAD_MODE_FLASHXIP
SOC_LDFLAGS := -DDOWNLOAD_MODE=DOWNLOAD_MODE_FLASHXIP

SOC_ASRCS :=
SOC_CSRCS :=
SOC_CXXSRCS :=

SOC_EOBJS :=

SOC_ASRCS += soc.S xwosasmimpl_skd.S

SOC_CSRCS += soc_init.c
SOC_CSRCS += xwosimpl_irq.c
SOC_CSRCS += xwosimpl_skd.c
SOC_CSRCS += xwosimpl_syshwt.c
