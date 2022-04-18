/**
 * @file
 * @brief STM32CUBE设备栈：SPI主机
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
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/cond.h>
#include <xwcd/ds/spi/master.h>
#include <bm/stm32cube/standard.h>
#include <bm/stm32cube/cubemx/Core/Inc/spi.h>
#include <bm/stm32cube/xwac/xwds/spim.h>

/******** ******** SPI1 ******** ********/
static
xwer_t stm32cube_spi1m_drv_start(struct xwds_device * dev);

static
xwer_t stm32cube_spi1m_drv_stop(struct xwds_device * dev);

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static
xwer_t stm32cube_spi1m_drv_suspend(struct xwds_device * dev);

static
xwer_t stm32cube_spi1m_drv_resume(struct xwds_device * dev);
#endif

static
xwer_t stm32cube_spi1m_drv_buscfg(struct xwds_spim * spim,
                                 xwid_t cfgid,
                                 xwtm_t to);

static
xwer_t stm32cube_spi1m_drv_xfer(struct xwds_spim * spim,
                                const xwu8_t txd[], xwu8_t * rxb,
                                xwsz_t * size, xwtm_t to);

const struct xwds_spim_driver stm32cube_spi1m_drv = {
        .base = {
                .name = "stm32cube.spim.1",
                .probe = NULL,
                .remove = NULL,
                .start = stm32cube_spi1m_drv_start,
                .stop = stm32cube_spi1m_drv_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
                .suspend = stm32cube_spi1m_drv_suspend,
                .resume =  stm32cube_spi1m_drv_resume,
#endif
        },
        .buscfg = stm32cube_spi1m_drv_buscfg,
        .xfer = stm32cube_spi1m_drv_xfer,
};

struct xwds_spim stm32cube_spi1m_cb = {
        /* attributes */
        .dev = {
                .name = "stm32cube.spim.1",
                .id = 1,
                .resources = NULL,
                .drv = xwds_cast(struct xwds_driver *, &stm32cube_spi1m_drv),
                .data = (void *)&hspi1_drvdata,
        },
        .buscfg = NULL,
        .buscfg_num = 0,
};

static
xwer_t stm32cube_spi1m_drv_start(struct xwds_device * dev)
{
        struct xwds_spim * spim;
        struct MX_SPI_MasterDriverData * drvdata;

        spim = xwds_cast(struct xwds_spim *, dev);
        drvdata = spim->dev.data;
        drvdata->spim = spim;
        MX_SPI1_Init();
        return XWOK;
}

