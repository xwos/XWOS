--[[--------
XWLUA类：顺序锁对象强指针

顺序锁对象强指针是Lua语言中的一种 `userdata` ，用于 **强引用** XWLUA的顺序锁对象。

**强引用** 表示会增加XWLUA的顺序锁对象的 **引用计数** 。

@classmod sqlksp
]]


--[[--------
元方法：__copy



将顺序锁对象强指针拷贝到全局导出表 `xwxt` 中。

此元方法意味着顺序锁对象的强引用多了一个， **引用计数** 加 **1** 。

@within MetaMethods
@function sqlksp:metatable.__copy

@tparam userdata sqlksp (**in**) 顺序锁对象强指针

@tparam userdata vm (**in**) 目标虚拟机

@usage
sqlksp = ... -- 创建顺序锁的代码（省略）
xwxt.somesqlk = sqlksp -- 顺序锁对象的引用计数加1
sqlksp2 = xwxt.somesqlk -- 顺序锁对象的引用计数加1
xwxt.somesqlk = nil -- 顺序锁对象的引用计数减1
]]


--[[--------
元方法：__gc



顺序锁对象强指针的垃圾回收方法。

此元方法意味着顺序锁对象的强引用少了一个， **引用计数** 减 **1** 。

如果顺序锁对象被销毁时还处于 **上锁状态** ，将自动被解锁。
**上锁状态** 是指处于 **独占写** 与 **独占读** 临界区，而 **非独占读** 临界区不属于 **上锁状态** 。

@within MetaMethods
@function sqlksp:metatable.__gc

@tparam userdata sqlksp (**in**) 顺序锁对象强指针

@usage
sqlksp = nil -- 删除引用
collectgarbage() -- 强制垃圾回收，将调用__gc()函数
]]


--[[--------
元方法：__close



顺序锁对象强指针具有 **to-be-close** 特性。当顺序锁对象强指针离开其作用域时，若关联的顺序锁还处于 **上锁状态** ，将自动解锁。
**上锁状态** 是指处于 **独占写** 与 **独占读** 临界区，而 **非独占读** 临界区不属于 **上锁状态** 。

@within MetaMethods
@function sqlksp:metatable.__close

@tparam userdata sqlksp (**in**) 顺序锁对象强指针


@usage
mysqlk = xwos.sqlk.new()
function test()
  local scopelock<close> = mysqlk
  scopelock:lock("wr")
  -- 临界区
end -- 自动解锁顺序锁
]]


--[[--------
元方法：__tostring



将顺序锁对象强指针格式化成字符串，可用于终端打印调试。

@within MetaMethods
@function sqlksp:metatable.__tostring

@tparam userdata sqlksp (**in**) 顺序锁对象强指针

@usage
print(sqlksp)
]]


--[[--------
开启读临界区



@tparam userdata sqlksp (**in**) 顺序锁对象强指针

@treturn number 锁当前的顺序值

@usage
lock = xwos.sqlk.new()
repeat
  seq = lock:rd_begin()
  -- 读临界区
  retry = lock:rd_retry(seq)
until (not retry)
]]
function sqlksp:rd_begin(sqlksp)
end


--[[--------
测试顺序值是否改变



@tparam userdata sqlksp (**in**) 顺序锁对象强指针

@tparam number seq (**in**) 顺序锁非独占读临界区开启时的顺序值

@treturn boolean 是否需要重试

@usage
lock = xwos.sqlk.new()
repeat
  seq = lock:rd_begin()
  -- 读临界区
  retry = lock:rd_retry(seq)
until (not retry)
]]
function sqlksp:rd_retry(sqlksp, seq)
end


--[[--------
获取顺序锁的顺序值



@tparam userdata sqlksp (**in**) 顺序锁对象强指针

@treturn userdata 锁的顺序值

@usage
seq = lock:get_seq()
]]
function sqlksp:get_seq(sqlksp)
end


--[[--------
开启写临界区



若顺序锁无法上锁，就自旋等待。

@tparam userdata sqlksp (**in**) 顺序锁对象强指针

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EINVAL** 参数错误

@usage
lock = xwos.sqlk.new()
lock:wr_lock()
-- 独占写临界区
lock:unlock()
]]
function sqlksp:wr_lock(sqlksp)
end


--[[--------
尝试开启写临界区



若顺序锁无法上锁，不会自旋等待。

@tparam userdata sqlksp (**in**) 顺序锁对象强指针

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EINVAL** 参数错误<br>
　● **-EAGAIN** 尝试上锁失败

@usage
lock = xwos.sqlk.new()
rc = lock:wr_trylock()
if (rc == 0) then
  -- 独占写临界区
  lock:unlock()
end
]]
function sqlksp:wr_trylock(sqlksp)
end


--[[--------
开启独占读临界区



若顺序锁无法上锁，就自旋等待。

@tparam userdata sqlksp (**in**) 顺序锁对象强指针

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EINVAL** 参数错误

@usage
lock = xwos.sqlk.new()
lock:rdex_lock()
-- 独占读临界区
lock:unlock()
]]
function sqlksp:rdex_lock(sqlksp)
end


--[[--------
尝试开启独占读临界区



若顺序锁无法上锁，不会自旋等待。

@tparam userdata sqlksp (**in**) 顺序锁对象强指针

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EINVAL** 参数错误<br>
　● **-EAGAIN** 尝试上锁失败

@usage
lock = xwos.sqlk.new()
rc = lock:rdex_trylock()
if (rc == 0) then
  -- 独占读临界区
  lock:unlock()
end
]]
function sqlksp:rdex_trylock(sqlksp)
end


--[[--------
解锁顺序锁



此方法会根据顺序锁上锁的类型（独占写或独占读），自动选择解锁方式。

@tparam userdata sqlksp (**in**) 顺序锁对象强指针

@usage
lock = xwos.sqlk.new()
lock:wr_lock()
-- 独占写临界区
lock:unlock()

@usage
lock = xwos.sqlk.new()
rc = lock:rdex_trylock()
if (rc == 0) then
  -- 独占读临界区
  lock:unlock()
end
]]
function sqlksp:unlock(sqlksp)
end


--[[--------
获取顺序锁状态



@tparam userdata sqlksp (**in**) 顺序锁对象强指针

@treturn number 锁状态<br>
　● **0** 未锁定<br>
　● **1** 独占读锁<br>
　● **2** 写锁

@usage
lock = xwos.sqlk.new()
rc = lock:lock()
if (rc == 0) then
  -- 临界区
  lkst = lock:get_lkst()
  lock:unlock()
end
]]
function sqlksp:get_lkst(sqlksp)
end
