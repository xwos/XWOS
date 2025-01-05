/**
 * @file
 * @brief xwos::Pm
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include "xwos/cxx/Pm.hxx"

namespace xwos {

/* Non-static Member */
Pm::Pm()
{
    xwos_pm_set_cb(sOnResume, sOnSuspend, sOnWakeup, sOnSleep, this);
}

Pm::~Pm()
{
}

/* Static Member */
void Pm::sOnResume(void * obj)
{
    Pm * mgr = reinterpret_cast<Pm *>(obj);
    mgr->onResume();
}

void Pm::sOnSuspend(void * obj)
{
    Pm * mgr = reinterpret_cast<Pm *>(obj);
    mgr->onSuspend();
}

void Pm::sOnWakeup(void * obj)
{
    Pm * mgr = reinterpret_cast<Pm *>(obj);
    mgr->onWakeup();
}

void Pm::sOnSleep(void * obj)
{
    Pm * mgr = reinterpret_cast<Pm *>(obj);
    mgr->onSleep();
}

} // namespace xwos
