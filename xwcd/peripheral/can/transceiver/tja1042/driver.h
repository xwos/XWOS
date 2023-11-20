/**
 * @file
 * @brief CAN Transceiver TJA1042驱动
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

#ifndef __xwcd_peripheral_can_transceiver_tja1042_driver_h__
#define __xwcd_peripheral_can_transceiver_tja1042_driver_h__

#include <xwos/standard.h>
#include <xwcd/ds/can/transceiver.h>

struct xwds_cantrcv_tja1042_cfg {
        const struct xwds_resource_gpio *gpiorsc_stb; /**< STANDBY GPIO资源 */
        const struct xwds_resource_gpio *gpiorsc_eirq; /**< 唤醒外部中断 GPIO资源 */
        const xwsq_t eirq; /**< 唤醒外部中断资源 */
};

extern __xwbsp_rodata const struct xwds_cantrcv_driver tja1042_cantrcv_drv;

void tja1042_cantrcv_eirq_wkup(struct xwds_soc *soc, xwid_t eiid,
                               xwds_eirq_arg_t arg);

#endif /* xwcd/peripheral/can/transceiver/tja1042/driver.h */
