/**
 * @file
 * @brief STM32F7x的链接脚本
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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

        .xwos.rx : ALIGN(8) {
                *(.xwos.init.text)
                *(.xwos.init.rodata)
                *(.xwos.exit.text)
                *(.xwos.exit.rodata)
                *(.xwos.isr.text)
                *(.xwos.text)
                *(.xwos.rodata)
        } > code_mr AT> code_mr

        .rx : ALIGN(8) {
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

        /* ++++++++ 以下与unwinding有关，用于异常处理和栈回溯 ++++++++ */
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
        /* -------- 以上与unwinding有关，用于异常处理和栈回溯 -------- */

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

        /* ++++++++ Global Offset Table ++++++++ */
        /* 当-ftls-model=initial-exec时，tls变量的offset是从got表的地址加载的 */
        /* 当-ftls-model=local-exec时，tls变量的offset是直接加载立即数，此时不需要 .got */
        /* MCU静态连接的环境，C11标准中的thread_local 变量只能是以上两种模型。 */
        .got : ALIGN(8) {
                *(.got.plt) *(.igot.plt) *(.got) *(.igot)
        } > code_mr AT> code_mr
        /* -------- Global Offset Table -------- */

        .xwos.data : ALIGN(32) {
                *(.xwos.data)
                . = ALIGN(32);
                xwos_data_vma_end = .;
        } > xwos_data_mr AT> code_mr
        xwos_data_vma_base = ADDR(.xwos.data);
        xwos_data_lma_base = LOADADDR(.xwos.data);

        .data : ALIGN(32) {
                *(.data .data.* .gnu.linkonce.d.*)
                . = ALIGN(32);
                data_vma_end = .;
        } > data_mr AT> code_mr
        data_vma_base = ADDR(.data);
        data_lma_base = LOADADDR(.data);

        .tdata : ALIGN(32) {
                *(.tdata .tdata.* .gnu.linkonce.td.*)
                tdata_vma_end = .;
	} > data_mr AT> code_mr
        tdata_vma_base = ADDR(.tdata);
        tdata_lma_base = LOADADDR(.tdata);

        .tbss (NOLOAD) :  {
                *(.tbss .tbss.* .gnu.linkonce.tb.*)
                *(.tcommon)
                tbss_vma_end = .;
        } > data_mr
        tbss_vma_base = ADDR(.tbss);
        tbss_vma_offset = ADDR(.tbss) - ADDR(.tdata);
        tls_align = MAX(ALIGNOF(.tdata), ALIGNOF(.tbss));
        tls_offset = MAX(8, tls_align);

        /* STM32H7的SRAM4分区 */
        .data4 : ALIGN(32) {
                *(.data4)
                . = ALIGN(32);
                data4_vma_end = .;
        } > data4_mr AT> code_mr
        data4_vma_base = ADDR(.data4);
        data4_lma_base = LOADADDR(.data4);

        .firmware.tail : ALIGN(32) {
                KEEP(*(.firmware.tail))
                firmware_tail_vma_end = .;
        } > code_mr AT> code_mr
        firmware_tail_vma_base = ADDR(.firmware.tail);
        firmware_tail_size = firmware_tail_vma_end - firmware_tail_vma_base;
        firmware_tail_lma_base = LOADADDR(.firmware.tail);
        firmware_tail_lma_end = firmware_tail_lma_base + firmware_tail_size;

        .bss (NOLOAD) : ALIGN(16) {
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
        } > data_mr

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
        /* -------- 以上与动态连接有关，不适用于MCU的ROM环境 -------- */

        .ARM.attributes 0 : { /* architecture infomations @ref aaelf.pdf */
                *(.ARM.attributes)
                *(.gnu.attributes)
        }
}

/* 用于提供firmware文件头信息 */
firmware_head_lma = armv7m_ivt_lma_base;
