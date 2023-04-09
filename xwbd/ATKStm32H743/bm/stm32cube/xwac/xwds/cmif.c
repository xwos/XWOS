/**
 * @file
 * @brief STM32CUBE设备栈：子模块接口
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

#include <bm/stm32cube/standard.h>
#include <xwos/osal/pm.h>
#include <xwcd/ds/xwds.h>
#include <xwcd/ds/device.h>
#include <xwcd/ds/soc/chip.h>
#include <xwcd/ds/uart/controller.h>
#include <xwcd/ds/spi/master.h>
#include <bm/stm32cube/xwac/xwds/device.h>
#include <bm/stm32cube/xwac/xwds/pm.h>
#include <bm/stm32cube/xwac/xwds/cmif.h>

/**
 * @brief 探测设备栈
 */
xwer_t stm32cube_xwds_probe(void)
{
        xwer_t rc;

        xwds_init(&stm32cube_ds);

        rc = stm32cube_xwds_soc_start();
        if (__xwcc_unlikely(rc < 0)) {
                goto err_soc_start;
        }

        xwos_pm_set_cb(stm32cube_pm_resume,
                       stm32cube_pm_suspend,
                       stm32cube_pm_wakeup,
                       stm32cube_pm_sleep,
                       NULL);

        return XWOK;

err_soc_start:
        BDL_BUG();
        return rc;
}

/**
 * @brief 删除设备栈
 */
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

/**
 * @brief 启动设备栈
 */
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

        rc = stm32cube_xwds_spi_start();
        if (rc < 0) {
                goto err_spi_start;
        }

        return XWOK;

err_spi_start:
        stm32cube_xwds_eeprom_stop();
err_eeprom_start:
        stm32cube_xwds_i2c_stop();
err_i2c_start:
        stm32cube_xwds_uart_stop();
err_uart_start:
        return rc;
}

/**
 * @brief 停止设备栈
 */
xwer_t stm32cube_xwds_stop(void)
{
        xwer_t rc;

        rc = stm32cube_xwds_spi_stop();
        if (rc < 0) {
                goto err_spi_stop;
        }

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
err_spi_stop:
        return rc;
}

/**
 * @brief 启动SOC
 */
xwer_t stm32cube_xwds_soc_start(void)
{
        xwer_t rc;

        xwds_soc_construct(&stm32soc);
        rc = xwds_device_probe(&stm32cube_ds,
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

/**
 * @brief 停止SOC
 */
xwer_t stm32cube_xwds_soc_stop(void)
{
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32soc));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32soc));
        xwds_soc_destruct(&stm32soc);
        return XWOK;
}

/**
 * @brief 启动UART
 */
xwer_t stm32cube_xwds_uart_start(void)
{
        xwer_t rc;

        xwds_uartc_construct(&stm32usart1);
        rc = xwds_device_probe(&stm32cube_ds,
                               xwds_cast(struct xwds_device *, &stm32usart1),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_usart1_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32usart1));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_usart1_start;
        }

        xwds_uartc_construct(&stm32usart2);
        rc = xwds_device_probe(&stm32cube_ds,
                               xwds_cast(struct xwds_device *, &stm32usart2),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_usart2_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32usart2));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_usart2_start;
        }
        return XWOK;

err_usart2_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32usart2));
err_usart2_probe:
        xwds_uartc_destruct(&stm32usart2);

        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32usart1));
err_usart1_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32usart1));
err_usart1_probe:
        xwds_uartc_destruct(&stm32usart1);
        return rc;
}

/**
 * @brief 停止UART
 */
xwer_t stm32cube_xwds_uart_stop(void)
{
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32usart2));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32usart2));
        xwds_uartc_destruct(&stm32usart2);

        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32usart1));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32usart1));
        xwds_uartc_destruct(&stm32usart1);
        return XWOK;
}

/**
 * @brief 启动I2C
 */
xwer_t stm32cube_xwds_i2c_start(void)
{
        xwer_t rc;

        xwds_i2cm_construct(&stm32i2c2m);
        rc = xwds_device_probe(&stm32cube_ds,
                               xwds_cast(struct xwds_device *, &stm32i2c2m),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32i2c2m));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_start;
        }

        return XWOK;

err_dev_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32i2c2m));
err_dev_probe:
        xwds_i2cm_destruct(&stm32i2c2m);
        return rc;
}

/**
 * @brief 停止I2C
 */
xwer_t stm32cube_xwds_i2c_stop(void)
{
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32i2c2m));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32i2c2m));
        xwds_i2cm_destruct(&stm32i2c2m);
        return XWOK;
}

/**
 * @brief 启动EEPROM
 */
xwer_t stm32cube_xwds_eeprom_start(void)
{
        xwer_t rc;

        xwds_eeprom_construct(&at24c02);
        rc = xwds_device_probe(&stm32cube_ds,
                               xwds_cast(struct xwds_device *, &at24c02),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &at24c02));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_start;
        }

        return XWOK;

err_dev_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &at24c02));
err_dev_probe:
        xwds_eeprom_destruct(&at24c02);
        return rc;
}

/**
 * @brief 停止EEPROM
 */
xwer_t stm32cube_xwds_eeprom_stop(void)
{
        xwds_device_stop(xwds_cast(struct xwds_device *, &at24c02));
        xwds_device_remove(xwds_cast(struct xwds_device *, &at24c02));
        xwds_eeprom_destruct(&at24c02);
        return XWOK;
}

/**
 * @brief 启动SPI
 */
xwer_t stm32cube_xwds_spi_start(void)
{
        xwer_t rc;

        /* SPI1 */
        xwds_spim_construct(&stm32spi1m);
        rc = xwds_device_probe(&stm32cube_ds,
                               xwds_cast(struct xwds_device *, &stm32spi1m),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_spi1_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32spi1m));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_spi1_start;
        }

        /* SPI2 */
        xwds_spim_construct(&stm32spi2m);
        rc = xwds_device_probe(&stm32cube_ds,
                               xwds_cast(struct xwds_device *, &stm32spi2m),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_spi2_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32spi2m));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_spi2_start;
        }

        return XWOK;

        /* SPI2 */
err_spi2_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32spi2m));
err_spi2_probe:
        xwds_spim_destruct(&stm32spi2m);

        /* SPI1 */
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32spi1m));
err_spi1_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32spi1m));
err_spi1_probe:
        xwds_spim_destruct(&stm32spi1m);
        return rc;
}

/**
 * @brief 停止SPI
 */
xwer_t stm32cube_xwds_spi_stop(void)
{
        /* SPI2 */
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32spi2m));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32spi2m));
        xwds_spim_destruct(&stm32spi2m);

        /* SPI1 */
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32spi1m));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32spi1m));
        xwds_spim_destruct(&stm32spi1m);
        return XWOK;
}
