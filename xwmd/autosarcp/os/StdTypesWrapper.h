/**
 * @file
 * @brief AUTOSAR::CP::Os::StdTypesWrapper
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef xwmd_autosarcp_os_StdTypesWrapper_h__
#define xwmd_autosarcp_os_StdTypesWrapper_h__

#include <xwos/standard.h>

#if (defined(XWMDCFG_autosarcp_os_USING_STDTYPES_H) && \
     (1 == XWMDCFG_autosarcp_os_USING_STDTYPES_H))
#  include <Std_Types.h>
#else
/* [SWS_Std_00005] [SWS_Std_00011] */
typedef unsigned char Std_ReturnType;

/* [[SWS_Std_00006]] */
#  ifndef STATUSTYPEDEFINED
#    define STATUSTYPEDEFINED
typedef unsigned char StatusType; /* OSEK compliance */
#    define E_OK E_XWARCOS_OK
#  endif
#  define E_NOT_OK 0x01U

/* [SWS_Std_00007] */
#  define STD_HIGH 0x01U
#  define STD_LOW 0x00U

/* [SWS_Std_00013] */
#  define STD_ACTIVE 0x01U
#  define STD_IDLE 0x00U

/* [SWS_Std_00010] */
#  define STD_ON 0x01U
#  define STD_OFF 0x00U

/* [SWS_Std_00031] */
#  define NULL_PTR ((void *)0)
#endif

#endif /* xwmd/autosarcp/os/StdTypesWrapper.h */
