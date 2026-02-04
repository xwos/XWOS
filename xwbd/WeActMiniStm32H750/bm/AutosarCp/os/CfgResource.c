/**
 * @file
 * @brief AUTOSAR::CP::Os::Cfg::Resource
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include "bm/AutosarCp/os/CfgResource.h"

struct xwarcos_resource Rte_Resource_Object = XWARCOS_RESOURCE_INITIALIZER;

struct xwarcos_resource * const xwarcos_resource_table[] = {
        [0] = &Rte_Resource_Object,
};

const xwu32_t xwarcos_resource_num = xw_array_size(xwarcos_resource_table);
