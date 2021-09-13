/**
 * @file
 * @brief STM32CUBE：子模块接口
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
#include <xwos/mm/common.h>
#include <xwos/mm/bma.h>
#include <xwcd/ds/xwds.h>
#include <xwcd/ds/device.h>
#include <xwcd/ds/soc/chip.h>
#include <xwcd/ds/uart/dma.h>
#include <bm/stm32cube/xwac/xwds/device.h>
#include <bm/stm32cube/xwac/xwds/cmif.h>

/**
 * @brief 准备启动设备栈
 * @retrun 错误码
 * @note
 * - 在初始化过程中调用此函数，此函数中只能初始化SOC的基本资源，
 *   例如：GPIO、时钟等。
 * @note
 * - 同步/异步：同步
 * - 上下文：初始化流程
 * - 重入性：不可重入
 */
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

/**
 * @brief 准备停止设备栈
 * @retrun 错误码
 * @note
 * - 在反初始化过程中调用此函数，此函数中只能停止SOC的基本资源，
 *   例如：GPIO、时钟等。
 * @note
 * - 同步/异步：同步
 * - 上下文：反初始化流程
 * - 重入性：不可重入
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
 * @retrun 错误码
 * @note
 * - 此函数会启动所有外设，有些外设启动流程需要延时，
 *   因此此函数只能运行在线程中。
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
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

/**
 * @brief 停止设备栈
 * @retrun 错误码
 * @note
 * - 此函数会停止所有外设，有些外设的停止流程需要延时，
 *   因此此函数只能运行在线程中。
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
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

/**
 * @brief 启动SOC
 * @retrun 错误码
 * @note
 * - 已经由@ref stm32cube_xwds_ll_start()调用。
 * @note
 * - 同步/异步：同步
 * - 上下文：初始化流程
 * - 重入性：不可重入
 */
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

/**
 * @brief 停止SOC
 * @retrun 错误码
 * @note
 * - 已经由@ref stm32cube_xwds_ll_stop()调用。
 * @note
 * - 同步/异步：同步
 * - 上下文：反初始化流程
 * - 重入性：不可重入
 */
xwer_t stm32cube_xwds_soc_stop(void)
{
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32cube_soc_cb));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32cube_soc_cb));
        xwds_soc_destruct(&stm32cube_soc_cb);
        return XWOK;
}

/**
 * @brief 启动UART
 * @retrun 错误码
 * @note
 * - 已经由@ref stm32cube_xwds_start()调用。
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
xwer_t stm32cube_xwds_uart_start(void)
{
        xwer_t rc;

        xwds_dmauartc_construct(&stm32cube_usart1_cb);
        rc = xwds_device_probe(&stm32cube_ds,
                               xwds_cast(struct xwds_device *, &stm32cube_usart1_cb),
                               NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_probe;
        }
        rc = xwds_device_start(xwds_cast(struct xwds_device *, &stm32cube_usart1_cb));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_start;
        }

        return XWOK;

err_dev_start:
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32cube_usart1_cb));
err_dev_probe:
        xwds_dmauartc_destruct(&stm32cube_usart1_cb);
        return rc;
}

/**
 * @brief 停止UART
 * @retrun 错误码
 * @note
 * - 已经由@ref stm32cube_xwds_stop()调用。
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
xwer_t stm32cube_xwds_uart_stop(void)
{
        xwds_device_stop(xwds_cast(struct xwds_device *, &stm32cube_usart1_cb));
        xwds_device_remove(xwds_cast(struct xwds_device *, &stm32cube_usart1_cb));
        xwds_dmauartc_destruct(&stm32cube_usart1_cb);
        return XWOK;
}
