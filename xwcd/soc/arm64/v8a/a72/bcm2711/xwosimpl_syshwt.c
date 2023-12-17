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
#include <xwos/ospl/skd.h>
#include <xwos/ospl/syshwt.h>
#include <xwcd/soc/arm64/v8a/arch_gic2.h>
#include <xwcd/soc/arm64/v8a/arch_timer.h>

__xwbsp_code
void xwospl_syshwt_timeout_callback(xwid_t cpuid)
{
        struct xwospl_skd * xwskd;
        XWOS_UNUSED(cpuid);
        armv8a_cnthp_reload(XWOSPL_SYSHWT_HZ);
        xwskd = xwosplcb_skd_get_lc();
        xwosplcb_syshwt_task(&xwskd->tt.hwt);
}

__xwbsp_rodata const xwirq_t armv8a_cnthp_irqrsc[1] = {
        ARMV8A_IRQn_CNTHP,
};

__xwbsp_code
xwer_t xwospl_syshwt_init(struct xwospl_syshwt * hwt)
{
        hwt->irqrsc = armv8a_cnthp_irqrsc;
        hwt->irqs_num = 1;
        return XWOK;
}

__xwbsp_code
xwer_t xwospl_syshwt_start(struct xwospl_syshwt * hwt)
{
        XWOS_UNUSED(hwt);
        armv8a_cnthp_init(XWOSPL_SYSHWT_HZ, xwospl_syshwt_timeout_callback);
        armv8a_gic_irq_dump(ARMV8A_IRQn_CNTHP);
        return XWOK;
}

__xwbsp_code
xwer_t xwospl_syshwt_stop(struct xwospl_syshwt * hwt)
{
        XWOS_UNUSED(hwt);
        armv8a_cnthp_fini();
        return XWOK;
}

__xwbsp_code
xwtm_t xwospl_syshwt_get_timeconfetti(struct xwospl_syshwt * hwt)
{
        XWOS_UNUSED(hwt);
        return armv8a_cnthp_get_timeconfetti(XWOSPL_SYSHWT_HZ);
}
