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

#include <xwos/standard.h>
#include <xwos/lib/errno.h>
#include <xwos/lib/xwbop.h>
#include <xwem/fs/fatfs/ff.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>

#define NEWLIBAC_FATFS_FD_NUM           32U
#define NEWLIBAC_FATFS_FD_OFFSET        3 /* 0,1,2 分别代表 stdin, stdout, stderr. */

static xwbmpop_define(newlibac_fatfs_node_bmp, NEWLIBAC_FATFS_FD_NUM) = {0U};
static xwbmpop_define(newlibac_fatfs_nodetype_bmp, NEWLIBAC_FATFS_FD_NUM) = {0U};
// cppcheck-suppress [misra-c2012-9.3]
static void * newlibac_fatfs_fatfs_node[NEWLIBAC_FATFS_FD_NUM] = {NULL};
// cppcheck-suppress [misra-c2012-9.2, misra-c2012-9.3]
static FILINFO * newlibac_fatfs_fatfs_filinfo[NEWLIBAC_FATFS_FD_NUM] = {NULL};

#ifdef O_DIRECTORY
static
int newlibac_fatfs_opendir(struct _reent * r, const char * path, int flag, int mode);
static
int newlibac_fatfs_closedir(struct _reent * r, xwsq_t idx);
#endif

static
int newlibac_fatfs_openfile(struct _reent * r, const char * path, int oflag, int mode);
static
int newlibac_fatfs_closefile(struct _reent * r, xwsq_t idx);
static
xwssz_t newlibac_fatfs_read_file(struct _reent * r, int fd, void * buf, size_t cnt);
static
xwssz_t newlibac_fatfs_write_file(struct _reent * r,
                                  int fd, const void * data, size_t cnt);


#ifdef O_DIRECTORY
static
int newlibac_fatfs_opendir(struct _reent * r, const char * path, int flag, int mode)
{
        int fd;
        FRESULT fsrc;
        DIR * dp;
        xwssq_t idx;

        XWOS_UNUSED(r);
        XWOS_UNUSED(flag);
        XWOS_UNUSED(mode);

        idx = xwbmpop_ffz(newlibac_fatfs_node_bmp, NEWLIBAC_FATFS_FD_NUM);
        if (idx < 0) {
                errno = EMFILE;
                fd = -1;
                goto err_mdir;
        }
        dp = malloc(sizeof(DIR));
        if (NULL == dp) {
                errno = ENOMEM;
                fd = -1;
                goto err_nomem;
        }

        fsrc = f_opendir(dp, path);
        switch (fsrc) {
        case FR_OK:
                xwbmpop_s1i(newlibac_fatfs_node_bmp, (xwsq_t)idx);
                xwbmpop_s1i(newlibac_fatfs_nodetype_bmp, (xwsq_t)idx);
                newlibac_fatfs_fatfs_node[idx] = dp;
                fd = idx + NEWLIBAC_FATFS_FD_OFFSET;
                errno = 0;
                break;
        case FR_NO_FILE:
        case FR_NO_PATH:
                errno = ENOENT;
                fd = -1;
                break;
        case FR_INVALID_NAME:
        case FR_INVALID_OBJECT:
        case FR_INVALID_PARAMETER:
                errno = EINVAL;
                fd = -1;
                break;
        case FR_DENIED:
        case FR_LOCKED:
                errno = EACCES;
                fd = -1;
                break;
        case FR_EXIST:
                errno = EEXIST;
                fd = -1;
                break;
        case FR_WRITE_PROTECTED:
                errno = EROFS;
                fd = -1;
                break;
        case FR_NOT_ENABLED:
                errno = ENOSPC;
                fd = -1;
                break;
        case FR_NO_FILESYSTEM:
        case FR_INVALID_DRIVE:
                errno = ENODEV;
                fd = -1;
                break;
        case FR_TIMEOUT:
                errno = EINTR;
                fd = -1;
                break;
        case FR_NOT_ENOUGH_CORE:
                errno = ENOMEM;
                fd = -1;
                break;
        case FR_TOO_MANY_OPEN_FILES:
                errno = EMFILE;
                fd = -1;
                break;
        case FR_DISK_ERR:
        case FR_INT_ERR:
        case FR_NOT_READY:
        default:
                errno = EIO;
                fd = -1;
                break;
        }
        if (fd < 0) {
                free(dp);
        }

err_nomem:
err_mdir:
        return fd;
}
#endif

