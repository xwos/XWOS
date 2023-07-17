#! /bin/make -f
# @file
# @brief 编译玄武模块的Makefile
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

include $(XWOS_ARCH_DIR)/arch.mk
include $(XWOS_CPU_DIR)/cpu.mk
include $(XWOS_SOC_DIR)/soc.mk
include $(XWOS_BRD_DIR)/brd.mk
include $(XWOS_OS_DIR)/xwos.mk
include $(XWOS_BRD_DIR)/lib.mk
include $(XWBS_UTIL_MK_XWMO)
include xwbs/$(XWOS_CFG_ARCH).$(XWOS_CFG_COMPILER).rule

ifeq ($(XWOS_CFG_COMPILER),gcc)
  XWMO_CSRCS += $(XWMO_CSRCS_gcc)
  XWMO_CXXSRCS += $(XWMO_CXXSRCS_gcc)
  XWMO_CFLAGS += $(XWMO_CFLAGS_gcc)
  XWMO_CXXFLAGS += $(XWMO_CXXFLAGS_gcc)
  XWMO_INCDIRS += $(XWMO_INCDIRS_gcc)
endif

ifeq ($(XWOS_CFG_COMPILER),llvm)
  XWMO_CSRCS += $(XWMO_CSRCS_llvm)
  XWMO_CXXSRCS += $(XWMO_CXXSRCS_llvm)
  XWMO_CFLAGS += $(XWMO_CFLAGS_llvm)
  XWMO_CXXFLAGS += $(XWMO_CXXFLAGS_llvm)
  XWMO_INCDIRS += $(XWMO_INCDIRS_llvm)
endif

ARCH_INCDIRS := $(addprefix $(XWOS_ARCH_DIR)/,$(ARCH_INCDIRS))
CPU_INCDIRS := $(addprefix $(XWOS_CPU_DIR)/,$(CPU_INCDIRS))
SOC_INCDIRS := $(addprefix $(XWOS_SOC_DIR)/,$(SOC_INCDIRS))
BRD_INCDIRS := $(addprefix $(XWOS_BRD_DIR)/,$(BRD_INCDIRS))

INCDIRS += $(if $(strip $(EINCDIRS)),$(addprefix -I,$(strip $(EINCDIRS))))
INCDIRS += $(if $(strip $(ARCH_INCDIRS)),$(addprefix -I,$(strip $(ARCH_INCDIRS))))
INCDIRS += $(if $(strip $(CPU_INCDIRS)),$(addprefix -I,$(strip $(CPU_INCDIRS))))
INCDIRS += $(if $(strip $(SOC_INCDIRS)),$(addprefix -I,$(strip $(SOC_INCDIRS))))
INCDIRS += $(if $(strip $(BRD_INCDIRS)),$(addprefix -I,$(strip $(BRD_INCDIRS))))

XWMO_NAME := $(call getXwmoName)
XWMO_DIR := $(call getXwmoDir)
XWMO_OBJ_DIR ?= $(XWMO_DIR)

