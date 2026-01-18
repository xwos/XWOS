/**
 * @file
 * @brief AUTOSAR::CP::Os::Resource
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwmd/autosarcp/os/Resource.h>
#include <xwmd/autosarcp/os/Error.h>

extern struct xwarcos_resource * const xwarcos_resource_table[];
extern const xwu32_t xwarcos_resource_num;

StatusType GetResource(ResourceType ResID)
{
        xwer_t xwrc;
        struct xwarcos_resource * resource;

        if (RES_SCHEDULER == ResID) {
                resource = XWARCOS_RES_SCHEDULER;
                xwrc = xwarcos_resource_get(resource);
        } else if (ResID >= xwarcos_resource_num) {
                xwrc = E_XWARCOS_ID;
        } else {
                resource = xwarcos_resource_table[ResID];
                xwrc = xwarcos_resource_get(resource);
        }
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}

StatusType ReleaseResource(ResourceType ResID)
{
        xwer_t xwrc;
        struct xwarcos_resource * resource;

        if (RES_SCHEDULER == ResID) {
                resource = XWARCOS_RES_SCHEDULER;
                xwrc = xwarcos_resource_release(resource);
        } else if (ResID >= xwarcos_resource_num) {
                xwrc = E_XWARCOS_ID;
        } else {
                resource = xwarcos_resource_table[ResID];
                xwrc = xwarcos_resource_release(resource);
        }
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}
