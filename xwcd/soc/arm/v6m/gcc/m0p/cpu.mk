#! /bin/make -f
# @file
# @brief CPU描述层的编译规则
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

CPU_INCDIRS :=
CPU_CFLAGS := -mcpu=cortex-m0plus
CPU_CXXFLAGS := -mcpu=cortex-m0plus
CPU_AFLAGS := -mcpu=cortex-m0plus
CPU_LDFLAGS := -mcpu=cortex-m0plus
CPU_EOBJS :=

CPU_CSRCS := cpu_init.c
CPU_ASRCS :=
