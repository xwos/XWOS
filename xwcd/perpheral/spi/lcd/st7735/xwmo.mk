#! /bin/make -f
# @file
# @brief XWOS模块的编译规则
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

include $(XWOS_WKSPC_DIR)/XWOS.cfg
include xwbs/functions.mk

$(eval $(call XwmoReqCfg,XWCDCFG_ds_SPI_MASTER,y))
$(eval $(call XwmoReqCfg,XWCDCFG_ds_SPI_PERPHERAL,y))

XWMO_CSRCS := device.c driver.c
XWMO_CFLAGS :=
XWMO_INCDIRS :=
include xwbs/xwmo.mk
