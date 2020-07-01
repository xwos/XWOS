/**
 * @file
 * @brief newlib适配代码：文件操作
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
#include <xwos/lib/xwbop.h>
#include <xwem/filesystem/fatfs/ff.h>
#include <stdlib.h>
#include <fcntl.h>

#define NEWLIBAC_FOPS_FD_NUM         32
#define NEWLIBAC_FOPS_FD_OFFSET      3 /* 0,1,2 分别代表 stdin, stdout, stderr. */

extern
xwssz_t newlibac_fops_read_stdin(int fd, void * buf, size_t cnt);

extern
xwssz_t newlibac_fops_write_stdout(int fd, const void * data, size_t cnt);

extern
xwssz_t newlibac_fops_write_stderr(int fd, const void * data, size_t cnt);

extern int errno;

xwbmpop_declare(newlibac_fops_node_bmp, NEWLIBAC_FOPS_FD_NUM) = {0};
xwbmpop_declare(newlibac_fops_nodetype_bmp, NEWLIBAC_FOPS_FD_NUM) = {0};
void * newlibac_fops_fatfs_node[NEWLIBAC_FOPS_FD_NUM] = {NULL};

void newlibac_fops_linkage_placeholder(void)
{
}

#ifdef O_DIRECTORY
static
int newlibac_fops_opendir(const char * path, int flag, int mode)
{
        int fd;
        FRESULT fsrc;
        DIR * dp;
        xwssq_t idx;

        XWOS_UNUSED(flag);
        XWOS_UNUSED(mode);

        idx = xwbmpop_ffz(newlibac_fops_node_bmp, NEWLIBAC_FOPS_FD_NUM);
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
                xwbmpop_s1i(newlibac_fops_node_bmp, (xwsq_t)idx);
                xwbmpop_s1i(newlibac_fops_nodetype_bmp, (xwsq_t)idx);
                newlibac_fops_fatfs_node[idx] = dp;
                fd = idx + NEWLIBAC_FOPS_FD_OFFSET;
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
#endif /* O_DIRECTORY */

int newlibac_fops_openfile(const char * path, int flag, int mode)
{
        int fd;
        FRESULT fsrc;
        FIL * fp;
        BYTE fsmode;
        xwssq_t idx;

        XWOS_UNUSED(mode);

        idx = xwbmpop_ffz(newlibac_fops_node_bmp, NEWLIBAC_FOPS_FD_NUM);
        if (idx < 0) {
                errno = EMFILE;
                fd = -1;
                goto err_mfile;
        }
        fp = malloc(sizeof(FIL));
        if (NULL == fp) {
                errno = ENOMEM;
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
                free(fp);
        }

err_nomem:
err_mfile:
        return fd;
}

int _open(const char * path, int flag, int mode)
{
        int rc;

#ifdef O_DIRECTORY
        if (mode & O_DIRECTORY) {
                rc = newlibac_fops_opendir(path, flag, mode);
        } else {
                rc = newlibac_fops_openfile(path, flag, mode);
        }
#else /* O_DIRECTORY */
        rc = newlibac_fops_openfile(path, flag, mode);
#endif /* !O_DIRECTORY */
        return rc;
}

#ifdef O_DIRECTORY
static
int newlibac_fops_closedir(xwsq_t idx)
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
                errno = OK;
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
#endif /* O_DIRECTORY */

static
int newlibac_fops_closefile(xwsq_t idx)
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
                errno = OK;
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

int _close(int fd)
{
        int rc;
        xwssq_t idx;

        if ((fd <= 2) || (fd >= NEWLIBAC_FOPS_FD_NUM)) {
                errno = ENFILE;
                rc = -1;
        } else {
#ifdef O_DIRECTORY
                bool dir;

                idx = fd - NEWLIBAC_FOPS_FD_OFFSET;
                dir = xwbmpop_t1i(newlibac_fops_nodetype_bmp, (xwsq_t)idx);
                if (dir) {
                        rc = newlibac_fops_closedir((xwsq_t)idx);
                } else {
                        rc = newlibac_fops_closefile((xwsq_t)idx);
                }
#else /* O_DIRECTORY */
                idx = fd - NEWLIBAC_FOPS_FD_OFFSET;
                rc = newlibac_fops_closefile((xwsq_t)idx);
#endif /* !O_DIRECTORY */
        }
        return rc;
}

static
xwssz_t newlibac_fops_read_file(int fd, void * buf, size_t cnt)
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

int _read(int fd, void * buf, size_t cnt)
{
        int ret;

        if (0 == fd) {
                ret = (int)newlibac_fops_read_stdin(fd, buf, cnt);
        } else if ((fd <= 2) || (fd >= NEWLIBAC_FOPS_FD_NUM)) {
                errno = EPERM;
                ret = -1;
        } else {
                ret = (int)newlibac_fops_read_file(fd, buf, cnt);
        }
        return ret;
}

static
xwssz_t newlibac_fops_write_file(int fd, const void * data, size_t cnt)
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

int _write(int fd, const void * data, size_t cnt)
{
        int ret;

        if (1 == fd) {
                ret = (int)newlibac_fops_write_stdout(fd, data, cnt);
        } else if (2 == fd) {
                ret = (int)newlibac_fops_write_stderr(fd, data, cnt);
        } else if ((fd <= 2) || (fd >= NEWLIBAC_FOPS_FD_NUM)) {
                errno = EPERM;
                ret = -1;
        } else {
                ret = (int)newlibac_fops_write_file(fd, data, cnt);
        }
        return ret;
}

int _lseek(int fd, int pos, int whence)
{
        xwssq_t idx;
        FRESULT fsrc;
        FIL * fp;
        int curpos;

        if ((fd <= 2) || (fd >= NEWLIBAC_FOPS_FD_NUM)) {
                errno = EPERM;
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
