/**
 * @file
 * @brief XWOS移植实现层：线程本地存储
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

#include <xwos/standard.h>
#include <xwos/ospl/skd.h>

extern xwu8_t tdata_lma_base[];
extern xwu8_t tdata_vma_base[];
extern xwu8_t tdata_vma_end[];

extern xwu8_t tbss_vma_base[];
extern xwu8_t tbss_vma_end[];
extern xwu8_t tbss_vma_offset[];

extern xwu8_t tls_align[];

__xwbsp_code
void soc_tls_relocate(void * tls)
{
        xwsz_t cnt, i;
        xwu8_t * src;
        xwu8_t * dst;

        src = tdata_lma_base;
        dst = tls;
        if (dst != src) {
                cnt =  (xwsz_t)tdata_vma_end - (xwsz_t)tdata_vma_base;
                for (i = 0; i < cnt; i++) {
                        *dst = *src;
                        dst++;
                        src++;
                }
        }

        dst = tls + (xwptr_t)tbss_vma_offset;
        cnt = (xwsz_t)tbss_vma_end - (xwsz_t)tbss_vma_base;
        for (i = 0; i < cnt; i++) {
                *dst = 0;
                dst++;
        }
}

__xwbsp_code
void xwospl_tls_init(struct xwospl_skdobj_stack * stk)
{
        xwsz_t tls_size;

        tls_size = (xwsz_t)tbss_vma_end - (xwsz_t)tdata_vma_base;
        stk->guard_base = (xwstk_t *)((xwptr_t)stk->base +
                                      XWBOP_ALIGN(tls_size, (xwsz_t)tls_align));
        soc_tls_relocate(stk->tls);
}
