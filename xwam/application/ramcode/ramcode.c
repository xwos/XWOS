/**
 * @file
 * @brief XWOS Ramcode
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <string.h>
#include <arch_firmware.h>
#include <xwos/lib/crc32.h>
#include <xwam/application/ramcode/mif.h>

const char ramcode_tag[] = "XWOS.RC";

xwer_t ramcode_init(struct ramcode * ramcode,
                    const struct ramcode_operation * op, void * opcb,
                    xwsz_t firmware_info_offset,
                    const char * firmware_tailflag)
{
        memset(ramcode, 0, sizeof(struct ramcode));
        ramcode->op = op;
        ramcode->opcb = opcb;
        ramcode->firmware_info_offset = firmware_info_offset;
        ramcode->firmware_tailflag = firmware_tailflag;
        return XWOK;
}

xwer_t ramcode_load(struct ramcode * ramcode)
{
        xwer_t rc;
        xwssz_t rest;
        xwsz_t size;
        xwu32_t crc32;

        if ((NULL == ramcode->op) || (NULL == ramcode->op->load)) {
                rc = -ENOSYS;
                goto err_op;
        }
        size = sizeof(struct ramcode_info);
        rc = ramcode->op->load(ramcode->opcb,
                               (xwu8_t *)&ramcode->info, &size, XWTM_MAX);
        if ((rc < 0) || (size <= 0)) {
                goto err_info;
        }
        if (0 != memcmp(ramcode->info.tag, ramcode_tag, sizeof(ramcode_tag))) {
                goto err_tag;
        }

        rest = (xwssz_t)ramcode->info.size;
        while (rest > 0) {
                size = (xwsz_t)rest;
                rc = ramcode->op->load(ramcode->opcb,
                                       (xwu8_t *)ramcode->info.origin, &size,
                                       XWTM_MAX);
                if (XWOK == rc) {
                        rest -= (xwssz_t)size;
                }
        }
        if (rest != 0) {
                rc = -ESIZE;
                goto err_size;
        }
        size = ramcode->info.size;
        crc32 = xwlib_crc32_calms((const xwu8_t *)ramcode->info.origin, &size);
        if (crc32 != ramcode->info.crc32) {
                rc = -EBADMSG;
        } else {
                rc = XWOK;
        }

err_size:
err_tag:
err_info:
err_op:
        return rc;
}

xwer_t ramcode_boot(struct ramcode * ramcode)
{
        arch_boot_firmware((void *)ramcode->info.origin,
                           ramcode->firmware_info_offset,
                           ramcode->firmware_tailflag);
        return -EFAULT;
}
