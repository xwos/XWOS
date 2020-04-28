/**
 * @file
 * @brief 板级描述层：MISC设备驱动
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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
#include <xwos/standard.h>
#include <xwos/lib/xwlog.h>
#include <xwos/irq.h>
#include <soc_up_irqc.h>
#include <soc_adc.h>
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwmd/ds/soc/gpio.h>
#include <xwmd/ds/soc/clock.h>
#include <xwmd/ds/misc/chip.h>
#include <bdl/ds/misc/api.h>
#include <bdl/ds/misc/driver.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwmd_code
xwer_t mpc560xb_misc_check_desc(struct xwds_misc * adc);

static __xwmd_code
xwer_t mpc560xb_misc_drv_probe(struct xwds_device * dev);

static __xwmd_code
xwer_t mpc560xb_misc_drv_remove(struct xwds_device * dev);

static __xwmd_code
xwer_t mpc560xb_misc_drv_start(struct xwds_device * dev);

static __xwmd_code
xwer_t mpc560xb_misc_drv_stop(struct xwds_device * dev);

static __xwmd_code
xwer_t mpc560xb_misc_drv_suspend(struct xwds_device * dev);

static __xwmd_code
xwer_t mpc560xb_misc_drv_resume(struct xwds_device * dev);

static __xwmd_code
xwer_t mpc560xb_misc_drv_ioctl(struct xwds_misc * adc, xwsq_t cmd, va_list args);

static __xwmd_code
xwer_t mpc560xb_adc_setup(struct xwds_misc * misc);

static __xwmd_code
xwer_t mpc560xb_adc_clean(struct xwds_misc * misc);

static __xwmd_code
xwer_t mpc560xb_adc_convert(struct xwds_misc * misc, xwbmp_t * chmask, xws16_t * buf,
                            xwtm_t * xwtm);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwmd_rodata const struct xwds_misc_driver mpc560xb_misc_drv = {
        .base = {
                .name = "mpc560xb_misc_drv",
                .probe = mpc560xb_misc_drv_probe,
                .remove = mpc560xb_misc_drv_remove,
                .start = mpc560xb_misc_drv_start,
                .stop = mpc560xb_misc_drv_stop,
                .suspend = mpc560xb_misc_drv_suspend,
                .resume =  mpc560xb_misc_drv_resume,
        },
        .ioctl = mpc560xb_misc_drv_ioctl,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwmd_code
xwer_t mpc560xb_misc_check_desc(struct xwds_misc * misc)
{
        const struct xwds_resources * resources;
        const struct bdl_misc_private_cfg * xwccfg;
        xwer_t rc;

        resources = misc->dev.resources;
        xwccfg = misc->misccfg;
        if (__unlikely(is_err_or_null(resources))) {
                rc = -EINVAL;
        } else {
                if(__unlikely(is_err_or_null(xwccfg))) {
                        rc = -EINVAL;
                } else {
                        rc = OK;
                }
        }

        return rc;
}

/******** ******** base driver ******** ********/
static __xwmd_code
xwer_t mpc560xb_misc_drv_probe(struct xwds_device * dev)
{
        struct xwds_misc * misc;
        struct mpc560xb_misc_drvdata * drvdata;
        xwer_t rc;

        misc = xwds_static_cast(struct xwds_misc *, dev);
        rc = mpc560xb_misc_check_desc(misc);
        if (__unlikely(rc < 0)) {
                goto err_chkdesc;
        }

        drvdata = misc->dev.data;
        xwosal_splk_init(&drvdata->adc.lock);
        drvdata->adc.tid = (xwid_t)0;
        xwosal_mtx_init(&drvdata->adc.mtx, XWOSAL_SD_PRIORITY_RT_MIN);

        return OK;

err_chkdesc:
        return rc;
}

static __xwmd_code
xwer_t mpc560xb_misc_drv_remove(struct xwds_device * dev)
{
        struct xwds_misc * misc;
        struct mpc560xb_misc_drvdata * drvdata;

        misc = xwds_static_cast(struct xwds_misc *, dev);
        drvdata = misc->dev.data;
        xwosal_mtx_destroy(&drvdata->adc.mtx);

        return OK;
}

