#! /usr/bin/lua
---
-- @file
-- @brief Lua示例：条件量
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

-- 测试1：无锁
tstcond = xwos.cond.new()
xwxt["tstcond"] = tstcond
function childthd_main()
  local sp = xwos.cthd.sp()
  print("Child Thread:", sp)
  rc = xwxt.tstcond:wait_to()
  if (rc == 0) then
    print("[Child Thread] Wait condition ... OK")
  else
    print("[Child Thread] Wait condition ... error:", rc)
  end
end
print("Main Thread:", xwos.cthd.sp())
childthd = xwos.thd.call(childthd_main)
print("[Main Thread] Broadcast")
tstcond:broadcast()
xwos.cthd.sleep(xwtm.ms(100))
childthd:stop()
childthd = nil
xwxt.tstcond = nil
xwxt.gc()
tstcond = nil
collectgarbage()


-- 测试2：互斥锁
tstmtx = xwos.mtx.new()
tstcond = xwos.cond.new()
xwxt["tstcond"] = tstcond
xwxt["tstmtx"] = tstmtx
function childthd_main()
  print("Child Thread:", xwos.cthd.sp())
  local tstmtx = xwxt.tstmtx
  local tstcond = xwxt.tstcond
  rc = tstmtx:lock()
  if (rc == 0) then
    rc = tstcond:wait_to(tstmtx)
    if (rc == 0) then
      print("[Child Thread] Wait condition ... OK")
      print("[Child Thread] Lock state:", tstmtx:get_lkst())
    else
      print("[Child Thread] Wait condition ... error:", rc)
    end
  end
end
print("Main Thread:", xwos.cthd.sp())
childthd = xwos.thd.call(childthd_main)
print("[Main Thread] Broadcast")
tstmtx:lock()
tstcond:broadcast()
tstmtx:unlock()
xwos.cthd.sleep(xwtm.ms(100))
childthd:stop()
childthd = nil
xwxt.tstcond = nil
xwxt.tstmtx = nil
xwxt.gc()
tstcond = nil
tstmtx = nil
collectgarbage()


-- 测试3：等待超时
tstmtx = xwos.mtx.new()
tstcond = xwos.cond.new()
xwxt["tstcond"] = tstcond
xwxt["tstmtx"] = tstmtx
function childthd_main()
  print("Child Thread:", xwos.cthd.sp())
  local tstmtx = xwxt.tstmtx
  local tstcond = xwxt.tstcond
  rc = tstmtx:lock()
  if (rc == 0) then
    rc = tstcond:wait_to(tstmtx, xwtm.s(1))
    if (rc == 0) then
      print("[Child Thread] Wait condition ... OK")
    else
      print("[Child Thread] Wait condition ... error:", rc)
      print("[Child Thread] Lock state:", tstmtx:get_lkst())
    end
  end
end
print("Main Thread:", xwos.cthd.sp())
childthd = xwos.thd.call(childthd_main)
print("[Main Thread] Broadcast")
xwos.cthd.sleep(xwtm.s(2))
tstmtx:lock()
tstcond:broadcast()
tstmtx:unlock()
childthd:stop()
childthd = nil
xwxt.tstcond = nil
xwxt.tstmtx = nil
xwxt.gc()
tstcond = nil
tstmtx = nil
collectgarbage()
