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

print("Thread Sleep TEST!\n")
mythdsp = xwos.cthd.sp()
print("Thread:", mythdsp)
now = xwtm.now()
print(string.format("Current Yimetick:%f ms", now / xwtm.ms(1)))
print("sleep 1s...\n")
xwos.cthd.sleep(xwtm.s(1))
now = xwtm.now()
print(string.format("Sleep 1s from %f ms\n", now / xwtm.ms(1)))
xwos.cthd.sleep_from(now, xwtm.s(1))
now = xwtm.now()
print(string.format("Current Timetick:%f ms", now /xwtm.ms(1)))
