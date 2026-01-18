/**
 * @file
 * @brief newlib适配层：控制链接流程的桩函数
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_libc_newlib_linkage_h__
#define __xwmd_libc_newlib_linkage_h__

void newlibac_errno_linkage_stub(void);
void newlibac_lock_linkage_stub(void);
void newlibac_sysconf_linkage_stub(void);
void newlibac_time_linkage_stub(void);
void newlibac_string_linkage_stub(void);
void newlibac_sprintf_linkage_stub(void);
void newlibac_mem_linkage_stub(void);
void newlibac_setjmp_linkage_stub(void);
void newlibac_fops_linkage_stub(void);
void newlibac_isatty_linkage_stub(void);
void newlibac_kill_linkage_stub(void);
void newlibac_getpid_linkage_stub(void);
void newlibac_exit_linkage_stub(void);

#endif /* xwmd/libc/newlib/linkage.h */
