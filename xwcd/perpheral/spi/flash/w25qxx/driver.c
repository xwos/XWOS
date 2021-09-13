/**
 * @file
 * @brief SPI Flash W25Qxx Driver
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

#include <xwos/standard.h>
#include <string.h>
#include <xwos/osal/skd.h>
#include <xwcd/ds/spi/perpheral.h>
#include <xwcd/perpheral/spi/flash/w25qxx/device.h>
#include <xwcd/perpheral/spi/flash/w25qxx/driver.h>

/******** ******** base driver ******** ********/
xwer_t xwds_w25qxx_drv_start(struct xwds_device * dev)
{
        struct xwds_w25qxx * w25qxx;
        const struct xwds_w25qxx_driver * drv;
        const struct xwds_w25qxx_cmd * cmdtbl;
        xwer_t rc;

        w25qxx = xwds_cast(struct xwds_w25qxx *, dev);
        drv = xwds_cast(struct xwds_w25qxx_driver *, dev->drv);
        cmdtbl = w25qxx->cmdtbl;
        if ((NULL == drv) || (NULL == drv->io) ||
            (NULL == cmdtbl) || (NULL == w25qxx->spip.bus)) {
                rc = -EINVAL;
                goto err_desc_err;
        }
        memset(w25qxx->txq, 0, sizeof(w25qxx->txq));
        memset(w25qxx->rxq, 0, sizeof(w25qxx->rxq));
        return XWOK;

err_desc_err:
        return rc;
}

