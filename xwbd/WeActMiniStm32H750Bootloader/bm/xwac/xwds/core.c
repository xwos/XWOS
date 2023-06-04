/**
 * @file
 * @brief 板级描述层：XWOS适配层：XWOS设备栈
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
#include "bm/xwac/xwds/device.h"

xwer_t stm32cube_xwds_soc_start(void)
{
        xwer_t rc;

        xwds_soc_construct(&stm32soc);
        rc = xwds_device_probe(&stm32xwds,
                               xwds_cast(struct xwds_device *, &stm32soc),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32soc));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_start;
        }
        return XWOK;

err_dev_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32soc));
err_dev_probe:
        xwds_soc_destruct(&stm32soc);
        return rc;
}

xwer_t stm32cube_xwds_soc_stop(void)
{
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32soc));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32soc));
        xwds_soc_destruct(&stm32soc);
        return XWOK;
}

xwer_t stm32cube_xwds_uart_start(void)
{
        xwer_t rc;

        xwds_uartc_construct(&stm32usart1);
        rc = xwds_device_probe(&stm32xwds,
                               xwds_cast(struct xwds_device *, &stm32usart1),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_usart1_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32usart1));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_usart1_start;
        }
        xwds_uartc_construct(&stm32usart3);
        rc = xwds_device_probe(&stm32xwds,
                               xwds_cast(struct xwds_device *, &stm32usart3),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_usart3_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32usart3));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_usart3_start;
        }
        return XWOK;

err_usart3_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32usart3));
err_usart3_probe:
        xwds_uartc_destruct(&stm32usart3);

        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32usart1));
err_usart1_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32usart1));
err_usart1_probe:
        xwds_uartc_destruct(&stm32usart1);
        return rc;
}

xwer_t stm32cube_xwds_uart_stop(void)
{
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32usart3));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32usart3));
        xwds_uartc_destruct(&stm32usart3);

        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32usart1));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32usart1));
        xwds_uartc_destruct(&stm32usart1);
        return XWOK;
}

xwer_t stm32cube_xwds_probe(void)
{
        xwer_t rc;

        xwds_init(&stm32xwds);
        rc = stm32cube_xwds_soc_start();
        if (rc < 0) {
                goto err_soc_start;
        }

        return XWOK;

err_soc_start:
        BOARD_BUG();
        return rc;
}

xwer_t stm32cube_xwds_remove(void)
{
        xwer_t rc;

        rc = stm32cube_xwds_soc_stop();
        if (rc < 0) {
                goto err_soc_stop;
        }
        return XWOK;

err_soc_stop:
        BOARD_BUG();
        return rc;
}

xwer_t stm32cube_xwds_start(void)
{
        xwer_t rc;

        rc = stm32cube_xwds_uart_start();
        if (rc < 0) {
                goto err_uart_start;
        }
        return XWOK;

err_uart_start:
        return rc;
}

xwer_t stm32cube_xwds_stop(void)
{
        xwer_t rc;

        rc = stm32cube_xwds_uart_stop();
        if (rc < 0) {
                goto err_uart_stop;
        }
        return XWOK;

err_uart_stop:
        return rc;
}
