//! XWOS RUST：错误码
//! ========
//!

use crate::types::XwEr;

////////////////////////////////////////////////////////////////
// libc errno.h 中定义的错误码
////////////////////////////////////////////////////////////////
/// Operation not permitted
pub const EPERM: XwEr = 1;
/// No such file or directory
pub const ENOENT: XwEr = 2;
/// No such process
pub const ESRCH: XwEr = 3;
/// Interrupted system call
pub const EINTR: XwEr = 4;
/// I/O error
pub const EIO: XwEr = 5;
/// No such device or address
pub const ENXIO: XwEr = 6;
/// Arg list too long
pub const E2BIG: XwEr = 7;
/// Exec format error
pub const ENOEXEC: XwEr = 8;
/// Bad file number
pub const EBADF: XwEr = 9;
/// No children
pub const ECHILD: XwEr = 10;
/// No more processes
pub const EAGAIN: XwEr = 11;
/// Not enough space
pub const ENOMEM: XwEr = 12;
/// Permission denied
pub const EACCES: XwEr = 13;
/// Bad address
pub const EFAULT: XwEr = 14;
/// Block device required
pub const ENOTBLK: XwEr = 15;
/// Device or resource busy
pub const EBUSY: XwEr = 16;
/// File exists
pub const EEXIST: XwEr = 17;
/// Cross-device link
pub const EXDEV: XwEr = 18;
/// No such device
pub const ENODEV: XwEr = 19;
/// Not a directory
pub const ENOTDIR: XwEr = 20;
/// Is a directory
pub const EISDIR: XwEr = 21;
/// Invalid argument
pub const EINVAL: XwEr = 22;
/// Too many open files in system
pub const ENFILE: XwEr = 23;
/// File descriptor value too large
pub const EMFILE: XwEr = 24;
/// Not a character device
pub const ENOTTY: XwEr = 25;
/// Text file busy
pub const ETXTBSY: XwEr = 26;
/// File too large
pub const EFBIG: XwEr = 27;
/// No space left on device
pub const ENOSPC: XwEr = 28;
/// Illegal seek
pub const ESPIPE: XwEr = 29;
/// Read-only file system
pub const EROFS: XwEr = 30;
/// Too many links
pub const EMLINK: XwEr = 31;
/// Broken pipe
pub const EPIPE: XwEr = 32;
/// Mathematics argument out of domain of function
pub const EDOM: XwEr = 33;
/// Result too large
pub const ERANGE: XwEr = 34;
/// No message of desired type
pub const ENOMSG: XwEr = 35;
/// Identifier removed
pub const EIDRM: XwEr = 36;
/// Channel number out of range
pub const ECHRNG: XwEr = 37;
/// Level 2 not synchronized
pub const EL2NSYNC: XwEr = 38;
/// Level 3 halted
pub const EL3HLT: XwEr = 39;
/// Level 3 reset
pub const EL3RST: XwEr = 40;
/// Link number out of range
pub const ELNRNG: XwEr = 41;
/// Protocol driver not attached
pub const EUNATCH: XwEr = 42;
/// No CSI structure available
pub const ENOCSI: XwEr = 43;
/// Level 2 halted
pub const EL2HLT: XwEr = 44;
/// Deadlock
pub const EDEADLK: XwEr = 45;
/// No lock
pub const ENOLCK: XwEr = 46;
/// Invalid exchange
pub const EBADE: XwEr = 50;
/// Invalid request descriptor
pub const EBADR: XwEr = 51;
/// Exchange full
pub const EXFULL: XwEr = 52;
/// No anode
pub const ENOANO: XwEr = 53;
/// Invalid request code
pub const EBADRQC: XwEr = 54;
/// Invalid slot
pub const EBADSLT: XwEr = 55;
/// File locking deadlock error
pub const EDEADLOCK: XwEr = 56;
/// Bad font file fmt
pub const EBFONT: XwEr = 57;
/// Not a stream
pub const ENOSTR: XwEr = 60;
/// No data (for no delay io)
pub const ENODATA: XwEr = 61;
/// Stream ioctl timeout
pub const ETIME: XwEr = 62;
/// No stream resources
pub const ENOSR: XwEr = 63;
/// Machine is not on the network
pub const ENONET: XwEr = 64;
/// Package not installed
pub const ENOPKG: XwEr = 65;
/// The object is remote
pub const EREMOTE: XwEr = 66;
/// Virtual circuit is gone
pub const ENOLINK: XwEr = 67;
/// Advertise error
pub const EADV: XwEr = 68;
/// Srmount error
pub const ESRMNT: XwEr = 69;
/// Communication error on send
pub const ECOMM: XwEr = 70;
/// Protocol error
pub const EPROTO: XwEr = 71;
/// Multihop attempted
pub const EMULTIHOP: XwEr = 74;
/// Inode is remote (not really error)
pub const ELBIN: XwEr = 75;
/// Cross mount point (not really error)
pub const EDOTDOT: XwEr = 76;
/// Bad message
pub const EBADMSG: XwEr = 77;
/// Inappropriate file type or format
pub const EFTYPE: XwEr = 79;
/// Given log. name not unique
pub const ENOTUNIQ: XwEr = 80;
/// f.d. invalid for this operation
pub const EBADFD: XwEr = 81;
/// Remote address changed
pub const EREMCHG: XwEr = 82;
/// Can't access a needed shared lib
pub const ELIBACC: XwEr = 83;
/// Accessing a corrupted shared lib
pub const ELIBBAD: XwEr = 84;
/// .lib section in a.out corrupted
pub const ELIBSCN: XwEr = 85;
/// Attempting to link in too many libs
pub const ELIBMAX: XwEr = 86;
/// Attempting to exec a shared library
pub const ELIBEXEC: XwEr = 87;
/// Function not implemented
pub const ENOSYS: XwEr = 88;
/// No more files
pub const ENMFILE: XwEr = 89;
/// Directory not empty
pub const ENOTEMPTY: XwEr = 90;
/// File or path name too long
pub const ENAMETOOLONG: XwEr = 91;
/// Too many symbolic links
pub const ELOOP: XwEr = 92;
/// Operation not supported on socket
pub const EOPNOTSUPP: XwEr = 95;
/// Protocol family not supported
pub const EPFNOSUPPORT: XwEr = 96;
/// Connection reset by peer
pub const ECONNRESET: XwEr = 104;
/// No buffer space available
pub const ENOBUFS: XwEr = 105;
/// Address family not supported by protocol family
pub const EAFNOSUPPORT: XwEr = 106;
/// Protocol wrong type for socket
pub const EPROTOTYPE: XwEr = 107;
/// Socket operation on non-socket
pub const ENOTSOCK: XwEr = 108;
/// Protocol not available
pub const ENOPROTOOPT: XwEr = 109;
/// Can't send after socket shutdown
pub const ESHUTDOWN: XwEr = 110;
/// Connection refused
pub const ECONNREFUSED: XwEr = 111;
/// Address already in use
pub const EADDRINUSE: XwEr = 112;
/// Software caused connection abort
pub const ECONNABORTED: XwEr = 113;
/// Network is unreachable
pub const ENETUNREACH: XwEr = 114;
/// Network interface is not configured
pub const ENETDOWN: XwEr = 115;
/// Connection timed out
pub const ETIMEDOUT: XwEr = 116;
/// Host is down
pub const EHOSTDOWN: XwEr = 117;
/// Host is unreachable
pub const EHOSTUNREACH: XwEr = 118;
/// Connection already in progress
pub const EINPROGRESS: XwEr = 119;
/// Socket already connected
pub const EALREADY: XwEr = 120;
/// Destination address required
pub const EDESTADDRREQ: XwEr = 121;
/// Message too long
pub const EMSGSIZE: XwEr = 122;
/// Unknown protocol
pub const EPROTONOSUPPORT: XwEr = 123;
/// Socket type not supported
pub const ESOCKTNOSUPPORT: XwEr = 124;
/// Address not available
pub const EADDRNOTAVAIL: XwEr = 125;
/// Connection aborted by network
pub const ENETRESET: XwEr = 126;
/// Socket is already connected
pub const EISCONN: XwEr = 127;
/// Socket is not connected
pub const ENOTCONN: XwEr = 128;
/// Too many references: cannot splice
pub const ETOOMANYREFS: XwEr = 129;
pub const EPROCLIM: XwEr = 130;
/// Too many users
pub const EUSERS: XwEr = 131;
/// Disk quota exceeded (POSIX.1-2001)
pub const EDQUOT: XwEr = 132;
/// Stale file handle (POSIX.1-2001).
pub const ESTALE: XwEr = 133;
/// Not supported
pub const ENOTSUP: XwEr = 134;
/// No medium (in tape drive)
pub const ENOMEDIUM: XwEr = 135;
/// No such host or network path
pub const ENOSHARE: XwEr = 136;
/// Filename exists with different case
pub const ECASECLASH: XwEr = 137;
/// Illegal byte sequence
pub const EILSEQ: XwEr = 138;
/// Value too large for defined data type
pub const EOVERFLOW: XwEr = 139;
/// Operation canceled
pub const ECANCELED: XwEr = 140;
/// State not recoverable
pub const ENOTRECOVERABLE: XwEr = 141;
/// Previous owner died
pub const EOWNERDEAD: XwEr = 142;
/// Streams pipe error
pub const ESTRPIPE: XwEr = 143;
/// Operation would block
pub const EWOULDBLOCK: XwEr = EAGAIN;

