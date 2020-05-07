/**
 * @file
 * @brief CAN Transceiver TJA1042驱动
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
#include <xwos/irq.h>
#include <soc_irq.h>
#include <xwmd/ds/soc/gpio.h>
#include <xwmd/ds/soc/eirq.h>
#include <xwmd/ds/can/transceiver.h>
#include <xwcd/perpheral/ds/can/transceiver/tja1042/driver.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwbsp_code
xwer_t tja1042_check_desc(struct xwds_cantrcv * cantrcv);

static __xwbsp_code
xwer_t tja1042_drv_probe(struct xwds_device * dev);

static __xwbsp_code
xwer_t tja1042_drv_start(struct xwds_device * dev);

static __xwbsp_code
xwer_t tja1042_drv_stop(struct xwds_device * dev);

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
static __xwbsp_code
xwer_t tja1042_drv_suspend(struct xwds_device * dev);

static __xwbsp_code
xwer_t tja1042_drv_resume(struct xwds_device * dev);
#endif /* XWMDCFG_ds_PM */

static __xwbsp_code
xwer_t tja1042_cantrcv_drv_set_opmode(struct xwds_cantrcv * cantrcv,
                                      xwsq_t opmode);

static __xwbsp_code
xwer_t tja1042_cantrcv_drv_enable_wkup(struct xwds_cantrcv * cantrcv);

