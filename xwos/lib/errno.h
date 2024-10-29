/**
 * @file
 * @brief XWOS通用库：错误码
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 * @note
 * 包括了:
 * - Unix标准错误码
 * - Linux扩展的错误码
 * - XWOS扩展的错误码
 */

#ifndef __xwos_lib_errno_h__
#define __xwos_lib_errno_h__

#if defined(ARCHCFG_COMPILER_ERRNO) && (1 == ARCHCFG_COMPILER_ERRNO)
#  define __LINUX_ERRNO_EXTENSIONS__
#  include <errno.h>
#else
#  ifndef EPERM
/**
 * @defgroup xwos_lib_errno 错误码
 * @ingroup xwos_lib
 * @{
 */
#    define EPERM 1 /**< @brief Operation not permitted */
#    define ENOENT 2 /**< @brief No such file or directory */
#    define ESRCH 3 /**< @brief No such process */
#    define EINTR 4 /**< @brief Interrupted system call */
#    define EIO 5 /**< @brief I/O error */
#    define ENXIO 6 /**< @brief No such device or address */
#    define E2BIG 7 /**< @brief Arg list too long */
#    define ENOEXEC 8 /**< @brief Exec format error */
#    define EBADF 9 /**< @brief Bad file number */
#    define ECHILD 10 /**< @brief No children */
#    define EAGAIN 11 /**< @brief No more processes */
#    define ENOMEM 12 /**< @brief Not enough space */
#    define EACCES 13 /**< @brief Permission denied */
#    define EFAULT 14 /**< @brief Bad address */
#    define ENOTBLK 15 /**< @brief Block device required */
#    define EBUSY 16 /**< @brief Device or resource busy */
#    define EEXIST 17 /**< @brief File exists */
#    define EXDEV 18 /**< @brief Cross-device link */
#    define ENODEV 19 /**< @brief No such device */
#    define ENOTDIR 20 /**< @brief Not a directory */
#    define EISDIR 21 /**< @brief Is a directory */
#    define EINVAL 22 /**< @brief Invalid argument */
#    define ENFILE 23 /**< @brief Too many open files in system */
#    define EMFILE 24 /**< @brief File descriptor value too large */
#    define ENOTTY 25 /**< @brief Not a character device */
#    define ETXTBSY 26 /**< @brief Text file busy */
#    define EFBIG 27 /**< @brief File too large */
#    define ENOSPC 28 /**< @brief No space left on device */
#    define ESPIPE 29 /**< @brief Illegal seek */
#    define EROFS 30 /**< @brief Read-only file system */
#    define EMLINK 31 /**< @brief Too many links */
#    define EPIPE 32 /**< @brief Broken pipe */
#    define EDOM 33 /**< @brief Mathematics argument out of domain of function */
#    define ERANGE 34 /**< @brief Result too large */
#    define ENOMSG 35 /**< @brief No message of desired type */
#    define EIDRM 36 /**< @brief Identifier removed */
#    define ECHRNG 37 /**< @brief Channel number out of range */
#    define EL2NSYNC 38 /**< @brief Level 2 not synchronized */
#    define EL3HLT 39 /**< @brief Level 3 halted */
#    define EL3RST 40 /**< @brief Level 3 reset */
#    define ELNRNG 41 /**< @brief Link number out of range */
#    define EUNATCH 42 /**< @brief Protocol driver not attached */
#    define ENOCSI 43 /**< @brief No CSI structure available */
#    define EL2HLT 44 /**< @brief Level 2 halted */
#    define EDEADLK 45 /**< @brief Deadlock */
#    define ENOLCK 46 /**< @brief No lock */
#    define EBADE 50 /**< @brief Invalid exchange */
#    define EBADR 51 /**< @brief Invalid request descriptor */
#    define EXFULL 52 /**< @brief Exchange full */
#    define ENOANO 53 /**< @brief No anode */
#    define EBADRQC 54 /**< @brief Invalid request code */
#    define EBADSLT 55 /**< @brief Invalid slot */
#    define EDEADLOCK 56 /**< @brief File locking deadlock error */
#    define EBFONT 57 /**< @brief Bad font file fmt */
#    define ENOSTR 60 /**< @brief Not a stream */
#    define ENODATA 61 /**< @brief No data (for no delay io) */
#    define ETIME 62 /**< @brief Stream ioctl timeout */
#    define ENOSR 63 /**< @brief No stream resources */
#    define ENONET 64 /**< @brief Machine is not on the network */
#    define ENOPKG 65 /**< @brief Package not installed */
#    define EREMOTE 66 /**< @brief The object is remote */
#    define ENOLINK 67 /**< @brief Virtual circuit is gone */
#    define EADV 68 /**< @brief Advertise error */
#    define ESRMNT 69 /**< @brief Srmount error */
#    define ECOMM 70 /**< @brief Communication error on send */
#    define EPROTO 71 /**< @brief Protocol error */
#    define EMULTIHOP 74 /**< @brief Multihop attempted */
#    define ELBIN 75 /**< @brief Inode is remote (not really error) */
#    define EDOTDOT 76 /**< @brief Cross mount point (not really error) */
#    define EBADMSG 77 /**< @brief Bad message */
#    define EFTYPE 79 /**< @brief Inappropriate file type or format */
#    define ENOTUNIQ 80 /**< @brief Given log. name not unique */
#    define EBADFD 81 /**< @brief f.d. invalid for this operation */
#    define EREMCHG 82 /**< @brief Remote address changed */
#    define ELIBACC 83 /**< @brief Can't access a needed shared lib */
#    define ELIBBAD 84 /**< @brief Accessing a corrupted shared lib */
#    define ELIBSCN 85 /**< @brief .lib section in a.out corrupted */
#    define ELIBMAX 86 /**< @brief Attempting to link in too many libs */
#    define ELIBEXEC 87 /**< @brief Attempting to exec a shared library */
#    define ENOSYS 88 /**< @brief Function not implemented */
#    define ENMFILE 89 /**< @brief No more files */
#    define ENOTEMPTY 90 /**< @brief Directory not empty */
#    define ENAMETOOLONG 91 /**< @brief File or path name too long */
#    define ELOOP 92 /**< @brief Too many symbolic links */
#    define EOPNOTSUPP 95 /**< @brief Operation not supported on socket */
#    define EPFNOSUPPORT 96 /**< @brief Protocol family not supported */
#    define ECONNRESET 104 /**< @brief Connection reset by peer */
#    define ENOBUFS 105 /**< @brief No buffer space available */
#    define EAFNOSUPPORT 106 /**< @brief Address family not supported by protocol family */
#    define EPROTOTYPE 107 /**< @brief Protocol wrong type for socket */
#    define ENOTSOCK 108 /**< @brief Socket operation on non-socket */
#    define ENOPROTOOPT 109 /**< @brief Protocol not available */
#    define ESHUTDOWN 110 /**< @brief Can't send after socket shutdown */
#    define ECONNREFUSED 111 /**< @brief Connection refused */
#    define EADDRINUSE 112 /**< @brief Address already in use */
#    define ECONNABORTED 113 /**< @brief Software caused connection abort */
#    define ENETUNREACH 114 /**< @brief Network is unreachable */
#    define ENETDOWN 115 /**< @brief Network interface is not configured */
#    define ETIMEDOUT 116 /**< @brief Connection timed out */
#    define EHOSTDOWN 117 /**< @brief Host is down */
#    define EHOSTUNREACH 118 /**< @brief Host is unreachable */
#    define EINPROGRESS 119 /**< @brief Connection already in progress */
#    define EALREADY 120 /**< @brief Socket already connected */
#    define EDESTADDRREQ 121 /**< @brief Destination address required */
#    define EMSGSIZE 122 /**< @brief Message too long */
#    define EPROTONOSUPPORT 123 /**< @brief Unknown protocol */
#    define ESOCKTNOSUPPORT 124 /**< @brief Socket type not supported */
#    define EADDRNOTAVAIL 125 /**< @brief Address not available */
#    define ENETRESET 126 /**< @brief Connection aborted by network */
#    define EISCONN 127 /**< @brief Socket is already connected */
#    define ENOTCONN 128 /**< @brief Socket is not connected */
#    define ETOOMANYREFS 129 /**< @brief Too many references: cannot splice */
#    define EPROCLIM 130
#    define EUSERS 131 /**< @brief Too many users */
#    define EDQUOT 132 /**< @brief Disk quota exceeded (POSIX.1-2001) */
#    define ESTALE 133 /**< @brief Stale file handle (POSIX.1-2001). */
#    define ENOTSUP 134 /**< @brief Not supported */
#    define ENOMEDIUM 135 /**< @brief No medium (in tape drive) */
#    define ENOSHARE 136 /**< @brief No such host or network path */
#    define ECASECLASH 137 /**< @brief Filename exists with different case */
#    define EILSEQ 138 /**< @brief Illegal byte sequence */
#    define EOVERFLOW 139 /**< @brief Value too large for defined data type */
#    define ECANCELED 140 /**< @brief Operation canceled */
#    define ENOTRECOVERABLE 141 /**< @brief State not recoverable */
#    define EOWNERDEAD 142 /**< @brief Previous owner died */
#    define ESTRPIPE 143 /**< @brief Streams pipe error */
#    define EWOULDBLOCK EAGAIN /**< @brief Operation would block */
#  endif
#endif

