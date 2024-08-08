/**
 * @file
 * @brief XWOS移植实现层：系统硬件定时器
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
#include <xwos/ospl/syshwt.h>
#include <xwcd/soc/arm/v7m/arch_systick.h>

__xwbsp_code
xwer_t xwospl_syshwt_init(struct xwospl_syshwt * hwt)
{
        arch_systick_init(hwt);
        return XWOK;
}

__xwbsp_code
xwer_t xwospl_syshwt_start(struct xwospl_syshwt * hwt)
{
        return arch_systick_start(hwt);
}

__xwbsp_code
xwer_t xwospl_syshwt_stop(struct xwospl_syshwt * hwt)
{
        return arch_systick_stop(hwt);
}

__xwbsp_code
xwtm_t xwospl_syshwt_get_timeconfetti(struct xwospl_syshwt * hwt)
{
        return arch_systick_get_timeconfetti(hwt);
}