static __xwbsp_code
xwer_t tja1042_cantrcv_drv_disable_wkup(struct xwds_cantrcv * cantrcv);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwbsp_rodata const struct xwds_cantrcv_driver tja1042_cantrcv_drv = {
        .base = {
                .name = "tja1042",
                .probe = tja1042_drv_probe,
                .remove = NULL,
                .start = tja1042_drv_start,
                .stop = tja1042_drv_stop,
#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
                .suspend = tja1042_drv_suspend,
                .resume =  tja1042_drv_resume,
#endif /* XWMDCFG_ds_PM */
        },
        .set_opmode = tja1042_cantrcv_drv_set_opmode,
        .enable_wkup = tja1042_cantrcv_drv_enable_wkup,
        .disable_wkup = tja1042_cantrcv_drv_disable_wkup,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwbsp_code
xwer_t tja1042_check_desc(struct xwds_cantrcv * cantrcv)
{
        const struct xwds_resources * resources;
        const struct xwds_cantrcv_cfg * cfg;
        const struct xwds_cantrcv_tja1042_cfg * chipcfg;
        xwer_t rc;

        resources = cantrcv->bc.dev.resources;
        cfg = cantrcv->cfg;
        chipcfg = cfg->chipcfg;
        if (__unlikely(is_err_or_null(resources))) {
                rc = -EINVAL;
        } else if (__unlikely(is_err_or_null(cfg))) {
                rc = -EINVAL;
        } else if (__unlikely(is_err_or_null(chipcfg))) {
                rc = -EINVAL;
        } else if (cfg->init_opmode >= SODS_CANTRCV_OPMODE_NUM) {
                rc = -EINVAL;
        } else {
                rc = OK;
        }
        return rc;
}

/******** ******** base driver ******** ********/
static __xwbsp_code
xwer_t tja1042_drv_probe(struct xwds_device * dev)
{
        struct xwds_cantrcv * cantrcv;
        xwer_t rc;

        cantrcv = xwds_static_cast(struct xwds_cantrcv *, dev);
        rc = tja1042_check_desc(cantrcv);
        if (__unlikely(rc < 0)) {
                goto err_chkdesc;
        }
        return OK;

err_chkdesc:
        return rc;
}

static __xwbsp_code
xwer_t tja1042_drv_start(struct xwds_device * dev)
{
        struct xwds_cantrcv * cantrcv;
        const struct xwds_resources * resources;
        const struct xwds_resource_gpio * gpiorsc;
        xwssz_t i, j;
        xwer_t rc;

        cantrcv = xwds_static_cast(struct xwds_cantrcv *, dev);
        resources = cantrcv->bc.dev.resources;

        /* request GPIO resources */
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
        return OK;

err_req_gpios:
        return rc;
}

static __xwbsp_code
xwer_t tja1042_drv_stop(struct xwds_device * dev)
{
        struct xwds_cantrcv * cantrcv;
        const struct xwds_resources * resources;
        const struct xwds_resource_gpio * gpiorsc;
        xwssz_t j;
        xwer_t rc;

        cantrcv = xwds_static_cast(struct xwds_cantrcv *, dev);
        resources = cantrcv->bc.dev.resources;

        /* release GPIO resources */
        for (j = (xwssz_t)resources->gpiorsc_num - 1; j >= 0; j--) {
                gpiorsc = &resources->gpiorsc_array[j];
                rc = xwds_gpio_rls(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
                if (__unlikely(rc < 0)) {
                        goto err_gpio_rls;
                }
        }
        return OK;

err_gpio_rls:
        return rc;
}

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
static __xwbsp_code
xwer_t tja1042_drv_suspend(struct xwds_device * dev)
{
        xwer_t rc;
        struct xwds_cantrcv * cantrcv;
        const struct xwds_cantrcv_cfg * cfg;
        const struct xwds_cantrcv_tja1042_cfg * chipcfg;
        const struct xwds_resource_gpio * gpiorsc;

        cantrcv = xwds_static_cast(struct xwds_cantrcv *, dev);
        cfg = cantrcv->cfg;
        chipcfg = cfg->chipcfg;

        gpiorsc = chipcfg->gpiorsc_stb;
        rc = xwds_gpio_rls(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
        if (__unlikely(rc < 0)) {
                goto err_gpio_rls;
        }
        return OK;

err_gpio_rls:
        return rc;
}

static __xwbsp_code
xwer_t tja1042_drv_resume(struct xwds_device * dev)
{
        xwer_t rc;
        struct xwds_cantrcv * cantrcv;
        const struct xwds_cantrcv_cfg * cfg;
        const struct xwds_cantrcv_tja1042_cfg * chipcfg;
        const struct xwds_resource_gpio * gpiorsc;

        cantrcv = xwds_static_cast(struct xwds_cantrcv *, dev);
        cfg = cantrcv->cfg;
        chipcfg = cfg->chipcfg;

        gpiorsc = chipcfg->gpiorsc_stb;
        rc = xwds_gpio_req(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
        if (__unlikely(rc < 0)) {
                goto err_gpio_req;
        }
        return OK;

err_gpio_req:
        return rc;
}
#endif /* XWMDCFG_ds_PM */

static __xwbsp_code
xwer_t tja1042_cantrcv_drv_set_opmode(struct xwds_cantrcv * cantrcv,
                                      xwsq_t opmode)
{
        xwer_t rc;
        const struct xwds_cantrcv_cfg * cfg;
        const struct xwds_cantrcv_tja1042_cfg * chipcfg;
        const struct xwds_resource_gpio *gpiorsc_stb;

        cfg = cantrcv->cfg;
        chipcfg = cfg->chipcfg;
        gpiorsc_stb = chipcfg->gpiorsc_stb;

        switch (opmode) {
        case SODS_CANTRCV_OPMODE_NORMAL:
                rc = xwds_gpio_reset(gpiorsc_stb->soc,
                                     gpiorsc_stb->port,
                                     gpiorsc_stb->pinmask);
                if (OK == rc) {
                        cantrcv->wkuprs = SODS_CANTRCV_WKUPRS_BY_PIN;
                }
                break;
        case SODS_CANTRCV_OPMODE_SLEEP:
        case SODS_CANTRCV_OPMODE_STANDBY:
                rc = xwds_gpio_set(gpiorsc_stb->soc,
                                   gpiorsc_stb->port,
                                   gpiorsc_stb->pinmask);
                break;
        default:
                rc = -ERANGE;
                break;
        }
        return rc;
}

static __xwbsp_code
xwer_t tja1042_cantrcv_drv_enable_wkup(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;
        const struct xwds_cantrcv_cfg * cfg;
        const struct xwds_cantrcv_tja1042_cfg * chipcfg;
        const struct xwds_resource_gpio *gpiorsc_eirq;
        xwsq_t eirq;

        cfg = cantrcv->cfg;
        chipcfg = cfg->chipcfg;
        gpiorsc_eirq = chipcfg->gpiorsc_eirq;
        eirq = chipcfg->eirq;

        if (gpiorsc_eirq) {
                rc = xwds_eirq_req(gpiorsc_eirq->soc,
                                   gpiorsc_eirq->port, gpiorsc_eirq->pinmask,
                                   eirq, SODS_SOC_EIF_TM_FALLING | SODS_SOC_EIF_WKUP,
                                   tja1042_cantrcv_eirq_wkup, cantrcv);
        } else {
                rc = -EOPNOTSUPP;
        }

        return rc;
}

static __xwbsp_code
xwer_t tja1042_cantrcv_drv_disable_wkup(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;
        const struct xwds_cantrcv_cfg * cfg;
        const struct xwds_cantrcv_tja1042_cfg * chipcfg;
        const struct xwds_resource_gpio *gpiorsc_eirq;
        xwsq_t eirq;

        cfg = cantrcv->cfg;
        chipcfg = cfg->chipcfg;
        gpiorsc_eirq = chipcfg->gpiorsc_eirq;
        eirq = chipcfg->eirq;

        if (gpiorsc_eirq) {
                rc = xwds_eirq_rls(gpiorsc_eirq->soc,
                                   gpiorsc_eirq->port, gpiorsc_eirq->pinmask,
                                   eirq);
        } else {
                rc = -EOPNOTSUPP;
        }
        return rc;
}

__xwbsp_code
void tja1042_cantrcv_eirq_wkup(struct xwds_soc * soc, xwid_t eiid,
                               xwds_eirq_arg_t arg)
{
        struct xwds_cantrcv * cantrcv;
        const struct xwds_cantrcv_cfg * cfg;
        const struct xwds_cantrcv_tja1042_cfg * chipcfg;

        XWOS_UNUSED(soc);
        cantrcv = arg;
        cfg = cantrcv->cfg;
        chipcfg = cfg->chipcfg;

        if (eiid == chipcfg->eirq) {
                cantrcv->wkuprs = SODS_CANTRCV_WKUPRS_BY_BUS;
                xwds_cantrcv_lib_wakeup_notification(cantrcv);
        }
}
