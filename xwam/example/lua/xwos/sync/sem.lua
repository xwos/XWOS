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

tstsem = xwos.sem.new(0, -1)
xwxt["tstsem"] = tstsem
function semthd_main()
  rc = xwxt.tstsem:wait()
  if (rc == 0) then
    print("Wait semaphore ... OK")
  else
    print("Wait semaphore ... error:", rc)
  end
end
semthd = xwos.thd.call(semthd_main)
tstsem:post()
semthd:stop()
xwxt.tstsem = nil
xwxt.gc()
tstsem = nil
collectgarbage()
