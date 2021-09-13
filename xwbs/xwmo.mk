#! /bin/make -f
# @file
# @brief 编译玄武模块的Makefile
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

include $(XuanWuOS_ARCH_DIR)/arch.mk
include $(XuanWuOS_CPU_DIR)/cpu.mk
include $(XuanWuOS_SOC_DIR)/soc.mk
include $(XuanWuOS_BDL_DIR)/bdl.mk
include $(XuanWuOS_XWOS_DIR)/xwos.mk
include $(XWBS_UTIL_MK_XWMO)
include xwbs/$(XuanWuOS_CFG_ARCH).$(XuanWuOS_CFG_COMPILER).rule
include $(XuanWuOS_BRD_DIR)/cfg/lib.mk

ifeq ($(XuanWuOS_CFG_COMPILER),gcc)
  XWMO_CSRCS += $(XWMO_CSRCS_gcc)
  XWMO_CXXSRCS += $(XWMO_CXXSRCS_gcc)
  XWMO_CFLAGS += $(XWMO_CFLAGS_gcc)
  XWMO_CXXFLAGS += $(XWMO_CXXFLAGS_gcc)
  XWMO_INCDIRS += $(XWMO_INCDIRS_gcc)
endif

ifeq ($(XuanWuOS_CFG_COMPILER),llvm)
  XWMO_CSRCS += $(XWMO_CSRCS_llvm)
  XWMO_CXXSRCS += $(XWMO_CXXSRCS_llvm)
  XWMO_CFLAGS += $(XWMO_CFLAGS_llvm)
  XWMO_CXXFLAGS += $(XWMO_CXXFLAGS_llvm)
  XWMO_INCDIRS += $(XWMO_INCDIRS_llvm)
endif

ARCH_INCDIRS := $(addprefix $(XuanWuOS_ARCH_DIR)/,$(ARCH_INCDIRS))
CPU_INCDIRS := $(addprefix $(XuanWuOS_CPU_DIR)/,$(CPU_INCDIRS))
SOC_INCDIRS := $(addprefix $(XuanWuOS_SOC_DIR)/,$(SOC_INCDIRS))
BDL_INCDIRS := $(addprefix $(XuanWuOS_BDL_DIR)/,$(BDL_INCDIRS))

INCDIRS += $(if $(strip $(EINCDIRS)),$(addprefix -I,$(strip $(EINCDIRS))))
INCDIRS += $(if $(strip $(ARCH_INCDIRS)),$(addprefix -I,$(strip $(ARCH_INCDIRS))))
INCDIRS += $(if $(strip $(CPU_INCDIRS)),$(addprefix -I,$(strip $(CPU_INCDIRS))))
INCDIRS += $(if $(strip $(SOC_INCDIRS)),$(addprefix -I,$(strip $(SOC_INCDIRS))))
INCDIRS += $(if $(strip $(BDL_INCDIRS)),$(addprefix -I,$(strip $(BDL_INCDIRS))))

XWMO_NAME := $(call getXwmoName)
XWMO_DIR := $(call getXwmoDir)
XWMO_OBJ_DIR ?= $(XWMO_DIR)