////////////////////////////////////////////////////////////////
// Linux 扩展的错误码
////////////////////////////////////////////////////////////////
/// restart syscall..
pub const ERESTARTSYS: XwEr = 512;
pub const ERESTARTNOINTR: XwEr = 513;
/// restart if no handler..
pub const ERESTARTNOHAND: XwEr = 514;
/// No ioctl command
pub const ENOIOCTLCMD: XwEr = 515;
/// restart by calling sys_restart_syscall
pub const ERESTART_RESTARTBLOCK: XwEr = 516;
/// Driver requests probe retry
pub const EPROBE_DEFER: XwEr = 517;
/// open found a stale dentry
pub const EOPENSTALE: XwEr = 518;
/// Parameter not supported
pub const ENOPARAM: XwEr = 519;
/// Illegal NFS file handle
pub const EBADHANDLE: XwEr = 521;
/// Update synchronization mismatch
pub const ENOTSYNC: XwEr = 522;
/// Cookie is stale
pub const EBADCOOKIE: XwEr = 523;
/// Operation is not supported
pub const ENOTSUPP: XwEr = 524;
/// Buffer or request is too small
pub const ETOOSMALL: XwEr = 525;
/// An untranslatable error occurred
pub const ESERVERFAULT: XwEr = 526;
/// Type not supported by server
pub const EBADTYPE: XwEr = 527;
/// Request initiated, but will not complete before timeout
pub const EJUKEBOX: XwEr = 528;
/// iocb queued, will get completion event
pub const EIOCBQUEUED: XwEr = 529;
/// conflict with recalled state
pub const ERECALLCONFLICT: XwEr = 530;