XWMO_AOBJS := $(addprefix $(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/,$(addsuffix .o,$(basename $(XWMO_ASRCS))))
XWMO_COBJS := $(addprefix $(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/,$(addsuffix .o,$(basename $(XWMO_CSRCS))))
XWMO_CXXOBJS := $(addprefix $(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/,$(addsuffix .o,$(basename $(XWMO_CXXSRCS))))
XWMO_DSMS := $(addprefix $(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/,$(addsuffix .dsm,$(basename $(XWMO_CSRCS))))
XWMO_DSMS += $(addprefix $(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/,$(addsuffix .dsm,$(basename $(XWMO_CXXSRCS))))
XWMO_INCDIRS := $(if $(strip $(XWMO_INCDIRS)),$(addprefix -I,$(strip $(XWMO_INCDIRS))))

XWMO_LUA2HEXCSRCS := $(addprefix $(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/,$(addsuffix .c,$(XWMO_LUASRCS)))
XWMO_LUA2HEXCOBJS := $(addprefix $(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/,$(addsuffix .o,$(XWMO_LUASRCS)))

MM_ARGS = $(strip $(MMFLAGS))

AS_ARGS = $(strip $(AFLAGS) $(ARCH_AFLAGS) $(CPU_AFLAGS) $(SOC_AFLAGS) \
                  $(BRD_AFLAGS) $(XWMO_AFLAGS) $(INCDIRS) $(XWMO_INCDIRS))

CC_ARGS = $(strip $(CFLAGS) $(ARCH_CFLAGS) $(CPU_CFLAGS) $(SOC_CFLAGS) \
                  $(BRD_CFLAGS) $(XWMO_CFLAGS) $(INCDIRS) $(XWMO_INCDIRS))

CXX_ARGS = $(strip $(CXXFLAGS) $(ARCH_CXXFLAGS) $(CPU_CXXFLAGS) $(SOC_CXXFLAGS) \
                   $(BRD_CXXFLAGS) $(XWMO_CXXFLAGS) $(INCDIRS) $(XWMO_INCDIRS))

XWMO_OBJS_LST := $(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/objs.txt

$(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/$(XWMO_NAME): $(XWMO_AOBJS) $(XWMO_COBJS) \
  $(XWMO_LUA2HEXCOBJS) $(XWMO_CXXOBJS) $(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(file > $(XWMO_OBJS_LST),$(XWMO_AOBJS) $(XWMO_COBJS) $(XWMO_LUA2HEXCOBJS) $(XWMO_CXXOBJS))
	$(SHOW_AR) $(AR) rcs $@ @$(XWMO_OBJS_LST)

$(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR):
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

$(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/%.s: %.S
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_AS) $(AS) -E $(AS_ARGS) $< -o $@

$(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/%.o.d: %.S
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_MM) $(CC) $(MM_ARGS) $(AS_ARGS) $< > $@;
	@sed -i 's|\(^.*\)\.o[ :]*|$(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/$*.o $@: \\\n |g' $@

$(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/%.o: %.S
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_AS) $(AS) -c $(AS_ARGS) $< -o $@

$(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/%.o: $(XWMO_DIR)/%.s
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_AS) $(AS) -c $(AS_ARGS) $< -o $@

$(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/%.o.d: $(XWMO_DIR)/%.c
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_MM) $(CC) $(MM_ARGS) $(CC_ARGS) $< > $@;
	@sed -i 's|\(^.*\)\.o[ :]*|$(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/$*.o $@: \\\n |g' $@

$(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/%.lua.c: $(XWMO_DIR)/%.lua
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_LUA2HEX) $(LUA2HEX) -o $@ $<

$(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/%.lua.o: $(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/%.lua.c
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_CC) $(CC) -c $(CC_ARGS) $< -o $@

$(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/%.o: $(XWMO_DIR)/%.c
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_CC) $(CC) -c $(CC_ARGS) $< -o $@

$(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/%.o.d: $(XWMO_DIR)/%.cpp
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_MM) $(CXX) $(MM_ARGS) $(CXX_ARGS) $< > $@;
	@sed -i 's|\(^.*\)\.o[ :]*|$(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/$*.o $@: \\\n |g' $@

$(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/%.o: $(XWMO_DIR)/%.cpp
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_CXX) $(CXX) -c $(CXX_ARGS) $< -o $@

$(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/%.o.d: $(XWMO_DIR)/%.cxx
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_MM) $(CXX) $(MM_ARGS) $(CXX_ARGS) $< > $@;
	@sed -i 's|\(^.*\)\.o[ :]*|$(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/$*.o $@: \\\n |g' $@

$(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/%.o: $(XWMO_DIR)/%.cxx
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
	@$(RM) -f $(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)/$(XWMO_NAME)
	@$(RM) -f $(XWMO_OBJS_LST)

distclean:
	$(RM) -rf $(XWOS_OBJ_DIR)/$(XWMO_OBJ_DIR)

.PHONY: dsm clean distclean
