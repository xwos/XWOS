/**
 * @file
 * @brief 玄武设备栈：CAN收发器：通用型104x/105x系列收发器：驱动
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
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

#include <xwos/standard.h>
#include <xwos/osal/irq.h>
#include <xwcd/ds/soc/gpio.h>
#include <xwcd/ds/soc/eirq.h>
#include <xwcd/ds/can/transceiver.h>
#include <xwcd/peripheral/can/transceiver/xxx10xx/driver.h>

static __xwbsp_code
xwer_t xwds_cantrcv_xxx10xx_check_desc(struct xwds_cantrcv_xxx10xx * xxx10xx);

__xwbsp_rodata const struct xwds_cantrcv_driver xwds_cantrcv_xxx10xx_drv = {
        .base = {
                .name = "can.transceiver.xxx10xx",
                .probe = xwds_cantrcv_xxx10xx_drv_probe,
                .remove = NULL,
                .start = xwds_cantrcv_xxx10xx_drv_start,
                .stop = xwds_cantrcv_xxx10xx_drv_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
                .suspend = xwds_cantrcv_xxx10xx_drv_suspend,
                .resume =  xwds_cantrcv_xxx10xx_drv_resume,
#endif
        },
        .set_opmode = xwds_cantrcv_xxx10xx_drv_set_opmode,
        .enable_wkup = xwds_cantrcv_xxx10xx_drv_enable_wkup,
        .disable_wkup = xwds_cantrcv_xxx10xx_drv_disable_wkup,
};

static __xwbsp_code
xwer_t xwds_cantrcv_xxx10xx_check_desc(struct xwds_cantrcv_xxx10xx * xxx10xx)
{
        const struct xwds_cantrcv_cfg * cfg;
        xwer_t rc;

        cfg = xxx10xx->cantrcv.cfg;
        if (cfg->init_opmode >= XWDS_CANTRCV_OPMODE_NUM) {
                rc = -EINVAL;
        } else {
                rc = XWOK;
        }
        return rc;
}

/******** ******** base driver ******** ********/
__xwbsp_code
xwer_t xwds_cantrcv_xxx10xx_drv_probe(struct xwds_device * dev)
{
        struct xwds_cantrcv_xxx10xx * xxx10xx;
        xwer_t rc;

        xxx10xx = xwds_cast(struct xwds_cantrcv_xxx10xx *, dev);
        rc = xwds_cantrcv_xxx10xx_check_desc(xxx10xx);
        if (rc < 0) {
                goto err_chkdesc;
        }
        return XWOK;

err_chkdesc:
        return rc;
}

