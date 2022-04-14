#! /usr/bin/lua
---
-- @file
-- @brief Lua示例脚本：线程睡眠
-- @author
-- + 隐星魂 (Roy Sun) <xwos@xwos.tech>
-- @copyright
-- + Copyright © 2015 xwos.tech, All Rights Reserved.
-- > Licensed under the Apache License, Version 2.0 (the "License");
-- > you may not use this file except in compliance with the License.
-- > You may obtain a copy of the License at
-- >
-- >         http://www.apache.org/licenses/LICENSE-2.0
-- >
-- > Unless required by applicable law or agreed to in writing, software
-- > distributed under the License is distributed on an "AS IS" BASIS,
-- > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
-- > See the License for the specific language governing permissions and
-- > limitations under the License.
--

_G.us = 1000
_G.ms = 1000000
_G.s = 1000000000

print("XWOS Sleep() TEST!\n")
mythdsp = xwos.cthd.sp()
print("Thread strong pointer:", mythdsp)
now = xwos.skd.now()
print(string.format("Current timetick:%f ms", now / ms))
print("sleep 1s...\n")
xwos.cthd.sleep(1 * s)
now = xwos.skd.now()
print(string.format("Sleep 1s from %f ms\n", now / ms))
xwos.cthd.sleepFrom(now, 1 * s)
now = xwos.skd.now()
print(string.format("Current Timetick:%f ms", now /ms))
