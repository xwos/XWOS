#! /bin/make -f
# @file
# @brief ARCH描述层的编译规则
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

ARCH_INCDIRS :=
ARCH_CFLAGS :=
ARCH_CXXFLAGS :=
ARCH_AFLAGS :=
ARCH_LDFLAGS := -specs=nano.specs -nostdlib -fno-use-cxa-atexit
ARCH_EOBJS :=

ARCH_CSRCS :=
ARCH_ASRCS :=

ARCH_CSRCS += asmlib/lfq.c

ifeq ($(ARCHCFG_LIB_XWAOP8),y)
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/add.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/and.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/load.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/or.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/read.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/store.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/t1ma_then_c0m.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/t1mo_then_c0m.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/t0ma_then_s1m.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/t0mo_then_s1m.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/teq_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/teq_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/teq_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/teq_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tge_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tge_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tge_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tge_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tgele_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tgele_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tgele_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tgele_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tgelt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tgelt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tgelt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tgelt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tgt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tgt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tgt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tgt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tgtle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tgtle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tgtle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tgtle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tgtlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tgtlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tgtlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tgtlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tne_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tne_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tne_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tne_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/tst_then_op.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/write.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/xor.c
    ARCH_CSRCS += asmlib/xwaop/xwu8_t/c0m.c

    ARCH_CSRCS += asmlib/xwaop/xws8_t/add.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/and.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/load.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/or.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/read.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/store.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/sub.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/teq_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/teq_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/teq_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/teq_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tge_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tge_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tge_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tge_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tgele_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tgele_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tgele_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tgele_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tgelt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tgelt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tgelt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tgelt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tgt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tgt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tgt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tgt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tgtle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tgtle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tgtle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tgtle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tgtlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tgtlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tgtlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tgtlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tne_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tne_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tne_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tne_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/tst_then_op.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/write.c
    ARCH_CSRCS += asmlib/xwaop/xws8_t/xor.c
endif

ifeq ($(ARCHCFG_LIB_XWAOP16),y)
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/add.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/and.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/load.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/or.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/read.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/store.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/t1ma_then_c0m.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/t1mo_then_c0m.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/t0ma_then_s1m.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/t0mo_then_s1m.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/teq_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/teq_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/teq_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/teq_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tge_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tge_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tge_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tge_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tgele_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tgele_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tgele_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tgele_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tgelt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tgelt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tgelt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tgelt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tgt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tgt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tgt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tgt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tgtle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tgtle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tgtle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tgtle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tgtlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tgtlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tgtlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tgtlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tne_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tne_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tne_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tne_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/tst_then_op.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/write.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/xor.c
    ARCH_CSRCS += asmlib/xwaop/xwu16_t/c0m.c

    ARCH_CSRCS += asmlib/xwaop/xws16_t/add.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/and.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/load.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/or.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/read.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/store.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/sub.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/teq_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/teq_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/teq_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/teq_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tge_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tge_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tge_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tge_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tgele_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tgele_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tgele_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tgele_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tgelt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tgelt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tgelt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tgelt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tgt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tgt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tgt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tgt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tgtle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tgtle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tgtle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tgtle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tgtlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tgtlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tgtlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tgtlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tne_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tne_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tne_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tne_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/tst_then_op.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/write.c
    ARCH_CSRCS += asmlib/xwaop/xws16_t/xor.c
endif

