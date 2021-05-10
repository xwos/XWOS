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
        flash_mr (rx): org = 0x00000000, len = 128k /* internal FLASH */
        sram_l_mr (rwx): org = 0x1FFFF000, len = 4k /* internal SRAM_L */
        sram_u_mr (rwx): org = 0x20000000, len = 12k /* internal SRAM_U */

        ivt_lmr (rx): org = 0x00000000, len = 1k /* vectors table lma region */
        ivt_vmr (rw): org = 0x00000000, len = 1k /* vectors table vma region */
        flash_cfg_mr (rx): org = 0x00000400, len = 16 /* flash config */
        image_description_mr (rx): org = 0x00000410, len = 240 /* image description */
        code_mr (rx): org = 0x00000500, len = 129792 /* .text & .rodata */
        data_mr (arw): org = 0x1FFFF000, len = 14k /* .data and .bss */
        xwos_stk_mr (rw): org = 0x20002800, len = 1k /* xwos stack */
}

INCLUDE soc.lds
