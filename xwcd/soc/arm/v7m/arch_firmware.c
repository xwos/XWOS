/**
 * @file
 * @brief 架构描述层：Firmware信息
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

#include <xwcd/soc/arm/v7m/arch_firmware.h>
#include <string.h>

static __xwbsp_code __xwcc_naked
void arch_goto_firmware(__xwcc_unused void (*entry)(void), __xwcc_unused xwstk_t * sp)
{
        __asm__ volatile(".syntax       unified");
        __asm__ volatile("      cpsid   i");
        __asm__ volatile("      mov     r2, #0");
        __asm__ volatile("      msr     control, r2");
        __asm__ volatile("      msr     msp, r1");
        __asm__ volatile("      orr     r0, #1");
        __asm__ volatile("      dsb");
        __asm__ volatile("      isb");
        __asm__ volatile("      bx      r0");
}

__xwbsp_code
xwer_t arch_boot_firmware(void * firmware, xwsz_t info_offset, const char * tailflag)
{
        xwer_t rc;
        const struct firmware_info * fmwi;
        const struct firmware_tail * fmwt;
        bool head_matched;
        bool tail_matched;
        bool flag_matched;

        fmwi = (const struct firmware_info *)(((xwptr_t)firmware) + info_offset);
        head_matched = !!((xwptr_t)fmwi->head == (xwptr_t)firmware);
        tail_matched = !!((((xwptr_t)fmwi->tail_flag_addr) + 0x20U) ==
                          (xwptr_t)fmwi->end_addr);
        if ((head_matched) && (tail_matched)) {
                fmwt = fmwi->tail_flag_addr;
                flag_matched = !strcmp(fmwt->flag, tailflag);
                if ((flag_matched) && (fmwi->entry)) {
                        rc = XWOK;
                        arch_goto_firmware(fmwi->entry, fmwi->sp);
                        /* never return to here. */
                } else {
                        rc = -EFAULT;
                }
        } else {
                rc = -EFAULT;
        }
        return rc;
}
