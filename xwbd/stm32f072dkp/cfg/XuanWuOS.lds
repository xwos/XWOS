/**
 * @file
 * @brief 链接脚本内存区域
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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
 */

flash_mr_origin = 0x08000000;
flash_mr_size = 65536;

isram_mr_origin = 0x20000000;
isram_mr_size = 16384;

code_mr_origin = DEFINED(code_origin)? code_origin : flash_mr_origin;
data_mr_origin = DEFINED(data_origin)? data_origin : isram_mr_origin;

MEMORY {
        flash_mr (rx): org = flash_mr_origin, len = flash_mr_size /* internal FLASH */
        isram_mr (rwx): org = isram_mr_origin, len = isram_mr_size /* internal SRAM */

        /* vectors table lma region */
        vctbl_lmr (rx):
        org = code_mr_origin,
        len = 512

        /* vectors table vma region */
        vctbl_vmr (rw):
        org = code_mr_origin,
        len = 512

        /* image description */
        image_description_mr (rx):
        org = ORIGIN(vctbl_lmr) + LENGTH(vctbl_lmr),
        len = 512

        /* .text & .rodata */
        rx_mr (rx):
        org = ORIGIN(image_description_mr) + LENGTH(image_description_mr),
        len = flash_mr_size - LENGTH(image_description_mr) - LENGTH(vctbl_lmr)

        /* .data and .bss */
        rwx_mr (arw):
        org = data_mr_origin,
        len = 10k

        /* .heap */
        heap_mr (arw):
        org = ORIGIN(rwx_mr) + LENGTH(rwx_mr),
        len = 4k

        /* xwos stack */
        xwos_stk_mr (rw):
        org = ORIGIN(rwx_mr) + LENGTH(rwx_mr),
        len = isram_mr_size - LENGTH(rwx_mr) - LENGTH(heap_mr)
}

heap_mr_origin = ORIGIN(heap_mr);
heap_mr_size = LENGTH(heap_mr);

INCLUDE soc.lds
