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
#include <xwcd/ds/xwds.h>
#include <xwcd/ds/device.h>
#include <xwcd/ds/soc/chip.h>
#include <xwcd/ds/uart/controller.h>
#include "board/xwac/xwds/device.h"

struct xwds stm32xwds;

xwer_t stm32xwds_soc_init(void)
{
        xwer_t rc;

        xwds_soc_construct(&stm32xwds_soc);
        rc = xwds_device_probe(&stm32xwds,
                               xwds_cast(struct xwds_device *, &stm32xwds_soc),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32xwds_soc));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_start;
        }
        return XWOK;

err_dev_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32xwds_soc));
err_dev_probe:
        xwds_soc_destruct(&stm32xwds_soc);
        return rc;
}

xwer_t stm32xwds_soc_fini(void)
{
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32xwds_soc));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32xwds_soc));
        xwds_soc_destruct(&stm32xwds_soc);
        return XWOK;
}

xwer_t stm32xwds_uart_init(void)
{
        xwer_t rc;

        xwds_uartc_construct(&stm32xwds_usart1);
        rc = xwds_device_probe(&stm32xwds,
                               xwds_cast(struct xwds_device *, &stm32xwds_usart1),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_usart1_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32xwds_usart1));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_usart1_start;
        }
        xwds_uartc_construct(&stm32xwds_usart3);
        rc = xwds_device_probe(&stm32xwds,
                               xwds_cast(struct xwds_device *, &stm32xwds_usart3),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_usart3_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32xwds_usart3));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_usart3_start;
        }
        return XWOK;

err_usart3_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32xwds_usart3));
err_usart3_probe:
        xwds_uartc_destruct(&stm32xwds_usart3);

        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32xwds_usart1));
err_usart1_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32xwds_usart1));
err_usart1_probe:
        xwds_uartc_destruct(&stm32xwds_usart1);
        return rc;
}

xwer_t stm32xwds_uart_fini(void)
{
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32xwds_usart3));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32xwds_usart3));
        xwds_uartc_destruct(&stm32xwds_usart3);

        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32xwds_usart1));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32xwds_usart1));
        xwds_uartc_destruct(&stm32xwds_usart1);
        return XWOK;
}

void stm32xwds_init(void)
{
        xwds_init(&stm32xwds);
}

void stm32xwds_fini(void)
{
}
