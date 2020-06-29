#! /bin/make -f
# @file
# @brief Makefile模板 (eppc-e200zx-codewarrior)
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

include $(XWOS_WKSPC_DIR)/XuanWuOS.cfg
include xwbs/$(XuanWuOS_CFG_MK_RULE)
include $(XWOS_BRD_DIR)/cfg/lib.mk

ARCH_CSRCS := $(addprefix $(XWOS_ARCH_DIR)/,$(ARCH_CSRCS))
ARCH_ASRCS := $(addprefix $(XWOS_ARCH_DIR)/,$(ARCH_ASRCS))
ARCH_COBJS := $(addprefix $(OBJ_DIR),$(addsuffix .o,$(basename $(ARCH_CSRCS))))
ARCH_AOBJS := $(addprefix $(OBJ_DIR),$(addsuffix .o,$(basename $(ARCH_ASRCS))))
ARCH_OBJS := $(ARCH_COBJS) $(ARCH_AOBJS)
ARCH_LIB := $(OBJ_DIR)$(XWOS_ARCH_DIR)/arch.a

CPU_CSRCS := $(addprefix $(XWOS_CPU_DIR)/,$(CPU_CSRCS))
CPU_ASRCS := $(addprefix $(XWOS_CPU_DIR)/,$(CPU_ASRCS))
CPU_COBJS := $(addprefix $(OBJ_DIR),$(addsuffix .o,$(basename $(CPU_CSRCS))))
CPU_AOBJS := $(addprefix $(OBJ_DIR),$(addsuffix .o,$(basename $(CPU_ASRCS))))
CPU_OBJS := $(CPU_COBJS) $(CPU_AOBJS)
CPU_LIB := $(OBJ_DIR)$(XWOS_CPU_DIR)/cpu.a

SOC_CSRCS := $(addprefix $(XWOS_SOC_DIR)/,$(SOC_CSRCS))
SOC_ASRCS := $(addprefix $(XWOS_SOC_DIR)/,$(SOC_ASRCS))
SOC_COBJS := $(addprefix $(OBJ_DIR),$(addsuffix .o,$(basename $(SOC_CSRCS))))
SOC_AOBJS := $(addprefix $(OBJ_DIR),$(addsuffix .o,$(basename $(SOC_ASRCS))))
SOC_OBJS := $(SOC_COBJS) $(SOC_AOBJS)
SOC_LIB := $(OBJ_DIR)$(XWOS_SOC_DIR)/soc.a

BDL_CSRCS := $(addprefix $(XWOS_BDL_DIR)/,$(BDL_CSRCS))
BDL_ASRCS := $(addprefix $(XWOS_BDL_DIR)/,$(BDL_ASRCS))
BDL_COBJS := $(addprefix $(OBJ_DIR),$(addsuffix .o,$(basename $(BDL_CSRCS))))
BDL_AOBJS := $(addprefix $(OBJ_DIR),$(addsuffix .o,$(basename $(BDL_ASRCS))))
BDL_OBJS := $(BDL_COBJS) $(BDL_AOBJS)
BDL_LIB := $(OBJ_DIR)$(XWOS_BDL_DIR)/bdl.a

XWOS_CSRCS := $(addprefix $(XWOS_KN_DIR)/,$(XWOS_CSRCS))
XWOS_ASRCS := $(addprefix $(XWOS_KN_DIR)/,$(XWOS_ASRCS))
XWOS_COBJS := $(addprefix $(OBJ_DIR),$(addsuffix .o,$(basename $(XWOS_CSRCS))))
XWOS_AOBJS := $(addprefix $(OBJ_DIR),$(addsuffix .o,$(basename $(XWOS_ASRCS))))
XWOS_OBJS := $(XWOS_COBJS) $(XWOS_AOBJS)
XWOS_LIB := $(OBJ_DIR)$(XWOS_KN_DIR)/xwos.a

LIBS :=
LIBS += $(XWOS_LIB)
LIBS += $(BDL_LIB)
LIBS += $(SOC_LIB)
LIBS += $(CPU_LIB)
LIBS += $(ARCH_LIB)

ifeq ($(CWMCUEPPC_LICENSE),)
    LIBS := $(addprefix @@,$(LIBS))
endif

OBJS :=
OBJS += $(XWOS_OBJS)
OBJS += $(BDL_OBJS)
OBJS += $(SOC_OBJS)
OBJS += $(CPU_OBJS)
OBJS += $(ARCH_OBJS)

