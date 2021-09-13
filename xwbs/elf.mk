#! /bin/make -f
# @file
# @brief 编译ELF的Makefile
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

include $(XuanWuOS_WKSPC_DIR)/XuanWuOS.cfg
include $(XuanWuOS_ARCH_DIR)/arch.mk
include $(XuanWuOS_CPU_DIR)/cpu.mk
include $(XuanWuOS_SOC_DIR)/soc.mk
include $(XuanWuOS_BDL_DIR)/bdl.mk
include $(XuanWuOS_XWOS_DIR)/xwos.mk
include $(XuanWuOS_BRD_DIR)/cfg/lib.mk
include $(XWBS_UTIL_MK_XWMO)
include xwbs/$(XuanWuOS_CFG_ARCH).$(XuanWuOS_CFG_COMPILER).rule

ARCH_INCDIRS := $(addprefix $(XuanWuOS_ARCH_DIR)/,$(ARCH_INCDIRS))
ARCH_CSRCS := $(addprefix $(XuanWuOS_ARCH_DIR)/,$(ARCH_CSRCS))
ARCH_ASRCS := $(addprefix $(XuanWuOS_ARCH_DIR)/,$(ARCH_ASRCS))
ARCH_COBJS := $(addprefix $(XuanWuOS_OBJ_DIR)/,$(addsuffix .o,$(basename $(ARCH_CSRCS))))
ARCH_AOBJS := $(addprefix $(XuanWuOS_OBJ_DIR)/,$(addsuffix .o,$(basename $(ARCH_ASRCS))))
ARCH_OBJS := $(ARCH_COBJS) $(ARCH_AOBJS)
ARCH_OBJS_LST := $(XuanWuOS_OBJ_DIR)/$(XuanWuOS_ARCH_DIR)/arch-objs.txt
ARCH_LIB := $(XuanWuOS_OBJ_DIR)/$(XuanWuOS_ARCH_DIR)/arch.a

CPU_INCDIRS := $(addprefix $(XuanWuOS_CPU_DIR)/,$(CPU_INCDIRS))
CPU_CSRCS := $(addprefix $(XuanWuOS_CPU_DIR)/,$(CPU_CSRCS))
CPU_ASRCS := $(addprefix $(XuanWuOS_CPU_DIR)/,$(CPU_ASRCS))
CPU_COBJS := $(addprefix $(XuanWuOS_OBJ_DIR)/,$(addsuffix .o,$(basename $(CPU_CSRCS))))
CPU_AOBJS := $(addprefix $(XuanWuOS_OBJ_DIR)/,$(addsuffix .o,$(basename $(CPU_ASRCS))))
CPU_OBJS := $(CPU_COBJS) $(CPU_AOBJS)
CPU_OBJS_LST := $(XuanWuOS_OBJ_DIR)/$(XuanWuOS_CPU_DIR)/cpu-objs.txt
CPU_LIB := $(XuanWuOS_OBJ_DIR)/$(XuanWuOS_CPU_DIR)/cpu.a

SOC_INCDIRS := $(addprefix $(XuanWuOS_SOC_DIR)/,$(SOC_INCDIRS))
SOC_CSRCS := $(addprefix $(XuanWuOS_SOC_DIR)/,$(SOC_CSRCS))
SOC_ASRCS := $(addprefix $(XuanWuOS_SOC_DIR)/,$(SOC_ASRCS))
SOC_COBJS := $(addprefix $(XuanWuOS_OBJ_DIR)/,$(addsuffix .o,$(basename $(SOC_CSRCS))))
SOC_AOBJS := $(addprefix $(XuanWuOS_OBJ_DIR)/,$(addsuffix .o,$(basename $(SOC_ASRCS))))
SOC_OBJS := $(SOC_COBJS) $(SOC_AOBJS)
SOC_OBJS_LST := $(XuanWuOS_OBJ_DIR)/$(XuanWuOS_SOC_DIR)/soc-objs.txt
SOC_LIB := $(XuanWuOS_OBJ_DIR)/$(XuanWuOS_SOC_DIR)/soc.a

