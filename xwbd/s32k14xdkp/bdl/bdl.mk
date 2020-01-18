#! /bin/make -f
# @file
# @brief 板级描述层的编译规则
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

BDL_EOBJS :=

BDL_CSRCS :=
BDL_ASRCS :=

BDL_CSRCS += board_init.c
BDL_CSRCS += vector.c
ifeq ($(BRDCFG_XWSD_IDLE_HOOK),y)
    BDL_CSRCS += xwkac/idle_hook.c
endif
ifeq ($(XuanWuOS_CFG_XWMD),y)
    ifeq ($(XWMDCFG_ds),y)
        BDL_CSRCS += ds/xwds.c
        BDL_CSRCS += ds/description/s32k14xdkp.c
        ifeq ($(XWMDCFG_ds_SOC),y)
            BDL_CSRCS += ds/description/soc_cfg.c
        endif
    endif
endif
BDL_CSRCS += main.c
