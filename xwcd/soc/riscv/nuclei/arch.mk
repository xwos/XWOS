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
ARCH_AFLAGS := -mcmodel=medany -mno-save-restore
ARCH_CFLAGS := -mcmodel=medany -mno-save-restore
ARCH_CXXFLAGS := -mcmodel=medany -mno-save-restore
ARCH_LDFLAGS := -mcmodel=medany -mno-save-restore

ARCH_ASRCS :=
ARCH_CSRCS :=
ARCH_CXXSRCS :=

ARCH_EOBJS :=

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

ifeq ($(XWLIBCFG_SETJMP),y)
  ARCH_CSRCS += xwosimpl_soc_setjmp.c
endif

ARCH_CSRCS += arch_init.c
ARCH_CSRCS += xwosimpl_soc_lfq.c xwosimpl_soc_xwsc.c

ARCH_ASRCS += xwosimpl_soc_spinlock.S xwosasmimpl_soc_xwsc.S
