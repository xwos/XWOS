/**
 * @file
 * @brief STM32CUBE设备栈：I2C Master
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
#include <xwcd/ds/i2c/master.h>
#include <bm/stm32cube/cubemx/Core/Inc/i2c.h>
#include <bm/stm32cube/xwac/xwds/device.h>
#include <bm/stm32cube/xwac/xwds/i2cm.h>

static
xwer_t stm32cube_i2c2m_drv_start(struct xwds_device * dev);

static
xwer_t stm32cube_i2c2m_drv_stop(struct xwds_device * dev);

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static
xwer_t stm32cube_i2c2m_drv_suspend(struct xwds_device * dev);

static
xwer_t stm32cube_i2c2m_drv_resume(struct xwds_device * dev);
#endif /* XWCDCFG_ds_PM */

static
xwer_t stm32cube_i2c2m_drv_xfer(struct xwds_i2cm * i2cm,
                                struct xwds_i2c_msg * msg,
                                xwtm_t * xwtm);

static
xwer_t stm32cube_i2c2m_drv_abort(struct xwds_i2cm * i2cm,
                                 xwu16_t address, xwu16_t addrmode,
                                 xwtm_t * xwtm);

const struct xwds_i2cm_driver stm32cube_i2c2m_drv = {
        .base = {
                .name = "stm32cube.i2cm.2",
                .probe = NULL,
                .remove = NULL,
                .start = stm32cube_i2c2m_drv_start,
                .stop = stm32cube_i2c2m_drv_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
                .suspend = stm32cube_i2c2m_drv_suspend,
                .resume =  stm32cube_i2c2m_drv_resume,
#endif /* XWCDCFG_ds_PM */
        },
        .xfer = stm32cube_i2c2m_drv_xfer,
        .abort = stm32cube_i2c2m_drv_abort,
};

struct xwds_i2cm stm32cube_i2c2m_cb = {
        /* attributes */
        .dev = {
                .name = "stm32cube.i2cm.2",
                .id = 2,
                .resources = NULL,
                .drv = xwds_cast(struct xwds_driver *, &stm32cube_i2c2m_drv),
                .data = (void *)&hi2c2_drvdata,
        },
        .xwccfg = NULL,
};

static
xwer_t stm32cube_i2c2m_drv_start(struct xwds_device * dev)
{
        struct xwds_i2cm * i2cm;
        struct MX_I2C_MasterDriverData * drvdata;

        i2cm = xwds_cast(struct xwds_i2cm *, dev);
        drvdata = i2cm->dev.data;
        drvdata->i2cm = i2cm;

        MX_I2C2_Init();
        return XWOK;
}

static
xwer_t stm32cube_i2c2m_drv_stop(struct xwds_device * dev)
{
        XWOS_UNUSED(dev);

        MX_I2C2_DeInit();
        return XWOK;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static
xwer_t stm32cube_i2c2m_drv_resume(struct xwds_device * dev)
{
        return stm32cube_i2c2m_drv_start(dev);
}

static
xwer_t stm32cube_i2c2m_drv_suspend(struct xwds_device * dev)
{
        return stm32cube_i2c2m_drv_stop(dev);
}
#endif /* XWCDCFG_ds_PM */

static
xwer_t stm32cube_i2c2m_drv_xfer(struct xwds_i2cm * i2cm,
                                struct xwds_i2c_msg * msg,
                                xwtm_t * xwtm)
{
        struct MX_I2C_MasterDriverData * drvdata;
        xwu32_t addm;
        union xwos_ulock ulk;
        xwreg_t cpuirq;
        xwsq_t lkst;
        xwer_t rc;

        addm = MX_I2C2_GetAddressingMode();
        if ((msg->flag & XWDS_I2C_F_ADDRMSK) != addm) {
                MX_I2C2_ReInit(msg->flag & XWDS_I2C_F_ADDRMSK);
        }
        drvdata = i2cm->dev.data;
        ulk.osal.splk = &drvdata->splk;
        xwos_splk_lock_cpuirqsv(&drvdata->splk, &cpuirq);
        drvdata->rc = -EINPROGRESS;
        drvdata->msg = msg;
        rc = MX_I2C2_Xfer(msg);
        if (XWOK == rc) {
                rc = xwos_cond_timedwait(&drvdata->cond,
                                         ulk, XWOS_LK_SPLK, NULL,
                                         xwtm, &lkst);
                if (XWOK == rc) {
                        if (XWOS_LKST_UNLOCKED == lkst) {
                                xwos_splk_lock(&drvdata->splk);
                        }
                        rc = drvdata->rc;
                } else {
                        if (XWOS_LKST_UNLOCKED == lkst) {
                                xwos_splk_lock(&drvdata->splk);
                        }
                        drvdata->rc = -ECANCELED;
                }
        } else {
                drvdata->rc = -ECANCELED;
        }
        xwos_splk_unlock_cpuirqrs(&drvdata->splk, cpuirq);
        return rc;
}

static
xwer_t stm32cube_i2c2m_drv_abort(struct xwds_i2cm * i2cm,
                                 xwu16_t address, xwu16_t addrmode,
                                 xwtm_t * xwtm)
{
        xwu32_t addm;
        xwer_t rc;

        XWOS_UNUSED(xwtm);

        addm = MX_I2C2_GetAddressingMode();
        if ((addrmode & XWDS_I2C_F_ADDRMSK) != addm) {
                MX_I2C2_ReInit(addrmode & XWDS_I2C_F_ADDRMSK);
        }
        rc = MX_I2C2_Abort(address);
        return rc;
}

void stm32cube_i2c2m_cb_xfercplt(struct xwds_i2cm * i2cm, xwer_t xrc)
{
        struct MX_I2C_MasterDriverData * drvdata;
        xwreg_t cpuirq;

        drvdata = i2cm->dev.data;
        xwos_splk_lock_cpuirqsv(&drvdata->splk, &cpuirq);
        if (-ECANCELED != drvdata->rc) {
                drvdata->rc = xrc;
        } else {
        }
        xwos_splk_unlock_cpuirqrs(&drvdata->splk, cpuirq);
        xwos_cond_broadcast(&drvdata->cond);
}
