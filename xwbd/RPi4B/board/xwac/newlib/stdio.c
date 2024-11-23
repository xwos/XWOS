/**
 * @file
 * @brief 板级描述层：XWOS适配层：newlib：标准输入输出
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

#include "board/std.h"
#include <xwos/lib/errno.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_console.h>
#include "board/xwac/xwds/device.h"

xwssz_t newlibac_fops_read_stdin(int fd, void * buf, xwsz_t cnt)
{
        XWOS_UNUSED(fd);
        soc_console_rx(buf, cnt);
        errno = 0;
        return cnt;
}

xwssz_t newlibac_fops_write_stdout(int fd, const void * data, xwsz_t cnt)
{
        XWOS_UNUSED(fd);
        soc_console_tx((const xwu8_t *)data, cnt);
        errno = 0;
        return cnt;
}

xwssz_t newlibac_fops_write_stderr(int fd, const void * data, xwsz_t cnt)
{
        XWOS_UNUSED(fd);
        soc_console_tx((const xwu8_t *)data, cnt);
        errno = 0;
        return cnt;
}
