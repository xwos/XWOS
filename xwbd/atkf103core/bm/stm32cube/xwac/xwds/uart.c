/**
 * @file
 * @brief STM32CUBE XWDS 设备：DMA-UART
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
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/condition.h>
#include <xwmd/ds/uart/dma.h>
#include <bm/stm32cube/cubemx/Core/Inc/tim.h>
#include <bm/stm32cube/cubemx/Core/Inc/usart.h>
#include <bm/stm32cube/xwac/xwds/stm32cube.h>
#include <bm/stm32cube/xwac/xwds/soc.h>
#include <bm/stm32cube/xwac/xwds/uart.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static
xwer_t stm32cube_usart1_drv_start(struct xwds_device * dev);

static
xwer_t stm32cube_usart1_drv_stop(struct xwds_device * dev);

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
static
xwer_t stm32cube_usart1_drv_suspend(struct xwds_device * dev);

static
xwer_t stm32cube_usart1_drv_resume(struct xwds_device * dev);
#endif /* XWMDCFG_ds_PM */

static
xwer_t stm32cube_usart1_drv_cfg(struct xwds_dmauartc * dmauartc,
                                const struct xwds_uart_cfg * cfg);

static
xwer_t stm32cube_usart1_drv_tx(struct xwds_dmauartc * dmauartc,
                               const xwu8_t * data, xwsz_t size,
                               xwtm_t * xwtm);

static
xwer_t stm32cube_usart1_drv_putc(struct xwds_dmauartc * dmauartc,
                                 const xwu8_t byte);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
const struct xwds_dmauartc_driver stm32cube_usart1_drv = {
        .base = {
                .name = "stm32cube.usart.1",
                .probe = NULL,
                .remove = NULL,
                .start = stm32cube_usart1_drv_start,
                .stop = stm32cube_usart1_drv_stop,
#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
                .suspend = stm32cube_usart1_drv_suspend,
                .resume =  stm32cube_usart1_drv_resume,
#endif /* XWMDCFG_ds_PM */
        },
        .cfg = stm32cube_usart1_drv_cfg,
        .tx = stm32cube_usart1_drv_tx,
        .putc = stm32cube_usart1_drv_putc,
};

