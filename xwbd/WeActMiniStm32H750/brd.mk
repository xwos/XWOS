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

STM32CUBE_DEFS := -DUSE_FULL_LL_DRIVER -DUSE_HAL_DRIVER -DSTM32H750xx
STM32CUBE_INCDIRS := bm/stm32cube/Core/Inc
STM32CUBE_INCDIRS += bm/stm32cube/Drivers/STM32H7xx_HAL_Driver/Inc
STM32CUBE_INCDIRS += bm/stm32cube/Drivers/STM32H7xx_HAL_Driver/Inc/Legacy
STM32CUBE_INCDIRS += bm/stm32cube/Drivers/CMSIS/Device/ST/STM32H7xx/Include
STM32CUBE_INCDIRS += bm/stm32cube/Drivers/CMSIS/Include

BRD_INCDIRS := $(STM32CUBE_INCDIRS)
BRD_AFLAGS := $(STM32CUBE_DEFS)
BRD_CFLAGS := $(STM32CUBE_DEFS)
BRD_CXFLAGS := $(STM32CUBE_DEFS)
BRD_LDFLAGS :=

BRD_ASRCS :=
BRD_CSRCS :=
BRD_CXXSRCS :=

BRD_EOBJS :=

BRD_CSRCS := board/init.c
BRD_CSRCS += board/firmware.c