static
int newlibac_fatfs_openfile(struct _reent * r, const char * path, int oflag, int mode)
{
        int fd;
        FRESULT fsrc;
        FIL * fp;
        FILINFO * fi;
        BYTE fsmode;
        xwssq_t idx;
        int flag;

        XWOS_UNUSED(r);
        XWOS_UNUSED(mode);

        flag = oflag;
        idx = xwbmpop_ffz(newlibac_fatfs_node_bmp, NEWLIBAC_FATFS_FD_NUM);
        if (idx < 0) {
                errno = EMFILE;
                fd = -1;
                goto err_mfile;
        }
        fp = malloc(sizeof(FIL));
        if (NULL == fp) {
                errno = ENOMEM;
                fd = -1;
                goto err_malloc_fp;
        }
        fi = malloc(sizeof(FILINFO));
        if (NULL == fi) {
                errno = ENOMEM;
                fd = -1;
                goto err_malloc_fi;
        }

        fsmode = 0;
        if (O_RDONLY == (flag & O_ACCMODE)) { /* r */
                fsmode = FA_READ;
                flag &= ~(O_ACCMODE);
        } else if (O_WRONLY == (flag & O_ACCMODE)) {
                fsmode = FA_WRITE;
                flag &= ~(O_ACCMODE);
                if (0 != (flag & O_EXCL)) { /* wx */
                        fsmode |= FA_CREATE_NEW;
                } else if ((flag & (O_CREAT | O_TRUNC)) ==
                           ((O_CREAT | O_TRUNC))) { /* w */
                        fsmode |= FA_CREATE_ALWAYS;
                } else if ((flag & (O_CREAT | O_APPEND)) ==
                           ((O_CREAT | O_APPEND))) { /* "a" */
                        fsmode |= FA_OPEN_APPEND;
                } else {
                }
        } else { /* + */
                fsmode = FA_READ | FA_WRITE;
                flag &= ~(O_ACCMODE);
                if (0 != (flag & O_EXCL)) { /* w+x */
                        fsmode |= FA_CREATE_NEW;
                } else if ((flag & (O_CREAT | O_TRUNC)) ==
                           ((O_CREAT | O_TRUNC))) { /* w+ */
                        fsmode |= FA_CREATE_ALWAYS;
                } else if ((flag & (O_CREAT | O_APPEND)) ==
                           ((O_CREAT | O_APPEND))) { /* "a+" */
                        fsmode |= FA_OPEN_APPEND;
                } else { /* r+ */
                }
        }
        fsrc = f_open(fp, path, fsmode);
        switch (fsrc) {
        case FR_OK:
                xwbmpop_s1i(newlibac_fatfs_node_bmp, (xwsq_t)idx);
                xwbmpop_c0i(newlibac_fatfs_nodetype_bmp, (xwsq_t)idx);
                newlibac_fatfs_fatfs_node[idx] = fp;
                fd = (int)idx + NEWLIBAC_FATFS_FD_OFFSET;
                errno = 0;
                break;
        case FR_NO_FILE:
        case FR_NO_PATH:
                errno = ENOENT;
                fd = -1;
                break;
        case FR_INVALID_NAME:
        case FR_INVALID_OBJECT:
        case FR_INVALID_PARAMETER:
                errno = EINVAL;
                fd = -1;
                break;
        case FR_DENIED:
        case FR_LOCKED:
                errno = EACCES;
                fd = -1;
                break;
        case FR_EXIST:
                errno = EEXIST;
                fd = -1;
                break;
        case FR_WRITE_PROTECTED:
                errno = EROFS;
                fd = -1;
                break;
        case FR_NOT_ENABLED:
                errno = ENOSPC;
                fd = -1;
                break;
        case FR_NO_FILESYSTEM:
        case FR_INVALID_DRIVE:
                errno = ENODEV;
                fd = -1;
                break;
        case FR_TIMEOUT:
                errno = EINTR;
                fd = -1;
                break;
        case FR_NOT_ENOUGH_CORE:
                errno = ENOMEM;
                fd = -1;
                break;
        case FR_TOO_MANY_OPEN_FILES:
                errno = EMFILE;
                fd = -1;
                break;
        case FR_DISK_ERR:
        case FR_INT_ERR:
        case FR_NOT_READY:
        default:
                errno = EIO;
                fd = -1;
                break;
        }
        if (fd < 0) {
                goto err_fopen;
        }

        fsrc = f_stat(path, fi);
        if (FR_OK == fsrc) {
                newlibac_fatfs_fatfs_filinfo[idx] = fi;
        } else {
                newlibac_fatfs_fatfs_filinfo[idx] = NULL;
        }
        return fd;

err_fopen:
        free(fi);
err_malloc_fi:
        free(fp);
err_malloc_fp:
err_mfile:
        return fd;
}

