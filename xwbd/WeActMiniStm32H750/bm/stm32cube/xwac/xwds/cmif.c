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
#include <xwcd/ds/uart/dma.h>
#include <xwcd/ds/spi/master.h>
#include <xwcd/perpheral/spi/flash/w25qxx/device.h>
#include <bm/stm32cube/xwac/xwds/device.h>
#include <bm/stm32cube/xwac/xwds/pm.h>
#include <bm/stm32cube/xwac/xwds/cmif.h>

xwer_t stm32cube_xwds_probe(void)
{
        xwer_t rc;

        xwds_init(&stm32cube_ds);
        rc = stm32cube_xwds_soc_start();
        if (rc < 0) {
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

xwer_t stm32cube_xwds_remove(void)
{
        xwer_t rc;

        rc = stm32cube_xwds_soc_stop();
        if (rc < 0) {
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
        rc = stm32cube_xwds_spi_start();
        if (rc < 0) {
                goto err_spi_start;
        }
        rc = stm32cube_xwds_w25q64jv_start();
        if (rc < 0) {
                goto err_w25q64jv_start;
        }
        rc = stm32cube_xwds_st7735_start();
        if (rc < 0) {
                goto err_st7735_start;
        }
        return XWOK;

err_st7735_start:
        stm32cube_xwds_w25q64jv_stop();
err_w25q64jv_start:
        stm32cube_xwds_spi_stop();
err_spi_start:
        stm32cube_xwds_uart_stop();
err_uart_start:
        return rc;
}

xwer_t stm32cube_xwds_stop(void)
{
        xwer_t rc;

        rc = stm32cube_xwds_st7735_stop();
        if (rc < 0) {
                goto err_st7735_stop;
        }
        rc = stm32cube_xwds_w25q64jv_stop();
        if (rc < 0) {
                goto err_w25q64jv_stop;
        }
        rc = stm32cube_xwds_spi_stop();
        if (rc < 0) {
                goto err_spi_stop;
        }
        rc = stm32cube_xwds_uart_stop();
        if (rc < 0) {
                goto err_uart_stop;
        }
        return XWOK;

err_st7735_stop:
err_w25q64jv_stop:
err_spi_stop:
err_uart_stop:
        return rc;
}

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

        xwds_dmauartc_construct(&stm32usart1);
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
        xwds_dmauartc_construct(&stm32usart3);
        rc = xwds_device_probe(&stm32cube_ds,
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
        xwds_dmauartc_destruct(&stm32usart3);

        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32usart1));
err_usart1_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32usart1));
err_usart1_probe:
        xwds_dmauartc_destruct(&stm32usart1);
        return rc;
}

xwer_t stm32cube_xwds_uart_stop(void)
{
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32usart3));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32usart3));
        xwds_dmauartc_destruct(&stm32usart3);

        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32usart1));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32usart1));
        xwds_dmauartc_destruct(&stm32usart1);
        return XWOK;
}

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

        /* SPI4 */
        xwds_spim_construct(&stm32spi4m);
        rc = xwds_device_probe(&stm32cube_ds,
                               xwds_cast(struct xwds_device *, &stm32spi4m),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_spi4_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32spi4m));
        if (__xwcc_unlikely(rc < 0)) {
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

xwer_t stm32cube_xwds_spi_stop(void)
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

xwer_t stm32cube_xwds_st7735_start(void)
{
        xwer_t rc;

        xwds_st7735_construct(&st7735);
        rc = xwds_device_probe(&stm32cube_ds,
                               xwds_cast(struct xwds_device *, &st7735),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_st7735_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &st7735));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_st7735_start;
        }

        return XWOK;

err_st7735_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &st7735));
err_st7735_probe:
        xwds_st7735_destruct(&st7735);
        return rc;
}

xwer_t stm32cube_xwds_st7735_stop(void)
{
        xwds_device_stop(xwds_cast(struct xwds_device *, &st7735));
        xwds_device_remove(xwds_cast(struct xwds_device *, &st7735));
        xwds_st7735_destruct(&st7735);
        return XWOK;
}

xwer_t stm32cube_xwds_w25q64jv_start(void)
{
        xwer_t rc;

        xwds_w25qxx_construct(&w25q64jv);
        rc = xwds_device_probe(&stm32cube_ds,
                               xwds_cast(struct xwds_device *, &w25q64jv),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_w25q64jv_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &w25q64jv));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_w25q64jv_start;
        }
        return XWOK;

err_w25q64jv_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &w25q64jv));
err_w25q64jv_probe:
        xwds_w25qxx_destruct(&w25q64jv);
        return rc;
}

xwer_t stm32cube_xwds_w25q64jv_stop(void)
{
        xwds_device_stop(xwds_cast(struct xwds_device *, &w25q64jv));
        xwds_device_remove(xwds_cast(struct xwds_device *, &w25q64jv));
        xwds_w25qxx_destruct(&w25q64jv);
        return XWOK;
}
