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

xwer_t stm32cube_xwds_ll_start(void)
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

xwer_t stm32cube_xwds_ll_stop(void)
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
err_uart_stop:
err_spi_stop:
        return rc;
}

xwer_t stm32cube_xwds_soc_start(void)
{
        xwer_t rc;

        xwds_soc_construct(&stm32cube_soc_cb);
        rc = xwds_device_probe(&stm32cube_ds,
                               xwds_cast(struct xwds_device *, &stm32cube_soc_cb),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32cube_soc_cb));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_start;
        }
        return XWOK;

err_dev_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32cube_soc_cb));
err_dev_probe:
        xwds_soc_destruct(&stm32cube_soc_cb);
        return rc;
}

xwer_t stm32cube_xwds_soc_stop(void)
{
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32cube_soc_cb));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32cube_soc_cb));
        xwds_soc_destruct(&stm32cube_soc_cb);
        return XWOK;
}

xwer_t stm32cube_xwds_uart_start(void)
{
        xwer_t rc;

        xwds_dmauartc_construct(&stm32cube_usart1_cb);
        rc = xwds_device_probe(&stm32cube_ds,
                               xwds_cast(struct xwds_device *, &stm32cube_usart1_cb),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_usart1_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32cube_usart1_cb));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_usart1_start;
        }
        xwds_dmauartc_construct(&stm32cube_usart3_cb);
        rc = xwds_device_probe(&stm32cube_ds,
                               xwds_cast(struct xwds_device *, &stm32cube_usart3_cb),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_usart3_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32cube_usart3_cb));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_usart3_start;
        }
        return XWOK;

err_usart3_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32cube_usart3_cb));
err_usart3_probe:
        xwds_dmauartc_destruct(&stm32cube_usart3_cb);

        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32cube_usart1_cb));
err_usart1_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32cube_usart1_cb));
err_usart1_probe:
        xwds_dmauartc_destruct(&stm32cube_usart1_cb);
        return rc;
}

xwer_t stm32cube_xwds_uart_stop(void)
{
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32cube_usart3_cb));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32cube_usart3_cb));
        xwds_dmauartc_destruct(&stm32cube_usart3_cb);

        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32cube_usart1_cb));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32cube_usart1_cb));
        xwds_dmauartc_destruct(&stm32cube_usart1_cb);
        return XWOK;
}

xwer_t stm32cube_xwds_spi_start(void)
{
        xwer_t rc;

        /* SPI1 */
        xwds_spim_construct(&stm32cube_spi1m_cb);
        rc = xwds_device_probe(&stm32cube_ds,
                               xwds_cast(struct xwds_device *, &stm32cube_spi1m_cb),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_spi1_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32cube_spi1m_cb));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_spi1_start;
        }

        /* SPI4 */
        xwds_spim_construct(&stm32cube_spi4m_cb);
        rc = xwds_device_probe(&stm32cube_ds,
                               xwds_cast(struct xwds_device *, &stm32cube_spi4m_cb),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_spi4_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32cube_spi4m_cb));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_spi4_start;
        }

        return XWOK;

err_spi4_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32cube_spi4m_cb));
err_spi4_probe:
        xwds_spim_destruct(&stm32cube_spi4m_cb);
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32cube_spi1m_cb));
err_spi1_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32cube_spi1m_cb));
err_spi1_probe:
        xwds_spim_destruct(&stm32cube_spi1m_cb);
        return rc;
}

xwer_t stm32cube_xwds_spi_stop(void)
{
        /* SPI4 */
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32cube_spi4m_cb));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32cube_spi4m_cb));
        xwds_spim_destruct(&stm32cube_spi4m_cb);

        /* SPI1 */
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32cube_spi1m_cb));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32cube_spi1m_cb));
        xwds_spim_destruct(&stm32cube_spi1m_cb);
        return XWOK;
}

xwer_t stm32cube_xwds_st7735_start(void)
{
        xwer_t rc;

        xwds_st7735_construct(&stm32cube_st7735_cb);
        rc = xwds_device_probe(&stm32cube_ds,
                               xwds_cast(struct xwds_device *, &stm32cube_st7735_cb),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_st7735_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32cube_st7735_cb));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_st7735_start;
        }

        return XWOK;

err_st7735_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32cube_st7735_cb));
err_st7735_probe:
        xwds_st7735_destruct(&stm32cube_st7735_cb);
        return rc;
}

xwer_t stm32cube_xwds_st7735_stop(void)
{
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32cube_st7735_cb));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32cube_st7735_cb));
        xwds_st7735_destruct(&stm32cube_st7735_cb);
        return XWOK;
}

xwer_t stm32cube_xwds_w25q64jv_start(void)
{
        xwer_t rc;

        xwds_w25qxx_construct(&stm32cube_w25q64jv_cb);
        rc = xwds_device_probe(&stm32cube_ds,
                               xwds_cast(struct xwds_device *, &stm32cube_w25q64jv_cb),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_w25q64jv_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32cube_w25q64jv_cb));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_w25q64jv_start;
        }
        return XWOK;

err_w25q64jv_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32cube_w25q64jv_cb));
err_w25q64jv_probe:
        xwds_w25qxx_destruct(&stm32cube_w25q64jv_cb);
        return rc;
}

xwer_t stm32cube_xwds_w25q64jv_stop(void)
{
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32cube_w25q64jv_cb));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32cube_w25q64jv_cb));
        xwds_w25qxx_destruct(&stm32cube_w25q64jv_cb);
        return XWOK;
}
