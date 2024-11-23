/**
 * @file
 * @brief newlib适配层：文件操作
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
#include <xwmd/libc/newlibac/linkage.h>
#include <xwmd/libc/newlibac/check.h>
#include <stdio.h>
#include <fcntl.h>

#if defined(XWMDCFG_libc_newlibac_FATFS) && (1 == XWMDCFG_libc_newlibac_FATFS)
#  include <xwmd/libc/newlibac/fatfs.h>
#endif

void newlibac_fops_linkage_stub(void)
{
}

int _open_r(struct _reent * r, const char * path, int flag, int mode);
int _open64_r(struct _reent * r, const char * path, int flag, int mode);
int _close_r(struct _reent * r, int fd);
_ssize_t _read_r(struct _reent * r, int fd, void * buf, size_t cnt);
_ssize_t _write_r(struct _reent * r, int fd, const void * data, size_t cnt);
_off_t _lseek_r(struct _reent * r, int fd, _off_t pos, int whence);
_off64_t _lseek64_r(struct _reent * r, int fd, _off64_t offset, int whence);
int _unlink_r(struct _reent * r, const char * path);
int _rename_r(struct _reent * r, const char * oldname, const char * newname);
int _fstat_r(struct _reent * r, int fd, struct stat * sbuf);

extern xwssz_t newlibac_fops_read_stdin(int fd, void * buf, xwsz_t cnt);
extern xwssz_t newlibac_fops_write_stdout(int fd, const void * data, xwsz_t cnt);
extern xwssz_t newlibac_fops_write_stderr(int fd, const void * data, xwsz_t cnt);

int _open_r(struct _reent * r, const char * path, int flag, int mode)
{
        int fd;
#if defined(XWMDCFG_libc_newlibac_FATFS) && (1 == XWMDCFG_libc_newlibac_FATFS)
        fd = newlibac_fatfs_open(r, path, flag, mode);
#else
        XWOS_UNUSED(r);
        XWOS_UNUSED(path);
        XWOS_UNUSED(flag);
        XWOS_UNUSED(mode);
        errno = ENOSYS;
        fd = -1;
#endif
        return fd;
}

int _open64_r(struct _reent * r, const char * path, int flag, int mode)
{
        return _open_r(r, path, flag, mode);
}

int _close_r(struct _reent * r, int fd)
{
        int rc;

        if (fd <= 2) {
                errno = EPERM;
                rc = -1;
        } else {
#if defined(XWMDCFG_libc_newlibac_FATFS) && (1 == XWMDCFG_libc_newlibac_FATFS)
                rc = newlibac_fatfs_close(r, fd);
#else
                XWOS_UNUSED(r);
                errno = ENOSYS;
                rc = -1;
#endif
        }
        return rc;
}

_ssize_t _read_r(struct _reent * r, int fd, void * buf, size_t cnt)
{
        _ssize_t ret;
        XWOS_UNUSED(r);
        XWOS_UNUSED(fd);
        XWOS_UNUSED(buf);
        XWOS_UNUSED(cnt);

        if (0 == fd) {
                ret = newlibac_fops_read_stdin(fd, buf, cnt);
        } else if (fd > 2) {
#if defined(XWMDCFG_libc_newlibac_FATFS) && (1 == XWMDCFG_libc_newlibac_FATFS)
                ret = newlibac_fatfs_read(r, fd, buf, cnt);
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

_ssize_t _write_r(struct _reent * r, int fd, const void * data, size_t cnt)
{
        _ssize_t ret;
        XWOS_UNUSED(r);

        if (1 == fd) {
                ret = newlibac_fops_write_stdout(fd, data, cnt);
        } else if (2 == fd) {
                ret = newlibac_fops_write_stderr(fd, data, cnt);
        } else if (fd > 2) {
#if defined(XWMDCFG_libc_newlibac_FATFS) && (1 == XWMDCFG_libc_newlibac_FATFS)
                ret = newlibac_fatfs_write(r, fd, data, cnt);
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

_off_t _lseek_r(struct _reent * r, int fd, _off_t pos, int whence)
{
        _off_t curpos;
        XWOS_UNUSED(r);
        XWOS_UNUSED(fd);
        XWOS_UNUSED(pos);
        XWOS_UNUSED(whence);

        if (fd <= 2) {
                errno = EPERM;
                curpos = -1;
        } else {
#if defined(XWMDCFG_libc_newlibac_FATFS) && (1 == XWMDCFG_libc_newlibac_FATFS)
                curpos = newlibac_fatfs_lseek(r, fd, pos, whence);
#else
                errno = ENOSYS;
                curpos = -1;
#endif
        }
        return curpos;
}

_off64_t _lseek64_r(struct _reent * r, int fd, _off64_t offset, int whence)
{
        return (_off64_t)_lseek_r(r, fd, (off_t)offset, whence);
}

int _unlink_r(struct _reent * r, const char * path)
{
        int rc;

#if defined(XWMDCFG_libc_newlibac_FATFS) && (1 == XWMDCFG_libc_newlibac_FATFS)
        rc = newlibac_fatfs_unlink(r, path);
#else
        XWOS_UNUSED(r);
        XWOS_UNUSED(path);
        errno = ENOSYS;
        rc = -1;
#endif
        return rc;
}

int _rename_r(struct _reent * r, const char * oldname, const char * newname)
{
        int rc;

#if defined(XWMDCFG_libc_newlibac_FATFS) && (1 == XWMDCFG_libc_newlibac_FATFS)
        rc = newlibac_fatfs_rename(r, oldname, newname);
#else
        XWOS_UNUSED(r);
        XWOS_UNUSED(oldname);
        XWOS_UNUSED(newname);
        errno = ENOSYS;
        rc = -1;
#endif
        return rc;
}

int _fstat_r(struct _reent * r, int fd, struct stat * sbuf)
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
#if defined(XWMDCFG_libc_newlibac_FATFS) && (1 == XWMDCFG_libc_newlibac_FATFS)
                rc = newlibac_fatfs_fstat(r, fd, sbuf);
#else
                XWOS_UNUSED(r);
                XWOS_UNUSED(fd);
                XWOS_UNUSED(sbuf);
                errno = ENOSYS;
                rc = -1;
#endif
                break;
        }
	return rc;
}