int newlibac_fatfs_open(struct _reent * r, const char * path, int flag, int mode)
{
        int fd;

#ifdef O_DIRECTORY
        if (mode & O_DIRECTORY) {
                fd = newlibac_fatfs_opendir(r, path, flag, mode);
        } else {
                fd = newlibac_fatfs_openfile(r, path, flag, mode);
        }
#else
        fd = newlibac_fatfs_openfile(r, path, flag, mode);
#endif
        return fd;
}

#ifdef O_DIRECTORY
static
int newlibac_fatfs_closedir(struct _reent * r, xwsq_t idx)
{
        int rc;
        FRESULT fsrc;
        DIR * dp;
        XWOS_UNUSED(r);

        dp = newlibac_fatfs_fatfs_node[idx];
        fsrc = f_closedir(dp);
        switch (fsrc) {
        case FR_OK:
                free(dp);
                newlibac_fatfs_fatfs_node[idx] = NULL;
                xwbmpop_c0i(newlibac_fatfs_node_bmp, (xwsq_t)idx);
                errno = XWOK;
                rc = 0;
                break;
        case FR_INVALID_OBJECT:
                errno = EINVAL;
                rc = -1;
                break;
        case FR_TIMEOUT:
                errno = EINTR;
                rc = -1;
                break;
        case FR_DISK_ERR:
        case FR_INT_ERR:
        default:
                errno = EIO;
                rc = -1;
                break;
        }
        return rc;
}
#endif

static
int newlibac_fatfs_closefile(struct _reent * r, xwsq_t idx)
{
        int rc;
        FRESULT fsrc;
        FIL * fp;
        FILINFO * fi;
        XWOS_UNUSED(r);

        fp = newlibac_fatfs_fatfs_node[idx];
        fi = newlibac_fatfs_fatfs_filinfo[idx];
        fsrc = f_close(fp);
        switch (fsrc) {
        case FR_OK:
                if (NULL != fi) {
                        free(fi);
                        newlibac_fatfs_fatfs_filinfo[idx] = NULL;
                }
                free(fp);
                newlibac_fatfs_fatfs_node[idx] = NULL;
                xwbmpop_c0i(newlibac_fatfs_node_bmp, idx);
                errno = XWOK;
                rc = 0;
                break;
        case FR_INVALID_OBJECT:
                errno = EINVAL;
                rc = -1;
                break;
        case FR_TIMEOUT:
                errno = EINTR;
                rc = -1;
                break;
        case FR_DISK_ERR:
        case FR_INT_ERR:
        default:
                errno = EIO;
                rc = -1;
                break;
        }
        return rc;
}

int newlibac_fatfs_close(struct _reent * r, int fd)
{
        int rc;
        xwssq_t idx;

        if (fd >= ((int)NEWLIBAC_FATFS_FD_NUM + (int)NEWLIBAC_FATFS_FD_OFFSET)) {
                errno = ENFILE;
                rc = -1;
        } else {
#ifdef O_DIRECTORY
                bool dir;

                idx = (xwssq_t)fd - (xwssq_t)NEWLIBAC_FATFS_FD_OFFSET;
                dir = xwbmpop_t1i(newlibac_fatfs_nodetype_bmp, (xwsq_t)idx);
                if (dir) {
                        rc = newlibac_fatfs_closedir(r, (xwsq_t)idx);
                } else {
                        rc = newlibac_fatfs_closefile(r, (xwsq_t)idx);
                }
#else
                idx = (xwssq_t)fd - (xwssq_t)NEWLIBAC_FATFS_FD_OFFSET;
                rc = newlibac_fatfs_closefile(r, (xwsq_t)idx);
#endif
        }
        return rc;
}

static
xwssz_t newlibac_fatfs_read_file(struct _reent * r, int fd, void * buf, size_t cnt)
{
        xwssz_t rdsz;
        xwssq_t idx;
        FRESULT fsrc;
        UINT fsrd;
        FIL * fp;
        XWOS_UNUSED(r);

        idx = (xwssq_t)fd - (xwssq_t)NEWLIBAC_FATFS_FD_OFFSET;
        fp = newlibac_fatfs_fatfs_node[idx];
        fsrc = f_read(fp, buf, cnt, &fsrd);
        switch (fsrc) {
        case FR_OK:
                errno = 0;
                rdsz = (xwssz_t)fsrd;
                break;
        case FR_INVALID_OBJECT:
                errno = EINVAL;
                rdsz = -1;
                break;
        case FR_TIMEOUT:
                errno = EINTR;
                rdsz = -1;
                break;
        case FR_DENIED:
                errno = EACCES;
                rdsz = -1;
                break;
        case FR_DISK_ERR:
        case FR_INT_ERR:
        default:
                errno = EIO;
                rdsz = -1;
                break;
        }
        return rdsz;
}

