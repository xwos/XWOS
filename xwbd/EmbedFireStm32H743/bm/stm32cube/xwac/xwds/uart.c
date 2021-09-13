/**
 * @file
 * @brief STM32CUBE设备栈：DMA-UART
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
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/cond.h>
#include <xwcd/ds/uart/dma.h>
#include <bm/stm32cube/cubemx/Core/Inc/usart.h>
#include <bm/stm32cube/xwac/xwds/device.h>
#include <bm/stm32cube/xwac/xwds/uart.h>

/******** USART1 ********/
static
xwer_t stm32cube_usart1_drv_start(struct xwds_device * dev);

static
xwer_t stm32cube_usart1_drv_stop(struct xwds_device * dev);

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static
xwer_t stm32cube_usart1_drv_suspend(struct xwds_device * dev);

static
xwer_t stm32cube_usart1_drv_resume(struct xwds_device * dev);
#endif

static
xwer_t stm32cube_usart1_drv_cfg(struct xwds_dmauartc * dmauartc,
                                const struct xwds_uart_cfg * cfg);
static
xwer_t stm32cube_usart1_drv_tx(struct xwds_dmauartc * dmauartc,
                               const xwu8_t * data, xwsz_t * size,
                               xwtm_t * xwtm);

static
xwer_t stm32cube_usart1_drv_putc(struct xwds_dmauartc * dmauartc,
                                 const xwu8_t byte);

const struct xwds_dmauartc_driver stm32cube_usart1_drv = {
        .base = {
                .name = "stm32cube.usart.1",
                .probe = NULL,
                .remove = NULL,
                .start = stm32cube_usart1_drv_start,
                .stop = stm32cube_usart1_drv_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
                .suspend = stm32cube_usart1_drv_suspend,
                .resume =  stm32cube_usart1_drv_resume,
#endif
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
                .drv = xwds_cast(struct xwds_driver *, &stm32cube_usart1_drv),
                .data = (void *)&huart1_drvdata,
        },
        .cfg = NULL,
};

static
xwer_t stm32cube_usart1_drv_start(struct xwds_device * dev)
{
        struct xwds_dmauartc * dmauartc;
        struct MX_UART_DriverData * drvdata;
        xwer_t rc;

        dmauartc = xwds_cast(struct xwds_dmauartc *, dev);
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

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
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
#endif

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
                               const xwu8_t * data, xwsz_t * size,
                               xwtm_t * xwtm)
{
        struct MX_UART_DriverData * drvdata;
        xwreg_t cpuirq;
        union xwos_ulock ulk;
        xwsz_t wrsz;
        xwsq_t lkst;
        xwer_t rc;

        wrsz = *size;
        drvdata = dmauartc->dev.data;
        MX_USART1_TXDMA_Prepare(data, wrsz);
        ulk.osal.splk = &drvdata->tx.splk;
        xwos_splk_lock_cpuirqsv(&drvdata->tx.splk, &cpuirq);
        drvdata->tx.rc = -EINPROGRESS;
        rc = MX_USART1_TXDMA_Start();
        if (XWOK == rc) {
                rc = xwos_cond_timedwait(&drvdata->tx.cond,
                                         ulk, XWOS_LK_SPLK, NULL,
                                         xwtm, &lkst);
                if (XWOK == rc) {
                        if (XWOS_LKST_UNLOCKED == lkst) {
                                xwos_splk_lock(&drvdata->tx.splk);
                        }
                        rc = drvdata->tx.rc;
                } else {
                        if (XWOS_LKST_UNLOCKED == lkst) {
                                xwos_splk_lock(&drvdata->tx.splk);
                        }
                        drvdata->tx.rc = -ECANCELED;
                }
        } else {
                drvdata->tx.rc = -ECANCELED;
        }
        xwos_splk_unlock_cpuirqrs(&drvdata->tx.splk, cpuirq);
        if (rc < 0) {
                *size = 0;
        }
        return rc;
}

void stm32cube_usart1_cb_txdma_cplt(struct xwds_dmauartc * dmauartc, xwer_t dmarc)
{
        struct MX_UART_DriverData * drvdata;
        xwreg_t cpuirq;

        drvdata = dmauartc->dev.data;
        xwos_splk_lock_cpuirqsv(&drvdata->tx.splk, &cpuirq);
        if (-ECANCELED != drvdata->tx.rc) {
                drvdata->tx.rc = dmarc;
        } else {
        }
        xwos_splk_unlock_cpuirqrs(&drvdata->tx.splk, cpuirq);
        xwos_cond_broadcast(&drvdata->tx.cond);
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
        xwds_dmauartc_drvcb_rxq_flush(dmauartc);
        MX_USART1_RXDMA_Start(dmauartc->rxq.mem, sizeof(dmauartc->rxq.mem));
}

