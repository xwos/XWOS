/**
 * @file
 * @brief 板级描述层：XWOS适配层：picolibc：标准输入输出
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
#include <stdio.h>
#include <xwos/lib/errno.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_console.h>
#include "board/xwac/xwds/device.h"

xwssz_t picolibcac_fops_read_stdin(int fd, void * buf, size_t cnt)
{
        XWOS_UNUSED(fd);

        soc_console_rx(buf, cnt);
        errno = 0;
        return cnt;
}

ssize_t read(int fd, void * buf, size_t cnt)
{
        int ret;

        if (0 == fd) {
                ret = (int)picolibcac_fops_read_stdin(fd, buf, cnt);
        } else {
                errno = EPERM;
                ret = -1;
        }
        return ret;
}

xwssz_t picolibcac_fops_write_stdout(int fd, const void * data, size_t cnt)
{
        XWOS_UNUSED(fd);

        soc_console_tx((const xwu8_t *)data, cnt);
        errno = 0;
        return cnt;
}

xwssz_t picolibcac_fops_write_stderr(int fd, const void * data, size_t cnt)
{
        XWOS_UNUSED(fd);

        soc_console_tx((const xwu8_t *)data, cnt);
        errno = 0;
        return cnt;
}

ssize_t write(int fd, const void * data, size_t cnt)
{
        int ret;

        if (1 == fd) {
                ret = (int)picolibcac_fops_write_stdout(fd, data, cnt);
        } else if (2 == fd) {
                ret = (int)picolibcac_fops_write_stderr(fd, data, cnt);
        } else {
                errno = EPERM;
                ret = -1;
        }
        return ret;
}

int open(const char * path, int flag, ...)
{

        XWOS_UNUSED(path);
        XWOS_UNUSED(flag);

        errno = ENOSYS;
        return -1;
}

int close(int fd)
{
        XWOS_UNUSED(fd);

        errno = ENOSYS;
        return -1;
}


_off_t lseek(int fd, _off_t pos, int whence)
{
        XWOS_UNUSED(fd);
        XWOS_UNUSED(pos);
        XWOS_UNUSED(whence);

        errno = ENOSYS;
        return -1;
}

struct stat;

int fstat(int fd, struct stat * sbuf)
{
        XWOS_UNUSED(fd);
        XWOS_UNUSED(sbuf);

        errno = ENOSYS;
        return -1;
}