static __xwmd_code
xwer_t mpc560xb_misc_drv_start(struct xwds_device * dev)
{
        struct xwds_misc * misc;
        const struct xwds_resources * resources;
        const struct xwos_irq_resource * irqrsc;
        const struct xwds_resource_clk * clkrsc;
        const struct xwds_resource_gpio * gpiorsc;
        xwssz_t i, j;
        xwer_t rc;

        misc = xwds_static_cast(struct xwds_misc *, dev);
        resources = misc->dev.resources;

        /* 请求中断 */
        for (i = 0; i < (xwssz_t)resources->irqrsc_num; i++) {
                irqrsc = &resources->irqrsc_array[i];
                rc = xwos_irq_request(irqrsc->irqn, irqrsc->isr,
                                      XWOS_UNUSED_ARGUMENT, dev);
                if (__unlikely(rc < 0)) {
                        for (j = i - 1; j >= 0; j--) {
                                irqrsc = &resources->irqrsc_array[j];
                                xwos_irq_release(irqrsc->irqn);
                        }
                        goto err_req_irqs;
                }
        }

        /* 配置Clock */
        for (i = 0; i < (xwssz_t)resources->clkrsc_num; i++) {
                clkrsc = &resources->clkrsc_array[i];
                rc = xwds_clk_req(clkrsc->soc, clkrsc->clkid);
                if (__unlikely(rc < 0)) {
                        for (j = i - 1; j >= 0; j--) {
                                clkrsc = &resources->clkrsc_array[j];
                                xwds_clk_rls(clkrsc->soc, clkrsc->clkid);
                        }
                        goto err_req_clks;
                }
        }

        /* 配置GPIO */
        for (i = 0; i < (xwssz_t)resources->gpiorsc_num; i++) {
                gpiorsc = &resources->gpiorsc_array[i];
                rc = xwds_gpio_req(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
                if (__unlikely(rc < 0)) {
                        for (j = i - 1; j >= 0; j--) {
                                gpiorsc = &resources->gpiorsc_array[j];
                                xwds_gpio_rls(gpiorsc->soc, gpiorsc->port,
                                              gpiorsc->pinmask);
                        }
                        goto err_req_gpios;
                }
        }

        /* 使能中断 */
        for (i = 0; i < (xwssz_t)resources->irqrsc_num; i++) {
                irqrsc = &resources->irqrsc_array[i];
                xwos_irq_clear(irqrsc->irqn);
                rc = xwos_irq_cfg(irqrsc->irqn, irqrsc->cfg);
                if (OK == rc)
                        rc = xwos_irq_enable(irqrsc->irqn);
                if (__unlikely(rc < 0)) {
                        for (j = i - 1; j >= 0; j--) {
                                irqrsc = &resources->irqrsc_array[j];
                                xwos_irq_disable(irqrsc->irqn);
                        }
                        goto err_en_irq;
                }
        }

        return OK;

err_en_irq:
        for (j = (xwssz_t)resources->gpiorsc_num - 1; j >= 0; j--) {
                gpiorsc = &resources->gpiorsc_array[j];
                xwds_gpio_rls(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
        }
err_req_gpios:
        for (j = (xwssz_t)resources->clkrsc_num - 1; j >= 0; j--) {
                clkrsc = &resources->clkrsc_array[j];
                xwds_clk_rls(clkrsc->soc, clkrsc->clkid);
        }
err_req_clks:
        for (j = (xwssz_t)resources->irqrsc_num - 1; j >= 0; j--) {
                irqrsc = &resources->irqrsc_array[j];
                xwos_irq_release(irqrsc->irqn);
        }
err_req_irqs:
        return rc;
}


static __xwmd_code
xwer_t mpc560xb_misc_drv_stop(struct xwds_device * dev)
{
        struct xwds_misc * misc;
        const struct xwds_resources * resources;
        const struct xwos_irq_resource * irqrsc;
        const struct xwds_resource_clk * clkrsc;
        const struct xwds_resource_gpio * gpiorsc;
        xwssz_t j;

        misc = xwds_static_cast(struct xwds_misc *, dev);
        resources = misc->dev.resources;

        for (j = (xwssz_t)resources->irqrsc_num; j >= 0; j--) {
                irqrsc = &resources->irqrsc_array[j];
                xwos_irq_disable(irqrsc->irqn);
        }

        for (j = (xwssz_t)resources->gpiorsc_num - 1; j >= 0; j--) {
                gpiorsc = &resources->gpiorsc_array[j];
                xwds_gpio_rls(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
        }

        for (j = (xwssz_t)resources->clkrsc_num - 1; j >= 0; j--) {
                clkrsc = &resources->clkrsc_array[j];
                xwds_clk_rls(clkrsc->soc, clkrsc->clkid);
        }

        for (j = (xwssz_t)resources->irqrsc_num - 1; j >= 0; j--) {
                irqrsc = &resources->irqrsc_array[j];
                xwos_irq_release(irqrsc->irqn);
        }
        return OK;
}

static __xwmd_code
xwer_t mpc560xb_misc_drv_suspend(struct xwds_device * dev)
{
        return mpc560xb_misc_drv_stop(dev);
}

static __xwmd_code
xwer_t mpc560xb_misc_drv_resume(struct xwds_device * dev)
{
        return mpc560xb_misc_drv_start(dev);
}

/******** ******** adc driver ******** ********/
static __xwmd_code
xwer_t mpc560xb_misc_drv_ioctl(struct xwds_misc * misc, xwsq_t cmd, va_list args)
{
        xwer_t rc;
        xws16_t * buf;
        xwbmp_t * chmask;
        xwtm_t * xwtm;

        rc = OK;
        switch(cmd) {
        case MPC560XB_ADC_IOC_SETUP:
                rc = mpc560xb_adc_setup(misc);
                break;
        case MPC560XB_ADC_IOC_CONVERT:
                chmask = va_arg(args, xwbmp_t *);
                buf = va_arg(args, xws16_t *);
                xwtm = va_arg(args, xwtm_t *);
                rc = mpc560xb_adc_convert(misc, chmask, buf, xwtm);
                break;
        case MPC560XB_ADC_IOC_CLEAN:
                rc = mpc560xb_adc_clean(misc);
                break;
        default:
                break;
        }

        return rc;
}

static __xwmd_code
xwer_t mpc560xb_adc_setup(struct xwds_misc * misc)
{
        const struct mpc560xb_misc_private_cfg * misccfg;
        const struct soc_adc_private_cfg * adc_cfg;

        misccfg = misc->misccfg;
        adc_cfg = misccfg->adc;

        /* setup adc */
        ADC_0.MCR.B.OWREN = SOC_ADC_MCR_OVERWRITE_EN;
        ADC_0.MCR.B.WLSIDE = SOC_ADC_MCR_WRITE_RIGHT;
        ADC_0.MCR.B.MODE = adc_cfg->mcr.bit.mode;
        ADC_0.MCR.B.ADCLKSEL = adc_cfg->mcr.bit.clock_select;
        ADC_0.MCR.B.PWDN = 0;

        ADC_0.NCMR[0].R = adc_cfg->ncmr[0];
        ADC_0.NCMR[1].R = adc_cfg->ncmr[1];
        ADC_0.NCMR[2].R = adc_cfg->ncmr[2];

        ADC_0.JCMR[0].R = adc_cfg->jcmr[0];
        ADC_0.JCMR[1].R = adc_cfg->jcmr[1];
        ADC_0.JCMR[2].R = adc_cfg->jcmr[2];

        ADC_0.CTR[0].R = adc_cfg->ctr[0].u32;
        ADC_0.CTR[1].R = adc_cfg->ctr[1].u32;
        ADC_0.CTR[2].R = adc_cfg->ctr[2].u32;

        ADC_0.CIMR[0].R = adc_cfg->ncmr[0] | adc_cfg->jcmr[0];
        ADC_0.CIMR[1].R = adc_cfg->ncmr[1] | adc_cfg->jcmr[1];
        ADC_0.CIMR[2].R = adc_cfg->ncmr[2] | adc_cfg->jcmr[2];

        ADC_0.IMR.B.MSKECH = 1; /* enable ECH IRQ */
        ADC_0.IMR.B.MSKJECH = 1;/* enable JECH IRQ */

        ADC_0.MCR.B.PWDN = 1;

        return OK;
}

static __xwmd_code
xwer_t mpc560xb_adc_clean(struct xwds_misc * misc)
{
        XWOS_UNUSED(misc);

        /* clean ADC */
        ADC_0.MCR.B.PWDN = 1;

        return OK;
}

static __xwmd_code
xwer_t mpc560xb_adc_convert(struct xwds_misc * misc, xwbmp_t * chmask, xws16_t * buf,
                            xwtm_t * xwtm)
{
        struct mpc560xb_misc_drvdata * drvdata;
        xwid_t mtxid;
        union xwlk_ulock ulk;
        xwsq_t lkst;
        xwu32_t ceocfr[3];
        xwreg_t flag;
        xwssq_t ch;
        xwer_t rc;
        __maybe_unused xwu32_t cdr;

        drvdata = misc->dev.data;
        ulk.osal.splk = &drvdata->adc.lock;
        mtxid = xwosal_mtx_get_id(&drvdata->adc.mtx);
        rc = xwosal_mtx_timedlock(mtxid, xwtm);
        if (__unlikely(rc < 0)) {
                goto err_mtxlock;
        }

        drvdata->adc.tid = xwosal_cthrd_get_id();
        ADC_0.MCR.B.PWDN = 0;
        xwosal_splk_lock_cpuirqsv(&drvdata->adc.lock, &flag);
        ADC_0.MCR.B.NSTART = 1; /* start ADC */
        rc = xwosal_cthrd_timedpause(ulk,
                                     XWLK_TYPE_SPLK_CPUIRQ,
                                     NULL, 0, xwtm, &lkst);
        if (OK == rc) {
                xwosal_splk_unlock_cpuirqrs(&drvdata->adc.lock, flag);
                ceocfr[0] = ADC_0.CEOCFR[0].R;
                ceocfr[1] = ADC_0.CEOCFR[1].R;
                ceocfr[2] = ADC_0.CEOCFR[2].R;
                while (1) {
                        ch = xwbmpop_ffs((xwbmp_t *)ceocfr, sizeof(ceocfr) * 8);
                        if (ch >= 0) {
                                xwbmpop_c0i((xwbmp_t *)ceocfr, (xwsq_t)ch);
                                xwbmpop_c0i((xwbmp_t *)ADC_0.CEOCFR, (xwsq_t)ch);
                                if (xwbmpop_t1i(chmask, (xwsq_t)ch)) {
                                        if (ADC_0.CDR[ch].B.VALID) {
                                                buf[ch] = (xws16_t)(ADC_0.CDR[ch].R &
                                                                    0x3FF);
                                        } else {
                                                buf[ch] = (xws16_t)-EINVAL;
                                        }
                                } else {
                                        xwmb_read(xwu32_t, cdr, &ADC_0.CDR[ch].R);
                                }
                        } else {
                                break;
                        }
                }
        } else {
                if (XWLK_STATE_LOCKED == lkst) {
                        xwosal_splk_unlock_cpuirqrs(&drvdata->adc.lock, flag);
                } else {
                        xwos_cpuirq_restore_lc(flag);
                }
        }
        ADC_0.MCR.B.PWDN = 1;
        drvdata->adc.tid = (xwid_t)0;
        xwosal_mtx_unlock(mtxid);
err_mtxlock:
        return rc;
}

/******** ******** adc isr ******** ********/
__xwmd_isr
void mpc560xb_adc_eoc_isr(void)
{
        struct soc_irq_data irqdata;
        struct xwds_misc * misc;
        struct mpc560xb_misc_drvdata * drvdata;

        xwos_irq_get_data(IRQ_ADC0_EOC, &irqdata);
        misc = irqdata.data;
        drvdata = misc->dev.data;

        if(ADC_0.ISR.B.ECH) {
                /* End of chain */
                ADC_0.ISR.B.ECH = 1;
                xwosal_thrd_continue(drvdata->adc.tid);
        } else if (ADC_0.ISR.B.JECH) {
                /* End of injected chain */
                ADC_0.ISR.B.JECH = 1;
                xwosal_thrd_continue(drvdata->adc.tid);
        } else {
                /* do_nothing(); */
        }
}

__xwmd_isr
void mpc560xb_adc_er_isr(void)
{
        /* TODO: */
}

__xwmd_isr
void mpc560xb_adc_wd_isr(void)
{
        /* TODO: */
}
