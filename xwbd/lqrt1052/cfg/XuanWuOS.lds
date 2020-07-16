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
        /* Define each memory region */
        FLASH (rx) : ORIGIN = 0x60000000, LENGTH = 0x2000000 /* 32M bytes (alias Flash) */
        DTCM (rwx) : ORIGIN = 0x20000000, LENGTH = 0x20000 /* 128K bytes (alias RAM) */
        ITCM (rwx) : ORIGIN = 0x0, LENGTH = 0x20000 /* 128K bytes (alias RAM2) */
        OCRAM (rwx) : ORIGIN = 0x20200000, LENGTH = 0x40000 /* 256K bytes (alias RAM3) */
        SDRAM (rwx) : ORIGIN = 0x80000000, LENGTH = 0x1e00000 /* 30M bytes (alias RAM4) */
        NCACHE (rwx) : ORIGIN = 0x81e00000, LENGTH = 0x200000 /* 2M bytes (alias RAM5) */
}

/* MCUXpresso自带的GCC有BUG，不能在MEMORY中使用ORIGIN()函数
 * https://sourceware.org/bugzilla/show_bug.cgi?id=24289
 */
MEMORY {
        /* boot header */
        /* boot_hdr_mr (rx): o = ORIGIN(FLASH), l = 8k */
        boot_hdr_mr (rx): o = 0x60000000, l = 8k

        /* vectors lma & vma */
        /* vctbl_lmr (rx): o = ORIGIN(boot_hdr_mr) + LENGTH(boot_hdr_mr), l = 1k */
        vctbl_lmr (rx): o = 0x60002000, l = 1k
        /* vctbl_vmr (rw): o = ORIGIN(vctbl_lmr), l = LENGTH(vctbl_lmr) */
        vctbl_vmr (rw): o = 0x60002000, l = 1k

        /* image description */
        /* image_description_mr (rx): o = ORIGIN(vctbl_lmr) + LENGTH(vctbl_lmr), l = 64 */
        image_description_mr (rx): o = 0x60002400, l = 64

        /* Code */
        /* code_mr (rx):
                o = ORIGIN(image_description_mr) + LENGTH(image_description_mr),
                l = LENGTH(FLASH) - LENGTH(image_description_mr)
                                  - LENGTH(vctbl_lmr)
                                  - LENGTH(boot_hdr_mr) */
        code_mr (rx): o = 0x60002440, l = 0x1FFDBC0

        /* Data */
        /* data_mr (rwx): o = ORIGIN(DTCM), l = LENGTH(DTCM) - 16k */
        data_mr (rwx): o = 0x20000000, l = 112k

        /* kernel stack */
        /* xwos_stk_mr (rw):
                o = ORIGIN(data_mr) + LENGTH(data_mr),
                l = LENGTH(DTCM) - LENGTH(data_mr) */
        xwos_stk_mr (rw): o = 0x2001C000, l = 16k

        /* itcm_mr (rwx): o = ORIGIN(ITCM), l = LENGTH(ITCM) */
        itcm_mr (rwx): o = 0, l = 0x20000
        /* ocram_mr (rwx): o = ORIGIN(OCRAM), l = LENGTH(OCRAM) */
        ocram_mr (rwx): o = 0x20200000, l = 0x40000
        ncache_mr (rwx): o = 0x81e00000, l = 0x200000
}

INCLUDE soc.lds
