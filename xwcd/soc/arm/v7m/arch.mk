#! /bin/make -f
# @file
# @brief ARCH描述层的编译规则
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

ARCH_INCDIRS :=
ARCH_AFLAGS :=
ARCH_CFLAGS :=
ARCH_CXXFLAGS :=
ARCH_LDFLAGS := -fno-use-cxa-atexit

ARCH_ASRCS :=
ARCH_CSRCS :=
ARCH_CXXSRCS :=

ARCH_EOBJS :=

ifeq ($(ARCHCFG_LIB_XWAOP8),y)
    ARCH_CSRCS += asmlib/xwaop/xwu8/add.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/and.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/load.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/or.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/read.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/store.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/t1ma_then_c0m.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/t1mo_then_c0m.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/t0ma_then_s1m.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/t0mo_then_s1m.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/teq_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/teq_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/teq_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/teq_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tge_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tge_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tge_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tge_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tgele_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tgele_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tgele_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tgele_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tgelt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tgelt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tgelt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tgelt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tgt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tgt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tgt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tgt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tgtle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tgtle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tgtle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tgtle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tgtlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tgtlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tgtlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tgtlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tne_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tne_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tne_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tne_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/tst_then_op.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/write.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/xor.c
    ARCH_CSRCS += asmlib/xwaop/xwu8/c0m.c

    ARCH_CSRCS += asmlib/xwaop/xws8/add.c
    ARCH_CSRCS += asmlib/xwaop/xws8/and.c
    ARCH_CSRCS += asmlib/xwaop/xws8/load.c
    ARCH_CSRCS += asmlib/xwaop/xws8/or.c
    ARCH_CSRCS += asmlib/xwaop/xws8/read.c
    ARCH_CSRCS += asmlib/xwaop/xws8/rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws8/store.c
    ARCH_CSRCS += asmlib/xwaop/xws8/sub.c
    ARCH_CSRCS += asmlib/xwaop/xws8/teq_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws8/teq_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws8/teq_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws8/teq_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tge_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tge_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tge_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tge_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tgele_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tgele_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tgele_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tgele_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tgelt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tgelt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tgelt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tgelt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tgt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tgt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tgt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tgt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tgtle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tgtle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tgtle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tgtle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tgtlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tgtlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tgtlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tgtlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tne_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tne_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tne_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tne_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws8/tst_then_op.c
    ARCH_CSRCS += asmlib/xwaop/xws8/write.c
    ARCH_CSRCS += asmlib/xwaop/xws8/xor.c
endif

ifeq ($(ARCHCFG_LIB_XWAOP16),y)
    ARCH_CSRCS += asmlib/xwaop/xwu16/add.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/and.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/load.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/or.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/read.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/store.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/t1ma_then_c0m.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/t1mo_then_c0m.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/t0ma_then_s1m.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/t0mo_then_s1m.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/teq_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/teq_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/teq_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/teq_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tge_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tge_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tge_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tge_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tgele_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tgele_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tgele_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tgele_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tgelt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tgelt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tgelt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tgelt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tgt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tgt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tgt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tgt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tgtle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tgtle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tgtle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tgtle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tgtlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tgtlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tgtlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tgtlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tne_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tne_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tne_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tne_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/tst_then_op.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/write.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/xor.c
    ARCH_CSRCS += asmlib/xwaop/xwu16/c0m.c

    ARCH_CSRCS += asmlib/xwaop/xws16/add.c
    ARCH_CSRCS += asmlib/xwaop/xws16/and.c
    ARCH_CSRCS += asmlib/xwaop/xws16/load.c
    ARCH_CSRCS += asmlib/xwaop/xws16/or.c
    ARCH_CSRCS += asmlib/xwaop/xws16/read.c
    ARCH_CSRCS += asmlib/xwaop/xws16/rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws16/store.c
    ARCH_CSRCS += asmlib/xwaop/xws16/sub.c
    ARCH_CSRCS += asmlib/xwaop/xws16/teq_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws16/teq_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws16/teq_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws16/teq_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tge_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tge_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tge_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tge_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tgele_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tgele_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tgele_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tgele_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tgelt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tgelt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tgelt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tgelt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tgt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tgt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tgt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tgt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tgtle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tgtle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tgtle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tgtle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tgtlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tgtlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tgtlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tgtlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tne_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tne_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tne_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tne_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws16/tst_then_op.c
    ARCH_CSRCS += asmlib/xwaop/xws16/write.c
    ARCH_CSRCS += asmlib/xwaop/xws16/xor.c
endif

