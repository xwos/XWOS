/**
 * @file
 * @brief RPi4B：外设驱动库：UART
 * @author
 * + Roy Sun <roy.sun@zqautomotive.com>
 */

#include <xwos/standard.h>
#include <string.h>
#include <xwos/lib/xwlog.h>
#include <xwos/osal/time.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/cond.h>
#include <xwos/osal/thd.h>
#include <xwcd/ds/uart/controller.h>
#include <xwcd/soc/arm64/v8a/arch_gic2.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_gpio.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_uart.h>

#define LOGTAG "RPi4B.UART.%d"
/* #define SOC_DBGF */
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_debug.h>
#define rpi4buart_dbgf(uartc, fmt, ...) \
        soc_dbgf(LOGTAG, fmt, uartc->dev.id, ##__VA_ARGS__)

struct rpi4bxwds_uartc_cfg {
        xwu64_t regbase;
        xwirq_t irqn;
        void (* pin_init)(void); /**< PIN申请函数 */
        void (* pin_fini)(void); /**< PIN释放函数 */
};

struct rpi4bxwds_uartc_driver_data {
        struct {
                struct xwos_splk splk;
                struct xwos_cond completion;
                xwer_t rc;
        } tx;
};

struct xwds_uartc * rpi4bxwds_uartc_table[] = {
        [0] = NULL,
        [1] = NULL,
        [2] = NULL,
        [3] = NULL,
        [4] = NULL,
        [5] = NULL,
};

static
void rpi4bxwds_uartc_cb_tx_completion(struct xwds_uartc * uartc, xwer_t rc)
{
        struct rpi4bxwds_uartc_driver_data * drvdata;
        xwreg_t cpuirq;

        drvdata = uartc->dev.data;

        xwos_splk_lock_cpuirqsv(&drvdata->tx.splk, &cpuirq);
        if (-EINPROGRESS == drvdata->tx.rc) {
                drvdata->tx.rc = rc;
        }
        xwos_splk_unlock_cpuirqrs(&drvdata->tx.splk, cpuirq);
        xwos_cond_broadcast(&drvdata->tx.completion);
}

static
void rpi4bxwds_uartc_cb_rx_error(struct xwds_uartc * uartc)
{
        xwds_uartc_drvcb_rxq_flush(uartc);
}

static
void rpi4bxwds_uartc_cb_rx(struct xwds_uartc * uartc)
{
        const struct xwds_uart_cfg * uartcfg;
        const struct rpi4bxwds_uartc_cfg * soccfg;
        volatile struct soc_uart_regs * regs;
        xwu8_t byte;
        xwsz_t i;
        xwsq_t pub;

        uartcfg = uartc->cfg;
        soccfg = uartcfg->soccfg;
        regs = (volatile struct soc_uart_regs *)soccfg->regbase;
        i = 0;
        while (0 == regs->fr.bit.rxfe) {
                byte = regs->dr & (xwu8_t)0xFF;
                pub = xwds_uartc_drvcb_rxq_fill(uartc, &byte, 1);
                i++;
        };
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
                if ((1 == regs->ris.bit.ctsmris) && (1 == regs->mis.bit.ctsmmis)) {
                        regs->icr.bit.ctsmic = 1;
                        rpi4buart_dbgf(uartc, "CTS IRQ\r\n");
                }
                if ((1 == regs->ris.bit.rxris) && (1 == regs->mis.bit.rxmis)) {
                        regs->icr.bit.rxic = 1;
                        rpi4buart_dbgf(uartc, "RX IRQ\r\n");
                        rpi4bxwds_uartc_cb_rx(uartc);
                }
                if ((1 == regs->ris.bit.txris) && (1 == regs->mis.bit.txmis)) {
                        regs->icr.bit.txic = 1;
                        rpi4buart_dbgf(uartc, "TX IRQ\r\n");
                        rpi4bxwds_uartc_cb_tx_completion(uartc, XWOK);
                }
                if ((1 == regs->ris.bit.rtris) && (1 == regs->mis.bit.rtmis)) {
                        regs->icr.bit.rtic = 1;
                        rpi4buart_dbgf(uartc, "RX Timeout IRQ\r\n");
                        rpi4bxwds_uartc_cb_rx(uartc);
                }
                if ((1 == regs->ris.bit.feris) && (1 == regs->mis.bit.femis)) {
                        regs->icr.bit.feic = 1;
                        rpi4buart_dbgf(uartc, "Frame Error IRQ\r\n");
                        rpi4bxwds_uartc_cb_rx_error(uartc);
                }
                if ((1 == regs->ris.bit.peris) && (1 == regs->mis.bit.pemis)) {
                        regs->icr.bit.peic = 1;
                        rpi4buart_dbgf(uartc, "Parity Error IRQ\r\n");
                        rpi4bxwds_uartc_cb_rx_error(uartc);
                }
                if ((1 == regs->ris.bit.beris) && (1 == regs->mis.bit.bemis)) {
                        regs->icr.bit.beic = 1;
                        rpi4buart_dbgf(uartc, "Break IRQ\r\n");
                }
                if ((1 == regs->ris.bit.oeris) && (1 == regs->mis.bit.oemis)) {
                        regs->icr.bit.oeic = 1;
                        rpi4buart_dbgf(uartc, "Overrun Error IRQ\r\n");
                        rpi4bxwds_uartc_cb_rx_error(uartc);
                }
        }
}

