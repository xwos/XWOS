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
        flash_mr (rx):                  org = 0x00010000, len = 0x000B0000
        sram_mr (rwx):                  org = 0x40000000, len = 0x00010000

        rcw_mr (rx):                    org = 0x00000000, len = 0x00000008
        code_mr (rx):                   org = 0x00010000, len = 0x00070000
        stdby_wkup_mr (rwx):            org = 0x40000000, len = 0x00000080
        bkup_mr (rw):                   org = 0x40000080, len = 0x00000080
        data_mr (rw):                   org = 0x40000100, len = 0x0000F300
        xwos_stk_mr (rw):               org = 0x4000F400, len = 0x00000C00
}

INCLUDE soc.lds