struct xwds_dmauartc stm32cube_usart1_cb = {
        /* attributes */
        .dev = {
                .name = "stm32cube.usart.1",
                .id = 1,
                .resources = NULL,
                .drv = xwds_static_cast(struct xwds_driver *, &stm32cube_usart1_drv),
                .data = (void *)&husart1_xwds_drvdata,
        },
        .cfg = NULL,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** base driver ******** ********/
static
xwer_t stm32cube_usart1_drv_start(struct xwds_device * dev)
{
        struct xwds_dmauartc * dmauartc;
        struct HAL_UART_Xwds_driver_data * drvdata;
        xwer_t rc;

        dmauartc = xwds_static_cast(struct xwds_dmauartc *, dev);
        drvdata = dmauartc->dev.data;
        drvdata->dmauartc = dmauartc;

        MX_USART1_UART_Init();
        rc = MX_USART1_RXDMA_Start(dmauartc->rxq.mem, sizeof(dmauartc->rxq.mem));
        if (rc < 0) {
                goto err_rxdma_start;
        }
        MX_USART1_Timer_Init();
        MX_USART1_Timer_Start();
        return XWOK;

err_rxdma_start:
        return rc;
}

static
xwer_t stm32cube_usart1_drv_stop(struct xwds_device * dev)
{
        XWOS_UNUSED(dev);

        MX_USART1_Timer_Stop();
        MX_USART1_Timer_DeInit();
        MX_USART1_UART_DeInit();
        return XWOK;
}

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
static
xwer_t stm32cube_usart1_drv_resume(struct xwds_device * dev)
{
        return stm32cube_usart1_drv_start(dev);
}

static
xwer_t stm32cube_usart1_drv_suspend(struct xwds_device * dev)
{
        return stm32cube_usart1_drv_stop(dev);
}
#endif /* XWMDCFG_ds_PM */

static
xwer_t stm32cube_usart1_drv_cfg(struct xwds_dmauartc * dmauartc,
                                const struct xwds_uart_cfg * cfg)
{
        XWOS_UNUSED(dmauartc);
        XWOS_UNUSED(cfg);
        return -ENOSYS;
}

static
xwer_t stm32cube_usart1_drv_tx(struct xwds_dmauartc * dmauartc,
                               const xwu8_t * data, xwsz_t size,
                               xwtm_t * xwtm)
{
        struct HAL_UART_Xwds_driver_data * drvdata;
        xwreg_t flag;
        union xwlk_ulock ulk;
        xwsq_t lkst;
        xwer_t rc;

        drvdata = dmauartc->dev.data;
        ulk.osal.splk = &drvdata->tx.splk;
        xwosal_splk_lock_cpuirqsv(&drvdata->tx.splk, &flag);
        drvdata->tx.rc = -EINPROGRESS;
        rc = MX_USART1_TXDMA_Start((xwu8_t *)data, size);
        if (XWOK == rc) {
                rc = xwosal_cdt_timedwait(xwosal_cdt_get_id(&drvdata->tx.cdt),
                                          ulk, XWLK_TYPE_SPLK, NULL,
                                          xwtm, &lkst);
                if (XWOK == rc) {
                        if (XWLK_STATE_UNLOCKED == lkst) {
                                xwosal_splk_lock(&drvdata->tx.splk);
                        }
                        rc = drvdata->tx.rc;
                } else {
                        if (XWLK_STATE_UNLOCKED == lkst) {
                                xwosal_splk_lock(&drvdata->tx.splk);
                        }
                        drvdata->tx.rc = -ECANCELED;
                }
        } else {
                drvdata->tx.rc = -ECANCELED;
        }
        xwosal_splk_unlock_cpuirqrs(&drvdata->tx.splk, flag);
        return rc;
}

void stm32cube_usart1_cb_txdma_cplt(struct xwds_dmauartc * dmauartc, xwer_t dmarc)
{
        struct HAL_UART_Xwds_driver_data * drvdata;
        xwreg_t flag;

        drvdata = dmauartc->dev.data;
        xwosal_splk_lock_cpuirqsv(&drvdata->tx.splk, &flag);
        if (-ECANCELED != drvdata->tx.rc) {
                drvdata->tx.rc = dmarc;
        } else {
        }
        xwosal_splk_unlock_cpuirqrs(&drvdata->tx.splk, flag);
        xwosal_cdt_broadcast(xwosal_cdt_get_id(&drvdata->tx.cdt));
}

static
xwer_t stm32cube_usart1_drv_putc(struct xwds_dmauartc * dmauartc,
                                 const xwu8_t byte)
{
        XWOS_UNUSED(dmauartc);
        return MX_USART1_Putc(byte);
}

void stm32cube_usart1_cb_rxdma_restart(struct xwds_dmauartc * dmauartc)
{
        MX_USART1_RXDMA_Start(dmauartc->rxq.mem, sizeof(dmauartc->rxq.mem));
}

void stm32cube_usart1_cb_rxdma_halfcplt(struct xwds_dmauartc * dmauartc)
{
        MX_USART1_Timer_Stop();
        xwds_dmauartc_lib_rxq_pub(dmauartc, XWDS_DMAUART_RXQ_SIZE);
        MX_USART1_Timer_Start();
}

void stm32cube_usart1_cb_rxdma_cplt(struct xwds_dmauartc * dmauartc)
{
        MX_USART1_Timer_Stop();
        xwds_dmauartc_lib_rxq_pub(dmauartc, 0);
        MX_USART1_Timer_Start();
}

void stm32cube_usart1_cb_rxdma_timer(struct xwds_dmauartc * dmauartc)
{
        xwsq_t tail;

        tail = sizeof(dmauartc->rxq.mem) - MX_USART1_RXDMA_GetCounter();
        if (sizeof(dmauartc->rxq.mem) == tail) {
                xwds_dmauartc_lib_rxq_pub(dmauartc, 0);
        } else {
                xwds_dmauartc_lib_rxq_pub(dmauartc, tail);
        }
}
