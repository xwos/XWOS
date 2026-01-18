#! /usr/bin/lua
---
-- @file
-- @brief 示例：Lua示例脚本
-- @author
-- + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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

_G.null = false

print("XWLUA Test Script\n")
mythdsp = xwos.cthd.sp()
print("Thread strong pointer:", mythdsp)
print("pi =", math.pi)
print("2^11 =", 2 ^ 11)
print("exp(10) =", math.exp(10))

t1 = {1, 2, null, 4, 5}
t1["b"] = "b"
print(string.format("t1 = %d", #t1))
print(string.format("t1[%d] = %s", 5, t1[5]))
t2 = {6, 7}
t2["a"] = "a"
t2["c"] = "c"

function tbl_add(t1, t2)
  local t = {}
  for k, v in pairs(t1) do
    if (type(k) == "number") then
      table.insert(t, v)
    else
      t[k] = v
    end
  end
  for k, v in pairs(t2) do
    if (type(k) == "number") then
      table.insert(t, v)
    else
      t[k] = v
    end
  end
  return t
end

TblMetatable = {
  ["__add"] = tbl_add
}

setmetatable(t2, TblMetatable)

t3 = t1 + t2

for k, v in pairs(t3) do
  print(k, v);
end

return "OK"