EOBJS :=
EOBJS += $(XWOS_EOBJS)
EOBJS += $(BDL_EOBJS)
EOBJS += $(SOC_EOBJS)
EOBJS += $(CPU_EOBJS)
EOBJS += $(ARCH_EOBJS)

INCDIRS += $(if $(strip $(EINCDIRS)),$(addprefix -I,$(strip $(EINCDIRS))))

DSMS += $(addsuffix .dsm,$(basename $(OBJS)))

AS_ARGS = $(strip -c $(AFLAGS) $(ARCH_AFLAGS) $(CPU_AFLAGS) $(BDL_AFLAGS) $(INCDIRS))
CC_ARGS = $(strip -c $(CFLAGS) $(ARCH_CFLAGS) $(CPU_CFLAGS) $(BDL_CFLAGS) $(INCDIRS))
LD_ARGS = $(strip $(XWOEM_LIBS) $(XWEM_LIBS) $(XWBM_LIBS) $(XWPP_LIBS) $(XWMD_LIBS) $(LIBS) \
                  $(EOBJS) $(ELIBS) \
                  $(LDFLAGS) $(ARCH_LDFLAGS) $(CPU_LDFLAGS) $(BDL_LDFLAGS))

all: $(OBJS) libs $(XWMD) $(XWPP) $(XWBM) $(XWEM) $(XWOEM) $(XWOS_WKSPC_DIR)/$(TARGET).elf \
     $(XWOS_WKSPC_DIR)/$(TARGET).mot $(XWOS_WKSPC_DIR)/$(TARGET).bin

ifeq ($(XuanWuOS_CFG_XWMD),y)
    include xwbs/xwmd.mk
    ifeq ($(CWMCUEPPC_LICENSE),)
        XWMD_LIBS = $(addprefix @@,$(XWMD))
    else
        XWMD_LIBS = $(XWMD)
    endif
endif

include xwbs/xwpp.mk
ifeq ($(CWMCUEPPC_LICENSE),)
    XWPP_LIBS = $(addprefix @@,$(XWPP))
else
    XWPP_LIBS = $(XWPP)
endif

include xwbs/xwbm.mk
ifeq ($(CWMCUEPPC_LICENSE),)
    XWBM_LIBS = $(addprefix @@,$(XWBM))
else
    XWBM_LIBS = $(XWBM)
endif

ifeq ($(XuanWuOS_CFG_XWEM),y)
    include xwbs/xwem.mk
    ifeq ($(CWMCUEPPC_LICENSE),)
        XWEM_LIBS = $(addprefix @@,$(XWEM))
    else
        XWEM_LIBS = $(XWEM)
    endif
endif

ifneq ($(XWOS_OEM_DIR),)
    include xwbs/xwoem.mk
    ifeq ($(CWMCUEPPC_LICENSE),)
        XWOEM_LIBS = $(addprefix @@,$(XWOEM))
    else
        XWOEM_LIBS = $(XWOEM)
    endif
endif

libs: $(ARCH_LIB) $(CPU_LIB) $(SOC_LIB) $(BDL_LIB) $(XWOS_LIB)

$(ARCH_LIB): $(ARCH_OBJS)
ifeq ($(CWMCUEPPC_LICENSE),)
	$(SHOW_AR) echo -n "-L$(XWOS_ARCH_DIR)" $(ARCH_OBJS) > $@
else
	$(SHOW_AR) $(AR) -o $@ $(ARCH_OBJS)
endif

$(CPU_LIB): $(CPU_OBJS)
ifeq ($(CWMCUEPPC_LICENSE),)
	$(SHOW_AR) echo -n "-L$(XWOS_CPU_DIR)" $(CPU_OBJS) > $@
else
	$(SHOW_AR) $(AR) -o $@ $(CPU_OBJS)
endif

$(SOC_LIB): $(SOC_OBJS)
ifeq ($(CWMCUEPPC_LICENSE),)
	$(SHOW_AR) echo -n "-L$(XWOS_SOC_DIR)" $(SOC_OBJS) > $@
else
	$(SHOW_AR) $(AR) -o $@ $(SOC_OBJS)
endif

$(BDL_LIB): $(BDL_OBJS)
ifeq ($(CWMCUEPPC_LICENSE),)
	$(SHOW_AR) echo -n "-L$(XWOS_BDL_DIR)" $(BDL_OBJS) > $@
else
	$(SHOW_AR) $(AR) -o $@ $(BDL_OBJS)
endif

$(XWOS_LIB): $(XWOS_OBJS)
ifeq ($(CWMCUEPPC_LICENSE),)
	$(SHOW_AR) echo -n "-L." $(XWOS_OBJS) > $@
