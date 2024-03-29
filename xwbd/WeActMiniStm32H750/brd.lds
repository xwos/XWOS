/**
 * @file
 * @brief 链接脚本内存区域
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

        armv7m_ivt_lmr (rx): org = 0x90000000, len = 2048 /* vectors table lma region */
        armv7m_ivt_vmr (rw): org = 0x20000000, len = 2048 /* vectors table vma region */
        firmware_info_mr (rx): org = 0x90000800, len = 1k /* firmware info */
        code_mr (rx): org = 0x90000C00, len = 8189k /* text & rodata */
        xwos_data_mr (arw): org = 0x20000800, len = 126k /* .xwos.data in DTCM */
        data_mr (arw): org = 0x30000000, len = 288k /* .data & .bss in SRAM */
        data4_mr (arw): org = 0x38000000, len = 64k /* .data4 for D3 Domain */
        armv7m_isr_stack_mr (arw): org = 0x0000E000, len = 8k /* ISR stack in ITCM */
}

dtcm_mr_origin = ORIGIN(dtcm_mr);
dtcm_mr_size = LENGTH(dtcm_mr);
dtcm_mr_pos = xwos_data_vma_end;

axisram_mr_origin = ORIGIN(axisram_mr);
axisram_mr_size = LENGTH(axisram_mr);

qspiflash_mr_origin = ORIGIN(qspiflash_mr);
qspiflash_mr_size = LENGTH(qspiflash_mr);

firmware_info_mr_origin = ORIGIN(firmware_info_mr);
firmware_info_mr_size = LENGTH(firmware_info_mr);
firmware_info_offset = firmware_info_mr_origin - qspiflash_mr_origin;

armv7m_isr_stack_mr_origin = ORIGIN(armv7m_isr_stack_mr);
armv7m_isr_stack_mr_size = LENGTH(armv7m_isr_stack_mr);
armv7m_isr_stack_top = armv7m_isr_stack_mr_origin + armv7m_isr_stack_mr_size;
armv7m_isr_stack_end = armv7m_isr_stack_mr_origin;

INCLUDE h7.lds