_ssize_t newlibac_fatfs_read(struct _reent * r, int fd, void * buf, size_t cnt)
{
        _ssize_t ret;

        if (fd >= ((int)NEWLIBAC_FATFS_FD_NUM + (int)NEWLIBAC_FATFS_FD_OFFSET)) {
                errno = EPERM;
                ret = -1;
        } else {
                ret = (_ssize_t)newlibac_fatfs_read_file(r, fd, buf, cnt);
        }
        return ret;
}

static
xwssz_t newlibac_fatfs_write_file(struct _reent * r,
                                  int fd, const void * data, size_t cnt)
{
        xwssz_t wrsz;
        xwssq_t idx;
        FRESULT fsrc;
        UINT fswr;
        FIL * fp;
        XWOS_UNUSED(r);

        idx = (xwssq_t)fd - (xwssq_t)NEWLIBAC_FATFS_FD_OFFSET;
        fp = newlibac_fatfs_fatfs_node[idx];
        fsrc = f_write(fp, data, cnt, &fswr);
        switch (fsrc) {
        case FR_OK:
                errno = 0;
                wrsz = (xwssz_t)fswr;
                break;
        case FR_INVALID_OBJECT:
                errno = EINVAL;
                wrsz = -1;
                break;
        case FR_TIMEOUT:
                errno = EINTR;
                wrsz = -1;
                break;
        case FR_DENIED:
                errno = EACCES;
                wrsz = -1;
                break;
        case FR_DISK_ERR:
        case FR_INT_ERR:
        default:
                errno = EIO;
                wrsz = -1;
                break;
        }
        return wrsz;
}

_ssize_t newlibac_fatfs_write(struct _reent * r, int fd, const void * data, size_t cnt)
{
        _ssize_t ret;

        if (fd >= ((int)NEWLIBAC_FATFS_FD_NUM + (int)NEWLIBAC_FATFS_FD_OFFSET)) {
                errno = EPERM;
                ret = -1;
        } else {
                ret = (_ssize_t)newlibac_fatfs_write_file(r, fd, data, cnt);
        }
        return ret;
}

_off_t newlibac_fatfs_lseek(struct _reent * r, int fd, _off_t pos, int whence)
{
        xwssq_t idx;
        FRESULT fsrc;
        FIL * fp;
        _off_t curpos;
        XWOS_UNUSED(r);

        if (fd >= ((int)NEWLIBAC_FATFS_FD_NUM + (int)NEWLIBAC_FATFS_FD_OFFSET)) {
                errno = EPERM;
                curpos = -1;
        } else {
                idx = (xwssq_t)fd - (xwssq_t)NEWLIBAC_FATFS_FD_OFFSET;
                fp = newlibac_fatfs_fatfs_node[idx];
                switch (whence) {
                case SEEK_SET:
                        fsrc = f_lseek(fp, (FSIZE_t)pos);
                        break;
                case SEEK_CUR:
                        fsrc = f_lseek(fp, f_tell(fp) + (FSIZE_t)pos);
                        break;
                case SEEK_END:
                        fsrc = f_lseek(fp, f_size(fp) + (FSIZE_t)pos);
                        break;
                default:
                        fsrc = FR_INVALID_PARAMETER;
                        break;
                }
                switch (fsrc) {
                case FR_OK:
                        curpos = (_off_t)f_tell(fp);
                        errno = 0;
                        break;
                case FR_INVALID_PARAMETER:
                case FR_INVALID_OBJECT:
                        curpos = -1;
                        errno = EINVAL;
                        break;
                case FR_TIMEOUT:
                        curpos = -1;
                        errno = EINTR;
                        break;
                case FR_DISK_ERR:
                case FR_INT_ERR:
                default:
                        curpos = -1;
                        errno = EIO;
                        break;
                }
        }
        return curpos;
}

