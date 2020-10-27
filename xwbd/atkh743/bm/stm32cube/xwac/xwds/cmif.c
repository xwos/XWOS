/**
 * @file
 * @brief STM32CUBE设备栈：子模块接口
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
#include <xwmd/ds/xwds.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/soc/chip.h>
#include <xwmd/ds/uart/dma.h>
#include <bm/stm32cube/xwac/xwds/device.h>
#include <bm/stm32cube/xwac/xwds/cmif.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

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
        if (__xwcc_unlikely(rc < 0)) {
                goto err_soc_start;
        }

        return XWOK;

err_soc_start:
        BDL_BUG();
        return rc;
}

xwer_t stm32cube_xwds_ll_stop(void)
{
        xwer_t rc;

        rc = stm32cube_xwds_soc_stop();
        if (__xwcc_unlikely(rc < 0)) {
                goto err_soc_stop;
        }

        return XWOK;

err_soc_stop:
        BDL_BUG();
        return rc;
}

xwer_t stm32cube_xwds_start(void)
{
        xwer_t rc;

        rc = stm32cube_xwds_uart_start();
        if (rc < 0) {
                goto err_uart_start;
        }

        rc = stm32cube_xwds_i2c_start();
        if (rc < 0) {
                goto err_i2c_start;
        }

        rc = stm32cube_xwds_eeprom_start();
        if (rc < 0) {
                goto err_eeprom_start;
        }

        return XWOK;

err_eeprom_start:
        stm32cube_xwds_i2c_stop();
err_i2c_start:
        stm32cube_xwds_uart_stop();
err_uart_start:
        return rc;
}

xwer_t stm32cube_xwds_stop(void)
{
        xwer_t rc;

        rc = stm32cube_xwds_eeprom_stop();
        if (rc < 0) {
                goto err_eeprom_stop;
        }

        rc = stm32cube_xwds_i2c_stop();
        if (rc < 0) {
                goto err_i2c_stop;
        }

        rc = stm32cube_xwds_uart_stop();
        if (rc < 0) {
                goto err_uart_stop;
        }

        return XWOK;

err_uart_stop:
err_i2c_stop:
err_eeprom_stop:
        return rc;
}

xwer_t stm32cube_xwds_soc_start(void)
{
        xwer_t rc;

        xwds_soc_construct(&stm32cube_soc_cb);
        rc = xwds_device_probe(&stm32cube_ds,
                               xwds_static_cast(struct xwds_device *,
                                                &stm32cube_soc_cb),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_probe;
        }
        rc = xwds_device_start(xwds_static_cast(struct xwds_device *,
                                                &stm32cube_soc_cb));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_start;
        }

        return XWOK;

err_dev_start:
        xwds_device_remove(xwds_static_cast(struct xwds_device *, &stm32cube_soc_cb));
err_dev_probe:
        xwds_soc_destruct(&stm32cube_soc_cb);
        return rc;
}

xwer_t stm32cube_xwds_soc_stop(void)
{
        xwds_device_stop(xwds_static_cast(struct xwds_device *,
                                          &stm32cube_soc_cb));
        xwds_device_remove(xwds_static_cast(struct xwds_device *,
                                            &stm32cube_soc_cb));
        xwds_soc_destruct(&stm32cube_soc_cb);
        return XWOK;
}

xwer_t stm32cube_xwds_uart_start(void)
{
        xwer_t rc;

        xwds_dmauartc_construct(&stm32cube_usart1_cb);
        rc = xwds_device_probe(&stm32cube_ds,
                               xwds_static_cast(struct xwds_device *,
                                                &stm32cube_usart1_cb),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_usart1_probe;
        }
        rc = xwds_device_start(xwds_static_cast(struct xwds_device *,
                                                &stm32cube_usart1_cb));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_usart1_start;
        }

        xwds_dmauartc_construct(&stm32cube_usart2_cb);
        rc = xwds_device_probe(&stm32cube_ds,
                               xwds_static_cast(struct xwds_device *,
                                                &stm32cube_usart2_cb),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_usart2_probe;
        }
        rc = xwds_device_start(xwds_static_cast(struct xwds_device *,
                                                &stm32cube_usart2_cb));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_usart2_start;
        }

        return XWOK;

err_usart2_start:
        xwds_device_remove(xwds_static_cast(struct xwds_device *,
                                            &stm32cube_usart2_cb));
err_usart2_probe:
        xwds_dmauartc_destruct(&stm32cube_usart2_cb);

        xwds_device_stop(xwds_static_cast(struct xwds_device *,
                                          &stm32cube_usart1_cb));
err_usart1_start:
        xwds_device_remove(xwds_static_cast(struct xwds_device *,
                                            &stm32cube_usart1_cb));
err_usart1_probe:
        xwds_dmauartc_destruct(&stm32cube_usart1_cb);
        return rc;
}

xwer_t stm32cube_xwds_uart_stop(void)
{
        xwds_device_stop(xwds_static_cast(struct xwds_device *,
                                          &stm32cube_usart2_cb));
        xwds_device_remove(xwds_static_cast(struct xwds_device *,
                                            &stm32cube_usart2_cb));
        xwds_dmauartc_destruct(&stm32cube_usart2_cb);

        xwds_device_stop(xwds_static_cast(struct xwds_device *,
                                          &stm32cube_usart1_cb));
        xwds_device_remove(xwds_static_cast(struct xwds_device *,
                                            &stm32cube_usart1_cb));
        xwds_dmauartc_destruct(&stm32cube_usart1_cb);
        return XWOK;
}

xwer_t stm32cube_xwds_i2c_start(void)
{
        xwer_t rc;

        xwds_i2cm_construct(&stm32cube_i2c2m_cb);
        rc = xwds_device_probe(&stm32cube_ds,
                               xwds_static_cast(struct xwds_device *,
                                                &stm32cube_i2c2m_cb),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_probe;
        }
        rc = xwds_device_start(xwds_static_cast(struct xwds_device *,
                                                &stm32cube_i2c2m_cb));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_start;
        }

        return XWOK;

err_dev_start:
        xwds_device_remove(xwds_static_cast(struct xwds_device *,
                                            &stm32cube_i2c2m_cb));
err_dev_probe:
        xwds_i2cm_destruct(&stm32cube_i2c2m_cb);
        return rc;
}

xwer_t stm32cube_xwds_i2c_stop(void)
{
        xwds_device_stop(xwds_static_cast(struct xwds_device *,
                                          &stm32cube_i2c2m_cb));
        xwds_device_remove(xwds_static_cast(struct xwds_device *,
                                            &stm32cube_i2c2m_cb));
        xwds_i2cm_destruct(&stm32cube_i2c2m_cb);
        return XWOK;
}

xwer_t stm32cube_xwds_eeprom_start(void)
{
        xwer_t rc;

        xwds_eeprom_construct(&stm32cube_at24c02_cb);
        rc = xwds_device_probe(&stm32cube_ds,
                               xwds_static_cast(struct xwds_device *,
                                                &stm32cube_at24c02_cb),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_probe;
        }
        rc = xwds_device_start(xwds_static_cast(struct xwds_device *,
                                                &stm32cube_at24c02_cb));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_start;
        }

        return XWOK;

err_dev_start:
        xwds_device_remove(xwds_static_cast(struct xwds_device *,
                                            &stm32cube_at24c02_cb));
err_dev_probe:
        xwds_eeprom_destruct(&stm32cube_at24c02_cb);
        return rc;
}

xwer_t stm32cube_xwds_eeprom_stop(void)
{
        xwds_device_stop(xwds_static_cast(struct xwds_device *,
                                          &stm32cube_at24c02_cb));
        xwds_device_remove(xwds_static_cast(struct xwds_device *,
                                            &stm32cube_at24c02_cb));
        xwds_eeprom_destruct(&stm32cube_at24c02_cb);
        return XWOK;
}
