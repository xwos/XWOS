/**
 * @file
 * @brief STM32CUBE：设备栈初始化
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <bm/stm32cube/standard.h>
#include <xwos/mm/common.h>
#include <xwos/mm/bma.h>
#include <xwmd/ds/xwds.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/soc/chip.h>
#include <xwmd/ds/uart/dma.h>
#include <bm/stm32cube/xwac/xwds/stm32cube.h>
#include <bm/stm32cube/xwac/xwds/init.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
xwer_t stm32cube_xwds_soc_start(void);

static
xwer_t stm32cube_xwds_soc_stop(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t stm32cube_xwds_ll_start(void)
{
        xwer_t rc;

        xwds_init(&stm32cube_ds);

        rc = stm32cube_xwds_soc_start();
        if (__unlikely(rc < 0)) {
                goto err_soc_start;
        }

        rc = stm32cube_xwds_uart_start();
        if (rc < 0) {
                goto err_xwds_uart_start;
        }
        return OK;

err_xwds_uart_start:
        BDL_BUG();
err_soc_start:
        BDL_BUG();
        return rc;
}

xwer_t stm32cube_xwds_ll_stop(void)
{
        xwer_t rc;

        rc = stm32cube_xwds_uart_stop();
        if (rc < 0) {
                goto err_xwds_uart_stop;
        }

        rc = stm32cube_xwds_soc_stop();
        if (__unlikely(rc < 0)) {
                goto err_soc_stop;
        }
        return OK;

err_xwds_uart_stop:
        BDL_BUG();
err_soc_stop:
        BDL_BUG();
        return rc;
}

static
xwer_t stm32cube_xwds_soc_start(void)
{
        xwer_t rc;

        xwds_soc_construct(&stm32cube_soc_cb);
        rc = xwds_device_probe(&stm32cube_ds,
                               xwds_static_cast(struct xwds_device *,
                                                &stm32cube_soc_cb),
                               NULL);
        if (__unlikely(rc < 0)) {
                goto err_dev_probe;
        }
        rc = xwds_device_start(xwds_static_cast(struct xwds_device *,
                                                &stm32cube_soc_cb));
        if (__unlikely(rc < 0)) {
                goto err_dev_start;
        }

        return OK;

err_dev_start:
        xwds_device_remove(xwds_static_cast(struct xwds_device *, &stm32cube_soc_cb));
err_dev_probe:
        xwds_soc_destruct(&stm32cube_soc_cb);
        return rc;
}

static
xwer_t stm32cube_xwds_soc_stop(void)
{
        xwds_device_stop(xwds_static_cast(struct xwds_device *,
                                          &stm32cube_soc_cb));
        xwds_device_remove(xwds_static_cast(struct xwds_device *,
                                            &stm32cube_soc_cb));
        xwds_soc_destruct(&stm32cube_soc_cb);
        return OK;
}

xwer_t stm32cube_xwds_uart_start(void)
{
        xwer_t rc;

        xwds_dmauartc_construct(&stm32cube_usart1_cb);
        rc = xwds_device_probe(&stm32cube_ds,
                               xwds_static_cast(struct xwds_device *,
                                                &stm32cube_usart1_cb),
                               NULL);
        if (__unlikely(rc < 0)) {
                goto err_dev_probe;
        }
        rc = xwds_device_start(xwds_static_cast(struct xwds_device *,
                                                &stm32cube_usart1_cb));
        if (__unlikely(rc < 0)) {
                goto err_dev_start;
        }

        return OK;

err_dev_start:
        xwds_device_remove(xwds_static_cast(struct xwds_device *,
                                            &stm32cube_usart1_cb));
err_dev_probe:
        xwds_dmauartc_destruct(&stm32cube_usart1_cb);
        return rc;
}

xwer_t stm32cube_xwds_uart_stop(void)
{
        xwds_device_stop(xwds_static_cast(struct xwds_device *,
                                          &stm32cube_usart1_cb));
        xwds_device_remove(xwds_static_cast(struct xwds_device *,
                                            &stm32cube_usart1_cb));
        xwds_dmauartc_destruct(&stm32cube_usart1_cb);
        return OK;
}