__xwbsp_code
xwer_t xwds_cantrcv_xxx10xx_drv_start(struct xwds_device * dev)
{
        struct xwds_cantrcv_xxx10xx * xxx10xx;
        const struct xwds_resource_gpio * gpiorsc;
        xwer_t rc;

        xxx10xx = xwds_cast(struct xwds_cantrcv_xxx10xx *, dev);
        gpiorsc = xxx10xx->gpiorsc.stb;
        if (NULL != gpiorsc) {
                rc = xwds_gpio_req(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
                if (rc < 0) {
                        goto err_gpio_req_stb;
                }
        }
        gpiorsc = xxx10xx->gpiorsc.eirq;
        if (NULL != gpiorsc) {
                rc = xwds_gpio_req(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
                if (rc < 0) {
                        goto err_gpio_req_eirq;
                }
        }
        return XWOK;

err_gpio_req_eirq:
        gpiorsc = xxx10xx->gpiorsc.stb;
        if (NULL != gpiorsc) {
                xwds_gpio_rls(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
        }
err_gpio_req_stb:
        return rc;
}

__xwbsp_code
xwer_t xwds_cantrcv_xxx10xx_drv_stop(struct xwds_device * dev)
{
        struct xwds_cantrcv_xxx10xx * xxx10xx;
        const struct xwds_resource_gpio * gpiorsc;

        xxx10xx = xwds_cast(struct xwds_cantrcv_xxx10xx *, dev);
        gpiorsc = xxx10xx->gpiorsc.eirq;
        if (NULL != gpiorsc) {
                xwds_gpio_rls(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
        }
        gpiorsc = xxx10xx->gpiorsc.stb;
        if (NULL != gpiorsc) {
                xwds_gpio_rls(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
        }
        return XWOK;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
__xwbsp_code
xwer_t xwds_cantrcv_xxx10xx_drv_suspend(struct xwds_device * dev)
{
        struct xwds_cantrcv_xxx10xx * xxx10xx;
        const struct xwds_resource_gpio * gpiorsc;

        xxx10xx = xwds_cast(struct xwds_cantrcv_xxx10xx *, dev);
        gpiorsc = xxx10xx->gpiorsc.stb;
        if (NULL != gpiorsc) {
                xwds_gpio_rls(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
        }
        return XWOK;
}

__xwbsp_code
xwer_t xwds_cantrcv_xxx10xx_drv_resume(struct xwds_device * dev)
{
        struct xwds_cantrcv_xxx10xx * xxx10xx;
        const struct xwds_resource_gpio * gpiorsc;
        xwer_t rc;

        xxx10xx = xwds_cast(struct xwds_cantrcv_xxx10xx *, dev);
        gpiorsc = xxx10xx->gpiorsc.stb;
        if (NULL != gpiorsc) {
                rc = xwds_gpio_req(gpiorsc->soc, gpiorsc->port, gpiorsc->pinmask);
                if (rc < 0) {
                        goto err_gpio_req_stb;
                }
        }
        return XWOK;

err_gpio_req_stb:
        return rc;
}
#endif

__xwbsp_code
xwer_t xwds_cantrcv_xxx10xx_drv_set_opmode(struct xwds_cantrcv * cantrcv,
                                           xwsq_t opmode)
{
        struct xwds_cantrcv_xxx10xx * xxx10xx;
        const struct xwds_resource_gpio * gpiorsc;
        xwer_t rc;

        xxx10xx = xwds_cast(struct xwds_cantrcv_xxx10xx *, cantrcv);
        gpiorsc = xxx10xx->gpiorsc.stb;
        switch (opmode) {
        case XWDS_CANTRCV_OPMODE_NORMAL:
                rc = xwds_gpio_reset(gpiorsc->soc,
                                     gpiorsc->port,
                                     gpiorsc->pinmask);
                if (XWOK == rc) {
                        xxx10xx->cantrcv.wkuprs = XWDS_CANTRCV_WKUPRS_BY_PIN;
                }
                break;
        case XWDS_CANTRCV_OPMODE_SLEEP:
        case XWDS_CANTRCV_OPMODE_STANDBY:
                rc = xwds_gpio_set(gpiorsc->soc,
                                   gpiorsc->port,
                                   gpiorsc->pinmask);
                break;
        default:
                rc = -ERANGE;
                break;
        }
        return rc;
}

__xwbsp_code
xwer_t xwds_cantrcv_xxx10xx_drv_enable_wkup(struct xwds_cantrcv * cantrcv)
{
        struct xwds_cantrcv_xxx10xx * xxx10xx;
        const struct xwds_resource_gpio * gpiorsc;
        xwsq_t eirq;
        xwer_t rc;

        xxx10xx = xwds_cast(struct xwds_cantrcv_xxx10xx *, cantrcv);
        gpiorsc = xxx10xx->gpiorsc.eirq;
        eirq = xxx10xx->eirq;
        if (gpiorsc) {
                rc = xwds_eirq_req(gpiorsc->soc,
                                   gpiorsc->port, gpiorsc->pinmask,
                                   eirq, XWDS_SOC_EIF_TM_FALLING | XWDS_SOC_EIF_WKUP,
                                   xwds_cantrcv_xxx10xx_eirq_wkup, xxx10xx);
        } else {
                rc = -EOPNOTSUPP;
        }

        return rc;
}

__xwbsp_code
xwer_t xwds_cantrcv_xxx10xx_drv_disable_wkup(struct xwds_cantrcv * cantrcv)
{
        struct xwds_cantrcv_xxx10xx * xxx10xx;
        const struct xwds_resource_gpio * gpiorsc;
        xwsq_t eirq;
        xwer_t rc;

        xxx10xx = xwds_cast(struct xwds_cantrcv_xxx10xx *, cantrcv);
        gpiorsc = xxx10xx->gpiorsc.eirq;
        eirq = xxx10xx->eirq;
        if (gpiorsc) {
                rc = xwds_eirq_rls(gpiorsc->soc,
                                   gpiorsc->port, gpiorsc->pinmask,
                                   eirq);
        } else {
                rc = -EOPNOTSUPP;
        }
        return rc;
}

__xwbsp_code
void xwds_cantrcv_xxx10xx_eirq_wkup(struct xwds_soc * soc, xwid_t eiid,
                                    xwds_eirq_arg_t arg)
{
        struct xwds_cantrcv_xxx10xx * xxx10xx;

        XWOS_UNUSED(soc);
        xxx10xx = arg;
        if (eiid == xxx10xx->eirq) {
                xxx10xx->cantrcv.wkuprs = XWDS_CANTRCV_WKUPRS_BY_BUS;
                xwds_cantrcv_drvcb_wakeup_notification(&xxx10xx->cantrcv);
        }
}
