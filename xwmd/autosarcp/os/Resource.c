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

#include <xwmd/autosarcp/os/Resource.h>
#include <xwmd/autosarcp/os/Error.h>

StatusType GetResource(ResourceType ResID)
{
        xwer_t xwrc;

        xwrc = xwarcos_resource_get(ResID);
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}

StatusType ReleaseResource(ResourceType ResID)
{
        xwer_t xwrc;

        xwrc = xwarcos_resource_release(ResID);
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}
