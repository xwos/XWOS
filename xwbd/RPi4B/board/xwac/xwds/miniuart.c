/**
 * @file
 * @brief RPi4B：外设驱动库：MINIUART
 * @author
 * + Roy Sun <roy.sun@zqautomotive.com>
 */

#include <xwos/standard.h>
#include <string.h>
#include <xwos/lib/xwlog.h>
#include <xwos/osal/time.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/sem.h>
#include <xwos/osal/sync/cond.h>
#include <xwos/osal/thd.h>
#include <xwcd/ds/uart/controller.h>
#include <xwcd/soc/arm64/v8a/arch_gic2.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_gpio.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_aux.h>

struct rpi4bxwds_miniuart_cfg {
        xwirq_t irqn;
};

struct rpi4bxwds_miniuart_driver_data {
        struct {
                struct xwos_splk splk;
                struct xwos_cond completion;
                xwer_t rc;
                xwsz_t qsize;
                xwsz_t qpos;
                xwsz_t qtail;
                xwu8_t * q;
        } tx;
};

static
xwer_t rpi4bxwds_miniuart_drv_probe(struct xwds_device * dev);

static
xwer_t rpi4bxwds_miniuart_drv_remove(struct xwds_device * dev);

static
xwer_t rpi4bxwds_miniuart_drv_start(struct xwds_device * dev);

static
xwer_t rpi4bxwds_miniuart_drv_stop(struct xwds_device * dev);

static
xwer_t rpi4bxwds_miniuart_drv_suspend(struct xwds_device * dev);

static
xwer_t rpi4bxwds_miniuart_drv_resume(struct xwds_device * dev);

static
xwer_t rpi4bxwds_miniuart_drv_tx(struct xwds_uartc * uartc,
                                 const xwu8_t * data, xwsz_t * size,
                                 xwtm_t to);

static
xwer_t rpi4bxwds_miniuart_drv_etx(struct xwds_uartc * uartc,
                                  const xwu8_t * data, xwsz_t * size);

static
xwer_t rpi4bxwds_miniuart_drv_putc(struct xwds_uartc * uartc,
                                   const xwu8_t byte);

const struct xwds_uartc_driver rpi4bxwds_miniuart_drv = {
        .base = {
                .name = "rpi4bxwds.miniuart",
                .probe = rpi4bxwds_miniuart_drv_probe,
                .remove = rpi4bxwds_miniuart_drv_remove,
                .start = rpi4bxwds_miniuart_drv_start,
                .stop = rpi4bxwds_miniuart_drv_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
                .suspend = rpi4bxwds_miniuart_drv_suspend,
                .resume =  rpi4bxwds_miniuart_drv_resume,
#endif
        },
        .cfg = NULL,
        .tx = rpi4bxwds_miniuart_drv_tx,
        .eq = NULL,
        .etx = rpi4bxwds_miniuart_drv_etx,
        .putc = rpi4bxwds_miniuart_drv_putc,
};


/******** Device ********/
__xwcc_alignl1cache xwu8_t rpi4bxwds_miniuart_txq[8192U] = {0};
__xwcc_alignl1cache xwu8_t rpi4bxwds_miniuart_rxq[8192U] = {0};

const struct rpi4bxwds_miniuart_cfg rpi4bxwds_miniuart_soc_cfg = {
        .irqn = SOC_VC_IRQ_AUX,
};

const struct xwds_uart_cfg rpi4bxwds_miniuart_cfg = {
        .baudrate = 2000000U,
        .bus = {
                .bits = XWDS_UART_BITS_8,
                .stopbits = XWDS_UART_STOPBITS_1_0,
                .parity = XWDS_UART_PARITY_NONE,
                .hfc = XWDS_UART_HFC_NONE,
                .mode = XWDS_UART_MODE_TX | XWDS_UART_MODE_RX,
        },
        .soccfg = (void *)&rpi4bxwds_miniuart_soc_cfg,
};

struct rpi4bxwds_miniuart_driver_data rpi4bxwds_miniuart_drvdata = {
        .tx = {
                .splk = XWOS_SPLK_INITIALIZER,
                .rc = XWOK,
                .qsize = sizeof(rpi4bxwds_miniuart_txq),
                .qpos = 0U,
                .qtail = 0U,
                .q = rpi4bxwds_miniuart_txq,
        },
};

