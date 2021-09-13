/**
 * @file
 * @brief 链接脚本内存区域
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
 */

MEMORY {
        flash_mr (rx): org = 0x08000000, len = 1M /* internal FLASH */
        isram_mr (rwx): org = 0x20000000, len = 128k /* internal SRAM */
        isramb1_mr (rwx): org = 0x20000000, len = 112k /* internal SRAM bank1 */
        isramb2_mr (rwx): org = 0x2001C000, len = 16k /* internal SRAM bank2 */
        ccmram_mr (rw): org = 0x10000000, len = 64k /* CCM SRAM */
        bkram_mr (rw): org = 0x40024000, len = 4k /* BACKUP SRAM */
        sdram_mr (rw): org = 0xD0000000, len = 16M /* external RAM */

        ivt_lmr (rx): org = 0x08000000, len = 2048 /* vectors table lma region */
        ivt_vmr (rw): org = 0x08000000, len = 2048 /* vectors table vma region */
        image_description_mr (rx): org = 0x08000800, len = 1k /* image description */
        code_mr (rx): org = 0x08000C00, len = 2045k /* text & rodata */
        stkmempool_mr (arw): org = 0x20000000, len = 128k /* .stkmempool */
        data_mr (arw): org = 0x20020000, len = 64k /* .data, .bss & .heap */
        xwos_data_mr (arw): org = 0x10000000, len = 24k /* .xwos.data */
        ccmheap_mr (arw): org = 0x10006000, len = 32k /* .ccmheap */
        xwos_stk_mr (rw): org = 0x1000E000, len = 8k /* xwos stack */
}

stkmempool_mr_origin = ORIGIN(stkmempool_mr);
stkmempool_mr_size = LENGTH(stkmempool_mr);

ccmheap_mr_origin = ORIGIN(ccmheap_mr);
ccmheap_mr_size = LENGTH(ccmheap_mr);

sdram_mr_origin = ORIGIN(sdram_mr);
sdram_mr_size = LENGTH(sdram_mr);

INCLUDE f4.lds