BDL_INCDIRS := $(addprefix $(XuanWuOS_BDL_DIR)/,$(BDL_INCDIRS))
BDL_CSRCS := $(addprefix $(XuanWuOS_BDL_DIR)/,$(BDL_CSRCS))
BDL_ASRCS := $(addprefix $(XuanWuOS_BDL_DIR)/,$(BDL_ASRCS))
BDL_COBJS := $(addprefix $(XuanWuOS_OBJ_DIR)/,$(addsuffix .o,$(basename $(BDL_CSRCS))))
BDL_AOBJS := $(addprefix $(XuanWuOS_OBJ_DIR)/,$(addsuffix .o,$(basename $(BDL_ASRCS))))
BDL_OBJS := $(BDL_COBJS) $(BDL_AOBJS)
BDL_OBJS_LST := $(XuanWuOS_OBJ_DIR)/$(XuanWuOS_BDL_DIR)/bdl-objs.txt
BDL_LIB := $(XuanWuOS_OBJ_DIR)/$(XuanWuOS_BDL_DIR)/bdl.a

XWOS_CSRCS := $(addprefix $(XuanWuOS_XWOS_DIR)/,$(XWOS_CSRCS))
XWOS_ASRCS := $(addprefix $(XuanWuOS_XWOS_DIR)/,$(XWOS_ASRCS))
XWOS_COBJS := $(addprefix $(XuanWuOS_OBJ_DIR)/,$(addsuffix .o,$(basename $(XWOS_CSRCS))))
XWOS_AOBJS := $(addprefix $(XuanWuOS_OBJ_DIR)/,$(addsuffix .o,$(basename $(XWOS_ASRCS))))
XWOS_OBJS := $(XWOS_COBJS) $(XWOS_AOBJS)
XWOS_OBJS_LST := $(XuanWuOS_OBJ_DIR)/$(XuanWuOS_XWOS_DIR)/xwos-objs.txt
XWOS_LIB := $(XuanWuOS_OBJ_DIR)/$(XuanWuOS_XWOS_DIR)/xwos.a

LIBS :=
LIBS += $(XWOS_LIB)
LIBS += $(BDL_LIB)
LIBS += $(SOC_LIB)
LIBS += $(CPU_LIB)
LIBS += $(ARCH_LIB)

LIB_OBJS :=
LIB_OBJS += $(XWOS_OBJS)
LIB_OBJS += $(BDL_OBJS)
LIB_OBJS += $(SOC_OBJS)
LIB_OBJS += $(CPU_OBJS)
LIB_OBJS += $(ARCH_OBJS)

INCDIRS += $(if $(strip $(EINCDIRS)),$(addprefix -I,$(strip $(EINCDIRS))))
INCDIRS += $(if $(strip $(ARCH_INCDIRS)),$(addprefix -I,$(strip $(ARCH_INCDIRS))))
INCDIRS += $(if $(strip $(CPU_INCDIRS)),$(addprefix -I,$(strip $(CPU_INCDIRS))))
INCDIRS += $(if $(strip $(SOC_INCDIRS)),$(addprefix -I,$(strip $(SOC_INCDIRS))))
INCDIRS += $(if $(strip $(BDL_INCDIRS)),$(addprefix -I,$(strip $(BDL_INCDIRS))))

DSMS := $(addsuffix .dsm,$(basename $(OBJS)))
DSMS += $(addsuffix .dsm,$(basename $(LIB_OBJS)))

LD_OBJS = $(strip -Wl,--whole-archive -Wl,--start-group \
                  $(OEM) $(BM) $(XWAM) $(XWEM) $(XWCD) $(XWMD) \
                  $(XWOS_LIB) $(XWOS_EOBJS) \
                  $(BDL_LIB) $(BDL_EOBJS) \
                  $(SOC_LIB) $(SOC_EOBJS) \
                  $(CPU_LIB) $(CPU_EOBJS) \
                  $(ARCH_LIB) $(ARCH_EOBJS) \
                  -Wl,--end-group -Wl,--no-whole-archive \
                  -Wl,--start-group $(ELIBS) -Wl,--end-group)
LD_OBJS_LST := $(XuanWuOS_OBJ_DIR)/$(TARGET)-objs.txt

