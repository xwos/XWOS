/**
 * @file
 * @brief AUTOSAR::CP::Os::Resource
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef xwmd_autosarcp_os_Resource_h__
#define xwmd_autosarcp_os_Resource_h__

#include <xwmd/autosarcp/os/StdTypesWrapper.h>
#include <xwos/arcos/resource.h>

typedef xwu32_t ResourceType;
#define DeclareResource(name)
#define RES_SCHEDULER (0xFFFFFFFF)

StatusType GetResource(ResourceType ResID);
StatusType ReleaseResource(ResourceType ResID);

#endif /* xwmd/autosarcp/os/Resource.h */
