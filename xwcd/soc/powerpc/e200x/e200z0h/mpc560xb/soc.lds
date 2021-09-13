/**
 * @file
 * @brief 链接脚本
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This file is a free software and is licensed under
 * > the Apache License, Version 2.0 (the "License").
 * > You may not use this file except in compliance with the License.
 * > You may obtain a copy of the License at
 * >         <http://www.apache.org/licenses/LICENSE-2.0/>.
 */
ENTRY(arch_isr_reset)

SECTIONS {
        . = 0x0;
        .bam_bootarea : {
                KEEP(*(.bam_bootarea))
        } > rcw_mr AT> rcw_mr

        .image_header : {
                KEEP(*(.image_header))
                image_header_vma_end = .;
        } > code_mr AT> code_mr

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
                *(.text)
                *(.text.*)
                KEEP (*(.init))
                KEEP (*(.fini))
        } > code_mr AT> code_mr

        .ctors : ALIGN(8) {
                __CTOR_LIST__ = .;
                ctors_vma_base = .;
                /* gcc uses crtbegin.o to find the start of
                   the constructors, so we make sure it is
                   first.  Because this is a wildcard, it
                   doesn't matter if the user does not
                   actually link against crtbegin.o; the
                   linker won't look for a file to match a
                   wildcard.  The wildcard also means that it
                   doesn't matter which directory crtbegin.o
                   is in.  */
                KEEP (*crtbegin.o(.ctors))
                KEEP (*crtbegin?.o(.ctors))
                /* We don't want to include the .ctor section from
                   from the crtend.o file until after the sorted ctors.
                   The .ctor section from the crtend file contains the
                   end of ctors marker and it must be last */
                KEEP (*(EXCLUDE_FILE(*crtend?.o * crtend.o) .ctors))
                KEEP (*(SORT(.ctors.*)))
                KEEP (*(.ctors))
                ctors_vma_end = .;
                __CTOR_END__ = .;
        } > code_mr AT> code_mr

        .dtors : ALIGN(8) {
                __DTOR_LIST__ = .;
                dtors_vma_base = .;
                KEEP (*crtbegin.o(.dtors))
                KEEP (*crtbegin?.o(.dtors))
                KEEP (*(EXCLUDE_FILE(*crtend?.o * crtend.o) .dtors))
                KEEP (*(SORT(.dtors.*)))
                KEEP (*(.dtors))
                dtors_vma_end = .;
                __DTOR_END__ = .;
        } > code_mr AT> code_mr

        .preinit_array : ALIGN(8) {
                PROVIDE_HIDDEN (__preinit_array_start = .);
                KEEP (*(.preinit_array*))
                PROVIDE_HIDDEN (__preinit_array_end = .);
        } > code_mr AT> code_mr

        .init_array : ALIGN(8) {
                PROVIDE_HIDDEN (__init_array_start = .);
                KEEP (*(SORT(.init_array.*)))
                KEEP (*(.init_array*))
                PROVIDE_HIDDEN (__init_array_end = .);
        } > code_mr AT> code_mr

        .fini_array : ALIGN(8) {
                PROVIDE_HIDDEN (__fini_array_start = .);
                KEEP (*(SORT(.fini_array.*)))
                KEEP (*(.fini_array*))
                PROVIDE_HIDDEN (__fini_array_end = .);
        } > code_mr AT> code_mr

        .rodata : ALIGN(8) {
                *(.rodata)
                *(.rodata.*)
        } > code_mr AT> code_mr

        .stdby_wkup : ALIGN(8) {
                KEEP(*(.stdby_wkup))
                stdby_wkup_vma_end = .;
        } > stdby_wkup_mr AT> code_mr
        stdby_wkup_vma_base = ADDR(.stdby_wkup);
        stdby_wkup_lma_base = LOADADDR(.stdby_wkup);

        .bkup : ALIGN(8) {
                KEEP(*(.bkup))
                bkup_vma_end = .;
        } > bkup_mr AT> code_mr
        bkup_vma_base = ADDR(.bkup);
        bkup_lma_base = LOADADDR(.bkup);

        .xwos.data : ALIGN(8) {
                *(.xwos.data)
                xwos_data_vma_end = .;
        } > data_mr AT> code_mr
        xwos_data_vma_base = ADDR(.xwos.data);
        xwos_data_lma_base = LOADADDR(.xwos.data);

        .flsopc : ALIGN(8) {
                *(.flsopc)
                flsopc_vma_end = .;
        } > data_mr AT> code_mr
        flsopc_vma_base = ADDR(.flsopc);
        flsopc_lma_base = LOADADDR(.flsopc);

        .data : ALIGN(8) {
                *(.data)
                *(.data.*)
                data_vma_end = .;
        } > data_mr AT> code_mr
        data_vma_base = ADDR(.data);
        data_lma_base = LOADADDR(.data);

        .bss ALIGN(8) (NOLOAD) : {
                *(.bss)
                *(.bss.*)
                *(COMMON)
                bss_vma_end = .;
        } > data_mr
        bss_vma_base = ADDR(.bss);

        .sdata2 : ALIGN(8) {
                *(.sdata2)
                *(.sdata2.*)
                sdata2_vma_end = .;
        } > data_mr AT> code_mr
        sdata2_vma_base = ADDR(.sdata2);
        sdata2_lma_base = LOADADDR(.sdata2);

        .sbss2 ALIGN(8) (NOLOAD) : {
                *(.sbss2)
                *(.sbss2.*)
                sbss2_vma_end = .;
        } > data_mr
        sbss2_vma_base = ADDR(.sbss2);

        .sdata : ALIGN(8) {
                *(.sdata)
                *(.sdata.*)
                sdata_vma_end = .;
        } > data_mr AT> code_mr
        sdata_vma_base = ADDR(.sdata);
        sdata_lma_base = LOADADDR(.sdata);

        .sbss ALIGN(8) (NOLOAD) : {
                *(.sbss)
                *(.sbss.*)
                sbss_vma_end = .;
        } > data_mr
        sbss_vma_base = ADDR(.sbss);
        rwx_vma_end = .;

        .xwos.ivt : ALIGN(0x800) {
                *(.xwos.ivt)
                xwos_ivt_vma_end = .;
        } > code_mr AT> code_mr
        xwos_ivt_vma_base = ADDR(.xwos.ivt);
        xwos_ivt_lma_base = LOADADDR(.xwos.ivt);

        .arch_isrtable : ALIGN(0x1000) {
                KEEP(*(.arch_isrtable))
        } > code_mr AT> code_mr

        .image_tail : ALIGN(16) {
                KEEP(*(.image_tail))
                image_tail_vma_end = .;
        } > code_mr AT> code_mr
        image_tail_vma_base = ADDR(.image_tail);
        image_tail_size = image_tail_vma_end - image_tail_vma_base;
        image_tail_lma_base = LOADADDR(.image_tail);
        image_tail_lma_end = image_tail_lma_base + image_tail_size;
}

xwos_stk_mr_origin = ORIGIN(xwos_stk_mr);
xwos_stk_mr_size = LENGTH(xwos_stk_mr);
xwos_stk_top = xwos_stk_mr_origin + xwos_stk_mr_size;
xwos_stk_end = xwos_stk_mr_origin;

data_mr_origin = ORIGIN(data_mr);
data_mr_size = LENGTH(data_mr);

xwmm_dkma_mmz_origin = data_mr_origin;
xwmm_dkma_mmz_size = data_mr_size;
xwmm_dkma_mmz_pos = rwx_vma_end - data_mr_origin;

bkup_mr_origin = ORIGIN(bkup_mr);
bkup_mr_size = LENGTH(bkup_mr);

stdby_wkup_mr_origin = ORIGIN(stdby_wkup_mr);
stdby_wkup_mr_size = LENGTH(stdby_wkup_mr);