xwer_t xwds_w25qxx_drv_stop(struct xwds_device * dev)
{
        XWOS_UNUSED(dev);
        return XWOK;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
xwer_t xwds_w25qxx_drv_resume(struct xwds_device * dev)
{
        return xwds_w25qxx_drv_start(dev);
}

xwer_t xwds_w25qxx_drv_suspend(struct xwds_device * dev)
{
        return xwds_w25qxx_drv_stop(dev);
}
#endif

/******** ******** ******** APIs ******** ******** ********/
xwer_t xwds_w25qxx_cfgbus(struct xwds_w25qxx * w25qxx, xwtm_t * xwtm)
{
        xwer_t rc;

        XWDS_VALIDATE(w25qxx, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_w25qxx_grab(w25qxx);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_w25qxx_grab;
        }
        rc = xwds_spim_buscfg(w25qxx->spip.bus, w25qxx->spip.buscfgid, xwtm);
        if (rc < 0) {
                goto err_spim_buscfg;
        }
        xwds_w25qxx_put(w25qxx);
        return XWOK;

err_spim_buscfg:
        xwds_w25qxx_put(w25qxx);
err_w25qxx_grab:
        return rc;
}

xwer_t xwds_w25qxx_ctrl(struct xwds_w25qxx * w25qxx,
                        xwu8_t instruction,
                        xwu8_t address_size, xwu32_t address,
                        xwu32_t dummy_cycles,
                        const xwu8_t txd[], xwu8_t * rxb, xwsz_t size,
                        xwtm_t * xwtm)
{
        const struct xwds_w25qxx_driver * drv;
        xwsz_t i, j, idx, rxofs, xfersize;
        xwer_t rc;

        XWDS_VALIDATE(w25qxx, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_w25qxx_grab(w25qxx);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_w25qxx_grab;
        }
        drv = xwds_cast(struct xwds_w25qxx_driver *, w25qxx->spip.dev.drv);
        idx = 0;
        w25qxx->txq[idx] = instruction;
        idx++;
        if (address_size) {
                for (i = 0, j = (xwsz_t)(address_size - 1);
                     i < address_size;
                     i++, j--) {
                        w25qxx->txq[idx] = (xwu8_t)(address >> (j * 8));
                        idx++;
                }
        }
        if (dummy_cycles) {
                for (i = 0; i < (dummy_cycles / 8); i++) {
                        w25qxx->txq[idx] = 0;
                        idx++;
                }
        }
        rxofs = idx;
        for (i = 0; i < size; i++) {
                if (txd) {
                        w25qxx->txq[idx] = txd[i];
                } else {
                        w25qxx->txq[idx] = 0;
                }
                idx++;
        }
        xfersize = idx;
        rc = drv->io(w25qxx,
                     w25qxx->txq, w25qxx->rxq, &xfersize,
                     xwtm);
        if (rc < 0) {
                goto err_spim_xfer;
        }
        if (rxb) {
                for (i = 0; i < size; i++) {
                        rxb[i] = w25qxx->rxq[rxofs + i];
                }
        }
        xwds_w25qxx_put(w25qxx);
        return XWOK;

err_spim_xfer:
        xwds_w25qxx_put(w25qxx);
err_w25qxx_grab:
        return rc;
}

xwer_t xwds_w25qxx_reset(struct xwds_w25qxx * w25qxx, xwtm_t * xwtm)
{
        struct xwds_w25qxx_cmd cmd;
        xwer_t rc;

        XWDS_VALIDATE(w25qxx, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        cmd = w25qxx->cmdtbl[XWDS_W25QXX_CMD_ENABLE_RESET];
        if (cmd.existing) {
                rc = xwds_w25qxx_ctrl(w25qxx,
                                      cmd.instruction,
                                      cmd.address_size, cmd.address,
                                      cmd.dummy_cycles,
                                      NULL, NULL, 0, xwtm);
                if (rc < 0) {
                        goto err_w25qxx_ctrl;
                }
        }
        cmd = w25qxx->cmdtbl[XWDS_W25QXX_CMD_RESET];
        if (cmd.existing) {
                rc = xwds_w25qxx_ctrl(w25qxx,
                                      cmd.instruction,
                                      cmd.address_size, cmd.address,
                                      cmd.dummy_cycles,
                                      NULL, NULL, 0, xwtm);
                if (rc < 0) {
                        goto err_w25qxx_ctrl;
                }
        }
        return XWOK;

err_w25qxx_ctrl:
        return rc;
}

xwer_t xwds_w25qxx_init_parameter(struct xwds_w25qxx * w25qxx, xwtm_t * xwtm)
{
        xwu64_t uid;
        xwu32_t jid;
        xwu16_t mid;
        xwer_t rc;

        rc = xwds_w25qxx_read_uid(w25qxx, &uid, xwtm);
        if (rc < 0) {
                goto err_read_uid;
        }
        rc = xwds_w25qxx_read_mid(w25qxx, &mid, xwtm);
        if (rc < 0) {
                goto err_read_mid;
        }
        rc = xwds_w25qxx_read_jid(w25qxx, &jid, xwtm);
        if (rc < 0) {
                goto err_read_jid;
        }
        switch (w25qxx->parameter.mid) {
        case XWDS_W25Q80_MID:
                w25qxx->parameter.size = 1 * 1024 * 1024;
                break;
        case XWDS_W25Q16_MID:
                w25qxx->parameter.size = 2 * 1024 * 1024;
                break;
        case XWDS_W25Q32_MID:
                w25qxx->parameter.size = 4 * 1024 * 1024;
                break;
        case XWDS_W25Q64_MID:
                w25qxx->parameter.size = 8 * 1024 * 1024;
                break;
        case XWDS_W25Q128_MID:
                w25qxx->parameter.size = 16 * 1024 * 1024;
                break;
        case XWDS_W25Q256_MID:
                w25qxx->parameter.size = 32 * 1024 * 1024;
                break;
        default:
                break;
        }
        return XWOK;

err_read_jid:
err_read_mid:
err_read_uid:
        return rc;
}

xwer_t xwds_w25qxx_write_enable(struct xwds_w25qxx * w25qxx, xwtm_t * xwtm)
{
        struct xwds_w25qxx_cmd cmd;
        xwer_t rc;
        xwu8_t sr1;

        XWDS_VALIDATE(w25qxx, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        cmd = w25qxx->cmdtbl[XWDS_W25QXX_CMD_WRITE_ENABLE];
        if (1 != cmd.existing) {
                rc = -ENOTSUP;
                goto err_not_support;
        }
        rc = xwds_w25qxx_ctrl(w25qxx,
                              cmd.instruction,
                              cmd.address_size, cmd.address,
                              cmd.dummy_cycles,
                              NULL, NULL, 0, xwtm);
        if (rc < 0) {
                goto err_w25qxx_ctrl;
        }
        rc = xwds_w25qxx_read_sr(w25qxx,
                                 XWDS_W25QXX_SR_1, &sr1,
                                 xwtm);
        if (rc < 0) {
                goto err_chk_wel;
        }
        if (!(sr1 & XWDS_W25QXX_SR1_WEL)) {
                rc = -EACCES;
                goto err_chk_wel;
        }
        return XWOK;

err_chk_wel:
err_w25qxx_ctrl:
err_not_support:
        return rc;
}

xwer_t xwds_w25qxx_write_disable(struct xwds_w25qxx * w25qxx, xwtm_t * xwtm)
{
        struct xwds_w25qxx_cmd cmd;
        xwer_t rc;

        XWDS_VALIDATE(w25qxx, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        cmd = w25qxx->cmdtbl[XWDS_W25QXX_CMD_WRITE_DISABLE];
        if (cmd.existing) {
                rc = -ENOTSUP;
                goto err_not_support;
        }
        rc = xwds_w25qxx_ctrl(w25qxx,
                              cmd.instruction,
                              cmd.address_size, cmd.address,
                              cmd.dummy_cycles,
                              NULL, NULL, 0, xwtm);
        if (rc < 0) {
                goto err_w25qxx_ctrl;
        }
        return XWOK;

err_w25qxx_ctrl:
err_not_support:
        return rc;
}

xwer_t xwds_w25qxx_read_sr(struct xwds_w25qxx * w25qxx,
                           xwu32_t sridx, xwu8_t * srbuf,
                           xwtm_t * xwtm)
{
        struct xwds_w25qxx_cmd cmd;
        xwer_t rc;

        XWDS_VALIDATE(w25qxx, "nullptr", -EFAULT);
        XWDS_VALIDATE((sridx < XWDS_W25QXX_SR_NUM), "out-of-range", -ECHRNG);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        switch (sridx) {
        case XWDS_W25QXX_SR_2:
                cmd = w25qxx->cmdtbl[XWDS_W25QXX_CMD_READ_STATUS_REG_2];
                break;
        case XWDS_W25QXX_SR_3:
                cmd = w25qxx->cmdtbl[XWDS_W25QXX_CMD_READ_STATUS_REG_3];
                break;
        case XWDS_W25QXX_SR_1:
        default:
                cmd = w25qxx->cmdtbl[XWDS_W25QXX_CMD_READ_STATUS_REG_1];
                break;
        }
        if (1 != cmd.existing) {
                rc = -ENOTSUP;
                goto err_not_support;
        }
        rc = xwds_w25qxx_ctrl(w25qxx,
                              cmd.instruction,
                              cmd.address_size, cmd.address,
                              cmd.dummy_cycles,
                              NULL, srbuf, 1, xwtm);
        if (rc < 0) {
                goto err_w25qxx_ctrl;
        }
        return XWOK;

err_w25qxx_ctrl:
err_not_support:
        return rc;
}

xwer_t xwds_w25qxx_check_idle(struct xwds_w25qxx * w25qxx, xwtm_t * xwtm)
{
        xwer_t rc;
        xwu8_t sr1;

        rc = xwds_w25qxx_read_sr(w25qxx,
                                 XWDS_W25QXX_SR_1, &sr1,
                                 xwtm);
        if (XWOK == rc) {
                if (sr1 & XWDS_W25QXX_SR1_BUSY) {
                        rc = -EBUSY;
                }
        }
        return rc;
}

xwer_t xwds_w25qxx_wait_idle(struct xwds_w25qxx * w25qxx, xwtm_t period,
                             xwtm_t * xwtm)
{
        xwer_t rc;
        xwtm_t sleep, desired, tmp;

        rc = xwds_w25qxx_check_idle(w25qxx, xwtm);
        desired = *xwtm;
        sleep = (desired > period) ? period : desired;
        while ((sleep > 0) && (-EBUSY == rc)) {
                tmp = sleep;
                rc = xwos_cthd_sleep(&tmp);
                desired = desired - sleep;
                if (XWOK == rc) {
                        rc = xwds_w25qxx_check_idle(w25qxx, xwtm);
                        sleep = (desired > period) ? period : desired;
                } else {
                        desired = desired + tmp;
                }
        }
        *xwtm = desired;
        return rc;
}

xwer_t xwds_w25qxx_read_uid(struct xwds_w25qxx * w25qxx, xwu64_t * uidbuf,
                            xwtm_t * xwtm)
{
        struct xwds_w25qxx_cmd cmd;
        xwer_t rc;

        XWDS_VALIDATE(w25qxx, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        cmd = w25qxx->cmdtbl[XWDS_W25QXX_CMD_UNIQUE_ID];
        if (1 != cmd.existing) {
                rc = -ENOTSUP;
                goto err_not_support;
        }
        rc = xwds_w25qxx_ctrl(w25qxx,
                              cmd.instruction,
                              cmd.address_size, cmd.address,
                              cmd.dummy_cycles,
                              NULL, (xwu8_t *)uidbuf, 8, xwtm);
        if (rc < 0) {
                goto err_w25qxx_ctrl;
        }
        *uidbuf = xwbop_re(xwu64_t, *uidbuf);
        w25qxx->parameter.uid = *uidbuf;
        return XWOK;

err_w25qxx_ctrl:
err_not_support:
        return rc;
}

xwer_t xwds_w25qxx_read_mid(struct xwds_w25qxx * w25qxx, xwu16_t * midbuf,
                            xwtm_t * xwtm)
{
        struct xwds_w25qxx_cmd cmd;
        xwer_t rc;

        XWDS_VALIDATE(w25qxx, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        cmd = w25qxx->cmdtbl[XWDS_W25QXX_CMD_MANUFACTURER_DEVICE_ID];
        if (1 != cmd.existing) {
                rc = -ENOTSUP;
                goto err_not_support;
        }
        rc = xwds_w25qxx_ctrl(w25qxx,
                              cmd.instruction,
                              cmd.address_size, cmd.address,
                              cmd.dummy_cycles,
                              NULL, (xwu8_t *)midbuf, 2, xwtm);
        if (rc < 0) {
                goto err_w25qxx_ctrl;
        }
        *midbuf = xwbop_re(xwu16_t, *midbuf);
        w25qxx->parameter.mid = *midbuf;
        return XWOK;

err_w25qxx_ctrl:
err_not_support:
        return rc;
}

xwer_t xwds_w25qxx_read_jid(struct xwds_w25qxx * w25qxx, xwu32_t * jidbuf,
                            xwtm_t * xwtm)
{
        struct xwds_w25qxx_cmd cmd;
        xwer_t rc;

        XWDS_VALIDATE(w25qxx, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        cmd = w25qxx->cmdtbl[XWDS_W25QXX_CMD_JEDEC_DEVICE_ID];
        if (1 != cmd.existing) {
                rc = -ENOTSUP;
                goto err_not_support;
        }
        rc = xwds_w25qxx_ctrl(w25qxx,
                              cmd.instruction,
                              cmd.address_size, cmd.address,
                              cmd.dummy_cycles,
                              NULL, (xwu8_t *)jidbuf, 3, xwtm);
        if (rc < 0) {
                goto err_w25qxx_ctrl;
        }
        *jidbuf = xwbop_re(xwu32_t, *jidbuf);
        *jidbuf >>= 8;
        w25qxx->parameter.jid = *jidbuf;
        return XWOK;

err_w25qxx_ctrl:
err_not_support:
        return rc;
}

xwer_t xwds_w25qxx_read(struct xwds_w25qxx * w25qxx, xwu32_t address,
                        xwu8_t * rxb, xwsz_t * size, xwtm_t * xwtm)
{
        struct xwds_w25qxx_cmd cmd;
        xwsz_t xfsize;
        xwer_t rc;

        XWDS_VALIDATE(w25qxx, "nullptr", -EFAULT);
        XWDS_VALIDATE(rxb, "nullptr", -EFAULT);
        XWDS_VALIDATE(size, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        xfsize = *size;
        cmd = w25qxx->cmdtbl[XWDS_W25QXX_CMD_FAST_READ_DATA];
        if (1 != cmd.existing) {
                rc = -ENOTSUP;
                goto err_not_support;
        }
        rc = xwds_w25qxx_ctrl(w25qxx,
                              cmd.instruction,
                              cmd.address_size, address,
                              cmd.dummy_cycles,
                              NULL, rxb, xfsize, xwtm);
        if (rc < 0) {
                goto err_w25qxx_ctrl;
        }
        *size = xfsize;
        return XWOK;

err_w25qxx_ctrl:
err_not_support:
        return rc;
}

xwer_t xwds_w25qxx_write(struct xwds_w25qxx * w25qxx, xwu32_t address,
                         xwu8_t * txb, xwsz_t * size, xwtm_t * xwtm)
{
        struct xwds_w25qxx_cmd cmd;
        xwsz_t xfsize;
        xwer_t rc;

        XWDS_VALIDATE(w25qxx, "nullptr", -EFAULT);
        XWDS_VALIDATE(txb, "nullptr", -EFAULT);
        XWDS_VALIDATE(size, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_w25qxx_write_enable(w25qxx, xwtm);
        if (rc < 0) {
                goto err_we;
        }
        rc = xwds_w25qxx_wait_idle(w25qxx, 1 * XWTM_MS, xwtm);
        if (rc < 0) {
                goto err_wait_idle;
        }
        xfsize = *size;
        cmd = w25qxx->cmdtbl[XWDS_W25QXX_CMD_PAGE_PROGRAM];
        if (1 != cmd.existing) {
                rc = -ENOTSUP;
                goto err_not_support;
        }
        rc = xwds_w25qxx_ctrl(w25qxx,
                              cmd.instruction,
                              cmd.address_size, address,
                              cmd.dummy_cycles,
                              txb, NULL, xfsize, xwtm);
        if (rc < 0) {
                goto err_w25qxx_ctrl;
        }
        *size = xfsize;
        return XWOK;

err_w25qxx_ctrl:
err_not_support:
err_wait_idle:
        xwds_w25qxx_write_disable(w25qxx, xwtm);
err_we:
        return rc;
}

xwer_t xwds_w25qxx_erase_sector(struct xwds_w25qxx * w25qxx, xwu32_t address,
                                xwtm_t * xwtm)
{
        struct xwds_w25qxx_cmd cmd;
        xwer_t rc;

        XWDS_VALIDATE(w25qxx, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_w25qxx_write_enable(w25qxx, xwtm);
        if (rc < 0) {
                goto err_we;
        }
        cmd = w25qxx->cmdtbl[XWDS_W25QXX_CMD_SECTOR_ERASE];
        if (1 != cmd.existing) {
                rc = -ENOTSUP;
                goto err_not_support;
        }
        rc = xwds_w25qxx_ctrl(w25qxx,
                              cmd.instruction,
                              cmd.address_size, address,
                              cmd.dummy_cycles,
                              NULL, NULL, 0, xwtm);
        if (rc < 0) {
                goto err_w25qxx_ctrl;
        }
        rc = xwds_w25qxx_wait_idle(w25qxx, 400 * XWTM_MS, xwtm);
        if (rc < 0) {
                goto err_chk_idle;
        }
        return XWOK;

err_chk_idle:
err_w25qxx_ctrl:
err_not_support:
        xwds_w25qxx_write_disable(w25qxx, xwtm);
err_we:
        return rc;
}

xwer_t xwds_w25qxx_erase_32kblk(struct xwds_w25qxx * w25qxx, xwu32_t address,
                                xwtm_t * xwtm)
{
        struct xwds_w25qxx_cmd cmd;
        xwer_t rc;

        XWDS_VALIDATE(w25qxx, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_w25qxx_write_enable(w25qxx, xwtm);
        if (rc < 0) {
                goto err_we;
        }
        cmd = w25qxx->cmdtbl[XWDS_W25QXX_CMD_32KBLOCK_ERASE];
        if (1 != cmd.existing) {
                rc = -ENOTSUP;
                goto err_not_support;
        }
        rc = xwds_w25qxx_ctrl(w25qxx,
                              cmd.instruction,
                              cmd.address_size, address,
                              cmd.dummy_cycles,
                              NULL, NULL, 0, xwtm);
        if (rc < 0) {
                goto err_w25qxx_ctrl;
        }
        rc = xwds_w25qxx_wait_idle(w25qxx, 1600 * XWTM_MS, xwtm);
        if (rc < 0) {
                goto err_chk_idle;
        }
        return XWOK;

err_chk_idle:
err_w25qxx_ctrl:
err_not_support:
        xwds_w25qxx_write_disable(w25qxx, xwtm);
err_we:
        return rc;
}

xwer_t xwds_w25qxx_erase_64kblk(struct xwds_w25qxx * w25qxx, xwu32_t address,
                                xwtm_t * xwtm)
{
        struct xwds_w25qxx_cmd cmd;
        xwer_t rc;

        XWDS_VALIDATE(w25qxx, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_w25qxx_write_enable(w25qxx, xwtm);
        if (rc < 0) {
                goto err_we;
        }
        cmd = w25qxx->cmdtbl[XWDS_W25QXX_CMD_64KBLOCK_ERASE];
        if (1 != cmd.existing) {
                rc = -ENOTSUP;
                goto err_not_support;
        }
        rc = xwds_w25qxx_ctrl(w25qxx,
                              cmd.instruction,
                              cmd.address_size, address,
                              cmd.dummy_cycles,
                              NULL, NULL, 0, xwtm);
        if (rc < 0) {
                goto err_w25qxx_ctrl;
        }
        rc = xwds_w25qxx_wait_idle(w25qxx, 2000 * XWTM_MS, xwtm);
        if (rc < 0) {
                goto err_chk_idle;
        }
        return XWOK;

err_chk_idle:
err_w25qxx_ctrl:
err_not_support:
        xwds_w25qxx_write_disable(w25qxx, xwtm);
err_we:
        return rc;
}

xwer_t xwds_w25qxx_erase_chip(struct xwds_w25qxx * w25qxx, xwtm_t * xwtm)
{
        struct xwds_w25qxx_cmd cmd;
        xwer_t rc;

        XWDS_VALIDATE(w25qxx, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_w25qxx_write_enable(w25qxx, xwtm);
        if (rc < 0) {
                goto err_we;
        }
        cmd = w25qxx->cmdtbl[XWDS_W25QXX_CMD_CHIP_ERASE];
        if (1 != cmd.existing) {
                rc = -ENOTSUP;
                goto err_not_support;
        }
        rc = xwds_w25qxx_ctrl(w25qxx,
                              cmd.instruction,
                              cmd.address_size, cmd.address,
                              cmd.dummy_cycles,
                              NULL, NULL, 0, xwtm);
        if (rc < 0) {
                goto err_w25qxx_ctrl;
        }
        rc = xwds_w25qxx_wait_idle(w25qxx, 2000 * XWTM_MS, xwtm);
        if (rc < 0) {
                goto err_chk_idle;
        }
        return XWOK;

err_chk_idle:
err_w25qxx_ctrl:
err_not_support:
        xwds_w25qxx_write_disable(w25qxx, xwtm);
err_we:
        return rc;
}