MM_ARGS = $(strip $(MMFLAGS))

AS_ARGS = $(strip $(INCDIRS) $(AFLAGS) $(ARCH_AFLAGS) $(CPU_AFLAGS) \
                  $(SOC_AFLAGS) $(BDL_AFLAGS))

CC_ARGS = $(strip $(INCDIRS) $(CFLAGS) $(ARCH_CFLAGS) $(CPU_CFLAGS) \
                  $(SOC_CFLAGS) $(BDL_CFLAGS))

CXX_ARGS = $(strip $(INCDIRS) $(CXXFLAGS) $(ARCH_CXXFLAGS) $(CPU_CXXFLAGS) \
                   $(SOC_CXXFLAGS) $(BDL_CXXFLAGS))

LD_ARGS = $(strip $(LDFLAGS) $(ARCH_LDFLAGS) $(CPU_LDFLAGS) \
                  $(SOC_LDFLAGS) $(BDL_LDFLAGS))

all: $(LIB_OBJS) $(LIBS) $(XWMD) $(XWCD) $(XWEM) $(XWAM) $(BM) $(OEM) \
    $(XuanWuOS_WKSPC_DIR)/$(TARGET).elf \
    $(XuanWuOS_WKSPC_DIR)/$(TARGET).hex \
    $(XuanWuOS_WKSPC_DIR)/$(TARGET).smot \
    $(XuanWuOS_WKSPC_DIR)/$(TARGET).bin

ifeq ($(XuanWuOS_CFG_XWMD),y)
  include xwbs/xwmd.mk
endif

include xwbs/xwcd.mk
include xwbs/bm.mk

ifeq ($(XuanWuOS_CFG_XWEM),y)
  include xwbs/xwem.mk
endif

ifeq ($(XuanWuOS_CFG_XWAM),y)
  include xwbs/xwam.mk
endif

ifneq ($(XuanWuOS_OEM_DIR),)
  include xwbs/oem.mk
endif

ifneq ($(NODEP),y)
  -include $(LIB_OBJS:.o=.o.d)
endif

$(ARCH_LIB): $(ARCH_OBJS)
	$(file > $(ARCH_OBJS_LST),$(ARCH_OBJS))
	$(SHOW_AR) $(AR) rcs $@ @$(ARCH_OBJS_LST)

$(CPU_LIB): $(CPU_OBJS)
	$(file > $(CPU_OBJS_LST),$(CPU_OBJS))
	$(SHOW_AR) $(AR) rcs $@ @$(CPU_OBJS_LST)

$(SOC_LIB): $(SOC_OBJS)
	$(file > $(SOC_OBJS_LST),$(SOC_OBJS))
	$(SHOW_AR) $(AR) rcs $@ @$(SOC_OBJS_LST)

$(BDL_LIB): $(BDL_OBJS)
	$(file > $(BDL_OBJS_LST),$(BDL_OBJS))
	$(SHOW_AR) $(AR) rcs $@ @$(BDL_OBJS_LST)

$(XWOS_LIB): $(XWOS_OBJS)
	$(file > $(XWOS_OBJS_LST),$(XWOS_OBJS))
	$(SHOW_AR) $(AR) rcs $@ @$(XWOS_OBJS_LST)

dsm: $(DSMS) $(XWMD_DSM) $(XWCD_DSM) $(XWEM_DSM) $(XWAM_DSM) $(BM_DSM) $(OEM_DSM) \
  $(XuanWuOS_WKSPC_DIR)/$(TARGET).dsm

$(XuanWuOS_OBJ_DIR)/%.s: %.S
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_AS) $(AS) -E $(AS_ARGS) $< -o $@

$(XuanWuOS_OBJ_DIR)/%.o.d: %.S
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_MM) $(CC) $(MM_ARGS) $(AS_ARGS) $< > $@;
	@sed -i 's|\(^.*\)\.o[ :]*|$(XuanWuOS_OBJ_DIR)/$*.o $@: \\\n |g' $@

$(XuanWuOS_OBJ_DIR)/%.o: %.S
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_AS) $(AS) -c $(AS_ARGS) $< -o $@

