/**
 * @file
 * @brief GD32F10x的链接脚本
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
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
 * - Reference: riscv-none-embed-ld --verbose
 */

ENTRY(soc_boot)

SECTIONS {
        .soc : {
                KEEP(*(.soc.boot))
                . = ALIGN(64);
                KEEP(*(.soc.esr.entry))
                KEEP(*(.soc.isr.entry))
        } > soc_mr AT> soc_mr

        .evt : {
                KEEP(*(.evt))
                KEEP(*(.edvt))
                evt_vma_end = .;
        } > evt_vmr AT> evt_lmr
        evt_vma_base = ADDR(.evt);
        evt_lma_base = LOADADDR(.evt);

        .eclic.ivt : {
                KEEP(*(.eclic.ivt))
                KEEP(*(.eclic.idvt))
                eclic_ivt_vma_end = .;
        } > eclic_ivt_vmr AT> eclic_ivt_lmr
        eclic_ivt_vma_base = ADDR(.eclic.ivt);
        eclic_ivt_lma_base = LOADADDR(.eclic.ivt);

        .rx : ALIGN(4) {
                image_description_vma_base = .;
                KEEP(*(.image.description))
                KEEP(*(.xwos.ivt))
                *(.xwos.init.text)
                *(.xwos.init.rodata)
                *(.xwos.exit.text)
                *(.xwos.exit.rodata)
                *(.xwos.isr.text)
                *(.xwos.text)
                *(.xwos.rodata)
                *(.text .text.* .gnu.linkonce.t.*)
                *(.rodata .rodata.* .gnu.linkonce.r.*)
                *(.srodata .srodata.* .gnu.linkonce.sr.*)
        } > code_mr AT> code_mr
        image_description_lma_base = LOADADDR(.rx);

        .fini : {
                KEEP (*(SORT_NONE(.fini)))
        } > code_mr AT> code_mr

        .preinit_array : ALIGN(4) {
                PROVIDE_HIDDEN (__preinit_array_start = .);
                KEEP (*(.preinit_array*))
                PROVIDE_HIDDEN (__preinit_array_end = .);
                preinit_array_vma_end = .;
        } > code_mr AT> code_mr
        preinit_array_vma_base = ADDR(.preinit_array);
        preinit_array_lma_base = LOADADDR(.preinit_array);

        .init_array : ALIGN(4) {
                PROVIDE_HIDDEN (__init_array_start = .);
                KEEP (*(SORT_BY_INIT_PRIORITY(.init_array*)))
                KEEP (*(SORT_BY_INIT_PRIORITY(.ctors*)))
                PROVIDE_HIDDEN (__init_array_end = .);
                init_array_vma_end = .;
        } > code_mr AT> code_mr
        init_array_vma_base = ADDR(.init_array);
        init_array_lma_base = LOADADDR(.init_array);

        .fini_array : ALIGN(4) {
                PROVIDE_HIDDEN (__fini_array_start = .);
                KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array*)))
                KEEP (*(SORT_BY_INIT_PRIORITY(.dtors*)))
                PROVIDE_HIDDEN (__fini_array_end = .);
                fini_array_vma_end = .;
        } > code_mr AT> code_mr
        fini_array_vma_base = ADDR(.fini_array);
        fini_array_lma_base = LOADADDR(.fini_array);

        .xwos.data : ALIGN(4) {
                *(.xwos.data)
                xwos_data_vma_end = .;
        } > data_mr AT> code_mr
        xwos_data_vma_base = ADDR(.xwos.data);
        xwos_data_lma_base = LOADADDR(.xwos.data);

        .data : ALIGN(4) {
                *(.data .data.* .gnu.linkonce.d.*)
                . = ALIGN(8);
                PROVIDE( __global_pointer$ = . + 0x800 );
                *(.sdata .sdata.* .sdata* .gnu.linkonce.s.*)
                data_vma_end = .;
        } > data_mr AT> code_mr
        data_vma_base = ADDR(.data);
        data_lma_base = LOADADDR(.data);

        .image.tail : ALIGN(32) {
                KEEP(*(.image.tail))
                image_tail_vma_end = .;
        } > code_mr AT> code_mr
        image_tail_vma_base = ADDR(.image.tail);
        image_tail_size = image_tail_vma_end - image_tail_vma_base;
        image_tail_lma_base = LOADADDR(.image.tail);
        image_tail_lma_end = image_tail_lma_base + image_tail_size;

        .bss : ALIGN(4) {
                *(.bss .bss.* .gnu.linkonce.b.*)
                *(.sbss* .gnu.linkonce.sb.*)
                *(COMMON)
                bss_vma_end = .;
        } > data_mr
        bss_vma_base = ADDR(.bss);

        .heap : ALIGN(4) {
                PROVIDE(end = .);
        } > xwos_stk_mr

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
}

data_mr_origin = ORIGIN(data_mr);
data_mr_size = LENGTH(data_mr);

xwmm_dkma_mmz_origin = data_mr_origin;
xwmm_dkma_mmz_size = data_mr_size;
xwmm_dkma_mmz_pos = bss_vma_end - data_mr_origin;

xwos_stk_mr_origin = ORIGIN(xwos_stk_mr);
xwos_stk_mr_size = LENGTH(xwos_stk_mr);
xwos_stk_top = xwos_stk_mr_origin + xwos_stk_mr_size;
xwos_stk_end = xwos_stk_mr_origin;