struct xwds_uartc rpi4bxwds_miniuart = {
        /* attributes */
        .dev = {
                .name = "rpi4bxwds.miniuart",
                .id = 0,
                .resources = NULL,
                .drv = xwds_cast(struct xwds_driver *, &rpi4bxwds_miniuart_drv),
                .data = (void *)&rpi4bxwds_miniuart_drvdata,
        },
        .cfg = &rpi4bxwds_miniuart_cfg,
        .rxq = {
                .q = rpi4bxwds_miniuart_rxq,
                .qsize = sizeof(rpi4bxwds_miniuart_rxq),
        },
};

static
void rpi4bxwds_miniuart_on_tx_empty(struct xwds_uartc * uartc)
{
        struct rpi4bxwds_miniuart_driver_data * drvdata;
        xwreg_t cpuirq;

        drvdata = uartc->dev.data;
        soc_miniuart_disable_tx_irq();
        xwos_splk_lock_cpuirqsv(&drvdata->tx.splk, &cpuirq);
        if (-EINPROGRESS == drvdata->tx.rc) {
                while ((soc_aux.mu.lsr.b.tx_empty) &&
                       (drvdata->tx.qpos < drvdata->tx.qtail)) {
                        soc_aux.mu.io.u32 = (xwu32_t)drvdata->tx.q[drvdata->tx.qpos];
                        drvdata->tx.qpos++;
                }
                if (drvdata->tx.qpos == drvdata->tx.qtail) {
                        drvdata->tx.rc = XWOK;
                        xwos_splk_unlock_cpuirqrs(&drvdata->tx.splk, cpuirq);
                        xwos_cond_unicast(&drvdata->tx.completion);
                } else {
                        xwos_splk_unlock_cpuirqrs(&drvdata->tx.splk, cpuirq);
                        soc_miniuart_enable_tx_irq();
                }
        } else {
                xwos_splk_unlock_cpuirqrs(&drvdata->tx.splk, cpuirq);
        }
}

static
void rpi4bxwds_miniuart_on_rx_available(struct xwds_uartc * uartc)
{
        xwu8_t data[16U];
        xwsz_t i;
        xwsq_t pub;

        i = 0;
        soc_miniuart_disable_rx_irq();
        while ((soc_aux.mu.lsr.b.data_ready) && (i < sizeof(data))) {
                data[i] = (xwu8_t)soc_aux.mu.io.u32;
                i++;
        }
        soc_miniuart_enable_rx_irq();
        pub = xwds_uartc_drvcb_rxq_fill(uartc, data, i);
        if (i > 0) {
                xwds_uartc_drvcb_rxq_pub(uartc, pub);
        }
}

static
void rpi4bxwds_miniuart_isr(void)
{
        if (2U == soc_aux.mu.iir.b.status) {
                rpi4bxwds_miniuart_on_rx_available(&rpi4bxwds_miniuart);
        }
        if (1U == soc_aux.mu.iir.b.status) {
                rpi4bxwds_miniuart_on_tx_empty(&rpi4bxwds_miniuart);
        }
}

/******** ******** base driver ******** ********/
static
xwer_t rpi4bxwds_miniuart_drv_probe(struct xwds_device * dev)
{
        struct xwds_uartc * uartc;
        struct rpi4bxwds_miniuart_driver_data * drvdata;

        uartc = xwds_cast(struct xwds_uartc *, dev);
        drvdata = uartc->dev.data;
        xwos_splk_init(&drvdata->tx.splk);
        xwos_cond_init(&drvdata->tx.completion);
        return XWOK;
}

static
xwer_t rpi4bxwds_miniuart_drv_remove(struct xwds_device * dev)
{
        struct xwds_uartc * uartc;
        struct rpi4bxwds_miniuart_driver_data * drvdata;

        uartc = xwds_cast(struct xwds_uartc *, dev);
        drvdata = uartc->dev.data;
        xwos_cond_fini(&drvdata->tx.completion);
        return XWOK;
}

