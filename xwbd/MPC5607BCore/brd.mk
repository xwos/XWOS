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

BRD_INCDIRS :=
BRD_AFLAGS :=
BRD_CFLAGS :=
BRD_CXFLAGS :=
BRD_LDFLAGS :=

BRD_ASRCS :=
BRD_CSRCS :=
BRD_CXXSRCS :=

BRD_EOBJS :=

BRD_CSRCS += bdl/board_init.c
BRD_CSRCS += bdl/ivt.c
ifeq ($(BRDCFG_XWSKD_THD_STACK_POOL),y)
  BRD_CSRCS += bdl/xwac/thread_stack_pool.c
endif
BRD_CSRCS += bdl/xwac/xwskd_hook.c
BRD_CSRCS += bdl/eirq.c
BRD_CSRCS += bdl/bkup.c
BRD_CSRCS += bdl/main.c

BRD_CSRCS += bdl/ds/device.c
BRD_CSRCS += bdl/ds/driver/soc.c
BRD_CSRCS += bdl/ds/driver/dmauart0.c
BRD_CSRCS += bdl/ds/description/soc_cfg.c
BRD_CSRCS += bdl/ds/description/dmauart_cfg.c