void stm32cube_usart1_cb_rxdma_halfcplt(struct xwds_dmauartc * dmauartc)
{
        xwds_dmauartc_drvcb_rxq_pub(dmauartc, XWDS_DMAUART_RXQ_SIZE);
}

void stm32cube_usart1_cb_rxdma_cplt(struct xwds_dmauartc * dmauartc)
{
        xwds_dmauartc_drvcb_rxq_pub(dmauartc, 0);
}

void stm32cube_usart1_cb_rxdma_timer(struct xwds_dmauartc * dmauartc)
{
        xwsq_t tail;

        tail = sizeof(dmauartc->rxq.mem) - MX_USART1_RXDMA_GetCounter();
        if (sizeof(dmauartc->rxq.mem) == tail) {
                xwds_dmauartc_drvcb_rxq_pub(dmauartc, 0);
        } else {
                xwds_dmauartc_drvcb_rxq_pub(dmauartc, tail);
        }
}

/******** UART5 ********/
static
xwer_t stm32cube_uart5_drv_start(struct xwds_device * dev);

static
xwer_t stm32cube_uart5_drv_stop(struct xwds_device * dev);

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static
xwer_t stm32cube_uart5_drv_suspend(struct xwds_device * dev);

static
xwer_t stm32cube_uart5_drv_resume(struct xwds_device * dev);
#endif

static
xwer_t stm32cube_uart5_drv_cfg(struct xwds_dmauartc * dmauartc,
                               const struct xwds_uart_cfg * cfg);
static
xwer_t stm32cube_uart5_drv_tx(struct xwds_dmauartc * dmauartc,
                              const xwu8_t * data, xwsz_t * size,
                              xwtm_t * xwtm);

static
xwer_t stm32cube_uart5_drv_putc(struct xwds_dmauartc * dmauartc,
                                const xwu8_t byte);

const struct xwds_dmauartc_driver stm32cube_uart5_drv = {
        .base = {
                .name = "stm32cube.uart.5",
                .probe = NULL,
                .remove = NULL,
                .start = stm32cube_uart5_drv_start,
                .stop = stm32cube_uart5_drv_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
                .suspend = stm32cube_uart5_drv_suspend,
                .resume =  stm32cube_uart5_drv_resume,
#endif
        },
        .cfg = stm32cube_uart5_drv_cfg,
        .tx = stm32cube_uart5_drv_tx,
        .putc = stm32cube_uart5_drv_putc,
};

struct xwds_dmauartc stm32cube_uart5_cb = {
        /* attributes */
        .dev = {
                .name = "stm32cube.uart.5",
                .id = 1,
                .resources = NULL,
                .drv = xwds_cast(struct xwds_driver *, &stm32cube_uart5_drv),
                .data = (void *)&huart5_drvdata,
        },
        .cfg = NULL,
};

static
xwer_t stm32cube_uart5_drv_start(struct xwds_device * dev)
{
        struct xwds_dmauartc * dmauartc;
        struct MX_UART_DriverData * drvdata;
        xwer_t rc;

        dmauartc = xwds_cast(struct xwds_dmauartc *, dev);
        drvdata = dmauartc->dev.data;
        drvdata->dmauartc = dmauartc;

        MX_UART5_Init();
        rc = MX_UART5_RXDMA_Start(dmauartc->rxq.mem, sizeof(dmauartc->rxq.mem));
        if (rc < 0) {
                goto err_rxdma_start;
        }
        MX_UART5_Timer_Init();
        MX_UART5_Timer_Start();
        return XWOK;

err_rxdma_start:
        return rc;
}

