/**
 * @file
 * @brief FatFs适配层
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 */

#include <stdlib.h>
#include "ff.h"                 /* Obtains integer types */
#include "diskio.h"             /* Declarations of disk functions */

/* Definitions of physical drive number for each drive */
#define DEV_RAM         0       /* Map Ramdisk to physical drive 0 */
#define DEV_MMC         1       /* Map MMC/SD card to physical drive 1 */
#define DEV_USB         2       /* Map USB MSD to physical drive 2 */

extern int RAM_disk_status(void);
extern int MMC_disk_status(void);
extern int USB_disk_status(void);

extern int RAM_disk_initialize(void);
extern int MMC_disk_initialize(void);
extern int USB_disk_initialize(void);

extern int RAM_disk_read(uint8_t *buff, uint64_t sector, uint32_t count);
extern int MMC_disk_read(uint8_t *buff, uint64_t sector, uint32_t count);
extern int USB_disk_read(uint8_t *buff, uint64_t sector, uint32_t count);

extern int RAM_disk_write(uint8_t *buff, uint64_t sector, uint32_t count);
extern int MMC_disk_write(uint8_t *buff, uint64_t sector, uint32_t count);
extern int USB_disk_write(uint8_t *buff, uint64_t sector, uint32_t count);

extern int RAM_disk_ioctl(uint8_t cmd, void *buff);
extern int MMC_disk_ioctl(uint8_t cmd, void *buff);
extern int USB_disk_ioctl(uint8_t cmd, void *buff);

/* Disk Drive */
DSTATUS disk_status(BYTE pdrv)
{
        DSTATUS stat;
        int result;
        switch (pdrv) {
        case DEV_RAM :
#if (1 == FF_FS_DEV_RAM)
                result = RAM_disk_status();
                if (result < 0) {
                        stat = STA_NOINIT;
                } else {
                        stat = 0;
                }
#else
                stat = STA_NODISK;
#endif
                return stat;

        case DEV_MMC :
#if (1 == FF_FS_DEV_MMC)
                result = MMC_disk_status();
                if (result < 0) {
                        stat = STA_NOINIT;
                } else {
                        stat = 0;
                }
#else
                stat = STA_NODISK;
#endif
                return stat;

        case DEV_USB :
#if (1 == FF_FS_DEV_USB)
                result = USB_disk_status();
                if (result < 0) {
                        stat = STA_NOINIT;
                } else {
                        stat = 0;
                }
#else
                stat = STA_NODISK;
#endif
                return stat;
        }
        return STA_NOINIT;
}

DSTATUS disk_initialize(BYTE pdrv)
{
        DSTATUS stat;
        int result;

        switch (pdrv) {
        case DEV_RAM :
#if (1 == FF_FS_DEV_RAM)
                result = RAM_disk_initialize();
                if (result < 0) {
                        stat = STA_NOINIT;
                } else {
                        stat = 0;
                }
#else
                stat = STA_NODISK;
#endif
                return stat;

        case DEV_MMC :
#if (1 == FF_FS_DEV_MMC)
                result = MMC_disk_initialize();
                if (result < 0) {
                        stat = STA_NOINIT;
                } else {
                        stat = 0;
                }
#else
                stat = STA_NODISK;
#endif
                return stat;

        case DEV_USB :
#if (1 == FF_FS_DEV_USB)
                result = USB_disk_initialize();
                if (result < 0) {
                        stat = STA_NOINIT;
                } else {
                        stat = 0;
                }
#else
                stat = STA_NODISK;
#endif
                return stat;
        }
        return STA_NOINIT;
}

DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count)
{
        DRESULT res;
        int result;

        switch (pdrv) {
        case DEV_RAM :
#if (1 == FF_FS_DEV_RAM)
                result = RAM_disk_read((uint8_t *)buff, (uint64_t)sector, (uint32_t)count);
                if (result < 0) {
                        res = RES_ERROR;
                } else {
                        res = RES_OK;
                }
#else
                res = RES_PARERR;
#endif
                return res;

        case DEV_MMC :
#if (1 == FF_FS_DEV_MMC)
                result = MMC_disk_read((uint8_t *)buff, (uint64_t)sector, (uint32_t)count);
                if (result < 0) {
                        res = RES_ERROR;
                } else {
                        res = RES_OK;
                }
#else
                res = RES_PARERR;
#endif
                return res;

        case DEV_USB :
#if (1 == FF_FS_DEV_USB)
                result = USB_disk_read((uint8_t *)buff, (uint64_t)sector, (uint32_t)count);
                if (result < 0) {
                        res = RES_ERROR;
                } else {
                        res = RES_OK;
                }
#else
                res = RES_PARERR;
#endif
                return res;
        }
        return RES_PARERR;
}

#if FF_FS_READONLY == 0
DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count)
{
        DRESULT res;
        int result;

        switch (pdrv) {
        case DEV_RAM :
#if (1 == FF_FS_DEV_RAM)
                result = RAM_disk_write((uint8_t *)buff, (uint64_t)sector, (uint32_t)count);
                if (result < 0) {
                        res = RES_ERROR;
                } else {
                        res = RES_OK;
                }
#else
                res = RES_PARERR;
#endif
                return res;

        case DEV_MMC :
#if (1 == FF_FS_DEV_MMC)
                result = MMC_disk_write((uint8_t *)buff, (uint64_t)sector, (uint32_t)count);
                if (result < 0) {
                        res = RES_ERROR;
                } else {
                        res = RES_OK;
                }
#else
                res = RES_PARERR;
#endif
                return res;

        case DEV_USB :
#if (1 == FF_FS_DEV_USB)
                result = USB_disk_write((uint8_t *)buff, (uint64_t)sector, (uint32_t)count);
                if (result < 0) {
                        res = RES_ERROR;
                } else {
                        res = RES_OK;
                }
#else
                res = RES_PARERR;
#endif
                return res;
        }

        return RES_PARERR;
}

#endif /* FF_FS_READONLY == 0 */

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void * buff)
{
        DRESULT res;
        int result;

        switch (pdrv) {
        case DEV_RAM :
#if (1 == FF_FS_DEV_RAM)
                res = RAM_disk_ioctl((uint8_t)cmd, (void *)buff);
                if (result < 0) {
                        res = RES_ERROR;
                } else {
                        res = RES_OK;
                }
#else
                res = RES_PARERR;
#endif
                return res;

        case DEV_MMC :
#if (1 == FF_FS_DEV_MMC)
                result = MMC_disk_ioctl((uint8_t)cmd, (void *)buff);
                if (result < 0) {
                        res = RES_ERROR;
                } else {
                        res = RES_OK;
                }
#else
                res = RES_PARERR;
#endif
                return res;

        case DEV_USB :
#if (1 == FF_FS_DEV_USB)
                result = USB_disk_ioctl((uint8_t)cmd, (void *)buff);
                if (result < 0) {
                        res = RES_ERROR;
                } else {
                        res = RES_OK;
                }
#else
                res = RES_PARERR;
#endif
                return res;
        }

        return RES_PARERR;
}

/* Allocate & Free a memory block */
#if FF_USE_LFN == 3
void * ff_memalloc(UINT msize)
{
        return malloc(msize); /* Allocate a new memory block with POSIX API */
}

void ff_memfree(void* mblock)
{
        free(mblock); /* Free the memory block with POSIX API */
}

#endif /* FF_USE_LFN == 3 */
