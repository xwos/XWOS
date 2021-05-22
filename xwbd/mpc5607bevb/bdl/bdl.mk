#! /bin/make -f
# @file
# @brief 板级描述层的编译规则
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

BDL_INCDIRS :=
BDL_EOBJS :=

BDL_AFLAGS :=
BDL_CFLAGS :=
BDL_CXFLAGS :=
BDL_LDFLAGS :=

BDL_CSRCS :=
BDL_ASRCS :=

BDL_CSRCS += board_init.c
BDL_CSRCS += vector.c
ifeq ($(BRDCFG_XWSKD_THD_STACK_POOL),y)
    BDL_CSRCS += xwac/thread_stack_pool.c
endif
BDL_CSRCS += xwac/xwskd_hook.c
BDL_CSRCS += eirq.c
BDL_CSRCS += bkup.c
BDL_CSRCS += main.c

BDL_CSRCS += ds/device.c
BDL_CSRCS += ds/driver/soc.c
BDL_CSRCS += ds/driver/dmauart0.c
BDL_CSRCS += ds/description/soc_cfg.c
BDL_CSRCS += ds/description/dmauart_cfg.c
