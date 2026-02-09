/**
 * @file
 * @brief 板级描述层：XWOS适配层：newlib：标准输入输出
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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

#include "board/std.h"
#include <xwos/lib/errno.h>
#include <xwos/osal/skd.h>
#include <xwos/osal/thd.h>
#include <bm/Stm32Hal/mi.h>

xwssz_t newlibac_fops_read_stdin(int fd, void * buf, xwsz_t cnt)
{
        xwssz_t rdsz;
        xwsz_t bufsz = cnt;
        xwer_t rc;

        XWOS_UNUSED(fd);
        do {
                cnt = bufsz;
                rc = xwds_uartc_rx(&stm32xwds_usart1,
                                   buf, &cnt,
                                   xwtm_ft(xwtm_ms(20)));
                if (-ETIMEDOUT == rc) {
                        rdsz = (xwssz_t)cnt;
                        errno = 0;
                } else if (rc < 0) {
                        rdsz = -1;
                        errno = -rc;
                        break;
                } else {
                        rdsz = (xwssz_t)cnt;
                        errno = 0;
                }
        } while (0 == rdsz);
        return rdsz;
}

xwssz_t newlibac_fops_write_stdout(int fd, const void * data, xwsz_t cnt)
{
        xwsq_t ctx;
        xwssz_t wrsz;
        xwer_t rc;
        XWOS_UNUSED(fd);

        xwos_skd_get_context_lc(&ctx, NULL);
        if (XWOS_SKD_CONTEXT_BOOT == ctx) {
                rc = -EPERM;
        } else if (XWOS_SKD_CONTEXT_THD == ctx) {
                if (xwos_cthd_shld_frz()) {
                        rc = xwds_uartc_etx(&stm32xwds_usart1,
                                            (const xwu8_t *)data, &cnt);
                } else {
                        rc = xwds_uartc_tx(&stm32xwds_usart1,
                                           (const xwu8_t *)data, &cnt,
                                           XWTM_MAX);
                }
        } else {
                rc = xwds_uartc_etx(&stm32xwds_usart1, (const xwu8_t *)data, &cnt);
        }
        errno = -rc;
        if (rc < 0) {
                wrsz = -1;
        } else {
                wrsz = (xwssz_t)cnt;
        }
        return wrsz;
}

xwssz_t newlibac_fops_write_stderr(int fd, const void * data, xwsz_t cnt)
{
        xwsq_t ctx;
        xwssz_t wrsz;
        xwer_t rc;
        XWOS_UNUSED(fd);

        xwos_skd_get_context_lc(&ctx, NULL);
        if (XWOS_SKD_CONTEXT_BOOT == ctx) {
                rc = -EPERM;
        } else if (XWOS_SKD_CONTEXT_THD == ctx) {
                if (xwos_cthd_shld_frz()) {
                        rc = xwds_uartc_etx(&stm32xwds_usart1,
                                            (const xwu8_t *)data, &cnt);
                } else {
                        rc = xwds_uartc_tx(&stm32xwds_usart1,
                                           (const xwu8_t *)data, &cnt,
                                           XWTM_MAX);
                }
        } else {
                rc = xwds_uartc_etx(&stm32xwds_usart1, (const xwu8_t *)data, &cnt);
        }
        errno = -rc;
        if (rc < 0) {
                wrsz = -1;
        } else {
                wrsz = (xwssz_t)cnt;
        }
        return wrsz;
}