int newlibac_fatfs_unlink(struct _reent * r, const char * path)
{
        FRESULT fsrc;
        int rc;
        XWOS_UNUSED(r);

        rc = -1;
        fsrc = f_unlink(path);
        switch (fsrc) {
        case FR_OK:
                errno = XWOK;
                rc = 0;
                break;
        case FR_DISK_ERR:
        case FR_INT_ERR:
        case FR_NOT_ENABLED:
        case FR_NOT_READY:
                errno = EIO;
                break;
        case FR_NO_FILE:
        case FR_NO_PATH:
        case FR_INVALID_NAME:
                errno = ENOENT;
                break;
        case FR_DENIED:
                errno = EPERM;
                break;
        case FR_WRITE_PROTECTED:
        case FR_INVALID_DRIVE:
        case FR_NO_FILESYSTEM:
                errno = EACCES;
                break;
        case FR_TIMEOUT:
                errno = ETIMEDOUT;
                break;
        case FR_LOCKED:
                errno = EBUSY;
                break;
        case FR_NOT_ENOUGH_CORE:
                errno = ENOMEM;
                break;
        default:
                errno = EFAULT;
                break;
        }
        return rc;
}

int newlibac_fatfs_rename(struct _reent * r,
                          const char * oldname, const char * newname)
{
        FRESULT fsrc;
        int rc;

        XWOS_UNUSED(r);

        rc = -1;
        fsrc = f_rename(oldname, newname);
        switch (fsrc) {
        case FR_OK:
                errno = XWOK;
                rc = 0;
                break;
        case FR_DISK_ERR:
        case FR_INT_ERR:
        case FR_NOT_ENABLED:
        case FR_NOT_READY:
                errno = EIO;
                break;
        case FR_NO_FILE:
        case FR_NO_PATH:
        case FR_INVALID_NAME:
                errno = ENOENT;
                break;
        case FR_DENIED:
                errno = EPERM;
                break;
        case FR_WRITE_PROTECTED:
        case FR_INVALID_DRIVE:
        case FR_NO_FILESYSTEM:
                errno = EACCES;
                break;
        case FR_TIMEOUT:
                errno = ETIMEDOUT;
                break;
        case FR_LOCKED:
                errno = EBUSY;
                break;
        case FR_NOT_ENOUGH_CORE:
                errno = ENOMEM;
                break;
        default:
                errno = EFAULT;
                break;
        }
        return rc;
}

int newlibac_fatfs_fstat(struct _reent * r, int fd, struct stat * sbuf)
{
        FILINFO * fi;
        xwssq_t idx;
        int rc;

        XWOS_UNUSED(r);

        if (fd >= ((int)NEWLIBAC_FATFS_FD_NUM + (int)NEWLIBAC_FATFS_FD_OFFSET)) {
                errno = EPERM;
                rc = -1;
        } else {
                idx = (xwssq_t)fd - (xwssq_t)NEWLIBAC_FATFS_FD_OFFSET;
                fi = newlibac_fatfs_fatfs_filinfo[idx];
                if (NULL != fi) {
                        struct tm tm;
                        time_t sec;

                        sbuf->st_nlink = 1;
                        sbuf->st_uid = 0;
                        sbuf->st_gid = 0;
                        sbuf->st_mode = (mode_t)(S_IFREG | S_IRWXU |
                                                 S_IRWXG | S_IRWXO);
                        if ((xwu8_t)0 != ((xwu8_t)AM_RDO & fi->fattrib)) {
                                sbuf->st_mode &= (mode_t)(~(S_IRUSR |
                                                            S_IRGRP |
                                                            S_IROTH));
                        }
                        tm.tm_sec = (fi->ftime & 0x1FU) << 1U;
                        tm.tm_min = (fi->ftime >> 5U) & 0x3FU;
                        tm.tm_hour = (fi->ftime >> 11U) & 0xFU;
                        tm.tm_mday = fi->fdate & 0x1FU;
                        tm.tm_mon = (fi->fdate >> 5U) & 0xFU;
                        tm.tm_year = (fi->fdate >> 9U) & 0x7FU;
                        tm.tm_wday = 0;
                        tm.tm_yday = 0;
                        tm.tm_isdst = 0;
                        sec = mktime(&tm);
                        sbuf->st_mtim.tv_sec = sec;
                        sbuf->st_mtim.tv_nsec = 0;
                        sbuf->st_atim.tv_sec = sec;
                        sbuf->st_atim.tv_nsec = 0;
                        sbuf->st_ctim.tv_sec = sec;
                        sbuf->st_ctim.tv_nsec = 0;
                        sbuf->st_size = (off_t)fi->fsize;
                        errno = XWOK;
                        rc = 0;
                } else {
                        errno = ENOSYS;
                        rc = -1;
                }
        }
	return rc;
}
