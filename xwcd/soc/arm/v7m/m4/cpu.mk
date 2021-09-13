#! /bin/make -f
# @file
# @brief CPU描述层的编译规则
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

CPU_INCDIRS :=
CPU_CFLAGS := -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
CPU_CXXFLAGS := -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16 -fno-exceptions -fno-rtti
CPU_AFLAGS := -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16
CPU_LDFLAGS := -mcpu=cortex-m4 -mfloat-abi=hard -mfpu=fpv4-sp-d16

CPU_ASRCS :=
CPU_CSRCS := cpu_init.c
CPU_CXXSRCS :=

CPU_EOBJS :=

CPU_AFLAGS_llvm := --target=armv7em-none-eabi
CPU_CFLAGS_llvm := --target=armv7em-none-eabi
CPU_CXXFLAGS_llvm := --target=armv7em-none-eabi
CPU_LDFLAGS_llvm := --target=armv7em-none-eabi
SYSROOT_llvm := armv7em_hard_fpv4_sp_d16
