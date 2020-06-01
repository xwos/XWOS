/**
 * @file
 * @brief 玄武OS的版本号
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * @note
 * + 版本号规则: M.m.r
 *   - M: 大版本号
 *   - m: 小版本号
 *     + 奇数：开发版
 *     + 偶数：稳定版
 *   - r: 修正号
 */

#ifndef __xwos_version_h__
#define __xwos_version_h__

#ifndef __xwos_standard_h__
  #error "This file should be included from <xwos/standard.h>."
#endif

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macro       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @defgroup XuanWuOS_Version_definition
 * @{
 */
#define XWOS_VERSION_MAJOR              1
#define XWOS_VERSION_MINOR              2
#define XWOS_VERSION_REVISION           0
/**
 * @}
 */

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
        (stringify(XWOS_VERSION_MAJOR) "." \
         stringify(XWOS_VERSION_MINOR) "." \
         stringify(XWOS_VERSION_REVISION))

#endif /* xwos/version.h */