/******** ******** base driver ******** ********/
xwer_t rpi4bxwds_uartc_drv_probe(struct xwds_device * dev)
{
        struct xwds_uartc * uartc;
        const struct xwds_uart_cfg * uartcfg;
        const struct rpi4bxwds_uartc_cfg * soccfg;
        struct rpi4bxwds_uartc_driver_data * drvdata;

        uartc = xwds_cast(struct xwds_uartc *, dev);
        uartcfg = uartc->cfg;
        soccfg = uartcfg->soccfg;
        drvdata = uartc->dev.data;

        if (NULL != soccfg->pin_init) {
                soccfg->pin_init();
        }
        rpi4bxwds_uartc_table[uartc->dev.id] = uartc;
        xwos_splk_init(&drvdata->tx.splk);
        xwos_cond_init(&drvdata->tx.completion);
        rpi4buart_dbgf(uartc, "Probe RPi4B UART.\r\n");
        return XWOK;
}

xwer_t rpi4bxwds_uartc_drv_remove(struct xwds_device * dev)
{
        struct xwds_uartc * uartc;
        const struct xwds_uart_cfg * uartcfg;
        const struct rpi4bxwds_uartc_cfg * soccfg;
        struct rpi4bxwds_uartc_driver_data * drvdata;

        uartc = xwds_cast(struct xwds_uartc *, dev);
        uartcfg = uartc->cfg;
        soccfg = uartcfg->soccfg;
        drvdata = uartc->dev.data;

        xwos_cond_fini(&drvdata->tx.completion);
        rpi4bxwds_uartc_table[uartc->dev.id] = NULL;
        if (NULL != soccfg->pin_fini) {
                soccfg->pin_fini();
        }
        rpi4buart_dbgf(uartc, "Remove RPi4B UART.\r\n");
        return XWOK;
}

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
        regs->lcrh.bit.wlen = uartcfg->bus.bits;
        if (XWDS_UART_PARITY_NONE == uartcfg->bus.parity) {
                regs->lcrh.bit.pen = 0U;
        } else {
                regs->lcrh.bit.pen = 1U;
                regs->imsc.bit.peim = 1U;
                if (XWDS_UART_PARITY_EVEN == uartcfg->bus.parity) {
                        regs->lcrh.bit.eps = 1U;
                } else {
                        regs->lcrh.bit.eps = 0U;
                }
        }
        if (XWDS_UART_HFC_RTS & uartcfg->bus.hfc) {
                regs->cr.bit.rtsen = 1U;
        }
        if (XWDS_UART_HFC_CTS & uartcfg->bus.hfc) {
                regs->cr.bit.ctsen = 1U;
                regs->imsc.bit.ctsmim = 1U;
        }
        if (XWDS_UART_MODE_TX & uartcfg->bus.mode) {
                regs->cr.bit.txe = 1U;
        }
        if (XWDS_UART_MODE_RX & uartcfg->bus.mode) {
                regs->cr.bit.rxe = 1U;
                regs->imsc.bit.rtim = 1U;
                regs->imsc.bit.rxim = 1U;
        }

        /* Baudrate */
        xwu32_t integer = SOC_UART_CLK / (16U * uartcfg->baudrate);
        xwu64_t fractional = (SOC_UART_CLK * (xwu64_t)1000) / (16U * uartcfg->baudrate);
        fractional -= (integer * 1000UL);
        fractional = (fractional * 64UL + 500UL) / 1000UL;
        regs->ibrd.u32 = integer;
        regs->fbrd.u32 = (xwu32_t)fractional;

        /* FIFO */
        regs->ifls.bit.txiflsel = SOC_UART_IFLS_FIFO_1_8;
        regs->ifls.bit.rxiflsel = SOC_UART_IFLS_FIFO_1_2;
        regs->lcrh.bit.fen = 1U;

        /* IRQs */
        regs->imsc.bit.oeim = 1U;
        regs->imsc.bit.beim = 1U;
        regs->imsc.bit.feim = 1U;

        armv8a_gic_irq_set_isr(soccfg->irqn, rpi4bxwds_uartc_isr);
        armv8a_gic_irq_set_priority(soccfg->irqn, armv8a_gic_get_max_priority());
        armv8a_gic_irq_set_trigger_type(soccfg->irqn, ARMV8A_IRQ_TRIGGER_TYPE_LEVEL);
        armv8a_gic_irq_set_affinity_lc(soccfg->irqn);
        armv8a_gic_irq_enable(soccfg->irqn);
        armv8a_gic_irq_dump(soccfg->irqn);

        regs->cr.bit.uarten = 1U;
        rpi4buart_dbgf(uartc, "Start RPi4B UART.\r\n");
        return XWOK;
}

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
        regs->cr.bit.uarten = 0;
        rpi4buart_dbgf(uartc, "Stop RPi4B UART.\r\n");
        return XWOK;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
