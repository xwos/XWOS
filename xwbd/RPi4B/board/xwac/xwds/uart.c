/**
 * @file
 * @brief RPi4B：外设驱动库：UART
 * @author
 * + Roy Sun <roy.sun@zqautomotive.com>
 */

#include "board/std.h"
#include <string.h>
#include <xwos/osal/time.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/cond.h>
#include <xwos/osal/sync/sem.h>
#include <xwos/osal/thd.h>
#include <xwcd/ds/soc/gpio.h>
#include <xwcd/ds/uart/controller.h>
#include <xwcd/soc/arm64/v8a/arch_gic2.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_gpio.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_uart.h>
#include "board/xwac/xwds/device.h"
#include "board/xwac/xwds/soc.h"

#include <xwos/lib/xwlog.h>
#define LOGTAG "RPi4B.UART%d"
/* #define RPI4BUART_DBG */
#if defined(RPI4BUART_DBG)
#  define rpi4buart_dbgf(uartc, fmt, ...) \
        xwlogf(D, LOGTAG, fmt, uartc->dev.id, ##__VA_ARGS__)
#else
#  define rpi4buart_dbgf(uartc, fmt, ...)
#endif

enum rpi4bxwds_uartc_gpio_em {
        RPI4BXWDS_UARTC_GPIO_TXD = 0,
        RPI4BXWDS_UARTC_GPIO_RXD,
        RPI4BXWDS_UARTC_GPIO_NUM,
};

struct rpi4bxwds_uartc_cfg {
        xwu64_t regbase;
        xwirq_t irqn;
        struct rpi4bxwds_gpio_cfg gpiocfg[RPI4BXWDS_UARTC_GPIO_NUM];
};

struct rpi4bxwds_uartc_driver_data {
        struct {
                struct xwos_splk splk;
                struct xwos_cond completion;
                struct xwos_sem available;
                xwer_t rc;
                xwsz_t qsize;
                xwsz_t qpos;
                xwsz_t qtail;
                xwu8_t * q;
                xwds_uartc_eqcb_f eqcb;
        } tx;
};

static
xwer_t rpi4bxwds_uartc_drv_probe(struct xwds_device * dev);

static
xwer_t rpi4bxwds_uartc_drv_remove(struct xwds_device * dev);

static
xwer_t rpi4bxwds_uartc_drv_start(struct xwds_device * dev);

static
xwer_t rpi4bxwds_uartc_drv_stop(struct xwds_device * dev);
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static
xwer_t rpi4bxwds_uartc_drv_suspend(struct xwds_device * dev);

static
xwer_t rpi4bxwds_uartc_drv_resume(struct xwds_device * dev);
#endif

static
xwer_t rpi4bxwds_uartc_drv_tx(struct xwds_uartc * uartc,
                              const xwu8_t * data, xwsz_t * size,
                              xwtm_t to);

static
xwer_t rpi4bxwds_uartc_drv_etx(struct xwds_uartc * uartc,
                               const xwu8_t * data, xwsz_t * size);

static
xwer_t rpi4bxwds_uartc_drv_eq(struct xwds_uartc * uartc,
                              const xwu8_t * data, xwsz_t * size,
                              xwds_uartc_eqcb_f cb);

static
xwer_t rpi4bxwds_uartc_drv_putc(struct xwds_uartc * uartc,
                                const xwu8_t byte);

static
void rpi4bxwds_uartc_isr(void);

const struct xwds_uartc_driver rpi4bxwds_uartc_drv = {
        .base = {
                .name = "rpi4bxwds.uart",
                .probe = rpi4bxwds_uartc_drv_probe,
                .remove = rpi4bxwds_uartc_drv_remove,
                .start = rpi4bxwds_uartc_drv_start,
                .stop = rpi4bxwds_uartc_drv_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
                .suspend = rpi4bxwds_uartc_drv_suspend,
                .resume =  rpi4bxwds_uartc_drv_resume,
#endif
        },
        .cfg = NULL,
        .tx = rpi4bxwds_uartc_drv_tx,
        .etx = rpi4bxwds_uartc_drv_etx,
        .eq = rpi4bxwds_uartc_drv_eq,
        .putc = rpi4bxwds_uartc_drv_putc,
};

/******** UART5 ********/
__xwcc_alignl1cache xwu8_t rpi4bxwds_uart5_txq[8192U] = {0};
__xwcc_alignl1cache xwu8_t rpi4bxwds_uart5_rxq[8192U] = {0};

const struct rpi4bxwds_uartc_cfg rpi4bxwds_uart5_soc_cfg = {
        .regbase = SOC_UART5_REGBASE,
        .irqn = SOC_VC_IRQ_UART,
        .gpiocfg = {
                [RPI4BXWDS_UARTC_GPIO_TXD] = {
                        .function = SOC_GPIO_ALT4,
                        .pud = SOC_GPIO_PUPDC_NO,
                },
                [RPI4BXWDS_UARTC_GPIO_RXD] = {
                        .function = SOC_GPIO_ALT4,
                        .pud = SOC_GPIO_PUPDC_PU,
                },
        },
};

const struct xwds_uart_cfg rpi4bxwds_uart5_cfg = {
        .baudrate = 2000000U,
        .bus = {
                .bits = XWDS_UART_BITS_8,
                .stopbits = XWDS_UART_STOPBITS_1_0,
                .parity = XWDS_UART_PARITY_NONE,
                .hfc = XWDS_UART_HFC_NONE,
                .mode = XWDS_UART_MODE_TX | XWDS_UART_MODE_RX,
        },
        .soccfg = (void *)&rpi4bxwds_uart5_soc_cfg,
};

struct rpi4bxwds_uartc_driver_data rpi4bxwds_uart5_drvdata = {
        .tx = {
                .splk = XWOS_SPLK_INITIALIZER,
                .rc = XWOK,
                .qsize = sizeof(rpi4bxwds_uart5_txq),
                .qpos = 0U,
                .qtail = 0U,
                .q = rpi4bxwds_uart5_txq,
                .eqcb = NULL,
        },
};

const struct xwds_resource_gpio rpi4bxwds_uart5_gpio_resources[] = {
        [RPI4BXWDS_UARTC_GPIO_TXD] = {
                .soc = &rpi4bxwds_soc,
                .port = XWDS_GPIO_PORT_A,
                .pinmask = XWDS_GPIO_PIN(12),
                .description = "rpi4bxwds.uart.5.txd",
        },
        [RPI4BXWDS_UARTC_GPIO_RXD] = {
                .soc = &rpi4bxwds_soc,
                .port = XWDS_GPIO_PORT_A,
                .pinmask = XWDS_GPIO_PIN(13),
                .description = "rpi4bxwds.uart.5.rxd",
        },
};

const struct xwds_resources rpi4bxwds_uart5_resources = {
        .gpiorsc_array = rpi4bxwds_uart5_gpio_resources,
        .gpiorsc_num = xw_array_size(rpi4bxwds_uart5_gpio_resources),
};

struct xwds_uartc rpi4bxwds_uart5 = {
        /* attributes */
        .dev = {
                .name = "rpi4bxwds.uart",
                .id = 5,
                .resources = &rpi4bxwds_uart5_resources,
                .drv = xwds_cast(struct xwds_driver *, &rpi4bxwds_uartc_drv),
                .data = (void *)&rpi4bxwds_uart5_drvdata,
        },
        .cfg = &rpi4bxwds_uart5_cfg,
        .rxq = {
                .q = rpi4bxwds_uart5_rxq,
                .qsize = sizeof(rpi4bxwds_uart5_rxq),
        },
};

struct xwds_uartc * rpi4bxwds_uartc_table[] = {
        [0] = NULL,
        [1] = NULL,
        [2] = NULL,
        [3] = NULL,
        [4] = NULL,
        [5] = &rpi4bxwds_uart5,
};

/******** ******** base driver ******** ********/
static
xwer_t rpi4bxwds_uartc_drv_probe(struct xwds_device * dev)
{
        struct xwds_uartc * uartc;
        const struct xwds_resources * resources;
        const struct xwds_resource_gpio * gpiorsc_txd;
        const struct xwds_resource_gpio * gpiorsc_rxd;
        const struct xwds_uart_cfg * uartcfg;
        const struct rpi4bxwds_uartc_cfg * soccfg;
        const struct rpi4bxwds_gpio_cfg * gpiocfg_txd;
        const struct rpi4bxwds_gpio_cfg * gpiocfg_rxd;
        struct rpi4bxwds_uartc_driver_data * drvdata;
        xwer_t rc;

        uartc = xwds_cast(struct xwds_uartc *, dev);
        resources = dev->resources;
        gpiorsc_txd = &resources->gpiorsc_array[RPI4BXWDS_UARTC_GPIO_TXD];
        gpiorsc_rxd = &resources->gpiorsc_array[RPI4BXWDS_UARTC_GPIO_RXD];
        uartcfg = uartc->cfg;
        soccfg = uartcfg->soccfg;
        gpiocfg_txd = &soccfg->gpiocfg[RPI4BXWDS_UARTC_GPIO_TXD];
        gpiocfg_rxd = &soccfg->gpiocfg[RPI4BXWDS_UARTC_GPIO_RXD];
        drvdata = uartc->dev.data;

        rc = xwds_gpio_cfg(gpiorsc_txd->soc,
                           gpiorsc_txd->port, gpiorsc_txd->pinmask,
                           (void *)gpiocfg_txd);
        if (rc < 0) {
                rpi4buart_dbgf(uartc, "Cfg GPIO TXD ... %d\r\n", rc);
                goto err_gpio_cfg;
        }
        rc = xwds_gpio_cfg(gpiorsc_rxd->soc,
                           gpiorsc_rxd->port, gpiorsc_rxd->pinmask,
                           (void *)gpiocfg_rxd);
        if (rc < 0) {
                rpi4buart_dbgf(uartc, "Cfg GPIO RXD ... %d\r\n", rc);
                goto err_gpio_cfg;
        }
        rpi4bxwds_uartc_table[uartc->dev.id] = uartc;
        xwos_splk_init(&drvdata->tx.splk);
        xwos_cond_init(&drvdata->tx.completion);
        xwos_sem_init(&drvdata->tx.available, 1, 1);
        rpi4buart_dbgf(uartc, "Probe RPi4B UART ... OK.\r\n");
        return XWOK;

err_gpio_cfg:
        return rc;
}

static
xwer_t rpi4bxwds_uartc_drv_remove(struct xwds_device * dev)
{
        struct xwds_uartc * uartc;
        struct rpi4bxwds_uartc_driver_data * drvdata;

        uartc = xwds_cast(struct xwds_uartc *, dev);
        drvdata = uartc->dev.data;

        xwos_sem_fini(&drvdata->tx.available);
        xwos_cond_fini(&drvdata->tx.completion);
        rpi4buart_dbgf(uartc, "Remove RPi4B UART ... OK\r\n");
        return XWOK;
}

static
xwer_t rpi4bxwds_uartc_drv_start(struct xwds_device * dev)
{
        struct xwds_uartc * uartc;
        const struct xwds_uart_cfg * uartcfg;
        const struct rpi4bxwds_uartc_cfg * soccfg;
        volatile struct soc_uart_regs * regs;

        uartc = xwds_cast(struct xwds_uartc *, dev);
        uartcfg = uartc->cfg;
        soccfg = uartcfg->soccfg;
        regs = (volatile struct soc_uart_regs *)soccfg->regbase;

        regs->cr.u32 = 0U;
        regs->imsc.u32 = 0U;
        /* Attribute */
        regs->lcrh.b.wlen = uartcfg->bus.bits;
        if (XWDS_UART_PARITY_NONE == uartcfg->bus.parity) {
                regs->lcrh.b.pen = 0U;
        } else {
                regs->lcrh.b.pen = 1U;
                regs->imsc.b.peim = 1U;
                if (XWDS_UART_PARITY_EVEN == uartcfg->bus.parity) {
                        regs->lcrh.b.eps = 1U;
                } else {
                        regs->lcrh.b.eps = 0U;
                }
        }
        if (XWDS_UART_HFC_RTS & uartcfg->bus.hfc) {
                regs->cr.b.rtsen = 1U;
        }
        if (XWDS_UART_HFC_CTS & uartcfg->bus.hfc) {
                regs->cr.b.ctsen = 1U;
                regs->imsc.b.ctsmim = 1U;
        }
        if (XWDS_UART_MODE_TX & uartcfg->bus.mode) {
                regs->cr.b.txe = 1U;
        }
        if (XWDS_UART_MODE_RX & uartcfg->bus.mode) {
                regs->cr.b.rxe = 1U;
                regs->imsc.b.rtim = 1U;
                regs->imsc.b.rxim = 1U;
        }

        /* Baudrate */
        xwu32_t integer = SOC_UART_CLK / (16U * uartcfg->baudrate);
        xwu64_t fractional = (SOC_UART_CLK * (xwu64_t)1000) / (16U * uartcfg->baudrate);
        fractional -= (integer * 1000UL);
        fractional = (fractional * 64UL + 500UL) / 1000UL;
        regs->ibrd.u32 = integer;
        regs->fbrd.u32 = (xwu32_t)fractional;

        /* FIFO */
        regs->ifls.b.txiflsel = SOC_UART_IFLS_FIFO_1_8;
        regs->ifls.b.rxiflsel = SOC_UART_IFLS_FIFO_1_2;
        regs->lcrh.b.fen = 1U;

        /* IRQs */
        regs->imsc.b.oeim = 1U;
        regs->imsc.b.beim = 1U;
        regs->imsc.b.feim = 1U;

        armv8a_gic_irq_set_isr(soccfg->irqn, rpi4bxwds_uartc_isr);
        armv8a_gic_irq_set_priority(soccfg->irqn, armv8a_gic_get_max_priority());
        armv8a_gic_irq_set_trigger_type(soccfg->irqn, ARMV8A_IRQ_TRIGGER_TYPE_LEVEL);
        armv8a_gic_irq_set_affinity_lc(soccfg->irqn);
        armv8a_gic_irq_enable(soccfg->irqn);
        /* armv8a_gic_irq_dump(soccfg->irqn); */

        regs->cr.b.uarten = 1U;
        rpi4buart_dbgf(uartc, "Start RPi4B UART ... OK\r\n");
        return XWOK;
}

static
xwer_t rpi4bxwds_uartc_drv_stop(struct xwds_device * dev)
{
        struct xwds_uartc * uartc;
        const struct xwds_uart_cfg * uartcfg;
        const struct rpi4bxwds_uartc_cfg * soccfg;
        volatile struct soc_uart_regs * regs;

        uartc = xwds_cast(struct xwds_uartc *, dev);
        uartcfg = uartc->cfg;
        soccfg = uartcfg->soccfg;
        regs = (volatile struct soc_uart_regs *)soccfg->regbase;

        regs->imsc.u32 = 0U;
        regs->cr.b.uarten = 0;
        rpi4buart_dbgf(uartc, "Stop RPi4B UART ... OK\r\n");
        return XWOK;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static
xwer_t rpi4bxwds_uartc_drv_suspend(struct xwds_device * dev)
{
        return rpi4bxwds_uartc_drv_stop(dev);
}

static
xwer_t rpi4bxwds_uartc_drv_resume(struct xwds_device * dev)
{
        return rpi4bxwds_uartc_drv_start(dev);
}
#endif

/******** ******** Uartc drivers ******** ********/
static
xwer_t rpi4bxwds_uartc_drv_tx(struct xwds_uartc * uartc,
                              const xwu8_t * data, xwsz_t * size,
                              xwtm_t to)
{
        const struct xwds_uart_cfg * uartcfg;
        const struct rpi4bxwds_uartc_cfg * soccfg;
        volatile struct soc_uart_regs * regs;
        struct rpi4bxwds_uartc_driver_data * drvdata;
        xwsz_t wrsz;
        xwreg_t cpuirq;
        union xwos_ulock ulk;
        xwsq_t lkst;
        xwer_t rc;

        uartcfg = uartc->cfg;
        soccfg = uartcfg->soccfg;
        regs = (volatile struct soc_uart_regs *)soccfg->regbase;
        drvdata = uartc->dev.data;
        rc = xwos_sem_wait_to(&drvdata->tx.available, to);
        if (rc < 0) {
                goto err_available;
        }
        wrsz = *size;
        wrsz = (wrsz > drvdata->tx.qsize) ? drvdata->tx.qsize : wrsz;
        if (0U == wrsz) {
                goto err_size;
        }
        ulk.osal.splk = &drvdata->tx.splk;
        xwos_splk_lock_cpuirqsv(&drvdata->tx.splk, &cpuirq);
        memcpy(&drvdata->tx.q[0U], &data[0U], wrsz);
        drvdata->tx.qpos = 0U;
        drvdata->tx.qtail = wrsz;
        drvdata->tx.rc = -EINPROGRESS;
        regs->icr.b.txic = 1U;
        regs->imsc.b.txim = 1U;
        while ((0U == regs->fr.b.txff) &&
               (drvdata->tx.qpos < drvdata->tx.qtail)) {
                regs->dr = (xwu32_t)drvdata->tx.q[drvdata->tx.qpos];
                drvdata->tx.qpos++;
        }
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

err_size:
err_available:
        return rc;
}

static
xwer_t rpi4bxwds_uartc_drv_etx(struct xwds_uartc * uartc,
                               const xwu8_t * data, xwsz_t * size)
{
        const struct xwds_uart_cfg * uartcfg;
        const struct rpi4bxwds_uartc_cfg * soccfg;
        volatile struct soc_uart_regs * regs;
        struct rpi4bxwds_uartc_driver_data * drvdata;
        xwreg_t cpuirq;
        xwsq_t idx;
        xwsz_t txsz;

        uartcfg = uartc->cfg;
        soccfg = uartcfg->soccfg;
        regs = (volatile struct soc_uart_regs *)soccfg->regbase;
        drvdata = uartc->dev.data;
        txsz = *size;
        idx = 0;
        xwos_splk_lock_cpuirqsv(&drvdata->tx.splk, &cpuirq);
        while (idx < txsz) {
                while (0U == regs->fr.b.txff) {
                        regs->dr = data[idx];
                        idx++;
                }
        }
        xwos_splk_unlock_cpuirqrs(&drvdata->tx.splk, cpuirq);
        return XWOK;
}

static
xwer_t rpi4bxwds_uartc_drv_eq(struct xwds_uartc * uartc,
                              const xwu8_t * data, xwsz_t * size,
                              xwds_uartc_eqcb_f cb)
{
        const struct xwds_uart_cfg * uartcfg;
        const struct rpi4bxwds_uartc_cfg * soccfg;
        volatile struct soc_uart_regs * regs;
        struct rpi4bxwds_uartc_driver_data * drvdata;
        xwsz_t wrsz;
        xwreg_t cpuirq;
        xwer_t rc;

        uartcfg = uartc->cfg;
        soccfg = uartcfg->soccfg;
        regs = (volatile struct soc_uart_regs *)soccfg->regbase;
        drvdata = uartc->dev.data;
        rc = xwos_sem_trywait(&drvdata->tx.available);
        if (rc < 0) {
                goto err_available;
        }
        wrsz = *size;
        wrsz = (wrsz > drvdata->tx.qsize) ? drvdata->tx.qsize : wrsz;
        if (0U == wrsz) {
                goto err_size;
        }
        xwos_splk_lock_cpuirqsv(&drvdata->tx.splk, &cpuirq);
        memcpy(&drvdata->tx.q[0U], &data[0U], wrsz);
        drvdata->tx.qpos = 0U;
        drvdata->tx.qtail = wrsz;
        drvdata->tx.rc = -EINPROGRESS;
        drvdata->tx.eqcb = cb;
        regs->icr.b.txic = 1U;
        regs->imsc.b.txim = 1U;
        while ((0U == regs->fr.b.txff) &&
               (drvdata->tx.qpos < drvdata->tx.qtail)) {
                regs->dr = (xwu32_t)drvdata->tx.q[drvdata->tx.qpos];
                drvdata->tx.qpos++;
        }
        xwos_splk_unlock_cpuirqrs(&drvdata->tx.splk, cpuirq);
        *size = wrsz;

err_size:
err_available:
        return rc;
}

static
xwer_t rpi4bxwds_uartc_drv_putc(struct xwds_uartc * uartc,
                                const xwu8_t byte)
{
        const struct xwds_uart_cfg * uartcfg;
        const struct rpi4bxwds_uartc_cfg * soccfg;
        volatile struct soc_uart_regs * regs;

        uartcfg = uartc->cfg;
        soccfg = uartcfg->soccfg;
        regs = (volatile struct soc_uart_regs *)soccfg->regbase;
        while (1U == regs->fr.b.txff) {
        }
        regs->dr = byte;
        return XWOK;
}

/******** ******** IRQ ******** ********/
static
void rpi4bxwds_uartc_on_tx(struct xwds_uartc * uartc)
{
        const struct xwds_uart_cfg * uartcfg;
        const struct rpi4bxwds_uartc_cfg * soccfg;
        volatile struct soc_uart_regs * regs;
        struct rpi4bxwds_uartc_driver_data * drvdata;
        xwds_uartc_eqcb_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        uartcfg = uartc->cfg;
        soccfg = uartcfg->soccfg;
        regs = (volatile struct soc_uart_regs *)soccfg->regbase;
        drvdata = uartc->dev.data;

        regs->imsc.b.txim = 0U;
        xwos_splk_lock_cpuirqsv(&drvdata->tx.splk, &cpuirq);
        if (-EINPROGRESS == drvdata->tx.rc) {
                while ((0U == regs->fr.b.txff) &&
                       (drvdata->tx.qpos < drvdata->tx.qtail)) {
                        regs->dr = (xwu32_t)drvdata->tx.q[drvdata->tx.qpos];
                        drvdata->tx.qpos++;
                }
                if (drvdata->tx.qpos == drvdata->tx.qtail) {
                        drvdata->tx.rc = XWOK;
                        cb = drvdata->tx.eqcb;
                        drvdata->tx.eqcb = NULL;
                        rc = drvdata->tx.rc;
                        xwos_splk_unlock_cpuirqrs(&drvdata->tx.splk, cpuirq);
                        xwos_cond_unicast(&drvdata->tx.completion);
                        if (NULL != cb) {
                                cb(uartc, rc);
                        }
                        xwos_sem_post(&drvdata->tx.available);
                } else {
                        xwos_splk_unlock_cpuirqrs(&drvdata->tx.splk, cpuirq);
                        regs->imsc.b.txim = 1U;
                }
        } else {
                cb = drvdata->tx.eqcb;
                drvdata->tx.eqcb = NULL;
                rc = drvdata->tx.rc;
                xwos_splk_unlock_cpuirqrs(&drvdata->tx.splk, cpuirq);
                if (NULL != cb) {
                        cb(uartc, rc);
                }
                xwos_sem_post(&drvdata->tx.available);
        }
}

static
void rpi4bxwds_uartc_on_rx_error(struct xwds_uartc * uartc)
{
        xwds_uartc_drvcb_rxq_flush(uartc);
}

static
void rpi4bxwds_uartc_on_rx(struct xwds_uartc * uartc)
{
        xwu8_t data[64U];
        const struct xwds_uart_cfg * uartcfg;
        const struct rpi4bxwds_uartc_cfg * soccfg;
        volatile struct soc_uart_regs * regs;
        xwsq_t pub;
        xwsz_t i;

        uartcfg = uartc->cfg;
        soccfg = uartcfg->soccfg;
        regs = (volatile struct soc_uart_regs *)soccfg->regbase;
        i = 0;
        while ((0 == regs->fr.b.rxfe) && (i < sizeof(data))) {
                data[i] = (xwu8_t)regs->dr;
                i++;
        }
        pub = xwds_uartc_drvcb_rxq_fill(uartc, data, i);
        if (i > 0) {
                xwds_uartc_drvcb_rxq_pub(uartc, pub);
        }
}

static
void rpi4bxwds_uartc_isr(void)
{
        struct xwds_uartc * uartc;
        const struct xwds_uart_cfg * uartcfg;
        const struct rpi4bxwds_uartc_cfg * soccfg;
        volatile struct soc_uart_regs * regs;
        xwsz_t num = xw_array_size(rpi4bxwds_uartc_table);
        xwsz_t i;

        for (i = 0; i < num; i++) {
                uartc = rpi4bxwds_uartc_table[i];
                if (NULL == uartc) {
                        continue;
                }
                uartcfg = uartc->cfg;
                soccfg = uartcfg->soccfg;
                regs = (volatile struct soc_uart_regs *)soccfg->regbase;
                if ((1 == regs->ris.b.ctsmris) && (1 == regs->mis.b.ctsmmis)) {
                        rpi4buart_dbgf(uartc, "CTS IRQ\r\n");
                        regs->icr.b.ctsmic = 1U;
                }
                if ((1 == regs->ris.b.rxris) && (1 == regs->mis.b.rxmis)) {
                        rpi4buart_dbgf(uartc, "RX IRQ\r\n");
                        regs->icr.b.rxic = 1U;
                        rpi4bxwds_uartc_on_rx(uartc);
                }
                if ((1 == regs->ris.b.txris) && (1 == regs->mis.b.txmis)) {
                        rpi4buart_dbgf(uartc, "TX IRQ\r\n");
                        regs->icr.b.txic = 1U;
                        rpi4bxwds_uartc_on_tx(uartc);
                }
                if ((1 == regs->ris.b.rtris) && (1 == regs->mis.b.rtmis)) {
                        rpi4buart_dbgf(uartc, "RX Timeout IRQ\r\n");
                        regs->icr.b.rtic = 1U;
                        rpi4bxwds_uartc_on_rx(uartc);
                }
                if ((1 == regs->ris.b.feris) && (1 == regs->mis.b.femis)) {
                        rpi4buart_dbgf(uartc, "Frame Error IRQ\r\n");
                        regs->icr.b.feic = 1U;
                        rpi4bxwds_uartc_on_rx_error(uartc);
                }
                if ((1 == regs->ris.b.peris) && (1 == regs->mis.b.pemis)) {
                        rpi4buart_dbgf(uartc, "Parity Error IRQ\r\n");
                        regs->icr.b.peic = 1U;
                        rpi4bxwds_uartc_on_rx_error(uartc);
                }
                if ((1 == regs->ris.b.beris) && (1 == regs->mis.b.bemis)) {
                        rpi4buart_dbgf(uartc, "Break IRQ\r\n");
                        regs->icr.b.beic = 1U;
                }
                if ((1 == regs->ris.b.oeris) && (1 == regs->mis.b.oemis)) {
                        rpi4buart_dbgf(uartc, "Overrun Error IRQ\r\n");
                        regs->icr.b.oeic = 1U;
                        rpi4bxwds_uartc_on_rx_error(uartc);
                }
        }
}
