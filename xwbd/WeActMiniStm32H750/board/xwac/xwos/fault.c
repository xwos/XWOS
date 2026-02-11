/**
 * @file
 * @brief 板级描述层：XWOS适配层：内核：异常
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

#include "board/std.h"
#include <bm/CmBacktrace/cm_backtrace.h>

__xwbsp_isr
void board_esr_nmi(void)
{
}

__xwbsp_isr
void board_esr_hardfault(xwreg_t lr, xwreg_t sp)
{
        cm_backtrace_fault(lr, sp);
}

__xwbsp_isr
void board_esr_mm(xwreg_t lr, xwreg_t sp)
{
        cm_backtrace_fault(lr, sp);
}

__xwbsp_isr
void board_esr_busfault(xwreg_t lr, xwreg_t sp)
{
        cm_backtrace_fault(lr, sp);
}

__xwbsp_isr
void board_esr_usagefault(xwreg_t lr, xwreg_t sp)
{
        cm_backtrace_fault(lr, sp);
}

__xwbsp_isr
void board_esr_dbgmon(xwreg_t lr, xwreg_t sp)
{
        cm_backtrace_fault(lr, sp);
}
