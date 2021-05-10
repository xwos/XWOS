/**
 * @file
 * @brief 架构描述层：对称多核系统的中断控制器配置
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

#include <xwos/standard.h>
#include <xwos/ospl/irq.h>
#include <mp_nvic_drv.h>

__xwbsp_rodata const struct cortexm_nvic_cfg armv7_nvic_cfg = {
        .subprio = SOCCFG_NVIC_SUBPRIO_BITIDX,
        .basepri = 0,
};

__xwbsp_data struct xwmp_irqc xwospl_irqc[CPUCFG_CPU_NUM] = {
        [0] = {
                .name = "arm.m.nvic",
                .drv = &cortexm_nvic_drv,
                .irqs_num = (SOCCFG_IRQ_NUM + ARCHCFG_IRQ_NUM),
                .ivt = &xwospl_ivt,
                .idvt = &xwospl_idvt,
                .soc_cfg = &armv7_nvic_cfg,
                .data = NULL,
        }
};
