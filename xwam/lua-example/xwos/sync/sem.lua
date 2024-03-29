#! /usr/bin/lua
---
-- @file
-- @brief Lua示例脚本：信号量
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

-- 测试1：成功获取信号量
tstsem = xwos.sem.new(0, -1)
xwxt["tstsem"] = tstsem
function childthd_main()
  local sp = xwos.cthd.sp()
  local tstsem = xwxt.tstsem
  print("Child Thread:", sp)
  rc = tstsem:wait()
  if (rc == 0) then
    print("[Child Thread] Wait semaphore ... OK, value:", tstsem:get_value(), ", max:", tstsem:get_max())
  else
    print("[Child Thread] Wait semaphore ... error:", rc)
  end
end
print("Main Thread:", xwos.cthd.sp())
childthd = xwos.thd.call(childthd_main, true)
xwos.cthd.sleep(xwtm.ms(100))
tstsem:post()
childthd:stop()
xwxt.tstsem = nil
xwxt.gc()
tstsem = nil
collectgarbage()


-- 测试2：等待超时
tstsem = xwos.sem.new(0, -1)
xwxt["tstsem"] = tstsem
function childthd_main()
  local sp = xwos.cthd.sp()
  local tstsem = xwxt.tstsem
  print("Child Thread:", sp)
  rc = tstsem:wait_to(xwtm.ms(100))
  if (rc == 0) then
    print("[Child Thread] Wait semaphore ... OK, value:", tstsem:get_value(), ", max:", tstsem:get_max())
  else
    print("[Child Thread] Wait semaphore ... error:", rc, ",value:", tstsem:get_value(), ", max:", tstsem:get_max())
  end
end
print("Main Thread:", xwos.cthd.sp())
childthd = xwos.thd.call(childthd_main, true)
xwos.cthd.sleep(xwtm.ms(1000))
tstsem:post()
childthd:stop()
xwxt.tstsem = nil
xwxt.gc()
tstsem = nil
collectgarbage()


-- 测试3：等待中断
tstsem = xwos.sem.new(0, -1)
xwxt["tstsem"] = tstsem
function childthd_main()
  local sp = xwos.cthd.sp()
  local tstsem = xwxt.tstsem
  print("Child Thread:", sp)
  rc = tstsem:wait()
  if (rc == 0) then
    print("[Child Thread] Wait semaphore ... OK, value:", tstsem:get_value(), ", max:", tstsem:get_max())
  else
    print("[Child Thread] Wait semaphore ... error:", rc, ",value:", tstsem:get_value(), ", max:", tstsem:get_max())
  end
end
print("Main Thread:", xwos.cthd.sp())
childthd = xwos.thd.call(childthd_main, true)
xwos.cthd.sleep(xwtm.ms(1000))
childthd:intr()
childthd:stop()
xwxt.tstsem = nil
xwxt.gc()
tstsem = nil
collectgarbage()
