/**
 * @file
 * @brief GD32F30x的链接脚本
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > Licensed under the Apache License, Version 2.0 (the "License");
 * > you may not use this file except in compliance with the License.
 * > You may obtain a copy of the License at
 * >
 * >         http://www.apache.org/licenses/LICENSE-2.0
 * >
 * > Unless required by applicable law or agreed to in writing, software
 * > distributed under the License is distributed on an "AS IS" BASIS,
 * > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * > See the License for the specific language governing permissions and
 * > limitations under the License.
 * @note
 * - Reference: arm-none-eabi-ld --verbose
 */

ENTRY(arch_isr_reset)

SECTIONS {
        .armv7m.ivt : ALIGN(8) {
                KEEP(*(.armv7m.ivt))
                armv7m_ivt_vma_end = .;
        } > armv7m_ivt_vmr AT> armv7m_ivt_lmr
        armv7m_ivt_vma_base = ADDR(.armv7m.ivt);
        armv7m_ivt_lma_base = LOADADDR(.armv7m.ivt);

        .firmware.info : ALIGN(8) {
                KEEP(*(.firmware.info))
                firmware_info_vma_end = .;
        } > firmware_info_mr AT> firmware_info_mr
        firmware_info_vma_base = ADDR(.firmware.info);
        firmware_info_lma_base = LOADADDR(.firmware.info);

        .rx : ALIGN(8) {
                *(.xwos.init.text)
                *(.xwos.init.rodata)
                *(.xwos.exit.text)
                *(.xwos.exit.rodata)
                *(.xwos.isr.text)
                *(.xwos.text)
                *(.xwos.rodata)
                KEEP (*(.init))
                KEEP (*(.fini))
                *(.text .text.* .gnu.linkonce.t.*)
                *(.rodata .rodata.* .gnu.linkonce.r.*)

                *(.gnu.warning)
                *(.glue_7) /* glue arm to thumb code */
                *(.glue_7t) /* glue thumb to arm code */
                *(.vfp11_veneer) /* 用于VFP单元 */
                *(.v4_bx) /* linker stubs */
        } > code_mr AT> code_mr

        /* ++++++++ 以下与unwinding有关，用于C++的异常处理 ++++++++ */
        .ARM.extab : ALIGN(8) { /* @ref ehabi.pdf for details */
                *(.ARM.extab* .gnu.linkonce.armextab.*)
        } > code_mr AT> code_mr

        .ARM.exidx : ALIGN(8) { /* @ref ehabi.pdf for details */
                PROVIDE(__exidx_start = .);
                *(.ARM.exidx* .gnu.linkonce.armexidx.*)
                PROVIDE(__exidx_end = .);
        } > code_mr AT> code_mr

        .eh_frame_hdr : ALIGN(8) { /* 用于栈展开 */
                KEEP(*(.eh_frame_hdr .eh_frame_entry .eh_frame_entry.*))
        } > code_mr AT> code_mr

        .eh_frame : ALIGN(8) { /* 用于栈展开 */
                KEEP(*(.eh_frame .eh_frame.*))
        } > code_mr AT> code_mr

        .gcc_except_table : ALIGN(8) { /* 用于try ... catch ... */
                *(.gcc_except_table .gcc_except_table.*)
        } > code_mr AT> code_mr

        .gnu_extab : ALIGN(8) {
                *(.gnu_extab*)
        } > code_mr AT> code_mr

        .exception_ranges : ALIGN(8) {
                *(.exception_ranges*)
        } > code_mr AT> code_mr
        /* -------- 以上与unwinding有关，用于C++的异常处理 -------- */

        /* ++++++++ 以下与C++全局对象的构造函数与析构函数有关 ++++++++ */
        .preinit_array : ALIGN(8) {
                PROVIDE_HIDDEN (__preinit_array_start = .);
                KEEP (*(.preinit_array*))
                PROVIDE_HIDDEN (__preinit_array_end = .);
                preinit_array_vma_end = .;
        } > code_mr AT> code_mr
        preinit_array_vma_base = ADDR(.preinit_array);
        preinit_array_lma_base = LOADADDR(.preinit_array);

        .init_array : ALIGN(8) {
                PROVIDE_HIDDEN (__init_array_start = .);
                KEEP (*(SORT_BY_INIT_PRIORITY(.init_array*)))
                KEEP (*(SORT_BY_INIT_PRIORITY(.ctors*)))
                PROVIDE_HIDDEN (__init_array_end = .);
                init_array_vma_end = .;
        } > code_mr AT> code_mr
        init_array_vma_base = ADDR(.init_array);
        init_array_lma_base = LOADADDR(.init_array);

        .fini_array : ALIGN(8) {
                PROVIDE_HIDDEN (__fini_array_start = .);
                KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array*)))
                KEEP (*(SORT_BY_INIT_PRIORITY(.dtors*)))
                PROVIDE_HIDDEN (__fini_array_end = .);
                fini_array_vma_end = .;
                . = ALIGN(32);
        } > code_mr AT> code_mr
        fini_array_vma_base = ADDR(.fini_array);
        fini_array_lma_base = LOADADDR(.fini_array);
        /* -------- 以上与C++全局对象的构造函数与析构函数有关 -------- */

        .xwos.data : ALIGN(16) {
                *(.xwos.data)
                . = ALIGN(16);
                xwos_data_vma_end = .;
        } > data_mr AT> code_mr
        xwos_data_vma_base = ADDR(.xwos.data);
        xwos_data_lma_base = LOADADDR(.xwos.data);

        .data : ALIGN(16) {
                *(.data .data.* .gnu.linkonce.d.*)
                . = ALIGN(16);
                data_vma_end = .;
        } > data_mr AT> code_mr
        data_vma_base = ADDR(.data);
        data_lma_base = LOADADDR(.data);

        /* 设置ALIGN(128)绕过ld.lld的overlaps Bug */
        .firmware.tail : ALIGN(128) {
                KEEP(*(.firmware.tail))
                firmware_tail_vma_end = .;
        } > code_mr AT> code_mr
        firmware_tail_vma_base = ADDR(.firmware.tail);
        firmware_tail_size = firmware_tail_vma_end - firmware_tail_vma_base;
        firmware_tail_lma_base = LOADADDR(.firmware.tail);
        firmware_tail_lma_end = firmware_tail_lma_base + firmware_tail_size;

        .bss : ALIGN(16) {
                *(.dynbss)
                *(.bss .bss.* .gnu.linkonce.b.*)
                *(COMMON)
                . = ALIGN(16);
                bss_vma_end = .;
        } > data_mr
        bss_vma_base = ADDR(.bss);

        .heap : ALIGN(16) {
                PROVIDE(end = .); /* 被libc中的malloc()使用 */
                PROVIDE(_end = .); /* 被libc中的malloc()使用 */
        } > xwos_stk_mr

        /* ++++++++ 以下与动态连接有关，不适用于MCU的ROM环境 ++++++++ */
        .rel.dyn : {
                *(.rel.init)
                *(.rel.text .rel.text.* .rel.gnu.linkonce.t.*)
                *(.rel.fini)
                *(.rel.rodata .rel.rodata.* .rel.gnu.linkonce.r.*)
                *(.rel.data.rel.ro .rel.data.rel.ro.* .rel.gnu.linkonce.d.rel.ro.*)
                *(.rel.data .rel.data.* .rel.gnu.linkonce.d.*)
                *(.rel.tdata .rel.tdata.* .rel.gnu.linkonce.td.*)
                *(.rel.tbss .rel.tbss.* .rel.gnu.linkonce.tb.*)
                *(.rel.ctors)
                *(.rel.dtors)
                *(.rel.got)
                *(.rel.bss .rel.bss.* .rel.gnu.linkonce.b.*)
                PROVIDE_HIDDEN (__rel_iplt_start = .);
                *(.rel.iplt)
                PROVIDE_HIDDEN (__rel_iplt_end = .);
                PROVIDE_HIDDEN (__rela_iplt_start = .);
                PROVIDE_HIDDEN (__rela_iplt_end = .);
        }

        .rela.dyn : {
                *(.rela.init)
                *(.rela.text .rela.text.* .rela.gnu.linkonce.t.*)
                *(.rela.fini)
                *(.rela.rodata .rela.rodata.* .rela.gnu.linkonce.r.*)
                *(.rela.data .rela.data.* .rela.gnu.linkonce.d.*)
                *(.rela.tdata .rela.tdata.* .rela.gnu.linkonce.td.*)
                *(.rela.tbss .rela.tbss.* .rela.gnu.linkonce.tb.*)
                *(.rela.ctors)
                *(.rela.dtors)
                *(.rela.got)
                *(.rela.bss .rela.bss.* .rela.gnu.linkonce.b.*)
                PROVIDE_HIDDEN (__rel_iplt_start = .);
                PROVIDE_HIDDEN (__rel_iplt_end = .);
                PROVIDE_HIDDEN (__rela_iplt_start = .);
                *(.rela.iplt)
                PROVIDE_HIDDEN (__rela_iplt_end = .);
        }

        .rel.plt : {
                *(.rel.plt)
        }
        .rela.plt : {
                *(.rela.plt)
        }

        .plt : {
                *(.plt)
        }
        .iplt : {
                *(.iplt)
        }

        .got : {
                *(.got.plt) *(.igot.plt) *(.got) *(.igot)
        }
        /* -------- 以上与动态连接有关，不适用于MCU的ROM环境 -------- */

        .ARM.attributes 0 : { /* architecture infomations @ref aaelf.pdf */
                *(.ARM.attributes)
                *(.gnu.attributes)
        }
}

/* 用于提供firmware文件头信息 */
firmware_head_lma = armv7m_ivt_lma_base;

data_mr_origin = ORIGIN(data_mr);
data_mr_size = LENGTH(data_mr);

xwmm_dkma_mmz_origin = data_mr_origin;
xwmm_dkma_mmz_size = data_mr_size;
xwmm_dkma_mmz_pos = bss_vma_end - data_mr_origin;

xwos_stk_mr_origin = ORIGIN(xwos_stk_mr);
xwos_stk_mr_size = LENGTH(xwos_stk_mr);
xwos_stk_top = xwos_stk_mr_origin + xwos_stk_mr_size;
xwos_stk_end = xwos_stk_mr_origin;
