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
#include <xwos/lib/xwlog.h>
#include <xwos/osal/pm.h>
#include "bm/xwac/xwds/pm.h"
#include "bm/xwac/xwds/device.h"

struct xwds stm32xwds;

xwer_t stm32cube_xwds_soc_init(void)
{
        xwer_t rc;

        xwds_soc_construct(&stm32soc);
        rc = xwds_device_probe(&stm32xwds,
                               xwds_cast(struct xwds_device *, &stm32soc),
                               NULL);
        if (rc < 0) {
                goto err_dev_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32soc));
        if (rc < 0) {
                goto err_dev_start;
        }
        return XWOK;

err_dev_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32soc));
err_dev_probe:
        xwds_soc_destruct(&stm32soc);
        return rc;
}

xwer_t stm32cube_xwds_soc_fini(void)
{
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32soc));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32soc));
        xwds_soc_destruct(&stm32soc);
        return XWOK;
}

xwer_t stm32cube_xwds_uart_init(void)
{
        xwer_t rc;

        xwds_uartc_construct(&stm32usart1);
        rc = xwds_device_probe(&stm32xwds,
                               xwds_cast(struct xwds_device *, &stm32usart1),
                               NULL);
        if (rc < 0) {
                goto err_usart1_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32usart1));
        if (rc < 0) {
                goto err_usart1_start;
        }
        xwds_uartc_construct(&stm32usart3);
        rc = xwds_device_probe(&stm32xwds,
                               xwds_cast(struct xwds_device *, &stm32usart3),
                               NULL);
        if (rc < 0) {
                goto err_usart3_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32usart3));
        if (rc < 0) {
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

xwer_t stm32cube_xwds_uart_fini(void)
{
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32usart3));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32usart3));
        xwds_uartc_destruct(&stm32usart3);

        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32usart1));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32usart1));
        xwds_uartc_destruct(&stm32usart1);
        return XWOK;
}

xwer_t stm32cube_xwds_spi_init(void)
{
        xwer_t rc;

        /* SPI1 */
        xwds_spim_construct(&stm32spi1m);
        rc = xwds_device_probe(&stm32xwds,
                               xwds_cast(struct xwds_device *, &stm32spi1m),
                               NULL);
        if (rc < 0) {
                goto err_spi1_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32spi1m));
        if (rc < 0) {
                goto err_spi1_start;
        }

        /* SPI4 */
        xwds_spim_construct(&stm32spi4m);
        rc = xwds_device_probe(&stm32xwds,
                               xwds_cast(struct xwds_device *, &stm32spi4m),
                               NULL);
        if (rc < 0) {
                goto err_spi4_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32spi4m));
        if (rc < 0) {
                goto err_spi4_start;
        }

        return XWOK;

err_spi4_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32spi4m));
err_spi4_probe:
        xwds_spim_destruct(&stm32spi4m);
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32spi1m));
err_spi1_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32spi1m));
err_spi1_probe:
        xwds_spim_destruct(&stm32spi1m);
        return rc;
}

xwer_t stm32cube_xwds_spi_fini(void)
{
        /* SPI4 */
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32spi4m));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32spi4m));
        xwds_spim_destruct(&stm32spi4m);

        /* SPI1 */
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32spi1m));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32spi1m));
        xwds_spim_destruct(&stm32spi1m);
        return XWOK;
}

xwer_t stm32cube_xwds_st7735_init(void)
{
        xwer_t rc;

        xwds_st7735_construct(&st7735);
        rc = xwds_device_probe(&stm32xwds,
                               xwds_cast(struct xwds_device *, &st7735),
                               NULL);
        if (rc < 0) {
                goto err_st7735_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &st7735));
        if (rc < 0) {
                goto err_st7735_start;
        }

        return XWOK;

err_st7735_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &st7735));
err_st7735_probe:
        xwds_st7735_destruct(&st7735);
        return rc;
}

xwer_t stm32cube_xwds_st7735_fini(void)
{
        xwds_device_stop(xwds_cast(struct xwds_device *, &st7735));
        xwds_device_remove(xwds_cast(struct xwds_device *, &st7735));
        xwds_st7735_destruct(&st7735);
        return XWOK;
}

xwer_t stm32cube_xwds_w25q64jv_init(void)
{
        xwer_t rc;

        xwds_w25qxx_construct(&w25q64jv);
        rc = xwds_device_probe(&stm32xwds,
                               xwds_cast(struct xwds_device *, &w25q64jv),
                               NULL);
        if (rc < 0) {
                goto err_w25q64jv_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &w25q64jv));
        if (rc < 0) {
                goto err_w25q64jv_start;
        }
        return XWOK;

err_w25q64jv_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &w25q64jv));
err_w25q64jv_probe:
        xwds_w25qxx_destruct(&w25q64jv);
        return rc;
}

xwer_t stm32cube_xwds_w25q64jv_fini(void)
{
        xwds_device_stop(xwds_cast(struct xwds_device *, &w25q64jv));
        xwds_device_remove(xwds_cast(struct xwds_device *, &w25q64jv));
        xwds_w25qxx_destruct(&w25q64jv);
        return XWOK;
}

