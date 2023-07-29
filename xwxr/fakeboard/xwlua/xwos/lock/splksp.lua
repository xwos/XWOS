--[[--------
XWLUA类：自旋锁对象强指针

自旋锁对象强指针是Lua语言中的一种 `userdata` ，用于 **强引用** XWLUA的自旋锁对象。

**强引用** 表示会增加XWLUA的自旋锁对象的 **引用计数** 。

@classmod splksp
]]


--[[--------
元方法：__copy



将自旋锁对象强指针拷贝到全局导出表 `xwxt` 中。

此元方法意味着自旋锁对象的强引用多了一个，**引用计数** 加 **1** 。

@within MetaMethods
@function splksp:metatable.__copy

@tparam userdata splksp (**in**) 自旋锁对象强指针

@tparam userdata vm (**in**) 目标虚拟机

@usage
splksp = ... -- 创建自旋锁的代码（省略）
xwxt.somesplk = splksp -- 自旋锁对象的引用计数加1
splksp2 = xwxt.somesplk -- 自旋锁对象的引用计数加1
xwxt.somesplk = nil -- 自旋锁对象的引用计数减1
]]


--[[--------
元方法：__gc



自旋锁对象强指针的垃圾回收方法。

此元方法意味着自旋锁对象的强引用少了一个， **引用计数** 减 **1** 。

如果自旋锁对象被销毁时还处于上锁状态，将自动被解锁。

@within MetaMethods
@function splksp:metatable.__gc

@tparam userdata splksp (**in**) 自旋锁对象强指针

@usage
splksp = nil -- 删除引用
collectgarbage() -- 强制垃圾回收，将调用__gc()函数
]]


--[[--------
元方法：__close



自旋锁对象强指针具有 **to-be-close** 特性。当自旋锁对象强指针离开其作用域时，将自动解锁关联的自旋锁。

@within MetaMethods
@function splksp:metatable.__close

@tparam userdata splksp (**in**) 自旋锁对象强指针

@usage
mysplk = xwos.splk.new()
function test()
  local scopelock<close> = mysplk
  scopelock:lock()
  -- 临界区
end -- 自动解锁自旋锁
]]


--[[--------
元方法：__tostring



将自旋锁对象强指针格式化成字符串，可用于终端打印调试。

@within MetaMethods
@function splksp:metatable.__tostring

@tparam userdata splksp (**in**) 自旋锁对象强指针

@usage
print(splksp)
]]


--[[--------
上锁自旋锁



若自旋锁无法上锁，会自旋等待。

@tparam userdata splksp (**in**) 自旋锁对象强指针

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EINVAL** 参数错误

@usage
lock = xwos.splk.new()
lock:lock()
-- 临界区
lock:unlock()
]]
function splksp:lock(splksp)
end

--[[--------
尝试上锁自旋锁



若自旋锁无法上锁，不会自旋等待。

@tparam userdata splksp (**in**) 自旋锁对象强指针

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EINVAL** 参数错误<br>
　● **-EAGAIN** 尝试上锁失败

@usage
lock = xwos.splk.new()
rc = lock:trylock()
if (rc == 0) then
  -- 临界区
  lock:unlock()
end
]]
function splksp:trylock(splksp)
end


--[[--------
解锁自旋锁



@tparam userdata splksp (**in**) 自旋锁对象强指针

@usage
lock = xwos.splk.new()
rc = lock:trylock()
if (rc == 0) then
  -- 临界区
  lock:unlock()
end
]]
function splksp:unlock(splksp)
end


--[[--------
获取自旋锁状态



@tparam userdata splksp (**in**) 自旋锁对象强指针

@treturn number 锁状态<br>
　● **0** 未锁定<br>
　● **1** 锁定

@usage
lock = xwos.splk.new()
rc = lock:lock()
if (rc == 0) then
  -- 临界区
  lkst = lock:get_lkst()
  lock:unlock()
end
]]
function splksp:get_lkst(splksp)
end
