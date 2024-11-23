/**
 * @file
 * @brief picolibc适配层：FatFS文件操作
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_libc_picolibcac_fatfs_h__
#define __xwmd_libc_picolibcac_fatfs_h__

#include <stdio.h>

int picolibcac_fatfs_open(const char * path, int flag, int mode);
int picolibcac_fatfs_close(int fd);
_ssize_t picolibcac_fatfs_read(int fd, void * buf, size_t cnt);
_ssize_t picolibcac_fatfs_write(int fd, const void * data, size_t cnt);
_off_t picolibcac_fatfs_lseek(int fd, _off_t pos, int whence);
int picolibcac_fatfs_unlink(const char * path);
int picolibcac_fatfs_rename(const char * oldname, const char * newname);
int picolibcac_fatfs_fstat(int fd, struct stat * sbuf);

#endif /* xwmd/libc/picolibcac/fatfs.h */
