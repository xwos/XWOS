/**
 * @file
 * @brief Stm32Hal::xwds::uart
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

#include <board/std.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/cond.h>
#include <xwcd/ds/uart/controller.h>
#include "bm/Stm32Hal/CubeMX/Core/Inc/usart.h"
#include "bm/Stm32Hal/xwds/device.h"
#include "bm/Stm32Hal/xwds/uart.h"

static
xwer_t stm32xwds_usart_drv_start(struct xwds_device * dev)
{
        struct xwds_uartc * uartc;
        struct MX_UART_DriverData * drvdata;
        xwer_t rc;

        uartc = xwds_cast(struct xwds_uartc *, dev);
        drvdata = uartc->dev.data;
        drvdata->uartc = uartc;

        MX_USART_UART_Init(dev->id);
        rc = MX_USART_RXDMA_Start(dev->id, uartc->rxq.mem, sizeof(uartc->rxq.mem));
        if (rc < 0) {
                goto err_rxdma_start;
        }
        MX_USART_Timer_Init(dev->id);
        MX_USART_Timer_Start(dev->id);
        return XWOK;

err_rxdma_start:
        return rc;
}

static
xwer_t stm32xwds_usart_drv_stop(struct xwds_device * dev)
{
        MX_USART_Timer_Stop(dev->id);
        MX_USART_Timer_DeInit(dev->id);
        MX_USART_UART_DeInit(dev->id);
        return XWOK;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static
xwer_t stm32xwds_usart_drv_resume(struct xwds_device * dev)
{
        return stm32xwds_usart_drv_start(dev);
}

static
xwer_t stm32xwds_usart_drv_suspend(struct xwds_device * dev)
{
        return stm32xwds_usart_drv_stop(dev);
}
#endif

static
xwer_t stm32xwds_usart_drv_cfg(struct xwds_uartc * uartc,
                               const struct xwds_uart_cfg * cfg)
{
        XWOS_UNUSED(uartc);
        XWOS_UNUSED(cfg);
        return -ENOSYS;
}

static
xwer_t stm32xwds_usart_drv_tx(struct xwds_uartc * uartc,
                              const xwu8_t * data, xwsz_t * size,
                              xwtm_t to)
{
        struct MX_UART_DriverData * drvdata;
        xwreg_t cpuirq;
        union xwos_ulock ulk;
        xwsz_t wrsz;
        xwsq_t lkst;
        xwer_t rc;

        wrsz = *size;
        drvdata = uartc->dev.data;

        rc = xwos_sem_wait_to(&drvdata->tx.available, to);
        if (rc < 0) {
                goto err_sem_wait_to;
        }

        MX_USART_TXDMA_Prepare(uartc->dev.id, data, wrsz);
        ulk.osal.splk = &drvdata->tx.splk;
        xwos_splk_lock_cpuirqsv(&drvdata->tx.splk, &cpuirq);
        drvdata->tx.rc = -EINPROGRESS;
        rc = MX_USART_TXDMA_Start(uartc->dev.id);
        if (XWOK == rc) {
                rc = xwos_cond_wait_to(&drvdata->tx.completion,
                                       ulk, XWOS_LK_SPLK, NULL,
                                       to, &lkst);
                if (XWOK == rc) {
                        XWOS_BUG_ON((xwsq_t)XWOS_LKST_UNLOCKED == lkst);
                        rc = drvdata->tx.rc;
                } else {
                        if ((xwsq_t)XWOS_LKST_UNLOCKED == lkst) {
                                xwos_splk_lock(&drvdata->tx.splk);
                        }
                        if (-EINPROGRESS == drvdata->tx.rc) {
                                drvdata->tx.rc = -ECANCELED;
                        } else {
                                rc = drvdata->tx.rc;
                        }
                }
                xwos_splk_unlock_cpuirqrs(&drvdata->tx.splk, cpuirq);
        } else {
                drvdata->tx.rc = -ECANCELED;
                xwos_splk_unlock_cpuirqrs(&drvdata->tx.splk, cpuirq);
                *size = (xwsz_t)0;
                xwos_sem_post(&drvdata->tx.available);
        }

err_sem_wait_to:
        return rc;
}

static
xwer_t stm32xwds_usart_drv_eq(struct xwds_uartc * uartc,
                              const xwu8_t * data, xwsz_t * size,
                              xwds_uartc_eqcb_f cb)
{
        struct MX_UART_DriverData * drvdata;
        xwreg_t cpuirq;
        xwsz_t wrsz;
        xwer_t rc;

        wrsz = *size;
        drvdata = uartc->dev.data;
        rc = xwos_sem_trywait(&drvdata->tx.available);
        if (rc < 0) {
                goto err_sem_trywait;
        }

        MX_USART_TXDMA_Prepare(uartc->dev.id, data, wrsz);
        xwos_splk_lock_cpuirqsv(&drvdata->tx.splk, &cpuirq);
        drvdata->tx.rc = -EINPROGRESS;
        drvdata->tx.eqcb = cb;
        rc = MX_USART_TXDMA_Start(uartc->dev.id);
        if (rc < 0) {
                drvdata->tx.eqcb = NULL;
                drvdata->tx.rc = -ECANCELED;
                xwos_splk_unlock_cpuirqrs(&drvdata->tx.splk, cpuirq);
                *size = 0;
                xwos_sem_post(&drvdata->tx.available);
        } else {
                xwos_splk_unlock_cpuirqrs(&drvdata->tx.splk, cpuirq);
        }

err_sem_trywait:
        return rc;
}

void stm32xwds_usart_cb_txdma_cplt(struct xwds_uartc * uartc, xwer_t dmarc)
{
        struct MX_UART_DriverData * drvdata;
        xwreg_t cpuirq;
        xwds_uartc_eqcb_f cb = NULL;

        drvdata = uartc->dev.data;
        xwos_splk_lock_cpuirqsv(&drvdata->tx.splk, &cpuirq);
        if (-ECANCELED != drvdata->tx.rc) {
                drvdata->tx.rc = dmarc;
                cb = drvdata->tx.eqcb;
        } else {
        }
        drvdata->tx.eqcb = NULL;
        xwos_splk_unlock_cpuirqrs(&drvdata->tx.splk, cpuirq);
        xwos_cond_broadcast(&drvdata->tx.completion);
        if (NULL != cb) {
                cb(uartc, dmarc);
        }
        xwos_sem_post(&drvdata->tx.available);
}

static
xwer_t stm32xwds_usart_drv_putc(struct xwds_uartc * uartc,
                                const xwu8_t byte)
{
        XWOS_UNUSED(uartc);
        return MX_USART_Putc(uartc->dev.id, byte);
}

void stm32xwds_usart_cb_rxdma_restart(struct xwds_uartc * uartc)
{
        xwds_uartc_drvcb_rxq_flush(uartc);
        MX_USART_RXDMA_Start(uartc->dev.id, uartc->rxq.mem, sizeof(uartc->rxq.mem));
}

void stm32xwds_usart_cb_rxdma_halfcplt(struct xwds_uartc * uartc)
{
        xwds_uartc_drvcb_rxq_pub(uartc, (sizeof(uartc->rxq.mem) >> (xwsq_t)1));
}

void stm32xwds_usart_cb_rxdma_cplt(struct xwds_uartc * uartc)
{
        xwds_uartc_drvcb_rxq_pub(uartc, 0);
}

void stm32xwds_usart_cb_rxdma_timer(struct xwds_uartc * uartc)
{
        xwsq_t tail;

        tail = sizeof(uartc->rxq.mem) - MX_USART_RXDMA_GetCounter(uartc->dev.id);
        if (sizeof(uartc->rxq.mem) == tail) {
                xwds_uartc_drvcb_rxq_pub(uartc, 0);
        } else {
                xwds_uartc_drvcb_rxq_pub(uartc, tail);
        }
}

const struct xwds_uartc_driver stm32xwds_usart_drv = {
        .base = {
                .name = "stm32xwds.usart",
                .probe = NULL,
                .remove = NULL,
                .start = stm32xwds_usart_drv_start,
                .stop = stm32xwds_usart_drv_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
                .suspend = stm32xwds_usart_drv_suspend,
                .resume =  stm32xwds_usart_drv_resume,
#endif
        },
        .cfg = stm32xwds_usart_drv_cfg,
        .tx = stm32xwds_usart_drv_tx,
        .eq = stm32xwds_usart_drv_eq,
        .putc = stm32xwds_usart_drv_putc,
};

/******** USART1 ********/
struct xwds_uartc stm32xwds_usart1 = {
        /* attributes */
        .dev = {
                .name = "stm32xwds.usart",
                .id = 1,
                .resources = NULL,
                .drv = xwds_cast(struct xwds_driver *, &stm32xwds_usart_drv),
                .data = (void *)&huart1_drvdata,
        },
        .cfg = NULL,
};

/******** USART3 ********/
struct xwds_uartc stm32xwds_usart3 = {
        /* attributes */
        .dev = {
                .name = "stm32xwds.usart",
                .id = 3,
                .resources = NULL,
                .drv = xwds_cast(struct xwds_driver *, &stm32xwds_usart_drv),
                .data = (void *)&huart3_drvdata,
        },
        .cfg = NULL,
};