xwer_t rpi4bxwds_uartc_drv_suspend(struct xwds_device * dev)
{
        rpi4buart_dbgf(xwds_cast(struct xwds_uartc *, dev), "Suspend RPi4B UART.\r\n");
        return rpi4bxwds_uartc_drv_stop(dev);
}

xwer_t rpi4bxwds_uartc_drv_resume(struct xwds_device * dev)
{
        rpi4buart_dbgf(xwds_cast(struct xwds_uartc *, dev), "Resume RPi4B UART.\r\n");
        return rpi4bxwds_uartc_drv_start(dev);
}
#endif

xwer_t rpi4bxwds_uartc_drv_tx(struct xwds_uartc * uartc,
                              const xwu8_t * data, xwsz_t * size,
                              xwtm_t to)
{
        const struct xwds_uart_cfg * uartcfg;
        const struct rpi4bxwds_uartc_cfg * soccfg;
        volatile struct soc_uart_regs * regs;
        struct rpi4bxwds_uartc_driver_data * drvdata;
        xwsz_t wrsz;
        xwsz_t i;
        const xwu8_t * pos;
        xwreg_t cpuirq;
        union xwos_ulock ulk;
        xwsq_t lkst;
        xwer_t rc;

        uartcfg = uartc->cfg;
        soccfg = uartcfg->soccfg;
        regs = (volatile struct soc_uart_regs *)soccfg->regbase;
        drvdata = uartc->dev.data;
        wrsz = *size;
        pos = data;
        rc = XWOK;
        ulk.osal.splk = &drvdata->tx.splk;

        while (wrsz > 0) {
                xwos_splk_lock_cpuirqsv(&drvdata->tx.splk, &cpuirq);
                drvdata->tx.rc = -EINPROGRESS;
                for (i = 0; ((i < wrsz) && (0U == regs->fr.bit.txff)); i++, pos++) {
                        regs->dr = *pos;
                }
                regs->imsc.bit.txim = 1U;
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
                if (rc < 0) {
                        break;
                }
                wrsz -= i;
        }
        *size -= wrsz;
        return rc;
}