XWMO_AOBJS := $(addprefix $(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/,$(addsuffix .o,$(basename $(XWMO_ASRCS))))
XWMO_COBJS := $(addprefix $(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/,$(addsuffix .o,$(basename $(XWMO_CSRCS))))
XWMO_CXXOBJS := $(addprefix $(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/,$(addsuffix .o,$(basename $(XWMO_CXXSRCS))))
XWMO_DSMS := $(addprefix $(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/,$(addsuffix .dsm,$(basename $(XWMO_CSRCS))))
XWMO_DSMS += $(addprefix $(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/,$(addsuffix .dsm,$(basename $(XWMO_CXXSRCS))))
XWMO_INCDIRS := $(if $(strip $(XWMO_INCDIRS)),$(addprefix -I,$(strip $(XWMO_INCDIRS))))

XWMO_LUA2HEXCSRCS := $(addprefix $(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/,$(addsuffix .c,$(XWMO_LUASRCS)))
XWMO_LUA2HEXCOBJS := $(addprefix $(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/,$(addsuffix .o,$(XWMO_LUASRCS)))

MM_ARGS = $(strip $(MMFLAGS))

AS_ARGS = $(strip $(AFLAGS) $(ARCH_AFLAGS) $(CPU_AFLAGS) $(SOC_AFLAGS) \
                  $(BDL_AFLAGS) $(XWMO_AFLAGS) $(INCDIRS) $(XWMO_INCDIRS))

CC_ARGS = $(strip $(CFLAGS) $(ARCH_CFLAGS) $(CPU_CFLAGS) $(SOC_CFLAGS) \
                  $(BDL_CFLAGS) $(XWMO_CFLAGS) $(INCDIRS) $(XWMO_INCDIRS))

CXX_ARGS = $(strip $(CXXFLAGS) $(ARCH_CXXFLAGS) $(CPU_CXXFLAGS) $(SOC_CXXFLAGS) \
                   $(BDL_CXXFLAGS) $(XWMO_CXXFLAGS) $(INCDIRS) $(XWMO_INCDIRS))

XWMO_OBJS_LST := $(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/objs.txt

$(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/$(XWMO_NAME): $(XWMO_AOBJS) $(XWMO_COBJS) \
  $(XWMO_LUA2HEXCOBJS) $(XWMO_CXXOBJS) $(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(file > $(XWMO_OBJS_LST),$(XWMO_AOBJS) $(XWMO_COBJS) $(XWMO_LUA2HEXCOBJS) $(XWMO_CXXOBJS))
	$(SHOW_AR) $(AR) rcs $@ @$(XWMO_OBJS_LST)

$(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR):
	@[ ! -d $@ ] && mkdir -p $@ || true

$(XWMO_LUA2HEXCOBJS): $(XWMO_LUA2HEXCSRCS)

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

ifneq ($(XWMO_CXXOBJS),)
  ifneq ($(NODEP),y)
    -include $(XWMO_CXXOBJS:.o=.o.d)
  endif
endif

$(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/%.s: %.S
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_AS) $(AS) -E $(AS_ARGS) $< -o $@

$(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/%.o.d: %.S
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_MM) $(CC) $(MM_ARGS) $(AS_ARGS) $< > $@;
	@sed -i 's|\(^.*\)\.o[ :]*|$(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/$*.o $@: \\\n |g' $@

$(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/%.o: %.S
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_AS) $(AS) -c $(AS_ARGS) $< -o $@

$(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/%.o.d: $(XWMO_DIR)/%.c
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_MM) $(CC) $(MM_ARGS) $(CC_ARGS) $< > $@;
	@sed -i 's|\(^.*\)\.o[ :]*|$(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/$*.o $@: \\\n |g' $@

$(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/%.lua.c: $(XWMO_DIR)/%.lua
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_LUA2HEX) $(LUA2HEX) -o $@ $<

$(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/%.lua.o: $(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/%.lua.c
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_CC) $(CC) -c $(CC_ARGS) $< -o $@

$(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/%.o: $(XWMO_DIR)/%.c
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_CC) $(CC) -c $(CC_ARGS) $< -o $@

$(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/%.o.d: $(XWMO_DIR)/%.cpp
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_MM) $(CXX) $(MM_ARGS) $(CXX_ARGS) $< > $@;
	@sed -i 's|\(^.*\)\.o[ :]*|$(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/$*.o $@: \\\n |g' $@

$(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/%.o: $(XWMO_DIR)/%.cpp
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_CXX) $(CXX) -c $(CXX_ARGS) $< -o $@

$(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/%.o.d: $(XWMO_DIR)/%.cxx
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_MM) $(CXX) $(MM_ARGS) $(CXX_ARGS) $< > $@;
	@sed -i 's|\(^.*\)\.o[ :]*|$(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/$*.o $@: \\\n |g' $@

$(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/%.o: $(XWMO_DIR)/%.cxx
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_CXX) $(CXX) -c $(CXX_ARGS) $< -o $@

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
	@$(RM) -f $(XWMO_LUA2HEXCOBJS:.o=.o.lst)
	@$(RM) -f $(XWMO_LUA2HEXCOBJS:.o=.dsm)
	@$(RM) -f $(XWMO_LUA2HEXCOBJS:.o=.i)
	@$(RM) -f $(XWMO_LUA2HEXCOBJS)
	@$(RM) -f $(XWMO_LUA2HEXCSRCS)
	@$(RM) -f $(XWMO_CXXOBJS:.o=.o.lst)
	@$(RM) -f $(XWMO_CXXOBJS:.o=.dsm)
	@$(RM) -f $(XWMO_CXXOBJS:.o=.i)
	@$(RM) -f $(XWMO_CXXOBJS)
	@$(RM) -f $(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/$(XWMO_NAME)
	@$(RM) -f $(XWMO_OBJS_LST)

distclean:
	$(RM) -rf $(XuanWuOS_OBJ_DIR)/$(XWMO_OBJ_DIR)

.PHONY: dsm clean distclean