$(XuanWuOS_OBJ_DIR)/%.i: %.c
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_CC) $(CC) -E $(CC_ARGS) $< -o $@

$(XuanWuOS_OBJ_DIR)/%.o.d: %.c
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_MM) $(CC) $(MM_ARGS) $(CC_ARGS) $< > $@;
	@sed -i 's|\(^.*\)\.o[ :]*|$(XuanWuOS_OBJ_DIR)/$*.o $@: \\\n |g' $@

$(XuanWuOS_OBJ_DIR)/%.o: %.c
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_CC) $(CC) -c $(CC_ARGS) $< -o $@

$(XuanWuOS_OBJ_DIR)/%.i: %.cpp
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_CXX) $(CXX) -E $(CXX_ARGS) $< -o $@

$(XuanWuOS_OBJ_DIR)/%.o.d: %.cpp
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_MM) $(CXX) $(MM_ARGS) $(CXX_ARGS) $< > $@;
	@sed -i 's|\(^.*\)\.o[ :]*|$(XuanWuOS_OBJ_DIR)/$*.o $@: \\\n |g' $@

$(XuanWuOS_OBJ_DIR)/%.o: %.cpp
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_CXX) $(CXX) -c $(CXX_ARGS) $< -o $@

$(XuanWuOS_OBJ_DIR)/%.i: %.cxx
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_CXX) $(CXX) -E $(CXX_ARGS) $< -o $@

$(XuanWuOS_OBJ_DIR)/%.o.d: %.cxx
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_MM) $(CXX) $(MM_ARGS) $(CXX_ARGS) $< > $@;
	@sed -i 's|\(^.*\)\.o[ :]*|$(XuanWuOS_OBJ_DIR)/$*.o $@: \\\n |g' $@

$(XuanWuOS_OBJ_DIR)/%.o: %.cxx
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_CXX) $(CXX) -c $(CXX_ARGS) $< -o $@

$(XuanWuOS_WKSPC_DIR)/$(TARGET).elf: $(LIB_OBJS) $(LIBS) \
  $(XWMD) $(XWCD) $(XWEM) $(XWAM) $(BM) $(OEM)
	$(file > $(LD_OBJS_LST),$(LD_OBJS))
	$(SHOW_LD) $(LD) $(LD_ARGS) @$(LD_OBJS_LST) -o $@
	$(SHOW_SIZE) $(SIZE) $@

%.hex: %.elf
	$(SHOW_OBJCOPY) $(OBJCOPY) -S -O ihex $< $@

%.smot: %.elf
	$(SHOW_OBJCOPY) $(OBJCOPY) -S -O srec $< $@

%.bin: %.elf
	$(SHOW_OBJCOPY) $(OBJCOPY) -S -O binary $< $@

$(XuanWuOS_WKSPC_DIR)/$(TARGET).dsm: $(XuanWuOS_WKSPC_DIR)/$(TARGET).elf
	$(SHOW_OD) $(OD) -D $< > $@

%.dsm: %.o
	$(SHOW_OD) $(OD) -D $< > $@

