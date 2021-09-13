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

/* 内部flash的信息 */
flash_mr_origin = 0x08000000;
flash_mr_size = 512k;

/* 内部RAM的信息 */
ocram_mr_origin = 0x20000000;
ocram_mr_size = 64k;

fmw_origin = DEFINED(fmw)? fmw : flash_mr_origin;

MEMORY {
        flash_mr (rx): org = flash_mr_origin, len = flash_mr_size
        ocram_mr (rwx): org = ocram_mr_origin, len = ocram_mr_size
        sram_mr (rw): org = 0x68000000, len = 1M

        /* 中断向量表的加载地址(LMA)区域 */
        ivt_lmr (rx):
                org = fmw_origin,
                len = 1024

        /* 中断向量表的运行地址(VMA)区域 */
        ivt_vmr (rw):
                org = fmw_origin,
                len = 1024

        /* 镜像文件的信息 */
        image_description_mr (rx):
                org = ORIGIN(ivt_lmr) + LENGTH(ivt_lmr),
                len = 64

        /* 代码段(.text)与只读数据段(.rodata) */
        code_mr (rx):
                org = ORIGIN(image_description_mr) + LENGTH(image_description_mr),
                len = flash_mr_size - LENGTH(image_description_mr) - LENGTH(ivt_lmr)

        /* 数据段(.data)与未初始化的数据段(.bss) */
        data_mr (arw):
                org = ocram_mr_origin,
                len = 29k

        /* 玄武OS的内核栈 */
        xwos_stk_mr (rw):
                org = ORIGIN(data_mr) + LENGTH(data_mr),
                len = 3k

        /* 内部堆内存 */
        ocheap_mr (awr):
                org = ORIGIN(xwos_stk_mr) + LENGTH(xwos_stk_mr),
                len = 32k
}

ocheap_mr_origin = ORIGIN(ocheap_mr);
ocheap_mr_size = LENGTH(ocheap_mr);

sram_mr_origin = ORIGIN(sram_mr);
sram_mr_size = LENGTH(sram_mr);

INCLUDE f1.lds