////////////////////////////////////////////////////////////////
// XWOS 扩展的错误码
////////////////////////////////////////////////////////////////
/// No error
pub const XWOK: XwEr = 0;

/// Thread context
pub const ETHDCTX: XwEr = 800;
/// Not thread context
pub const ENOTTHDCTX: XwEr = 801;
/// Bottom half context
pub const EBHCTX: XwEr = 802;
/// Not bottom half context
pub const ENOTBHCTX: XwEr = 803;
/// ISR context
pub const EISRCTX: XwEr = 804;
/// Not ISR context
pub const ENOTISRCTX: XwEr = 805;
/// Cannot preemption
pub const EDISPMPT: XwEr = 806;
/// Cannot enter BH
pub const EDISBH: XwEr = 807;
/// Cannot enter BH
pub const EDISIRQ: XwEr = 808;

/// Need to exit
pub const EEXIT: XwEr = 814;

/// Type error
pub const ETYPE: XwEr = 830;
/// Empty error
pub const EEMPTY: XwEr = 831;
/// Negative state
pub const ENEGATIVE: XwEr = 832;
/// Size error
pub const ESIZE: XwEr = 833;
/// Too small
pub const E2SMALL: XwEr = 835;
/// Not aligned
pub const EALIGN: XwEr = 836;
/// Owner error
pub const EOWNER: XwEr = 837;

/// BUG
pub const EBUG: XwEr = 860;

/// Nil object descriptor
pub const ENILOBJD: XwEr = 880;
/// Object dead
pub const EOBJDEAD: XwEr = 884;
/// Object active
pub const EOBJACTIVE: XwEr = 885;
