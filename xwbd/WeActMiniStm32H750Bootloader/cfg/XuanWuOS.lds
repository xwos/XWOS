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
        itcm_mr (rwx): org = 0x00000000, len = 64k /* ITCM */
        flash_mr (rx): org = 0x08000000, len = 128k /* internal FLASH */
        qspiflash_mr (rx): org = 0x90000000, len = 8M /* external FLASH */
        dtcm_mr (rwx): org = 0x20000000, len = 128k /* DTCM */
        axisram_mr (rwx): org = 0x24000000, len = 512k /* AXI SRAM bank1 */
        sram1_mr (rwx): org = 0x30000000, len = 128k /* internal SRAM bank1 */
        sram2_mr (rwx): org = 0x30020000, len = 128k /* internal SRAM bank2 */
        sram3_mr (rwx): org = 0x30040000, len = 32k /* internal SRAM bank3 */
        sram4_mr (rwx): org = 0x38000000, len = 64k /* internal SRAM bank4 */
        bkram_mr (rw): org = 0x38800000, len = 4k /* backup SRAM */

        ivt_lmr (rx): org = 0x08000000, len = 2048 /* vectors table lma region */
        ivt_vmr (rw): org = 0x08000000, len = 2048 /* vectors table vma region */
        image_description_mr (rx): org = 0x08000800, len = 1k /* image description */
        code_mr (rx): org = 0x08000C00, len = 125k /* text & rodata */
        xwos_data_mr (arw): org = 0x800, len = 54k /* .xwos.data */
        data_mr (arw): org = 0x30000000, len = 288k /* .data, .bss & .heap */
        data4_mr (rw): org = 0x38000000, len = 64k /* .data4 for D3 Domain */
        dtcmheap_mr (arw): org = 0x20000000, len = 128k /* .dtcmheap */
        xwos_stk_mr (rw): org = 0x0000E000, len = 8k /* xwos stack */
}

dtcmheap_mr_origin = ORIGIN(dtcmheap_mr);
dtcmheap_mr_size = LENGTH(dtcmheap_mr);

axisram_mr_origin = ORIGIN(axisram_mr);
axisram_mr_size = LENGTH(axisram_mr);

qspiflash_mr_origin = ORIGIN(qspiflash_mr);
qspiflash_mr_size = LENGTH(qspiflash_mr);

INCLUDE h7.lds
