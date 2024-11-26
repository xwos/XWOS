/**
 * @file
 * @brief xwos::DThd
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include "xwos/cxx/DThd.hxx"

namespace xwos {

/* Non-static Member */
DThd::DThd(const char * name,
           xwsz_t stack_size, xwsz_t stack_guard_size,
           xwpr_t priority, bool detached, bool privileged)
    : mThdDesc{nullptr, 0}
{
    struct xwos_thd_attr attr({
        .name = name,
        .stack = NULL,
        .stack_size = stack_size,
        .stack_guard_size = stack_guard_size,
        .priority = priority,
        .detached = detached,
        .privileged = privileged,
    });
    mCtorRc = xwos_thd_create(&mThdDesc, &attr, (xwos_thd_f)sThdMainFunction, this);
}

DThd::~DThd()
{
}

xwer_t DThd::thdMainFunction()
{
    return XWOK;
}

/* Static Member */
xwer_t DThd::sThdMainFunction(DThd * obj)
{
    return obj->thdMainFunction();
}

} // namespace xwos
