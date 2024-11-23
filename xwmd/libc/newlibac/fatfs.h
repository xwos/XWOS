/**
 * @file
 * @brief newlib适配层：FatFS文件操作
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_libc_newlib_fatfs_h__
#define __xwmd_libc_newlib_fatfs_h__

#include <stdio.h>

int newlibac_fatfs_open(struct _reent * r, const char * path, int flag, int mode);
int newlibac_fatfs_close(struct _reent * r, int fd);
_ssize_t newlibac_fatfs_read(struct _reent * r, int fd, void * buf, size_t cnt);
_ssize_t newlibac_fatfs_write(struct _reent * r, int fd, const void * data, size_t cnt);
_off_t newlibac_fatfs_lseek(struct _reent * r, int fd, _off_t pos, int whence);
int newlibac_fatfs_unlink(struct _reent * r, const char * path);
int newlibac_fatfs_rename(struct _reent * r,
                          const char * oldname, const char * newname);
int newlibac_fatfs_fstat(struct _reent * r, int fd, struct stat * sbuf);

#endif /* xwmd/libc/newlib/fatfs.h */
