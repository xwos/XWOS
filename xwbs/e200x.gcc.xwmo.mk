#! /bin/make -f
# @file
# @brief 编译模块并生成.a的Makefile模板 (ARM-cortex-m-gcc)
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
XWMO_AOBJS := $(addprefix $(OBJ_DIR)$(XWMO_OBJ_DIR)/,$(addsuffix .o,$(basename $(XWMO_ASRCS))))
XWMO_COBJS := $(addprefix $(OBJ_DIR)$(XWMO_OBJ_DIR)/,$(addsuffix .o,$(basename $(XWMO_CSRCS))))
XWMO_DSMS += $(addprefix $(OBJ_DIR)$(XWMO_OBJ_DIR)/,$(addsuffix .dsm,$(basename $(XWMO_CXXSRCS))))
XWMO_INCDIRS := $(if $(strip $(XWMO_INCDIRS)),$(addprefix -I,$(strip $(XWMO_INCDIRS))))

MM_ARGS = $(strip $(MMFLAGS))

AS_ARGS = $(strip $(AFLAGS) $(ARCH_AFLAGS) $(CPU_AFLAGS) $(XWMO_AFLAGS) \
                  $(INCDIRS) $(XWMO_INCDIRS))

CC_ARGS = $(strip $(CFLAGS) $(ARCH_CFLAGS) $(CPU_CFLAGS) $(XWMO_CFLAGS) \
                  $(INCDIRS) $(XWMO_INCDIRS))

XWMO_OBJS_LST := $(OBJ_DIR)$(XWMO_OBJ_DIR)/objs.txt

$(OBJ_DIR)$(XWMO_OBJ_DIR)/$(XWMO_NAME): $(XWMO_AOBJS) $(XWMO_COBJS) $(OBJ_DIR)$(XWMO_OBJ_DIR)
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	@echo $(XWMO_AOBJS) $(XWMO_COBJS) $(XWMO_LUA2HEXCOBJS) $(XWMO_CXXOBJS) > $(XWMO_OBJS_LST)
	$(SHOW_AR) $(AR) rcs $@ @$(XWMO_OBJS_LST)

$(OBJ_DIR)$(XWMO_OBJ_DIR):
	@[ ! -d $@ ] && mkdir -p $@ || true

ifneq ($(XWMO_AOBJS),)
    ifneq ($(NODEP),y)
        -include $(XWMO_AOBJS:.o=.o.d)
    endif
endif

ifneq ($(XWMO_COBJS),)
    ifneq ($(NODEP),y)
        -include $(XWMO_COBJS:.o=.o.d)
    endif
endif

$(OBJ_DIR)$(XWMO_OBJ_DIR)/%.s: %.S
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_AS) $(AS) -E $(AS_ARGS) $< -o $@

$(OBJ_DIR)$(XWMO_OBJ_DIR)/%.o.d: %.S
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_MM) $(CC) $(MM_ARGS) $(AS_ARGS) $< > $@;
	@sed -i 's|\(^.*\)\.o[ :]*|$(OBJ_DIR)$(XWMO_OBJ_DIR)/$*.o $@: \\\n |g' $@

$(OBJ_DIR)$(XWMO_OBJ_DIR)/%.o: %.S
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_AS) $(AS) -c $(AS_ARGS) $< -o $@

$(OBJ_DIR)$(XWMO_OBJ_DIR)/%.i: %.c
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_CC) $(CC) -E $(CC_ARGS) $< -o $@

$(OBJ_DIR)$(XWMO_OBJ_DIR)/%.o.d: $(XWMO_DIR)/%.c
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_MM) $(CC) $(MM_ARGS) $(CC_ARGS) $< > $@;
	@sed -i 's|\(^.*\)\.o[ :]*|$(OBJ_DIR)$(XWMO_OBJ_DIR)/$*.o $@: \\\n |g' $@

$(OBJ_DIR)$(XWMO_OBJ_DIR)/%.o: $(XWMO_DIR)/%.c
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_CC) $(CC) -c $(CC_ARGS) $< -o $@

%.dsm: %.o
	$(SHOW_OD) $(OD) -D $< > $@

dsm: $(XWMO_DSMS)

clean:
	@$(RM) -f $(XWMO_AOBJS:.o=.o.lst)
	@$(RM) -f $(XWMO_AOBJS:.o=.dsm)
	@$(RM) -f $(XWMO_AOBJS:.o=.i)
	@$(RM) -f $(XWMO_AOBJS)
	@$(RM) -f $(XWMO_COBJS:.o=.o.lst)
	@$(RM) -f $(XWMO_COBJS:.o=.dsm)
	@$(RM) -f $(XWMO_COBJS:.o=.i)
	@$(RM) -f $(XWMO_COBJS)
	@$(RM) -f $(OBJ_DIR)$(XWMO_OBJ_DIR)/$(XWMO_NAME)
	@$(RM) -f $(XWMO_OBJS_LST)

distclean:
	$(RM) -rf $(OBJ_DIR)$(XWMO_OBJ_DIR)

.PHONY: dsm clean distclean
