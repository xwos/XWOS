/**
 * @file
 * @brief 板级描述层：XWOS适配层：XWOS设备栈：设备
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

#include "board/std.h"
#include "board/xwac/xwds/device.h"

struct xwds rpi4bxwds;

void rpi4bxwds_init(void)
{
        xwds_init(&rpi4bxwds);
}

void rpi4bxwds_fini(void)
{
}

xwer_t rpi4bxwds_uart_init(void)
{
        xwer_t rc;

        xwds_uartc_construct(&rpi4bxwds_uart5);
        rc = xwds_device_probe(&rpi4bxwds,
                               xwds_cast(struct xwds_device *, &rpi4bxwds_uart5),
                               NULL);
        if (rc < 0) {
                goto err_uart5_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &rpi4bxwds_uart5));
        if (rc < 0) {
                goto err_uart5_start;
        }
        return XWOK;

err_uart5_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &rpi4bxwds_uart5));
err_uart5_probe:
        xwds_uartc_destruct(&rpi4bxwds_uart5);
        return rc;
}

xwer_t rpi4bxwds_uart_fini(void)
{
        xwds_device_stop(xwds_cast(struct xwds_device *, &rpi4bxwds_uart5));
        xwds_device_remove(xwds_cast(struct xwds_device *, &rpi4bxwds_uart5));
        xwds_uartc_destruct(&rpi4bxwds_uart5);
        return XWOK;
}
