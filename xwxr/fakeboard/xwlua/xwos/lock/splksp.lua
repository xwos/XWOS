--[[--------
XWLUA类：自旋锁对象强指针

自旋锁对象强指针是Lua语言中的一种 `userdata` ，用于**强引用**XWOS的自旋锁对象。

**强引用**表示会增加XWOS的自旋锁对象的**引用计数**。

@classmod splksp
]]


--[[--------
元方法：__copy<br>
<br>

将自旋锁对象强指针拷贝到全局导出表 `xwxt` 中。

此元方法意味着自旋锁对象的强引用多了一个，**引用计数**加1。

@within MetaMethods
@function splksp:metatable.__copy

@tparam userdata splksp (**in**) 自旋锁对象强指针<br>
<br>

@tparam userdata vm (**in**) 目标虚拟机<br>
<br>

@usage
splksp = ... -- 创建自旋锁的代码（省略）
xwxt.somesplk = splksp -- 自旋锁对象的引用计数加1
splksp2 = xwxt.somesplk -- 自旋锁对象的引用计数加1
xwxt.somesplk = nil -- 自旋锁对象的引用计数减1
]]


--[[--------
元方法：__gc<br>
<br>

自旋锁对象强指针的垃圾回收方法。

此元方法意味着自旋锁对象的强引用少了一个，**引用计数**减1。

如果自旋锁对象被销毁时还处于上锁状态，将自动被解锁。

@within MetaMethods
@function splksp:metatable.__gc

@tparam userdata splksp (**in**) 自旋锁对象强指针<br>
<br>

@usage
splksp = nil -- 删除引用
collectgarbage() -- 强制垃圾回收，将调用__gc()函数
]]


--[[--------
元方法：__close<br>
<br>

自旋锁对象强指针的**to-be-close**特性函数。当自旋锁对象强指针离开其作用域时，将自动解锁关联的自旋锁。

@within MetaMethods
@function splksp:metatable.__close

@tparam userdata splksp (**in**) 自旋锁对象强指针<br>
<br>

@usage
mysplk = xwos.splk.new()
function test()
  local scopelock<close> = mysplk
  scopelock:lock()
  -- 临界区
end -- 自动解锁自旋锁
]]


--[[--------
元方法：__tostring<br>
<br>

将自旋锁对象强指针格式化成字符串，可用于终端打印调试。

@within MetaMethods
@function splksp:metatable.__tostring

@tparam userdata splksp (**in**) 自旋锁对象强指针<br>
<br>

@usage
print(splksp)
]]


--[[--------
上锁自旋锁<br>
<br>

如果自旋锁无法上锁，就会阻塞当前线程。

@tparam userdata splksp (**in**) 自旋锁对象强指针<br>
<br>

@tparam string t (**optional**) (**in**)<br>
○ **"t"** 尝试上锁自旋锁，若无法上锁自旋锁，立即返回，不会自旋<br>
<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误
  + ○ **-EINVAL** 参数错误
  + ○ **-EAGAIN** 尝试上锁失败，仅当存在可选参数"t"时才会出现此错误值

@usage
lock = xwos.splk.new()
lock:lock()
-- 临界区
lock:unlock()

@usage
lock = xwos.splk.new()
rc = lock:lock("t")
if (rc == 0) then
  -- 临界区
  lock:unlock()
end
]]
function splksp:lock(splksp, t)
end


--[[--------
解锁自旋锁<br>
<br>

@tparam userdata splksp (**in**) 自旋锁对象强指针<br>
<br>

@usage
lock = xwos.splk.new()
lock:lock()
-- 临界区
lock:unlock()

@usage
lock = xwos.splk.new()
rc = lock:lock("t")
if (rc == 0) then
  -- 临界区
  lock:unlock()
end
]]
function splksp:unlock(splksp)
end