ifeq ($(ARCHCFG_LIB_XWAOP32),y)
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/add.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/and.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/load.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/or.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/read.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/store.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/t1ma_then_c0m.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/t1mo_then_c0m.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/t0ma_then_s1m.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/t0mo_then_s1m.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/teq_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/teq_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/teq_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/teq_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tge_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tge_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tge_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tge_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tgele_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tgele_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tgele_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tgele_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tgelt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tgelt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tgelt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tgelt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tgt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tgt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tgt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tgt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tgtle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tgtle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tgtle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tgtle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tgtlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tgtlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tgtlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tgtlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tne_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tne_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tne_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tne_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/tst_then_op.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/write.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/xor.c
    ARCH_CSRCS += asmlib/xwaop/xwu32_t/c0m.c

    ARCH_CSRCS += asmlib/xwaop/xws32_t/add.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/and.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/load.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/or.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/read.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/store.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/sub.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/teq_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/teq_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/teq_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/teq_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tge_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tge_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tge_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tge_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tgele_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tgele_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tgele_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tgele_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tgelt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tgelt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tgelt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tgelt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tgt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tgt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tgt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tgt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tgtle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tgtle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tgtle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tgtle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tgtlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tgtlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tgtlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tgtlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tne_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tne_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tne_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tne_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/tst_then_op.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/write.c
    ARCH_CSRCS += asmlib/xwaop/xws32_t/xor.c
endif

ifeq ($(ARCHCFG_LIB_XWAOP64),y)
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/add.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/and.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/load.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/or.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/read.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/store.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/t1ma_then_c0m.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/t1mo_then_c0m.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/t0ma_then_s1m.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/t0mo_then_s1m.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/teq_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/teq_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/teq_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/teq_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tge_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tge_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tge_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tge_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tgele_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tgele_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tgele_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tgele_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tgelt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tgelt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tgelt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tgelt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tgt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tgt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tgt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tgt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tgtle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tgtle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tgtle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tgtle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tgtlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tgtlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tgtlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tgtlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tne_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tne_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tne_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tne_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/tst_then_op.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/write.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/xor.c
    ARCH_CSRCS += asmlib/xwaop/xwu64_t/c0m.c

    ARCH_CSRCS += asmlib/xwaop/xws64_t/add.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/and.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/load.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/or.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/read.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/store.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/sub.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/teq_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/teq_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/teq_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/teq_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tge_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tge_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tge_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tge_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tgele_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tgele_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tgele_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tgele_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tgelt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tgelt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tgelt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tgelt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tgt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tgt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tgt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tgt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tgtle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tgtle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tgtle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tgtle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tgtlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tgtlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tgtlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tgtlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tne_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tne_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tne_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tne_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/tst_then_op.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/write.c
    ARCH_CSRCS += asmlib/xwaop/xws64_t/xor.c
endif

ifeq ($(ARCHCFG_LIB_XWBMPAOP),y)
    ARCH_CSRCS += asmlib/xwbmpaop/c0i.c
    ARCH_CSRCS += asmlib/xwbmpaop/x1i.c
    ARCH_CSRCS += asmlib/xwbmpaop/s1i.c
    ARCH_CSRCS += asmlib/xwbmpaop/t0i_then_s1i.c
    ARCH_CSRCS += asmlib/xwbmpaop/t1i_then_c0i.c
    ARCH_CSRCS += asmlib/xwbmpaop/ffs_then_c0i.c
    ARCH_CSRCS += asmlib/xwbmpaop/ffz_then_s1i.c
    ARCH_CSRCS += asmlib/xwbmpaop/fls_then_c0i.c
    ARCH_CSRCS += asmlib/xwbmpaop/flz_then_s1i.c
endif

ifeq ($(ARCHCFG_LIB_XWBOP_FFS8),y)
    ARCH_CSRCS += asmlib/xwbop/ffs8.c
endif
ifeq ($(ARCHCFG_LIB_XWBOP_FLS8),y)
    ARCH_CSRCS += asmlib/xwbop/fls8.c
endif
ifeq ($(ARCHCFG_LIB_XWBOP_RBIT8),y)
    ARCH_CSRCS += asmlib/xwbop/rbit8.c
endif

ifeq ($(ARCHCFG_LIB_XWBOP_FFS16),y)
    ARCH_CSRCS += asmlib/xwbop/ffs16.c
endif
ifeq ($(ARCHCFG_LIB_XWBOP_FLS16),y)
    ARCH_CSRCS += asmlib/xwbop/fls16.c