ifeq ($(ARCHCFG_LIB_XWAOP32),y)
    ARCH_CSRCS += asmlib/xwaop/xwu32/add.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/and.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/load.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/or.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/read.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/store.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/t1ma_then_c0m.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/t1mo_then_c0m.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/t0ma_then_s1m.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/t0mo_then_s1m.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/teq_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/teq_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/teq_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/teq_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tge_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tge_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tge_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tge_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tgele_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tgele_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tgele_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tgele_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tgelt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tgelt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tgelt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tgelt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tgt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tgt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tgt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tgt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tgtle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tgtle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tgtle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tgtle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tgtlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tgtlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tgtlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tgtlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tne_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tne_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tne_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tne_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/tst_then_op.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/write.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/xor.c
    ARCH_CSRCS += asmlib/xwaop/xwu32/c0m.c

    ARCH_CSRCS += asmlib/xwaop/xws32/add.c
    ARCH_CSRCS += asmlib/xwaop/xws32/and.c
    ARCH_CSRCS += asmlib/xwaop/xws32/load.c
    ARCH_CSRCS += asmlib/xwaop/xws32/or.c
    ARCH_CSRCS += asmlib/xwaop/xws32/read.c
    ARCH_CSRCS += asmlib/xwaop/xws32/rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws32/store.c
    ARCH_CSRCS += asmlib/xwaop/xws32/sub.c
    ARCH_CSRCS += asmlib/xwaop/xws32/teq_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws32/teq_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws32/teq_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws32/teq_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tge_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tge_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tge_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tge_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tgele_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tgele_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tgele_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tgele_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tgelt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tgelt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tgelt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tgelt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tgt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tgt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tgt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tgt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tgtle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tgtle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tgtle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tgtle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tgtlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tgtlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tgtlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tgtlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tne_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tne_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tne_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tne_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws32/tst_then_op.c
    ARCH_CSRCS += asmlib/xwaop/xws32/write.c
    ARCH_CSRCS += asmlib/xwaop/xws32/xor.c
endif

ifeq ($(ARCHCFG_LIB_XWAOP64),y)
    ARCH_CSRCS += asmlib/xwaop/xwu64/add.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/and.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/load.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/or.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/read.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/store.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/t1ma_then_c0m.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/t1mo_then_c0m.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/t0ma_then_s1m.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/t0mo_then_s1m.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/teq_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/teq_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/teq_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/teq_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tge_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tge_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tge_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tge_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tgele_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tgele_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tgele_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tgele_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tgelt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tgelt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tgelt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tgelt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tgt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tgt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tgt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tgt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tgtle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tgtle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tgtle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tgtle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tgtlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tgtlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tgtlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tgtlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tne_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tne_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tne_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tne_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/tst_then_op.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/write.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/xor.c
    ARCH_CSRCS += asmlib/xwaop/xwu64/c0m.c

    ARCH_CSRCS += asmlib/xwaop/xws64/add.c
    ARCH_CSRCS += asmlib/xwaop/xws64/and.c
    ARCH_CSRCS += asmlib/xwaop/xws64/load.c
    ARCH_CSRCS += asmlib/xwaop/xws64/or.c
    ARCH_CSRCS += asmlib/xwaop/xws64/read.c
    ARCH_CSRCS += asmlib/xwaop/xws64/rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws64/store.c
    ARCH_CSRCS += asmlib/xwaop/xws64/sub.c
    ARCH_CSRCS += asmlib/xwaop/xws64/teq_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws64/teq_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws64/teq_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws64/teq_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tge_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tge_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tge_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tge_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tgele_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tgele_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tgele_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tgele_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tgelt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tgelt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tgelt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tgelt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tgt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tgt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tgt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tgt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tgtle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tgtle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tgtle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tgtle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tgtlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tgtlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tgtlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tgtlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tle_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tle_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tle_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tle_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tlt_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tlt_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tlt_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tlt_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tne_then_add.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tne_then_rsb.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tne_then_sub.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tne_then_write.c
    ARCH_CSRCS += asmlib/xwaop/xws64/tst_then_op.c
    ARCH_CSRCS += asmlib/xwaop/xws64/write.c
    ARCH_CSRCS += asmlib/xwaop/xws64/xor.c
endif

ifeq ($(ARCHCFG_LIB_XWBMPAOP),y)
    ARCH_CSRCS += asmlib/xwbmpaop/t1i.c
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

ifeq ($(ARCHCFG_LIB_XWBMPOP_T1I),y)
    ARCH_CSRCS += asmlib/xwbmpop/t1i.c
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
ifeq ($(XWLIBCFG_SETJMP),y)
    ARCH_CSRCS += xwosimpl_soc_setjmp.c
endif

ARCH_CSRCS += arch_init.c
ARCH_CSRCS += arch_image.c arch_irq.c xwosimpl_soc_lfq.c xwosimpl_soc_xwsc.c
ARCH_CSRCS += arch_skd.c arch_systick.c
ARCH_CSRCS += arch_nvic.c
ifeq ($(ARCHCFG_FPU),y)
    ARCH_CSRCS += arch_fpu.c
endif