clean: $(XWMD_CLEAN) $(XWCD_CLEAN) $(XWEM_CLEAN) $(XWAM_CLEAN) $(BM_CLEAN) $(OEM_CLEAN)
	@echo "clean ..."
	@$(RM) -f $(XWOS_LIB)
	@$(RM) -f $(XWOS_OBJS)
	@$(RM) -f $(XWOS_OBJS:.o=.o.lst)
	@$(RM) -f $(XWOS_OBJS:.o=.dsm)
	@$(RM) -f $(XWOS_OBJS:.o=.i)
	@$(RM) -f $(XWOS_OBJS_LST)

	@$(RM) -f $(BDL_LIB)
	@$(RM) -f $(BDL_OBJS)
	@$(RM) -f $(BDL_OBJS:.o=.o.lst)
	@$(RM) -f $(BDL_OBJS:.o=.dsm)
	@$(RM) -f $(BDL_OBJS:.o=.i)
	@$(RM) -f $(BDL_OBJS_LST)

	@$(RM) -f $(SOC_LIB)
	@$(RM) -f $(SOC_OBJS)
	@$(RM) -f $(SOC_OBJS:.o=.o.lst)
	@$(RM) -f $(SOC_OBJS:.o=.dsm)
	@$(RM) -f $(SOC_OBJS:.o=.i)
	@$(RM) -f $(SOC_OBJS_LST)

	@$(RM) -f $(CPU_LIB)
	@$(RM) -f $(CPU_OBJS)
	@$(RM) -f $(CPU_OBJS:.o=.o.lst)
	@$(RM) -f $(CPU_OBJS:.o=.dsm)
	@$(RM) -f $(CPU_OBJS:.o=.i)
	@$(RM) -f $(CPU_OBJS_LST)

	@$(RM) -f $(ARCH_LIB)
	@$(RM) -f $(ARCH_OBJS)
	@$(RM) -f $(ARCH_OBJS:.o=.o.lst)
	@$(RM) -f $(ARCH_OBJS:.o=.dsm)
	@$(RM) -f $(ARCH_OBJS:.o=.i)
	@$(RM) -f $(ARCH_OBJS_LST)

	@$(RM) -f $(XuanWuOS_WKSPC_DIR)/$(TARGET).elf
	@$(RM) -f $(XuanWuOS_WKSPC_DIR)/$(TARGET).dsm
	@$(RM) -f $(XuanWuOS_WKSPC_DIR)/$(TARGET).map
	@$(RM) -f $(XuanWuOS_WKSPC_DIR)/$(TARGET).hex
	@$(RM) -f $(XuanWuOS_WKSPC_DIR)/$(TARGET).smot
	@$(RM) -f $(XuanWuOS_WKSPC_DIR)/$(TARGET).bin
	@$(RM) -rf doc/doxygen

distclean: $(XWMD_DISTCLEAN) $(XWCD_DISTCLEAN) $(XWEM_DISTCLEAN) $(XWAM_DISTCLEAN) $(BM_DISTCLEAN) $(OEM_DISTCLEAN)
	@echo "distclean ..."
	@$(RM) -rf $(XuanWuOS_OBJ_DIR)/$(XuanWuOS_XWOS_DIR)
	@$(RM) -rf $(XuanWuOS_OBJ_DIR)/$(XuanWuOS_BDL_DIR)
	@$(RM) -rf $(XuanWuOS_OBJ_DIR)/$(XuanWuOS_SOC_DIR)
	@$(RM) -rf $(XuanWuOS_OBJ_DIR)/$(XuanWuOS_CPU_DIR)
	@$(RM) -rf $(XuanWuOS_OBJ_DIR)/$(XuanWuOS_ARCH_DIR)

	@$(RM) -f $(XuanWuOS_WKSPC_DIR)/XuanWuOS.cfg
	@$(RM) -f $(XuanWuOS_WKSPC_DIR)/$(TARGET).elf
	@$(RM) -f $(XuanWuOS_WKSPC_DIR)/$(TARGET).dsm
	@$(RM) -f $(XuanWuOS_WKSPC_DIR)/$(TARGET).map
	@$(RM) -f $(XuanWuOS_WKSPC_DIR)/$(TARGET).hex
	@$(RM) -f $(XuanWuOS_WKSPC_DIR)/$(TARGET).smot
	@$(RM) -f $(XuanWuOS_WKSPC_DIR)/$(TARGET).bin
	@$(RM) -rf doc/doxygen

.PHONY: all clean distclean
.PHONY: $(XWMD) $(XWMD_DSM) $(XWMD_CLEAN) $(XWMD_DISTCLEAN)
.PHONY: $(XWCD) $(XWCD_DSM) $(XWCD_CLEAN) $(XWCD_DISTCLEAN)
.PHONY: $(XWEM) $(XWEM_DSM) $(XWEM_CLEAN) $(XWEM_DISTCLEAN)
.PHONY: $(XWAM) $(XWAM_DSM) $(XWAM_CLEAN) $(XWAM_DISTCLEAN)
.PHONY: $(BM) $(BM_DSM) $(BM_CLEAN) $(BM_DISTCLEAN)
.PHONY: $(OEM) $(OEM_DSM) $(OEM_CLEAN) $(OEM_DISTCLEAN)