endif
ifeq ($(ARCHCFG_LIB_XWBOP_RBIT16),y)
    ARCH_CSRCS += asmlib/xwbop/rbit16.c
endif
ifeq ($(ARCHCFG_LIB_XWBOP_RE16),y)
    ARCH_CSRCS += asmlib/xwbop/re16.c
endif
ifeq ($(ARCHCFG_LIB_XWBOP_RE16S32),y)
    ARCH_CSRCS += asmlib/xwbop/re16s32.c
endif

ifeq ($(ARCHCFG_LIB_XWBOP_FFS32),y)
    ARCH_CSRCS += asmlib/xwbop/ffs32.c
endif
ifeq ($(ARCHCFG_LIB_XWBOP_FLS32),y)
    ARCH_CSRCS += asmlib/xwbop/fls32.c
endif
ifeq ($(ARCHCFG_LIB_XWBOP_RBIT32),y)
    ARCH_CSRCS += asmlib/xwbop/rbit32.c
endif
ifeq ($(ARCHCFG_LIB_XWBOP_RE32),y)
    ARCH_CSRCS += asmlib/xwbop/re32.c
endif
ifeq ($(ARCHCFG_LIB_XWBOP_RE32S64),y)
    ARCH_CSRCS += asmlib/xwbop/re32s64.c
endif

ifeq ($(ARCHCFG_LIB_XWBOP_C0M64),y)
    ARCH_CSRCS += asmlib/xwbop/c0m64.c
endif
ifeq ($(ARCHCFG_LIB_XWBOP_FFS64),y)
    ARCH_CSRCS += asmlib/xwbop/ffs64.c
endif
ifeq ($(ARCHCFG_LIB_XWBOP_FLS64),y)
    ARCH_CSRCS += asmlib/xwbop/fls64.c
endif
ifeq ($(ARCHCFG_LIB_XWBOP_RBIT64),y)
    ARCH_CSRCS += asmlib/xwbop/rbit64.c
endif
ifeq ($(ARCHCFG_LIB_XWBOP_RE64),y)
    ARCH_CSRCS += asmlib/xwbop/re64.c
endif

ifeq ($(ARCHCFG_LIB_XWBMPOP_S1I),y)
    ARCH_CSRCS += asmlib/xwbmpop/s1i.c
endif
ifeq ($(ARCHCFG_LIB_XWBMPOP_C0I),y)
    ARCH_CSRCS += asmlib/xwbmpop/c0i.c
endif
ifeq ($(ARCHCFG_LIB_XWBMPOP_X1I),y)
    ARCH_CSRCS += asmlib/xwbmpop/x1i.c
endif
ifeq ($(ARCHCFG_LIB_XWBMPOP_T1I),y)
    ARCH_CSRCS += asmlib/xwbmpop/t1i.c
endif
ifeq ($(ARCHCFG_LIB_XWBMPOP_FFS),y)
    ARCH_CSRCS += asmlib/xwbmpop/ffs.c
endif
ifeq ($(ARCHCFG_LIB_XWBMPOP_FFZ),y)
    ARCH_CSRCS += asmlib/xwbmpop/ffz.c
endif
ifeq ($(ARCHCFG_LIB_XWBMPOP_FLS),y)
    ARCH_CSRCS += asmlib/xwbmpop/fls.c
endif
ifeq ($(ARCHCFG_LIB_XWBMPOP_FLZ),y)
    ARCH_CSRCS += asmlib/xwbmpop/flz.c
endif

ARCH_CSRCS += arch_init.c arch_image.c arch_irq.c
ARCH_CSRCS += arch_sched.c arch_systick.c

ifeq ($(XuanWuOS_CFG_CORE),smp)
    ARCH_CSRCS += smp_nvic.c smp_nvic_drv.c
else
    ARCH_CSRCS += up_nvic.c up_nvic_drv.c
endif
