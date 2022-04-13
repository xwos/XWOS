#! /bin/make -f
# @file
# @brief 外部静态库列表
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

EINCDIRS :=
ELIBS :=

ifeq ($(ARCHCFG_COMPILER_CLIB),y)
  ELIBS_gcc += -lstdc++ -lsupc++ -lm -lc -lnosys
endif
ELIBS_gcc += -lgcc

ifeq ($(ARCHCFG_COMPILER_CLIB),y)
  ELIBS_llvm += -lc++ -lc++abi -lm -lc -lnosys
endif
ELIBS_llvm += -lclang_rt.builtins-armv7em
ELIBS_llvm += -lunwind