xwer_t stm32cube_xwds_i2c_init(void)
{
        xwer_t rc;

        /* I2C1 */
        xwds_i2cm_construct(&stm32i2c1m);
        rc = xwds_device_probe(&stm32xwds,
                               xwds_cast(struct xwds_device *, &stm32i2c1m),
                               NULL);
        if (rc < 0) {
                goto err_i2c1_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32i2c1m));
        if (rc < 0) {
                goto err_i2c1_start;
        }

        return XWOK;

err_i2c1_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32i2c1m));
err_i2c1_probe:
        xwds_i2cm_destruct(&stm32i2c1m);
        return rc;
}

xwer_t stm32cube_xwds_i2c_fini(void)
{
        /* I2C1 */
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32i2c1m));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32i2c1m));
        xwds_i2cm_destruct(&stm32i2c1m);
        return XWOK;
}

xwer_t stm32cube_xwds_eeprom_init(void)
{
        xwer_t rc;

        xwds_eeprom_construct(&stm32eeprom_256k);
        rc = xwds_device_probe(&stm32xwds,
                               xwds_cast(struct xwds_device *, &stm32eeprom_256k),
                               NULL);
        if (rc < 0) {
                goto err_dev_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32eeprom_256k));
        if (rc < 0) {
                goto err_dev_start;
        }

        return XWOK;

err_dev_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32eeprom_256k));
err_dev_probe:
        xwds_eeprom_destruct(&stm32eeprom_256k);
        return rc;
}

xwer_t stm32cube_xwds_eeprom_fini(void)
{
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32eeprom_256k));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32eeprom_256k));
        xwds_eeprom_destruct(&stm32eeprom_256k);
        return XWOK;
}

xwer_t stm32cube_xwds_probe(void)
{
        xwer_t rc;

        xwds_init(&stm32xwds);
        rc = stm32cube_xwds_soc_init();
        if (rc < 0) {
                goto err_soc_init;
        }
        xwos_pm_set_cb(stm32cube_pm_resume,
                       stm32cube_pm_suspend,
                       stm32cube_pm_wakeup,
                       stm32cube_pm_sleep,
                       NULL);
        return XWOK;

err_soc_init:
        return rc;
}

void stm32cube_xwds_remove(void)
{
        xwer_t rc;

        rc = stm32cube_xwds_soc_fini();
        if (rc < 0) {
        }
}

xwer_t stm32cube_xwds_start(void)
{
        xwer_t rc;

        rc = stm32cube_xwds_uart_init();
        if (rc < 0) {
                xwlogf(ERR, "stm32cube", "Init UART ... <rc:%d>\n", rc);
                goto err_uart_init;
        }
        rc = stm32cube_xwds_spi_init();
        if (rc < 0) {
                xwlogf(ERR, "stm32cube", "Init SPI ... <rc:%d>\n", rc);
                goto err_spi_init;
        }
        rc = stm32cube_xwds_w25q64jv_init();
        if (rc < 0) {
                xwlogf(ERR, "stm32cube", "Init W25Q64JV ... <rc:%d>\n", rc);
        }
        rc = stm32cube_xwds_st7735_init();
        if (rc < 0) {
                xwlogf(ERR, "stm32cube", "Init ST7735 ... <rc:%d>\n", rc);
        }
        rc = stm32cube_xwds_i2c_init();
        if (rc < 0) {
                xwlogf(ERR, "stm32cube", "Init I2C ... <rc:%d>\n", rc);
                goto err_i2c_init;
        }
        rc = stm32cube_xwds_eeprom_init();
        if (rc < 0) {
                xwlogf(ERR, "stm32cube", "Init EEPROM ... <rc:%d>\n", rc);
        }
        return XWOK;

err_i2c_init:
        stm32cube_xwds_st7735_fini();
        stm32cube_xwds_w25q64jv_fini();
        stm32cube_xwds_spi_fini();
err_spi_init:
        stm32cube_xwds_uart_fini();
err_uart_init:
        return rc;
}

void stm32cube_xwds_stop(void)
{
        xwer_t rc;

        rc = stm32cube_xwds_eeprom_fini();
        if (rc < 0) {
                xwlogf(ERR, "stm32cube", "Fini EEPROM ... <rc:%d>\n", rc);
        }
        rc = stm32cube_xwds_i2c_fini();
        if (rc < 0) {
                xwlogf(ERR, "stm32cube", "Fini I2C ... <rc:%d>\n", rc);
        }
        rc = stm32cube_xwds_st7735_fini();
        if (rc < 0) {
                xwlogf(ERR, "stm32cube", "Fini ST7735 ... <rc:%d>\n", rc);
        }
        rc = stm32cube_xwds_w25q64jv_fini();
        if (rc < 0) {
                xwlogf(ERR, "stm32cube", "Fini W25Q64JV ... <rc:%d>\n", rc);
        }
        rc = stm32cube_xwds_spi_fini();
        if (rc < 0) {
                xwlogf(ERR, "stm32cube", "Fini SPI ... <rc:%d>\n", rc);
        }
        rc = stm32cube_xwds_uart_fini();
        if (rc < 0) {
                xwlogf(ERR, "stm32cube", "Fini UART ... <rc:%d>\n", rc);
        }
}
