/**
 * @file
 * @brief newlib适配代码：文件操作
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/lib/errno.h>
#include <xwos/lib/xwbop.h>
#include <xwem/fs/fatfs/ff.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>

#undef errno
#define NEWLIBAC_FOPS_FD_NUM         32
#define NEWLIBAC_FOPS_FD_OFFSET      3 /* 0,1,2 分别代表 stdin, stdout, stderr. */

extern
xwssz_t newlibac_fops_read_stdin(int fd, void * buf, size_t cnt);

extern
xwssz_t newlibac_fops_write_stdout(int fd, const void * data, size_t cnt);

extern
xwssz_t newlibac_fops_write_stderr(int fd, const void * data, size_t cnt);

xwbmpop_declare(newlibac_fops_node_bmp, NEWLIBAC_FOPS_FD_NUM) = {0};
xwbmpop_declare(newlibac_fops_nodetype_bmp, NEWLIBAC_FOPS_FD_NUM) = {0};
void * newlibac_fops_fatfs_node[NEWLIBAC_FOPS_FD_NUM] = {NULL};

void newlibac_fops_init(void)
{
}

#ifdef O_DIRECTORY
static
int newlibac_fops_opendir(struct _reent * r, const char * path, int flag, int mode)
{
        int fd;
        FRESULT fsrc;
        DIR * dp;
        xwssq_t idx;

        XWOS_UNUSED(flag);
        XWOS_UNUSED(mode);

        idx = xwbmpop_ffz(newlibac_fops_node_bmp, NEWLIBAC_FOPS_FD_NUM);
        if (idx < 0) {
                r->_errno = EMFILE;
                fd = -1;
                goto err_mdir;
        }
        dp = malloc(sizeof(DIR));
        if (NULL == dp) {
                r->_errno = ENOMEM;
                fd = -1;
                goto err_nomem;
        }

        fsrc = f_opendir(dp, path);
        switch (fsrc) {
        case FR_OK:
                xwbmpop_s1i(newlibac_fops_node_bmp, (xwsq_t)idx);
                xwbmpop_s1i(newlibac_fops_nodetype_bmp, (xwsq_t)idx);
                newlibac_fops_fatfs_node[idx] = dp;
                fd = idx + NEWLIBAC_FOPS_FD_OFFSET;
                r->_errno = 0;
                break;
        case FR_NO_FILE:
        case FR_NO_PATH:
                r->_errno = ENOENT;
                fd = -1;
                break;
        case FR_INVALID_NAME:
        case FR_INVALID_OBJECT:
        case FR_INVALID_PARAMETER:
                r->_errno = EINVAL;
                fd = -1;
                break;
        case FR_DENIED:
        case FR_LOCKED:
                r->_errno = EACCES;
                fd = -1;
                break;
        case FR_EXIST:
                r->_errno = EEXIST;
                fd = -1;
                break;
        case FR_WRITE_PROTECTED:
                r->_errno = EROFS;
                fd = -1;
                break;
        case FR_NOT_ENABLED:
                r->_errno = ENOSPC;
                fd = -1;
                break;
        case FR_NO_FILESYSTEM:
        case FR_INVALID_DRIVE:
                r->_errno = ENODEV;
                fd = -1;
                break;
        case FR_TIMEOUT:
                r->_errno = EINTR;
                fd = -1;
                break;
        case FR_NOT_ENOUGH_CORE:
                r->_errno = ENOMEM;
                fd = -1;
                break;
        case FR_TOO_MANY_OPEN_FILES:
                r->_errno = EMFILE;
                fd = -1;
                break;
        case FR_DISK_ERR:
        case FR_INT_ERR:
        case FR_NOT_READY:
        default:
                r->_errno = EIO;
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
#endif /* O_DIRECTORY */

int newlibac_fops_openfile(struct _reent * r, const char * path, int flag, int mode)
{
        int fd;
        FRESULT fsrc;
        FIL * fp;
        BYTE fsmode;
        xwssq_t idx;

        XWOS_UNUSED(mode);

        idx = xwbmpop_ffz(newlibac_fops_node_bmp, NEWLIBAC_FOPS_FD_NUM);
        if (idx < 0) {
                r->_errno = EMFILE;
                fd = -1;
                goto err_mfile;
        }
        fp = malloc(sizeof(FIL));
        if (NULL == fp) {
                r->_errno = ENOMEM;
                fd = -1;
                goto err_nomem;
        }

        fsmode = 0;
        if (O_RDONLY == (flag & O_ACCMODE)) { /* r */
                fsmode = FA_READ;
                flag &= ~(O_ACCMODE);
        } else if (O_WRONLY == (flag & O_ACCMODE)) {
                fsmode = FA_WRITE;
                flag &= ~(O_ACCMODE);
                if (flag & O_EXCL) { /* wx */
                        fsmode |= FA_CREATE_NEW;
                } else if ((flag & (O_CREAT | O_TRUNC)) ==
                           ((O_CREAT | O_TRUNC))) { /* w */
                        fsmode |= FA_CREATE_ALWAYS;
                } else if ((flag & (O_CREAT | O_APPEND)) ==
                           ((O_CREAT | O_APPEND))) { /* "a" */
                        fsmode |= FA_OPEN_APPEND;
                }
        } else { /* + */
                fsmode = FA_READ | FA_WRITE;
                flag &= ~(O_ACCMODE);
                if (flag & O_EXCL) { /* w+x */
                        fsmode |= FA_CREATE_NEW;
                } else if ((flag & (O_CREAT | O_TRUNC)) ==
                           ((O_CREAT | O_TRUNC))) { /* w+ */
                        fsmode |= FA_CREATE_ALWAYS;
                } else if ((flag & (O_CREAT | O_APPEND)) ==
                           ((O_CREAT | O_APPEND))) { /* "a+" */
                        fsmode |= FA_OPEN_APPEND;
                }/* else { r+ } */
        }
        fsrc = f_open(fp, path, fsmode);
        switch (fsrc) {
        case FR_OK:
                xwbmpop_s1i(newlibac_fops_node_bmp, (xwsq_t)idx);
                xwbmpop_c0i(newlibac_fops_nodetype_bmp, (xwsq_t)idx);
                newlibac_fops_fatfs_node[idx] = fp;
                fd = idx + NEWLIBAC_FOPS_FD_OFFSET;
                r->_errno = 0;
                break;
        case FR_NO_FILE:
        case FR_NO_PATH:
                r->_errno = ENOENT;
                fd = -1;
                break;
        case FR_INVALID_NAME:
        case FR_INVALID_OBJECT:
        case FR_INVALID_PARAMETER:
                r->_errno = EINVAL;
                fd = -1;
                break;
        case FR_DENIED:
        case FR_LOCKED:
                r->_errno = EACCES;
                fd = -1;
                break;
        case FR_EXIST:
                r->_errno = EEXIST;
                fd = -1;
                break;
        case FR_WRITE_PROTECTED:
                r->_errno = EROFS;
                fd = -1;
                break;
        case FR_NOT_ENABLED:
                r->_errno = ENOSPC;
                fd = -1;
                break;
        case FR_NO_FILESYSTEM:
        case FR_INVALID_DRIVE:
                r->_errno = ENODEV;
                fd = -1;
                break;
        case FR_TIMEOUT:
                r->_errno = EINTR;
                fd = -1;
                break;
        case FR_NOT_ENOUGH_CORE:
                r->_errno = ENOMEM;
                fd = -1;
                break;
        case FR_TOO_MANY_OPEN_FILES:
                r->_errno = EMFILE;
                fd = -1;
                break;
        case FR_DISK_ERR:
        case FR_INT_ERR:
        case FR_NOT_READY:
        default:
                r->_errno = EIO;
                fd = -1;
                break;
        }
        if (fd < 0) {
                free(fp);
        }

err_nomem:
err_mfile:
        return fd;
}

int _open_r(struct _reent * r, const char * path, int flag, int mode)
{
        int fd;

#ifdef O_DIRECTORY
        if (mode & O_DIRECTORY) {
                fd = newlibac_fops_opendir(r, path, flag, mode);
        } else {
                fd = newlibac_fops_openfile(r, path, flag, mode);
        }
#else /* O_DIRECTORY */
        fd = newlibac_fops_openfile(r, path, flag, mode);
#endif /* !O_DIRECTORY */
        return fd;
}

#ifdef O_DIRECTORY
static
int newlibac_fops_closedir(struct _reent * r, xwsq_t idx)
{
        int rc;
        FRESULT fsrc;
        DIR * dp;

        dp = newlibac_fops_fatfs_node[idx];
        fsrc = f_closedir(dp);
        switch (fsrc) {
        case FR_OK:
                free(dp);
                newlibac_fops_fatfs_node[idx] = NULL;
                xwbmpop_c0i(newlibac_fops_node_bmp, (xwsq_t)idx);
                r->_errno = XWOK;
                rc = 0;
                break;
        case FR_INVALID_OBJECT:
                r->_errno = EINVAL;
                rc = -1;
                break;
        case FR_TIMEOUT:
                r->_errno = EINTR;
                rc = -1;
                break;
        case FR_DISK_ERR:
        case FR_INT_ERR:
        default:
                r->_errno = EIO;
                rc = -1;
                break;
        }
        return rc;
}
#endif /* O_DIRECTORY */

static
int newlibac_fops_closefile(struct _reent * r, xwsq_t idx)
{
        int rc;
        FRESULT fsrc;
        FIL * fp;

        fp = newlibac_fops_fatfs_node[idx];
        fsrc = f_close(fp);
        switch (fsrc) {
        case FR_OK:
                free(fp);
                newlibac_fops_fatfs_node[idx] = NULL;
                xwbmpop_c0i(newlibac_fops_node_bmp, idx);
                r->_errno = XWOK;
                rc = 0;
                break;
        case FR_INVALID_OBJECT:
                r->_errno = EINVAL;
                rc = -1;
                break;
        case FR_TIMEOUT:
                r->_errno = EINTR;
                rc = -1;
                break;
        case FR_DISK_ERR:
        case FR_INT_ERR:
        default:
                r->_errno = EIO;
                rc = -1;
                break;
        }
        return rc;
}

int _close_r(struct _reent * r, int fd)
{
        int rc;
        xwssq_t idx;

        if ((fd <= 2) || (fd >= NEWLIBAC_FOPS_FD_NUM)) {
                r->_errno = ENFILE;
                rc = -1;
        } else {
#ifdef O_DIRECTORY
                bool dir;

                idx = fd - NEWLIBAC_FOPS_FD_OFFSET;
                dir = xwbmpop_t1i(newlibac_fops_nodetype_bmp, (xwsq_t)idx);
                if (dir) {
                        rc = newlibac_fops_closedir(r, (xwsq_t)idx);
                } else {
                        rc = newlibac_fops_closefile(r, (xwsq_t)idx);
                }
#else /* O_DIRECTORY */
                idx = fd - NEWLIBAC_FOPS_FD_OFFSET;
                rc = newlibac_fops_closefile(r, (xwsq_t)idx);
#endif /* !O_DIRECTORY */
        }
        return rc;
}

static
xwssz_t newlibac_fops_read_file(struct _reent * r, int fd, void * buf, size_t cnt)
{
        xwssz_t rdsz;
        xwssq_t idx;
        FRESULT fsrc;
        UINT fsrd;
        FIL * fp;

        idx = fd - NEWLIBAC_FOPS_FD_OFFSET;
        fp = newlibac_fops_fatfs_node[idx];
        fsrc = f_read(fp, buf, cnt, &fsrd);
        switch (fsrc) {
        case FR_OK:
                r->_errno = 0;
                rdsz = (xwssz_t)fsrd;
                break;
        case FR_INVALID_OBJECT:
                r->_errno = EINVAL;
                rdsz = -1;
                break;
        case FR_TIMEOUT:
                r->_errno = EINTR;
                rdsz = -1;
                break;
        case FR_DENIED:
                r->_errno = EACCES;
                rdsz = -1;
                break;
        case FR_DISK_ERR:
        case FR_INT_ERR:
        default:
                r->_errno = EIO;
                rdsz = -1;
                break;
        }
        return rdsz;
}

_ssize_t _read_r(struct _reent * r, int fd, void * buf, size_t cnt)
{
        int ret;

        if (0 == fd) {
                ret = (int)newlibac_fops_read_stdin(fd, buf, cnt);
        } else if ((fd <= 2) || (fd >= NEWLIBAC_FOPS_FD_NUM)) {
                r->_errno = EPERM;
                ret = -1;
        } else {
                ret = (int)newlibac_fops_read_file(r, fd, buf, cnt);
        }
        return ret;
}

static
xwssz_t newlibac_fops_write_file(struct _reent * r,
                                 int fd, const void * data, size_t cnt)
{
        xwssz_t wrsz;
        xwssq_t idx;
        FRESULT fsrc;
        UINT fswr;
        FIL * fp;

        idx = fd - NEWLIBAC_FOPS_FD_OFFSET;
        fp = newlibac_fops_fatfs_node[idx];
        fsrc = f_write(fp, data, cnt, &fswr);
        switch (fsrc) {
        case FR_OK:
                r->_errno = 0;
                wrsz = (xwssz_t)fswr;
                break;
        case FR_INVALID_OBJECT:
                r->_errno = EINVAL;
                wrsz = -1;
                break;
        case FR_TIMEOUT:
                r->_errno = EINTR;
                wrsz = -1;
                break;
        case FR_DENIED:
                r->_errno = EACCES;
                wrsz = -1;
                break;
        case FR_DISK_ERR:
        case FR_INT_ERR:
        default:
                r->_errno = EIO;
                wrsz = -1;
                break;
        }
        return wrsz;
}

_ssize_t _write_r(struct _reent * r, int fd, const void * data, size_t cnt)
{
        int ret;

        if (1 == fd) {
                ret = (int)newlibac_fops_write_stdout(fd, data, cnt);
        } else if (2 == fd) {
                ret = (int)newlibac_fops_write_stderr(fd, data, cnt);
        } else if ((fd <= 2) || (fd >= NEWLIBAC_FOPS_FD_NUM)) {
                r->_errno = EPERM;
                ret = -1;
        } else {
                ret = (int)newlibac_fops_write_file(r, fd, data, cnt);
        }
        return ret;
}

_off_t _lseek_r(struct _reent * r, int fd, _off_t pos, int whence)
{
        xwssq_t idx;
        FRESULT fsrc;
        FIL * fp;
        int curpos;

        if ((fd <= 2) || (fd >= NEWLIBAC_FOPS_FD_NUM)) {
                r->_errno = EPERM;
                curpos = -1;
        } else {
                idx = fd - NEWLIBAC_FOPS_FD_OFFSET;
                fp = newlibac_fops_fatfs_node[idx];
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
                        curpos = (int)f_tell(fp);
                        r->_errno = 0;
                        break;
                case FR_INVALID_PARAMETER:
                case FR_INVALID_OBJECT:
                        curpos = -1;
                        r->_errno = EINVAL;
                        break;
                case FR_TIMEOUT:
                        curpos = -1;
                        r->_errno = EINTR;
                        break;
                case FR_DISK_ERR:
                case FR_INT_ERR:
                default:
                        curpos = -1;
                        r->_errno = EIO;
                        break;
                }
        }
        return curpos;
}

int _unlink_r(struct _reent * r, const char * path)
{
        FRESULT fsrc;
        int rc;

        rc = -1;
        fsrc = f_unlink(path);
        switch (fsrc) {
        case FR_OK:
                r->_errno = XWOK;
                rc = 0;
                break;
        case FR_DISK_ERR:
        case FR_INT_ERR:
        case FR_NOT_ENABLED:
        case FR_NOT_READY:
                r->_errno = EIO;
                break;
        case FR_NO_FILE:
        case FR_NO_PATH:
        case FR_INVALID_NAME:
                r->_errno = ENOENT;
                break;
        case FR_DENIED:
                r->_errno = EPERM;
                break;
        case FR_WRITE_PROTECTED:
        case FR_INVALID_DRIVE:
        case FR_NO_FILESYSTEM:
                r->_errno = EACCES;
                break;
        case FR_TIMEOUT:
                r->_errno = ETIMEDOUT;
                break;
        case FR_LOCKED:
                r->_errno = EBUSY;
                break;
        case FR_NOT_ENOUGH_CORE:
                r->_errno = ENOMEM;
                break;
        default:
                r->_errno = EFAULT;
                break;
        }
        return rc;
}
