/**
 * @file
 * @brief 玄武OS Ramcode Image
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
#include <arch_image.h>
#include <xwos/lib/crc32.h>
#include <xwmd/ramcode/mif.h>

xwer_t ramcode_init(struct ramcode * ramcode,
                    const struct ramcode_operation * op,
                    void * opcb)
{
        memset(ramcode, 0 , sizeof(struct ramcode));
        ramcode->op = op;
        ramcode->opcb = opcb;
        return XWOK;
}

xwer_t ramcode_load(struct ramcode * ramcode)
{
        xwer_t rc;
        xwssz_t rest;
        xwsz_t size;
        xwtm_t xwtm;
        xwu32_t crc32;

        if ((NULL == ramcode->op) || (NULL == ramcode->op->load)) {
                rc = -ENOSYS;
                goto err_op;
        }
        size = sizeof(struct ramcode_info);
        xwtm = XWTM_MAX;
        rc = ramcode->op->load(ramcode->opcb,
                               (xwu8_t *)&ramcode->info, &size, &xwtm);
        if ((rc < 0) || (size <= 0)) {
                goto err_info;
        }
        rest = (xwssz_t)ramcode->info.size;
        xwtm = XWTM_MAX;
        while (rest > 0) {
                size = (xwsz_t)rest;
                rc = ramcode->op->load(ramcode->opcb,
                                       (xwu8_t *)ramcode->info.origin, &size,
                                       &xwtm);
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
        if ((((crc32 >> 24U) & 0xFFU) != ramcode->info.crc32[0]) ||
            (((crc32 >> 16U) & 0xFFU) != ramcode->info.crc32[1]) ||
            (((crc32 >> 8U) & 0xFFU) != ramcode->info.crc32[2]) ||
            (((crc32 >> 0U) & 0xFFU) != ramcode->info.crc32[3])) {
                rc = -EBADMSG;
        } else {
                rc = XWOK;
        }

err_size:
err_info:
err_op:
        return rc;
}

xwer_t ramcode_boot(struct ramcode * ramcode)
{
        arch_boot_image((void *)ramcode->info.origin);
        return -EFAULT;
}
