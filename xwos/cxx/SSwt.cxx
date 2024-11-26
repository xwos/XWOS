/**
 * @file
 * @brief xwos::SSwt
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include "xwos/cxx/SSwt.hxx"

namespace xwos {

/* Non-static Member */
SSwt::SSwt(xwsq_t flag)
{
    mCtorRc = xwos_swt_init(&mSwt, flag);
}

SSwt::~SSwt()
{
}

void SSwt::swtAlarmFunction()
{
}

/* Static Member */
void SSwt::sSwtAlarmFunction(struct xwos_swt * swt, SSwt * obj)
{
    XWOS_UNUSED(swt);
    obj->swtAlarmFunction();
}

} // namespace xwos