static
xwer_t rpi4bxwds_miniuart_drv_start(struct xwds_device * dev)
{
        struct xwds_uartc * uartc;
        const struct xwds_uart_cfg * uartcfg;
        const struct rpi4bxwds_miniuart_cfg * soccfg;

        uartc = xwds_cast(struct xwds_uartc *, dev);
        uartcfg = uartc->cfg;
        soccfg = uartcfg->soccfg;

        soc_miniuart_disable_tx_irq();
        soc_miniuart_disable_rx_irq();
        soc_miniuart_flush_rx_fifo();
        soc_miniuart_enable_rx_irq();
        armv8a_gic_irq_set_isr(soccfg->irqn, rpi4bxwds_miniuart_isr);
        armv8a_gic_irq_set_priority(soccfg->irqn, armv8a_gic_get_max_priority());
        armv8a_gic_irq_set_trigger_type(soccfg->irqn, ARMV8A_IRQ_TRIGGER_TYPE_LEVEL);
        armv8a_gic_irq_set_affinity_lc(soccfg->irqn);
        armv8a_gic_irq_enable(soccfg->irqn);
        /* armv8a_gic_irq_dump(soccfg->irqn); */
        return XWOK;
}

static
xwer_t rpi4bxwds_miniuart_drv_stop(struct xwds_device * dev)
{
        struct xwds_uartc * uartc;
        const struct xwds_uart_cfg * uartcfg;
        const struct rpi4bxwds_miniuart_cfg * soccfg;

        uartc = xwds_cast(struct xwds_uartc *, dev);
        uartcfg = uartc->cfg;
        soccfg = uartcfg->soccfg;

        armv8a_gic_irq_disable(soccfg->irqn);
        soc_miniuart_disable_tx_irq();
        soc_miniuart_disable_rx_irq();
        return XWOK;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static
xwer_t rpi4bxwds_miniuart_drv_suspend(struct xwds_device * dev)
{
        return rpi4bxwds_miniuart_drv_stop(dev);
}

static
xwer_t rpi4bxwds_miniuart_drv_resume(struct xwds_device * dev)
{
        return rpi4bxwds_miniuart_drv_start(dev);
}
#endif

static
xwer_t rpi4bxwds_miniuart_drv_tx(struct xwds_uartc * uartc,
                                 const xwu8_t * data, xwsz_t * size,
                                 xwtm_t to)
{
        struct rpi4bxwds_miniuart_driver_data * drvdata;
        xwsz_t wrsz;
        xwreg_t cpuirq;
        union xwos_ulock ulk;
        xwsq_t lkst;
        xwer_t rc;

        rc = XWOK;
        drvdata = uartc->dev.data;
        wrsz = *size;
        wrsz = (wrsz > drvdata->tx.qsize) ? drvdata->tx.qsize : wrsz;
        ulk.osal.splk = &drvdata->tx.splk;

        xwos_splk_lock_cpuirqsv(&drvdata->tx.splk, &cpuirq);
        memcpy(&drvdata->tx.q[0U], &data[0U], wrsz);
        drvdata->tx.qpos = 0U;
        drvdata->tx.qtail = wrsz;
        drvdata->tx.rc = -EINPROGRESS;
        soc_miniuart_enable_tx_irq();
        rc = xwos_cond_wait_to(&drvdata->tx.completion,
                               ulk, XWOS_LK_SPLK, NULL,
                               to, &lkst);
        if (XWOK == rc) {
                rc = drvdata->tx.rc;
        } else {
                if ((xwsq_t)XWOS_LKST_UNLOCKED == lkst) {
                        xwos_splk_lock(&drvdata->tx.splk);
                }
                if (-EINPROGRESS == drvdata->tx.rc) {
                        drvdata->tx.rc = -ECANCELED;
                }
        }
        xwos_splk_unlock_cpuirqrs(&drvdata->tx.splk, cpuirq);
        if (XWOK == rc) {
                *size = wrsz;
        }
        return rc;
}

static
xwer_t rpi4bxwds_miniuart_drv_etx(struct xwds_uartc * uartc,
                                  const xwu8_t * data, xwsz_t * size)
{
        struct rpi4bxwds_miniuart_driver_data * drvdata;
        xwreg_t cpuirq;
        xwsz_t txsz;

        drvdata = uartc->dev.data;
        txsz = *size;
        xwos_splk_lock_cpuirqsv(&drvdata->tx.splk, &cpuirq);
        soc_miniuart_write(data, txsz);
        xwos_splk_unlock_cpuirqrs(&drvdata->tx.splk, cpuirq);
        return XWOK;
}

static
xwer_t rpi4bxwds_miniuart_drv_putc(struct xwds_uartc * uartc,
                                   const xwu8_t byte)
{
        XWOS_UNUSED(uartc);
        soc_miniuart_putc(byte);
        return XWOK;
}
