/**
 * @file
 * @brief 板级描述层：低功耗模式内存备份数据
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

#ifndef __bdl_bkup_h__
#define __bdl_bkup_h__

#include <xwos/standard.h>

enum bdl_softreset_reason_em {
        BDL_SOFTRST_REASON_NONE = 0,
        BDL_SOFTRST_REASON_PROGRAM,
};

struct bdl_bkup {
        union {
                struct {
                        xwu16_t fes;
                        xwu16_t des;
                } reg;
                xwu32_t w;
        } soc_reset_flags;
        struct {
                xwu32_t nsr; /**< non-maskable IRQ */
                xwu32_t wisr; /**< wakeup status flags */
        } soc_wkup_flags;
        xwu32_t softreset_reason;
};

extern __bkup struct bdl_bkup bdl_bkup_data;

#endif /* bdl/bkup.h */
