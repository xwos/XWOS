/**
 * @file
 * @brief architecture image
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <string.h>
#include <arch_image.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwbsp_code
void arch_goto_image(xwstk_t * sp, void (*entry)(void));

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwbsp_code __naked
void arch_goto_image(__maybe_unused xwstk_t * sp, __maybe_unused void (*entry)(void))
{
        __asm__ volatile("      cpsid   i");
        __asm__ volatile("      mov     r2, #0");
        __asm__ volatile("      msr     control, r2");
        __asm__ volatile("      msr     msp, r0");
        __asm__ volatile("      bx      r1");
}

__xwbsp_code
xwer_t arch_boot_image(void * addr)
{
        xwer_t rc;
        const struct arch_image_description * imgd;
        const struct arch_image_tail * imgt;
        bool head_matched;
        bool tail_matched;
        bool flag_matched;

        imgd = (const struct arch_image_description *)(((xwptr_t)addr) +
                                                       ARCHCFG_IMAGE_FLAG_OFFSET);

        head_matched = !!((xwptr_t)imgd->head == (xwptr_t)addr);
        tail_matched = !!((((xwptr_t)imgd->tail_flag_addr) + 0x20U) ==
                          (xwptr_t)imgd->end_addr);
        if ((head_matched) && (tail_matched)) {
                imgt = imgd->tail_flag_addr;
                flag_matched = !strcmp(imgt->flag, ARCHCFG_IMAGE_TAILFLAG);
                if ((flag_matched) && (imgd->entry)) {
                        rc = XWOK;
                        arch_goto_image(*((xwstk_t **)imgd->head), imgd->entry);
                        /* never return to here. */
                } else {
                        rc = -EFAULT;
                }
        } else {
                rc = -EFAULT;
        }
        return rc;
}
