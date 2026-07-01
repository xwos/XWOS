/**
 * @file
 * @brief 链接脚本内存区域
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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
        ram_mr (rwx): org = 0x0, len = 8192M /* Ram */
        periph_mr (rw): org = 0x400000000, len = 16384M /* Peripherals */

        mmu_mr (rwx): org = 0x18000000, len = 128M
        xwos_mr (rwx): org = 0x80000, len = 392704K
}

ram_mr_origin = ORIGIN(ram_mr);
ram_mr_size = LENGTH(ram_mr);

periph_mr_origin = ORIGIN(periph_mr);
periph_mr_size = LENGTH(periph_mr);

mmu_mr_origin = ORIGIN(mmu_mr);
mmu_mr_size = LENGTH(mmu_mr);

INCLUDE soc.lds
