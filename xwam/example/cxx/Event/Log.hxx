/**
 * @file
 * @brief Event::Log
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > Licensed under the Apache License, Version 2.0 (the "License");
 * > you may not use this file except in compliance with the License.
 * > You may obtain a copy of the License at
 * >
 * >         http://www.apache.org/licenses/LICENSE-2.0
 * >
 * > Unless required by applicable law or agreed to in writing, software
 * > distributed under the License is distributed on an "AS IS" BASIS,
 * > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * > See the License for the specific language governing permissions and
 * > limitations under the License.
 */

#ifndef __xwam_example_cxx_Event_Log_hxx__
#define __xwam_example_cxx_Event_Log_hxx__

#include <xwos/standard.hxx>
#include <xwos/lib/xwlog.hxx>

namespace Event {

#ifndef LOGTAG
#  define LOGTAG "Event"
#endif

#if defined(EVENT_DEBUG)
#  define eventLogD(fmt, ...) xwlogf(D, LOGTAG, fmt, ##__VA_ARGS__)
#else
#  define eventLogD(fmt, ...)
#endif
#define eventLogI(fmt, ...) xwlogf(I, LOGTAG, fmt, ##__VA_ARGS__)
#define eventLogE(fmt, ...) xwlogf(E, LOGTAG, fmt, ##__VA_ARGS__)

} // namespace Event

#endif /* xwam/example/cxx/Event/Log.hxx */
