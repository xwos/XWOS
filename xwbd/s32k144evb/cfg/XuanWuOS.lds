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
        flash_mr (rx): org = 0x00000000, len = 512K /* internal FLASH */
        isram_pc_mr (rwx): org = 0x1FFF8000, len = 32K /* internal SRAM bank1 */
        isram_ps_mr (rwx): org = 0x20000000, len = 28K /* internal SRAM bank2 */

        vctbl_vmr (rx): org = 0x00000000, len = 1k /* vector table VMA */
        vctbl_lmr (rx): org = 0x00000000, len = 1k /* vector table VMA */
        flash_cfg_mr (rx): org = 0x00000400, len = 16 /* flash configurations */
        image_description_mr (rx): org = 0x0000420, len = 32 /* image description */
        code_mr (rx): org = 0x00000440, len = 0x7FBC0  /* .xwos.init.text &
                                                         .xwos.init.rodata &
                                                         .xwos.exit.text &
                                                         .xwos.exit.rodata &
                                                         .xwos.isr.text &
                                                         .xwos.text &
                                                         .xwos.rodata &
                                                         .xwmd.isr.text &
                                                         .xwmd.text &
                                                         .xwmd.rodata &
                                                         .text &
                                                         .rodata */
        xwos_data_mr (arw): org = 0x1FFF8000, len = 16k /* .xwos.data */
        data_mr (arw): org = 0x1FFFC000, len = 40k /* .data, .bss & .heap */
        xwos_stk_mr (rw): org = 0x20006000, len = 4k /* xwos stack */
}

INCLUDE soc.lds
