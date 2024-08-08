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
        ram_mr (rwx): org = 0x0, len = 8192M /* RAM */
        peri_mr (rw): org = 0x400000000, len = 8192M /* Peripherals */
        pcie_mr (rw): org = 0x600000000, len = 8192M /* PCIE */

        runtime_mr (rwx): org = 0x80000, len = 0x7F80000 /* code, 128M - 0x80000 */
}

ram_mr_origin = ORIGIN(ram_mr);
ram_mr_size = LENGTH(ram_mr);

peri_mr_origin = ORIGIN(peri_mr);
peri_mr_size = LENGTH(peri_mr);

pcie_mr_origin = ORIGIN(pcie_mr);
pcie_mr_size = LENGTH(pcie_mr);

INCLUDE soc.lds
