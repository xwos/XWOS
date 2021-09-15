/**
 * @file
 * @brief XWOS通用库：玄武OS的版本号
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_lib_version_h__
#define __xwos_lib_version_h__

#ifndef __xwos_standard_h__
#  error "This file should be included from <xwos/standard.h>."
#endif

/**
 * @defgroup XWOS版本号
 * @{
 */
/**
 * @page XWOS_Version XWOS版本号
 * #### 版本号规则: M.m.r
 *   - M: 大版本号
 *   - m: 小版本号
 *     + 奇数：开发版
 *     + 偶数：稳定版
 *   - r: 修正号
 */

#define XWOS_VERSION_MAJOR              1
#define XWOS_VERSION_MINOR              9
#define XWOS_VERSION_REVISION           0

/**
 * @brief XWOS的版本号（数值形式）
 */
#define XWOS_VER_NUM \
        (((XWOS_VERSION_MAJOR) << 24) | \
         ((XWOS_VERSION_MINOR) << 16) | \
         ((XWOS_VERSION_REVISION) << 0))

/**
 * @brief XWOS的版本号（字符串形式）
 */
#define XWOS_VER_STR \
        stringify(XWOS_VERSION_MAJOR) "." \
        stringify(XWOS_VERSION_MINOR) "." \
        stringify(XWOS_VERSION_REVISION)

#define XWOS_COPYRIGHT "XWOS " XWOS_VER_STR \
        "  Copyright (C) 2015 xwos.tech, All Rights Reserved."

/**
 * @} XWOS版本号
 */

#endif /* xwos/lib/version.h */