/* Linux扩展的错误码 */
#define ERESTARTSYS 512 /**< @brief restart syscall.. */
#define ERESTARTNOINTR 513
#define ERESTARTNOHAND 514 /**< @brief restart if no handler.. */
#define ENOIOCTLCMD 515 /**< @brief No ioctl command */
#define ERESTART_RESTARTBLOCK 516 /**< @brief restart by calling sys_restart_syscall */
#define EPROBE_DEFER 517 /**< @brief Driver requests probe retry */
#define EOPENSTALE 518 /**< @brief open found a stale dentry */
#define ENOPARAM 519 /**< @brief Parameter not supported */
#define EBADHANDLE 521 /**< @brief Illegal NFS file handle */
#define ENOTSYNC 522 /**< @brief Update synchronization mismatch */
#define EBADCOOKIE 523 /**< @brief Cookie is stale */
#define ENOTSUPP 524 /**< @brief Operation is not supported */
#define ETOOSMALL 525 /**< @brief Buffer or request is too small */
#define ESERVERFAULT 526 /**< @brief An untranslatable error occurred */
#define EBADTYPE 527 /**< @brief Type not supported by server */
#define EJUKEBOX 528 /**< @brief Request initiated, but will not complete before timeout */
#define EIOCBQUEUED 529 /**< @brief iocb queued, will get completion event */
#define ERECALLCONFLICT 530 /**< @brief conflict with recalled state */

