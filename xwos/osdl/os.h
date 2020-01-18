/**
 * @file
 * @brief 操作系统接口描述
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_osdl_os_h__
#define __xwos_osdl_os_h__

#if defined(XuanWuOS_CFG_CORE__smp)
  #include <xwos/osdl/smp.h>
#elif defined(XuanWuOS_CFG_CORE__up)
  #include <xwos/osdl/up.h>
#endif

#endif /* xwos/osdl/os.h */
