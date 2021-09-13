/**
 * @file
 * @brief STM32F7x的链接脚本
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
 * - Reference: arm-none-eabi-ld --verbose
 */

ENTRY(arch_isr_reset)

SECTIONS {
        .xwos.ivt : ALIGN(8) {
                KEEP(*(.xwos.ivt))
                xwos_ivt_vma_end = .;
        } > ivt_vmr AT> ivt_lmr
        xwos_ivt_vma_base = ADDR(.xwos.ivt);
        xwos_ivt_lma_base = LOADADDR(.xwos.ivt);

        .image.description : ALIGN(8) {
                KEEP(*(.image.description))
                image_description_vma_end = .;
        } > image_description_mr AT> image_description_mr
        image_description_vma_base = ADDR(.image.description);
        image_description_lma_base = LOADADDR(.image.description);

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
                *(.glue_7)      /* glue arm to thumb code */
                *(.glue_7t)     /* glue thumb to arm code */
                *(.eh_frame)
                *(.gnu.warning)
                *(.vfp11_veneer)
                *(.v4_bx)
                *(.rodata .rodata.* .gnu.linkonce.r.*)
        } > code_mr AT> code_mr

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
        } > code_mr AT> code_mr
        fini_array_vma_base = ADDR(.fini_array);
        fini_array_lma_base = LOADADDR(.fini_array);

        .ARM.extab : {  /* @ref ehabi.pdf for details */
                *(.ARM.extab* .gnu.linkonce.armextab.*)
        } > code_mr AT> code_mr

        .ARM.exidx : {  /* @ref ehabi.pdf for details */
                PROVIDE(__exidx_start = .);
                *(.ARM.exidx* .gnu.linkonce.armexidx.*)
                PROVIDE(__exidx_end = .);
        } > code_mr AT> code_mr

        .xwos.data : ALIGN(16) {
                *(.xwos.data)
                . = ALIGN(16);
                xwos_data_vma_end = .;
        } > xwos_data_mr AT> code_mr
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
        .image.tail : ALIGN(128) {
                KEEP(*(.image.tail))
                image_tail_vma_end = .;
        } > code_mr AT> code_mr
        image_tail_vma_base = ADDR(.image.tail);
        image_tail_size = image_tail_vma_end - image_tail_vma_base;
        image_tail_lma_base = LOADADDR(.image.tail);
        image_tail_lma_end = image_tail_lma_base + image_tail_size;

        .bss : ALIGN(8) {
                *(.dynbss)
                *(.bss .bss.* .gnu.linkonce.b.*)
                *(COMMON)
                bss_vma_end = .;
        } > data_mr
        bss_vma_base = ADDR(.bss);

        .heap : ALIGN(16) {
                PROVIDE(end = .);
        } > data_mr

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

        .ARM.attributes 0 : { /* architecture infomations @ref aaelf.pdf */
                *(.ARM.attributes)
        }
}

xwos_data_mr_origin = ORIGIN(xwos_data_mr);
xwos_data_mr_size = LENGTH(xwos_data_mr);
xwmm_dkma_mmz_origin = xwos_data_mr_origin;
xwmm_dkma_mmz_size = xwos_data_mr_size;
xwmm_dkma_mmz_pos = xwos_data_vma_end - xwos_data_mr_origin;

xwos_stk_mr_origin = ORIGIN(xwos_stk_mr);
xwos_stk_mr_size = LENGTH(xwos_stk_mr);
xwos_stk_top = xwos_stk_mr_origin + xwos_stk_mr_size;
xwos_stk_end = xwos_stk_mr_origin;
