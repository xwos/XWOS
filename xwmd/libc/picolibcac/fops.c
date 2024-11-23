/**
 * @file
 * @brief picolibc适配层：文件操作
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/lib/errno.h>
#include <xwos/lib/xwbop.h>
#include <xwmd/libc/picolibcac/linkage.h>
#include <xwmd/libc/picolibcac/check.h>
#include <stdio.h>
#include <fcntl.h>

#if defined(XWMDCFG_libc_picolibcac_FATFS) && (1 == XWMDCFG_libc_picolibcac_FATFS)
#  include <xwmd/libc/picolibcac/fatfs.h>
#endif

void picolibcac_fops_linkage_stub(void)
{
}

int open(const char * path, int flag, ...); // cppcheck-suppress [misra-c2012-5.8]
int open64(const char * path, int flag, ...);
int close(int fd); // cppcheck-suppress [misra-c2012-5.8]
ssize_t read(int fd, void * buf, size_t cnt);
ssize_t write(int fd, const void * data, size_t cnt);
_off_t lseek(int fd, _off_t pos, int whence);
_off64_t lseek64(int fd, _off64_t offset, int whence);
int unlink(const char * path);
// cppcheck-suppress [misra-c2012-21.2]
int rename(const char * oldname, const char * newname);
int fstat(int fd, struct stat * sbuf);

extern xwssz_t picolibcac_fops_read_stdin(int fd, void * buf, xwsz_t cnt);
extern xwssz_t picolibcac_fops_write_stdout(int fd, const void * data, xwsz_t cnt);
extern xwssz_t picolibcac_fops_write_stderr(int fd, const void * data, xwsz_t cnt);

int open(const char * path, int flag, ...)
{
        int fd;

#if defined(XWMDCFG_libc_newlibac_FATFS) && (1 == XWMDCFG_libc_newlibac_FATFS)
        va_list args;
        int mode;

        va_start(args, flag);
        mode = va_arg(args, int);
        fd = picolibcac_fatfs_open(path, flag, mode);
        va_end(args);
#else
        XWOS_UNUSED(path);
        XWOS_UNUSED(flag);
        errno = ENOSYS;
        fd = -1;
#endif
        return fd;
}

int open64(const char * path, int flag, ...)
{
        int fd;

#if defined(XWMDCFG_libc_newlibac_FATFS) && (1 == XWMDCFG_libc_newlibac_FATFS)
        va_list args;
        int mode;

        va_start(args, flag);
        mode = va_arg(args, int);
        fd = picolibcac_fatfs_open(path, flag, mode);
        va_end(args);
#else
        XWOS_UNUSED(path);
        XWOS_UNUSED(flag);
        errno = ENOSYS;
        fd = -1;
#endif
        return fd;
}

int close(int fd)
{
        int rc;

        if (fd <= 2) {
                errno = EPERM;
                rc = -1;
        } else {
#if defined(XWMDCFG_libc_picolibcac_FATFS) && (1 == XWMDCFG_libc_picolibcac_FATFS)
                rc = picolibcac_fatfs_close(fd);
#else
                errno = ENOSYS;
                rc = -1;
#endif
        }
        return rc;
}

ssize_t read(int fd, void * buf, size_t cnt)
{
        ssize_t ret;

        if (0 == fd) {
                ret = picolibcac_fops_read_stdin(fd, buf, cnt);
        } else if (fd > 2) {
#if defined(XWMDCFG_libc_picolibcac_FATFS) && (1 == XWMDCFG_libc_picolibcac_FATFS)
                ret = picolibcac_fatfs_read(fd, buf, cnt);
#else
                errno = ENOSYS;
                ret = -1;
#endif
        } else {
                errno = EPERM;
                ret = -1;
        }
        return ret;
}

ssize_t write(int fd, const void * data, size_t cnt)
{
        ssize_t ret;

        if (1 == fd) {
                ret = picolibcac_fops_write_stdout(fd, data, cnt);
        } else if (2 == fd) {
                ret = picolibcac_fops_write_stderr(fd, data, cnt);
        } else if (fd > 2) {
#if defined(XWMDCFG_libc_picolibcac_FATFS) && (1 == XWMDCFG_libc_picolibcac_FATFS)
                ret = picolibcac_fatfs_write(fd, data, cnt);
#else
                errno = ENOSYS;
                ret = -1;
#endif
        } else {
                errno = EPERM;
                ret = -1;
        }
        return ret;
}

_off_t lseek(int fd, _off_t pos, int whence)
{
        int curpos;

        if (fd <= 2) {
                errno = EPERM;
                curpos = -1;
        } else {
#if defined(XWMDCFG_libc_picolibcac_FATFS) && (1 == XWMDCFG_libc_picolibcac_FATFS)
                curpos = picolibcac_fatfs_lseek(fd, pos, whence);
#else
                errno = ENOSYS;
                curpos = -1;
#endif
        }
        return curpos;
}

_off64_t lseek64(int fd, _off64_t offset, int whence)
{
        return (_off64_t)lseek(fd, (off_t) offset, whence);
}

int unlink(const char * path)
{
        int rc;

#if defined(XWMDCFG_libc_picolibcac_FATFS) && (1 == XWMDCFG_libc_picolibcac_FATFS)
        rc = picolibcac_fatfs_unlink(path);
#else
        XWOS_UNUSED(path);
        errno = ENOSYS;
        rc = -1;
#endif
        return rc;
}

int rename(const char * oldname, const char * newname)
{
        int rc;

#if defined(XWMDCFG_libc_picolibcac_FATFS) && (1 == XWMDCFG_libc_picolibcac_FATFS)
        rc = picolibcac_fatfs_rename(oldname, newname);
#else
        XWOS_UNUSED(oldname);
        XWOS_UNUSED(newname);
        errno = ENOSYS;
        rc = -1;
#endif
        return rc;
}

int fstat(int fd, struct stat * sbuf)
{
        int rc;

        switch (fd) {
        case 0:
        case 1:
        case 2:
                errno = ENOSYS;
                rc = -1;
                break;
        default:
#if defined(XWMDCFG_libc_picolibcac_FATFS) && (1 == XWMDCFG_libc_picolibcac_FATFS)
                rc = picolibcac_fatfs_fstat(fd, sbuf);
#else
                XWOS_UNUSED(fd);
                XWOS_UNUSED(sbuf);
                errno = ENOSYS;
                rc = -1;
#endif
                break;
        }
	return rc;
}
