;; @file
;; @brief Emacs-script error numbers
;; @author
;; + 隐星魂 (Roy Sun) <xwos@xwos.tech>
;; @copyright
;; + Copyright © 2015 xwos.tech, All Rights Reserved.
;; > Licensed under the Apache License, Version 2.0 (the "License");
;; > you may not use this file except in compliance with the License.
;; > You may obtain a copy of the License at
;; >
;; >         http://www.apache.org/licenses/LICENSE-2.0
;; >
;; > Unless required by applicable law or agreed to in writing, software
;; > distributed under the License is distributed on an "AS IS" BASIS,
;; > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
;; > See the License for the specific language governing permissions and
;; > limitations under the License.

;;;;;;;; ;;;;;;;; ;;;;;;;; error info ;;;;;;;; ;;;;;;;; ;;;;;;;;
(defconst EPERM 1 "Operation not permitted")
(defconst ENOENT 2 "No such file or directory")
(defconst ESRCH 3 "No such process")
(defconst EINTR 4 "Interrupted system call")
(defconst EIO 5 "I/O error")
(defconst ENXIO 6 "No such device or address")
(defconst E2BIG 7 "Argument list too long")
(defconst ENOEXEC 8 "Exec format error")
(defconst EBADF 9 "Bad file number")
(defconst ECHILD 10 "No child processes")
(defconst EAGAIN 11 "Try again")
(defconst ENOMEM 12 "Out of memory")
(defconst EACCES 13 "Permission denied")
(defconst EFAULT 14 "Bad address")
(defconst ENOTBLK 15 "Block device required")
(defconst EBUSY 16 "Device or resource busy")
(defconst EEXIST 17 "File exists")
(defconst EXDEV 18 "Cross-device link")
(defconst ENODEV 19 "No such device")
(defconst ENOTDIR 20 "Not a directory")
(defconst EISDIR 21 "Is a directory")
(defconst EINVAL 22 "Invalid argument")
(defconst ENFILE 23 "File table overflow")
(defconst EMFILE 24 "Too many open files")
(defconst ENOTTY 25 "Not a typewriter")
(defconst ETXTBSY 26 "Text file busy")
(defconst EFBIG 27 "File too large")
(defconst ENOSPC 28 "No space left on device")
(defconst ESPIPE 29 "Illegal seek")
(defconst EROFS 30 "Read-only file system")
(defconst EMLINK 31 "Too many links")
(defconst EPIPE 32 "Broken pipe")
(defconst EDOM 33 "Math argument out of domain of func")
(defconst ERANGE 34 "Math result not representable")
(defconst EDEADLK 35 "Resource deadlock would occur")
(defconst ENAMETOOLONG 36 "File name too long")
(defconst ENOLCK 37 "No record locks available")
(defconst ENOSYS 38 "Function not implemented")
(defconst ENOTEMPTY 39 "Directory not empty")
(defconst ELOOP 40 "Too many symbolic links encountered")
(defconst EWOULDBLOCK EAGAIN "Operation would block")
(defconst ENOMSG 42 "No message of desired type")
(defconst EIDRM 43 "Identifier removed")
(defconst ECHRNG 44 "Channel number out of range")
(defconst EL2NSYNC 45 "Level 2 not synchronized")
(defconst EL3HLT 46 "Level 3 halted")
(defconst EL3RST 47 "Level 3 reset")
(defconst ELNRNG 48 "Link number out of range")
(defconst EUNATCH 49 "Protocol driver not attached")
(defconst ENOCSI 50 "No CSI structure available")
(defconst EL2HLT 51 "Level 2 halted")
(defconst EBADE 52 "Invalid exchange")
(defconst EBADR 53 "Invalid request descriptor")
(defconst EXFULL 54 "Exchange full")
(defconst ENOANO 55 "No anode")
(defconst EBADRQC 56 "Invalid request code")
(defconst EBADSLT 57 "Invalid slot")
(defconst EDEADLOCK EDEADLK)
(defconst EBFONT 59 "Bad font file format")
(defconst ENOSTR 60 "Device not a stream")
(defconst ENODATA 61 "No data available")
(defconst ETIME 62 "Timer expired")
(defconst ENOSR 63 "Out of streams resources")
(defconst ENONET 64 "Machine is not on the network")
(defconst ENOPKG 65 "Package not installed")
(defconst EREMOTE 66 "Object is remote")
(defconst ENOLINK 67 "Link has been severed")
(defconst EADV 68 "Advertise error")
(defconst ESRMNT 69 "Srmount error")
(defconst ECOMM 70 "Communication error on send")
(defconst EPROTO 71 "Protocol error")
(defconst EMULTIHOP 72 "Multihop attempted")
(defconst EDOTDOT 73 "RFS specific error")
(defconst EBADMSG 74 "Not a data message")
(defconst EOVERFLOW 75 "Value too large for defined data type")
(defconst ENOTUNIQ 76 "Name not unique on network")
(defconst EBADFD 77 "File descriptor in bad state")
(defconst EREMCHG 78 "Remote address changed")
(defconst ELIBACC 79 "Can not access a needed shared library")
(defconst ELIBBAD 80 "Accessing a corrupted shared library")
(defconst ELIBSCN 81 ".lib section in a.out corrupted")
(defconst ELIBMAX 82 "Attempting to link in too many shared libraries")
(defconst ELIBEXEC 83 "Cannot exec a shared library directly")
(defconst EILSEQ 84 "Illegal byte sequence")
(defconst ERESTART 85 "Interrupted system call should be restarted")
(defconst ESTRPIPE 86 "Streams pipe error")
(defconst EUSERS 87 "Too many users")
(defconst ENOTSOCK 88 "Socket operation on non-socket")
(defconst EDESTADDRREQ 89 "Destination address required")
(defconst EMSGSIZE 90 "Message too long")
(defconst EPROTOTYPE 91 "Protocol wrong type for socket")
(defconst ENOPROTOOPT 92 "Protocol not available")
(defconst EPROTONOSUPPORT 93 "Protocol not supported")
(defconst ESOCKTNOSUPPORT 94 "Socket type not supported")
(defconst EOPNOTSUPP 95 "Operation not supported on transport endpoint")
(defconst EPFNOSUPPORT 96 "Protocol family not supported")
(defconst EAFNOSUPPORT 97 "Address family not supported by protocol")
(defconst EADDRINUSE 98 "Address already in use")
(defconst EADDRNOTAVAIL 99 "Cannot assign requested address")
(defconst ENETDOWN 100 "Network is down")
(defconst ENETUNREACH 101 "Network is unreachable")
(defconst ENETRESET 102 "Network dropped connection because of reset")
(defconst ECONNABORTED 103 "Software caused connection abort")
(defconst ECONNRESET 104 "Connection reset by peer")
(defconst ENOBUFS 105 "No buffer space available")
(defconst EISCONN 106 "Transport endpoint is already connected")
(defconst ENOTCONN 107 "Transport endpoint is not connected")
(defconst ESHUTDOWN 108 "Cannot send after transport endpoint shutdown")
(defconst ETOOMANYREFS 109 "Too many references: cannot splice")
(defconst ETIMEDOUT 110 "Connection timed out")
(defconst ECONNREFUSED 111 "Connection refused")
(defconst EHOSTDOWN 112 "Host is down")
(defconst EHOSTUNREACH 113 "No route to host")
(defconst EALREADY 114 "Operation already in progress")
(defconst EINPROGRESS 115 "Operation now in progress")
(defconst ESTALE 116 "Stale NFS file handle")
(defconst EUCLEAN 117 "Structure needs cleaning")
(defconst ENOTNAM 118 "Not a XENIX named type file")
(defconst ENAVAIL 119 "No XENIX semaphores available")
(defconst EISNAM 120 "Is a named type file")
(defconst EREMOTEIO 121 "Remote I/O error")
(defconst EDQUOT 122 "Quota exceeded")
(defconst ENOMEDIUM 123 "No medium found")
(defconst EMEDIUMTYPE 124 "Wrong medium type")
(defconst ECANCELED 125 "Operation Canceled")
(defconst ENOKEY 126 "Required key not available")
(defconst EKEYEXPIRED 127 "Key has expired")
(defconst EKEYREVOKED 128 "Key has been revoked")
(defconst EKEYREJECTED 129 "Key was rejected by service")
(defconst EOWNERDEAD 130 "Owner died")
(defconst ENOTRECOVERABLE 131 "State not recoverable")
(defconst ERFKILL 132 "Operation not possible due to RF-kill")
(defconst EHWPOISON 133 "Memory page has hardware error")

(provide 'errno)
