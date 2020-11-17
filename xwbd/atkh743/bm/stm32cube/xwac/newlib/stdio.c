/**
 * @file
 * @brief STM32CUBE模块：定义标准输入输出
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
#include <bm/stm32cube/xwac/xwds/device.h>

extern int errno;

xwssz_t newlibac_fops_read_stdin(int fd, void * buf, size_t cnt)
{
        xwtm_t desired;
        xwssz_t rdsz;
        xwsz_t bufsz = cnt;
        xwer_t rc;

        XWOS_UNUSED(fd);
        do {
                desired = 20 * XWTM_MS;
                cnt = bufsz;
                rc = xwds_dmauartc_rx(&stm32cube_usart1_cb, buf, &cnt, &desired);
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

xwssz_t newlibac_fops_write_stdout(int fd, const void * data, size_t cnt)
{
        xwtm_t desired;
        xwssz_t wrsz;
        xwer_t rc;

        XWOS_UNUSED(fd);
        desired = XWTM_MAX;
        rc = xwds_dmauartc_tx(&stm32cube_usart1_cb, data, &cnt, &desired);
        errno = -rc;
        if (rc < 0) {
                wrsz = -1;
        } else {
                wrsz = (xwssz_t)cnt;
        }
        return wrsz;
}

xwssz_t newlibac_fops_write_stderr(int fd, const void * data, size_t cnt)
{
        xwtm_t desired;
        xwssz_t wrsz;
        xwer_t rc;

        XWOS_UNUSED(fd);
        desired = XWTM_MAX;
        rc = xwds_dmauartc_tx(&stm32cube_usart1_cb, data, &cnt, &desired);
        errno = -rc;
        if (rc < 0) {
                wrsz = -1;
        } else {
                wrsz = (xwssz_t)cnt;
        }
        return wrsz;
}