/* XWOS扩展的错误码 */
#define XWOK            0 /**< @brief No error */

#define ETHDCTX         800 /**< @brief Thread context */
#define ENOTTHDCTX      801 /**< @brief Not thread context */
#define EBHCTX          802 /**< @brief Bottom half context */
#define ENOTBHCTX       803 /**< @brief Not bottom half context */
#define EISRCTX         804 /**< @brief ISR context */
#define ENOTISRCTX      805 /**< @brief Not ISR context */
#define EDISPMPT        806 /**< @brief preemption disabled */
#define EDISBH          807 /**< @brief bottom half disabled */
#define EDISIRQ         808 /**< @brief irq disabled */

#define EEXIT           814 /**< @brief Need to exit */

#define ETYPE           830 /**< @brief Type error */
#define EEMPTY          831 /**< @brief Empty error */
#define ENEGATIVE       832 /**< @brief Negative state */
#define ESIZE           833 /**< @brief Size error */
#define E2SMALL         835 /**< @brief Too small */
#define EALIGN          836 /**< @brief Not aligned */
#define EOWNER          837 /**< @brief Owner error */
#define EMISSINGEND     838 /**< @brief forget closing error */

#define EBUG            860 /**< @brief BUG */

#define ENILOBJD        880 /**< @brief Nil object descriptor */
#define EOBJDEAD        884 /**< @brief Object dead */
#define EOBJACTIVE      885 /**< @brief Object active */


/**
 * @} xwos_lib_errno
 */

#endif /* xwos/lib/errno.h */