static
xwer_t stm32cube_spi1m_drv_stop(struct xwds_device * dev)
{
        XWOS_UNUSED(dev);

        MX_SPI1_DeInit();
        return XWOK;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static
xwer_t stm32cube_spi1m_drv_resume(struct xwds_device * dev)
{
        return stm32cube_spi1m_drv_start(dev);
}

static
xwer_t stm32cube_spi1m_drv_suspend(struct xwds_device * dev)
{
        return stm32cube_spi1m_drv_stop(dev);
}
#endif

static
xwer_t stm32cube_spi1m_drv_buscfg(struct xwds_spim * spim,
                                  xwid_t cfgid,
                                  xwtm_t to)
{
        const SPI_InitTypeDef ** buscfg;
        const SPI_InitTypeDef * cfg;
        xwer_t rc;

        XWOS_UNUSED(to);

        buscfg = spim->buscfg;
        if (buscfg) {
                cfg = buscfg[cfgid];
                rc = MX_SPI1_ReInit(cfg);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

static
xwer_t stm32cube_spi1m_drv_xfer(struct xwds_spim * spim,
                                const xwu8_t txd[], xwu8_t * rxb,
                                xwsz_t * size, xwtm_t to)
{
        struct MX_SPI_MasterDriverData * drvdata;
        union xwos_ulock ulk;
        xwreg_t cpuirq;
        xwsq_t lkst;
        xwer_t rc;

        drvdata = spim->dev.data;
        ulk.osal.splk = &drvdata->splk;
        xwos_splk_lock_cpuirqsv(&drvdata->splk, &cpuirq);
        drvdata->rc = -EINPROGRESS;
        rc = MX_SPI1_Xfer(txd, rxb, size);
        if (XWOK == rc) {
                rc = xwos_cond_wait_to(&drvdata->cond,
                                       ulk, XWOS_LK_SPLK, NULL,
                                       to, &lkst);
                if (XWOK == rc) {
                        if (XWOS_LKST_UNLOCKED == lkst) {
                                xwos_splk_lock(&drvdata->splk);
                        }
                        *size = hspi1_drvdata.size;
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

void stm32cube_spi1m_cb_xfercplt(struct xwds_spim * spim, xwer_t xrc)
{
        struct MX_SPI_MasterDriverData * drvdata;
        xwreg_t cpuirq;

        drvdata = spim->dev.data;
        xwos_splk_lock_cpuirqsv(&drvdata->splk, &cpuirq);
        if (-ECANCELED != drvdata->rc) {
                drvdata->rc = xrc;
        } else {
        }
        xwos_splk_unlock_cpuirqrs(&drvdata->splk, cpuirq);
        xwos_cond_broadcast(&drvdata->cond);
}

/******** ******** SPI2 ******** ********/
static
xwer_t stm32cube_spi2m_drv_start(struct xwds_device * dev);

static
xwer_t stm32cube_spi2m_drv_stop(struct xwds_device * dev);

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static
xwer_t stm32cube_spi2m_drv_suspend(struct xwds_device * dev);

static
xwer_t stm32cube_spi2m_drv_resume(struct xwds_device * dev);
#endif

static
xwer_t stm32cube_spi2m_drv_buscfg(struct xwds_spim * spim,
                                 xwid_t cfgid,
                                 xwtm_t to);

static
xwer_t stm32cube_spi2m_drv_xfer(struct xwds_spim * spim,
                               const xwu8_t txd[], xwu8_t * rxb,
                               xwsz_t * size, xwtm_t to);

const struct xwds_spim_driver stm32cube_spi2m_drv = {
        .base = {
                .name = "stm32cube.spim.2",
                .probe = NULL,
                .remove = NULL,
                .start = stm32cube_spi2m_drv_start,
                .stop = stm32cube_spi2m_drv_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
                .suspend = stm32cube_spi2m_drv_suspend,
                .resume =  stm32cube_spi2m_drv_resume,
#endif
        },
        .buscfg = stm32cube_spi2m_drv_buscfg,
        .xfer = stm32cube_spi2m_drv_xfer,
};

struct xwds_spim stm32cube_spi2m_cb = {
        /* attributes */
        .dev = {
                .name = "stm32cube.spim.2",
                .id = 1,
                .resources = NULL,
                .drv = xwds_cast(struct xwds_driver *, &stm32cube_spi2m_drv),
                .data = (void *)&hspi2_drvdata,
        },
        .buscfg = NULL,
        .buscfg_num = 0,
};

static
xwer_t stm32cube_spi2m_drv_start(struct xwds_device * dev)
{
        struct xwds_spim * spim;
        struct MX_SPI_MasterDriverData * drvdata;

        spim = xwds_cast(struct xwds_spim *, dev);
        drvdata = spim->dev.data;
        drvdata->spim = spim;
        MX_SPI2_Init();
        return XWOK;
}

static
xwer_t stm32cube_spi2m_drv_stop(struct xwds_device * dev)
{
        XWOS_UNUSED(dev);

        MX_SPI2_DeInit();
        return XWOK;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static
xwer_t stm32cube_spi2m_drv_resume(struct xwds_device * dev)
{
        return stm32cube_spi2m_drv_start(dev);
}

static
xwer_t stm32cube_spi2m_drv_suspend(struct xwds_device * dev)
{
        return stm32cube_spi2m_drv_stop(dev);
}
#endif

static
xwer_t stm32cube_spi2m_drv_buscfg(struct xwds_spim * spim,
                                  xwid_t cfgid,
                                  xwtm_t to)
{
        const SPI_InitTypeDef ** buscfg;
        const SPI_InitTypeDef * cfg;
        xwer_t rc;

        XWOS_UNUSED(to);

        buscfg = spim->buscfg;
        if (buscfg) {
                cfg = buscfg[cfgid];
                rc = MX_SPI2_ReInit(cfg);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

static
xwer_t stm32cube_spi2m_drv_xfer(struct xwds_spim * spim,
                                const xwu8_t txd[], xwu8_t * rxb,
                                xwsz_t * size, xwtm_t to)
{
        struct MX_SPI_MasterDriverData * drvdata;
        union xwos_ulock ulk;
        xwreg_t cpuirq;
        xwsq_t lkst;
        xwer_t rc;

        drvdata = spim->dev.data;
        ulk.osal.splk = &drvdata->splk;
        xwos_splk_lock_cpuirqsv(&drvdata->splk, &cpuirq);
        drvdata->rc = -EINPROGRESS;
        rc = MX_SPI2_Xfer(txd, rxb, size);
        if (XWOK == rc) {
                rc = xwos_cond_wait_to(&drvdata->cond,
                                       ulk, XWOS_LK_SPLK, NULL,
                                       to, &lkst);
                if (XWOK == rc) {
                        if (XWOS_LKST_UNLOCKED == lkst) {
                                xwos_splk_lock(&drvdata->splk);
                        }
                        *size = hspi2_drvdata.size;
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

void stm32cube_spi2m_cb_xfercplt(struct xwds_spim * spim, xwer_t xrc)
{
        struct MX_SPI_MasterDriverData * drvdata;
        xwreg_t cpuirq;

        drvdata = spim->dev.data;
        xwos_splk_lock_cpuirqsv(&drvdata->splk, &cpuirq);
        if (-ECANCELED != drvdata->rc) {
                drvdata->rc = xrc;
        } else {
        }
        xwos_splk_unlock_cpuirqrs(&drvdata->splk, cpuirq);
        xwos_cond_broadcast(&drvdata->cond);
}
