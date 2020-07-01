#! /bin/make -f
# @file
# @brief 编译模块并生成.a的Makefile模板 (eppc-e200zx-codewarrior)
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

include xwbs/$(XuanWuOS_CFG_MK_RULE)

XWMO_NAME := $(call getXwmoName)
XWMO_DIR := $(call getXwmoDir)
XWMO_OBJ_DIR ?= $(XWMO_DIR)
XWMO_COBJS := $(addprefix $(OBJ_DIR)$(XWMO_OBJ_DIR)/,$(addsuffix .o,$(basename $(XWMO_CSRCS))))
XWMO_DSMS := $(addprefix $(OBJ_DIR)$(XWMO_OBJ_DIR)/,$(addsuffix .dsm,$(basename $(XWMO_CSRCS))))
XWMO_INCDIRS := $(if $(strip $(XWMO_INCDIRS)),$(addprefix -I,$(strip $(XWMO_INCDIRS))))

CC_ARGS = $(strip -c $(CFLAGS) $(ARCH_CFLAGS) $(CPU_CFLAGS) $(XWMO_CFLAGS) \
                  $(INCDIRS) $(XWMO_INCDIRS))

#$(info "building $(XWMO_DIR) ---> $(OBJ_DIR)$(XWMO_OBJ_DIR)/$(XWMO_NAME)")

$(OBJ_DIR)$(XWMO_OBJ_DIR)/$(XWMO_NAME): $(XWMO_COBJS) $(OBJ_DIR)$(XWMO_OBJ_DIR)
ifeq ($(CWMCUEPPC_LICENSE),)
	$(SHOW_AR) echo -n $(XWMO_COBJS) > $@
else
	$(SHOW_AR) $(AR) -o $@ $(XWMO_COBJS)
endif

$(OBJ_DIR)$(XWMO_OBJ_DIR):
	@[ ! -d $@ ] && mkdir -p $@ || true

ifneq ($(NODEP),y)
#   -include $(XWMO_COBJS:.o=.d)
endif

$(OBJ_DIR)$(XWMO_OBJ_DIR)/%.o: $(XWMO_DIR)/%.c
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_CC) $(CC) $(CC_ARGS) $< -o $@

%.dsm: %.o
	$(SHOW_OD) $(OD) $(CFLAGS) $< > $@

dsm: $(XWMO_DSMS)

clean:
	@$(RM) -f $(XWMO_COBJS:.o=.lst)
	@$(RM) -f $(XWMO_COBJS:.o=.dsm)
	@$(RM) -f $(XWMO_COBJS)
	@$(RM) -f $(OBJ_DIR)$(XWMO_OBJ_DIR)/$(XWMO_NAME)

distclean:
	@$(RM) -rf $(OBJ_DIR)$(XWMO_OBJ_DIR)

.PHONY : dsm clean distclean
