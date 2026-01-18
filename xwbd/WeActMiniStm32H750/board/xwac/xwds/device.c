/**
 * @file
 * @brief 板级描述层：XWOS适配层：XWOS设备栈：设备
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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

struct xwds stm32xwds;

void stm32xwds_init(void)
{
        xwds_init(&stm32xwds);
}

void stm32xwds_fini(void)
{
}

xwer_t stm32xwds_soc_init(void)
{
        xwer_t rc;

        xwds_soc_construct(&stm32xwds_soc);
        rc = xwds_device_probe(&stm32xwds,
                               xwds_cast(struct xwds_device *, &stm32xwds_soc),
                               NULL);
        if (rc < 0) {
                goto err_dev_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32xwds_soc));
        if (rc < 0) {
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
        if (rc < 0) {
                goto err_usart1_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32xwds_usart1));
        if (rc < 0) {
                goto err_usart1_start;
        }
        xwds_uartc_construct(&stm32xwds_usart3);
        rc = xwds_device_probe(&stm32xwds,
                               xwds_cast(struct xwds_device *, &stm32xwds_usart3),
                               NULL);
        if (rc < 0) {
                goto err_usart3_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32xwds_usart3));
        if (rc < 0) {
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

xwer_t stm32xwds_spi_init(void)
{
        xwer_t rc;

        /* SPI1 */
        xwds_spim_construct(&stm32xwds_spi1m);
        rc = xwds_device_probe(&stm32xwds,
                               xwds_cast(struct xwds_device *, &stm32xwds_spi1m),
                               NULL);
        if (rc < 0) {
                goto err_spi1_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32xwds_spi1m));
        if (rc < 0) {
                goto err_spi1_start;
        }

        /* SPI4 */
        xwds_spim_construct(&stm32xwds_spi4m);
        rc = xwds_device_probe(&stm32xwds,
                               xwds_cast(struct xwds_device *, &stm32xwds_spi4m),
                               NULL);
        if (rc < 0) {
                goto err_spi4_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32xwds_spi4m));
        if (rc < 0) {
                goto err_spi4_start;
        }

        return XWOK;

err_spi4_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32xwds_spi4m));
err_spi4_probe:
        xwds_spim_destruct(&stm32xwds_spi4m);
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32xwds_spi1m));
err_spi1_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32xwds_spi1m));
err_spi1_probe:
        xwds_spim_destruct(&stm32xwds_spi1m);
        return rc;
}

xwer_t stm32xwds_spi_fini(void)
{
        /* SPI4 */
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32xwds_spi4m));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32xwds_spi4m));
        xwds_spim_destruct(&stm32xwds_spi4m);

        /* SPI1 */
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32xwds_spi1m));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32xwds_spi1m));
        xwds_spim_destruct(&stm32xwds_spi1m);
        return XWOK;
}

xwer_t stm32xwds_st7735_init(void)
{
        xwer_t rc;

        xwds_st7735_construct(&stm32xwds_st7735);
        rc = xwds_device_probe(&stm32xwds,
                               xwds_cast(struct xwds_device *, &stm32xwds_st7735),
                               NULL);
        if (rc < 0) {
                goto err_st7735_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32xwds_st7735));
        if (rc < 0) {
                goto err_st7735_start;
        }

        return XWOK;

err_st7735_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32xwds_st7735));
err_st7735_probe:
        xwds_st7735_destruct(&stm32xwds_st7735);
        return rc;
}

xwer_t stm32xwds_st7735_fini(void)
{
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32xwds_st7735));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32xwds_st7735));
        xwds_st7735_destruct(&stm32xwds_st7735);
        return XWOK;
}

xwer_t stm32xwds_w25q64jv_init(void)
{
        xwer_t rc;

        xwds_w25qxx_construct(&stm32xwds_w25q64jv);
        rc = xwds_device_probe(&stm32xwds,
                               xwds_cast(struct xwds_device *, &stm32xwds_w25q64jv),
                               NULL);
        if (rc < 0) {
                goto err_w25q64jv_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32xwds_w25q64jv));
        if (rc < 0) {
                goto err_w25q64jv_start;
        }
        return XWOK;

err_w25q64jv_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32xwds_w25q64jv));
err_w25q64jv_probe:
        xwds_w25qxx_destruct(&stm32xwds_w25q64jv);
        return rc;
}

xwer_t stm32xwds_w25q64jv_fini(void)
{
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32xwds_w25q64jv));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32xwds_w25q64jv));
        xwds_w25qxx_destruct(&stm32xwds_w25q64jv);
        return XWOK;
}

xwer_t stm32xwds_i2c_init(void)
{
        xwer_t rc;

        /* I2C1 */
        xwds_i2cm_construct(&stm32xwds_i2c1m);
        rc = xwds_device_probe(&stm32xwds,
                               xwds_cast(struct xwds_device *, &stm32xwds_i2c1m),
                               NULL);
        if (rc < 0) {
                goto err_i2c1_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32xwds_i2c1m));
        if (rc < 0) {
                goto err_i2c1_start;
        }

        return XWOK;

err_i2c1_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32xwds_i2c1m));
err_i2c1_probe:
        xwds_i2cm_destruct(&stm32xwds_i2c1m);
        return rc;
}

xwer_t stm32xwds_i2c_fini(void)
{
        /* I2C1 */
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32xwds_i2c1m));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32xwds_i2c1m));
        xwds_i2cm_destruct(&stm32xwds_i2c1m);
        return XWOK;
}

xwer_t stm32xwds_eeprom_init(void)
{
        xwer_t rc;

        xwds_eeprom_construct(&stm32xwds_eeprom256k);
        rc = xwds_device_probe(&stm32xwds,
                               xwds_cast(struct xwds_device *, &stm32xwds_eeprom256k),
                               NULL);
        if (rc < 0) {
                goto err_dev_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32xwds_eeprom256k));
        if (rc < 0) {
                goto err_dev_start;
        }

        return XWOK;

err_dev_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32xwds_eeprom256k));
err_dev_probe:
        xwds_eeprom_destruct(&stm32xwds_eeprom256k);
        return rc;
}

xwer_t stm32xwds_eeprom_fini(void)
{
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32xwds_eeprom256k));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32xwds_eeprom256k));
        xwds_eeprom_destruct(&stm32xwds_eeprom256k);
        return XWOK;
}