xwer_t rpi4bxwds_uartc_drv_putc(struct xwds_uartc * uartc,
                                const xwu8_t byte)
{
        const struct xwds_uart_cfg * uartcfg;
        const struct rpi4bxwds_uartc_cfg * soccfg;
        volatile struct soc_uart_regs * regs;

        uartcfg = uartc->cfg;
        soccfg = uartcfg->soccfg;
        regs = (volatile struct soc_uart_regs *)soccfg->regbase;
        while (1U == regs->fr.bit.txff) {
        }
        regs->dr = byte;
        return XWOK;
}

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
        .putc = rpi4bxwds_uartc_drv_putc,
};


/******** UART5 ********/
void rpi4bxwds_uart5_pin_init(void)
{
        soc_gpio.gpfsel1.bit.fsel12 = SOC_GPIO_ALT4;
        soc_gpio.gpfsel1.bit.fsel13 = SOC_GPIO_ALT4;
        soc_gpio.gpio_pup_pdn_cntrl_reg0.bit.pin12 = SOC_GPIO_PUPDC_PU;
        soc_gpio.gpio_pup_pdn_cntrl_reg0.bit.pin13 = SOC_GPIO_PUPDC_PU;
}

void rpi4bxwds_uart5_pin_fini(void)
{
        soc_gpio.gpfsel1.bit.fsel12 = SOC_GPIO_ALT_GPI;
        soc_gpio.gpfsel1.bit.fsel13 = SOC_GPIO_ALT_GPI;
        soc_gpio.gpio_pup_pdn_cntrl_reg0.bit.pin12 = SOC_GPIO_PUPDC_NO;
        soc_gpio.gpio_pup_pdn_cntrl_reg0.bit.pin13 = SOC_GPIO_PUPDC_NO;
}

const struct rpi4bxwds_uartc_cfg rpi4bxwds_uart5_soc_cfg = {
        .regbase = SOC_UART5_REGBASE,
        .irqn = 57 + SOC_GIC2_IRQn_OFFSET_VC,
        .pin_init = rpi4bxwds_uart5_pin_init,
        .pin_fini = rpi4bxwds_uart5_pin_fini,
};

const struct xwds_uart_cfg rpi4bxwds_uart5_cfg = {
        .baudrate = 1000000U,
        .bus = {
                .bits = XWDS_UART_BITS_8,
                .stopbits = XWDS_UART_STOPBITS_1_0,
                .parity = XWDS_UART_PARITY_NONE,
                .hfc = XWDS_UART_HFC_NONE,
                .mode = XWDS_UART_MODE_TX | XWDS_UART_MODE_RX,
        },
        .soccfg = (void *)&rpi4bxwds_uart5_soc_cfg,
};

struct rpi4bxwds_uartc_driver_data rpi4bxwds_uart5_drvdata;

struct xwds_uartc rpi4bxwds_uart5 = {
        /* attributes */
        .dev = {
                .name = "rpi4bxwds.uart",
                .id = 5,
                .resources = NULL,
                .drv = xwds_cast(struct xwds_driver *, &rpi4bxwds_uartc_drv),
                .data = (void *)&rpi4bxwds_uart5_drvdata,
        },
        .cfg = &rpi4bxwds_uart5_cfg,
};