static
xwer_t stm32cube_uart5_drv_stop(struct xwds_device * dev)
{
        XWOS_UNUSED(dev);

        MX_UART5_Timer_Stop();
        MX_UART5_Timer_DeInit();
        MX_UART5_DeInit();
        return XWOK;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static
xwer_t stm32cube_uart5_drv_resume(struct xwds_device * dev)
{
        return stm32cube_uart5_drv_start(dev);
}

static
xwer_t stm32cube_uart5_drv_suspend(struct xwds_device * dev)
{
        return stm32cube_uart5_drv_stop(dev);
}
#endif

static
xwer_t stm32cube_uart5_drv_cfg(struct xwds_dmauartc * dmauartc,
                               const struct xwds_uart_cfg * cfg)
{
        XWOS_UNUSED(dmauartc);
        XWOS_UNUSED(cfg);
        return -ENOSYS;
}

static
xwer_t stm32cube_uart5_drv_tx(struct xwds_dmauartc * dmauartc,
                              const xwu8_t * data, xwsz_t * size,
                              xwtm_t * xwtm)
{
        struct MX_UART_DriverData * drvdata;
        xwreg_t cpuirq;
        union xwos_ulock ulk;
        xwsz_t wrsz;
        xwsq_t lkst;
        xwer_t rc;

        wrsz = *size;
        drvdata = dmauartc->dev.data;
        MX_UART5_TXDMA_Prepare(data, wrsz);
        ulk.osal.splk = &drvdata->tx.splk;
        xwos_splk_lock_cpuirqsv(&drvdata->tx.splk, &cpuirq);
        drvdata->tx.rc = -EINPROGRESS;
        rc = MX_UART5_TXDMA_Start();
        if (XWOK == rc) {
                rc = xwos_cond_timedwait(&drvdata->tx.cond,
                                         ulk, XWOS_LK_SPLK, NULL,
                                         xwtm, &lkst);
                if (XWOK == rc) {
                        if (XWOS_LKST_UNLOCKED == lkst) {
                                xwos_splk_lock(&drvdata->tx.splk);
                        }
                        rc = drvdata->tx.rc;
                } else {
                        if (XWOS_LKST_UNLOCKED == lkst) {
                                xwos_splk_lock(&drvdata->tx.splk);
                        }
                        drvdata->tx.rc = -ECANCELED;
                }
        } else {
                drvdata->tx.rc = -ECANCELED;
        }
        xwos_splk_unlock_cpuirqrs(&drvdata->tx.splk, cpuirq);
        if (rc < 0) {
                *size = 0;
        }
        return rc;
}

void stm32cube_uart5_cb_txdma_cplt(struct xwds_dmauartc * dmauartc, xwer_t dmarc)
{
        struct MX_UART_DriverData * drvdata;
        xwreg_t cpuirq;

        drvdata = dmauartc->dev.data;
        xwos_splk_lock_cpuirqsv(&drvdata->tx.splk, &cpuirq);
        if (-ECANCELED != drvdata->tx.rc) {
                drvdata->tx.rc = dmarc;
        } else {
        }
        xwos_splk_unlock_cpuirqrs(&drvdata->tx.splk, cpuirq);
        xwos_cond_broadcast(&drvdata->tx.cond);
}

static
xwer_t stm32cube_uart5_drv_putc(struct xwds_dmauartc * dmauartc,
                                const xwu8_t byte)
{
        XWOS_UNUSED(dmauartc);
        return MX_UART5_Putc(byte);
}

void stm32cube_uart5_cb_rxdma_restart(struct xwds_dmauartc * dmauartc)
{
        xwds_dmauartc_drvcb_rxq_flush(dmauartc);
        MX_UART5_RXDMA_Start(dmauartc->rxq.mem, sizeof(dmauartc->rxq.mem));
}

void stm32cube_uart5_cb_rxdma_halfcplt(struct xwds_dmauartc * dmauartc)
{
        xwds_dmauartc_drvcb_rxq_pub(dmauartc, XWDS_DMAUART_RXQ_SIZE);
}

void stm32cube_uart5_cb_rxdma_cplt(struct xwds_dmauartc * dmauartc)
{
        xwds_dmauartc_drvcb_rxq_pub(dmauartc, 0);
}

void stm32cube_uart5_cb_rxdma_timer(struct xwds_dmauartc * dmauartc)
{
        xwsq_t tail;

        tail = sizeof(dmauartc->rxq.mem) - MX_UART5_RXDMA_GetCounter();
        if (sizeof(dmauartc->rxq.mem) == tail) {
                xwds_dmauartc_drvcb_rxq_pub(dmauartc, 0);
        } else {
                xwds_dmauartc_drvcb_rxq_pub(dmauartc, tail);
        }
}