else
	$(SHOW_AR) $(AR) -o $@ $(XWOS_OBJS)
endif

ifneq ($(NODEP),y)
#  -include $(OBJS:.o=.d)
endif

$(OBJ_DIR)%.o: %.c
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_CC) $(CC) $(CC_ARGS) $< -o $@

$(OBJ_DIR)%.o: %.s
	@[ ! -d $(@D) ] && mkdir -p $(@D) || true
	$(SHOW_AS) $(AS) $(AS_ARGS) $< -o $@

dsm: $(DSMS) $(XWMD_DSM) $(XWPP_DSM) $(XWBM_DSM) $(XWEM_DSM) $(XWOEM_DSM) $(XWOS_WKSPC_DIR)/$(TARGET).dsm

$(XWOS_WKSPC_DIR)/$(TARGET).dsm: $(XWOS_WKSPC_DIR)/$(TARGET).elf
	$(SHOW_OD) $(OD) $(CFLAGS) $< > $@

%.dsm: %.o
	$(SHOW_OD) $(OD) $(CFLAGS) $< > $@

%.elf %.mot: libs $(XWMD) $(XWPP) $(XWBM) $(XWEM) $(XWOEM)
	$(SHOW_LD) $(LD) $(LD_ARGS) -o $@

%.bin: %.mot
	$(SHOW_MOT2BIN) mot2bin $<

clean: $(XWMD_CLEAN) $(XWPP_CLEAN) $(XWBM_CLEAN) $(XWEM_CLEAN) $(XWOEM_CLEAN)
	@echo "clean ..."
	@$(RM) -f $(OBJS)
	@$(RM) -f $(OBJS:.o=.lst)
	@$(RM) -f $(OBJS:.o=.dsm)
	@$(RM) -f $(XWOS_LIB)
	@$(RM) -f $(BDL_LIB)
	@$(RM) -f $(SOC_LIB)
	@$(RM) -f $(CPU_LIB)
	@$(RM) -f $(ARCH_LIB)
	@$(RM) -f $(XWOS_WKSPC_DIR)/XuanWuOS.cfg
	@$(RM) -f $(XWOS_WKSPC_DIR)/$(TARGET).elf
	@$(RM) -f $(XWOS_WKSPC_DIR)/$(TARGET).dsm
	@$(RM) -f $(XWOS_WKSPC_DIR)/$(TARGET).mot
	@$(RM) -f $(XWOS_WKSPC_DIR)/$(TARGET).MAP
	@$(RM) -f $(XWOS_WKSPC_DIR)/$(TARGET).bin

distclean: $(XWMD_DISTCLEAN) $(XWPP_DISTCLEAN) $(XWBM_DISTCLEAN) $(XWEM_DISTCLEAN) $(XWOEM_DISTCLEAN)
	@echo "distclean ..."
	@$(RM) -rf $(OBJ_DIR)$(XWOS_KN_DIR)
	@$(RM) -rf $(OBJ_DIR)$(XWOS_BDL_DIR)
	@$(RM) -rf $(OBJ_DIR)$(XWOS_SOC_DIR)
	@$(RM) -rf $(OBJ_DIR)$(XWOS_CPU_DIR)
	@$(RM) -rf $(OBJ_DIR)$(XWOS_ARCH_DIR)
	@$(RM) -f $(XWOS_WKSPC_DIR)/XuanWuOS.cfg
	@$(RM) -f $(XWOS_WKSPC_DIR)/$(TARGET).elf
	@$(RM) -f $(XWOS_WKSPC_DIR)/$(TARGET).dsm
	@$(RM) -f $(XWOS_WKSPC_DIR)/$(TARGET).mot
	@$(RM) -f $(XWOS_WKSPC_DIR)/$(TARGET).MAP
	@$(RM) -f $(XWOS_WKSPC_DIR)/$(TARGET).bin

.PHONY: clean distclean all
.PHONY: $(XWMD) $(XWMD_DSM) $(XWMD_CLEAN) $(XWMD_DISTCLEAN)
.PHONY: $(XWPP) $(XWPP_DSM) $(XWPP_CLEAN) $(XWPP_DISTCLEAN)
.PHONY: $(XWBM) $(XWBM_DSM) $(XWBM_CLEAN) $(XWBM_DISTCLEAN)
.PHONY: $(XWEM) $(XWEM_DSM) $(XWEM_CLEAN) $(XWEM_DISTCLEAN)
.PHONY: $(XWOEM) $(XWOEM_DSM) $(XWOEM_CLEAN) $(XWOEM_DISTCLEAN)
