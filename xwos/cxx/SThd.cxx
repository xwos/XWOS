/**
 * @file
 * @brief xwos::SThd
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include "xwos/cxx/SThd.hxx"

namespace xwos {

/* Non-static Member */
SThd::SThd(const char * name,
           xwstk_t * stack, xwsz_t stack_size, xwsz_t stack_guard_size,
           xwpr_t priority, bool detached, bool privileged)
    : mThdDesc{nullptr, 0}
{
    struct xwos_thd_attr attr({
        .name = name,
        .stack = stack,
        .stack_size = stack_size,
        .stack_guard_size = stack_guard_size,
        .priority = priority,
        .detached = detached,
        .privileged = privileged,
    });
    mCtorRc = xwos_thd_init(&mThd, &mThdDesc, &attr, (xwos_thd_f)sThdMainFunction, this);
}

SThd::~SThd()
{
}

xwer_t SThd::thdMainFunction()
{
    return XWOK;
}

/* Static Member */
xwer_t SThd::sThdMainFunction(SThd * obj)
{
    return obj->thdMainFunction();
}

} // namespace xwos
