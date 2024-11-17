--[[--------
XWLUA类：互斥锁对象强指针

互斥锁对象强指针是Lua语言中的一种 `userdata` ，用于 **强引用** XWOS的互斥锁对象。

**强引用** 表示会增加XWOS的互斥锁对象的 **引用计数** 。

@classmod mtxsp
]]


--[[--------
元方法：__copy



将互斥锁对象强指针拷贝到全局导出表 `xwxt` 中。

此元方法意味着互斥锁对象的强引用多了一个，**引用计数** 加 **1** 。

@within MetaMethods
@function mtxsp:metatable.__copy

@tparam userdata mtxsp (**in**) 互斥锁对象强指针

@tparam userdata vm (**in**) 目标虚拟机

@usage
mtxsp = ... -- 创建互斥锁的代码（省略）
xwxt.somemtx = mtxsp -- 互斥锁对象的引用计数加1
mtxsp2 = xwxt.somemtx -- 互斥锁对象的引用计数加1
xwxt.somemtx = nil -- 互斥锁对象的引用计数减1
]]


--[[--------
元方法：__gc



互斥锁对象强指针的垃圾回收方法。

此元方法意味着互斥锁对象的强引用少了一个，**引用计数** 减 **1** 。

如果互斥锁对象被销毁时还处于上锁状态，将自动被解锁。

@within MetaMethods
@function mtxsp:metatable.__gc

@tparam userdata mtxsp (**in**) 互斥锁对象强指针

@usage
mtxsp = nil -- 删除引用
collectgarbage() -- 强制垃圾回收，将调用__gc()函数
]]


--[[--------
元方法：__close



互斥锁对象强指针的 **to-be-close** 特性函数。

当互斥锁对象强指针离开其作用域时，将自动解锁关联的互斥锁。

@within MetaMethods
@function mtxsp:metatable.__close

@tparam userdata mtxsp (**in**) 互斥锁对象强指针

@usage
mymtx = xwos.mtx.new()
function test()
  local scopelock<close> = mymtx
  rc = scopelock:lock()
  if (rc == 0) then
    -- 临界区
  end
end -- 自动解锁互斥锁
]]


--[[--------
元方法：__tostring



将互斥锁对象强指针格式化成字符串，可用于终端打印调试。

@within MetaMethods
@function mtxsp:metatable.__tostring

@tparam userdata mtxsp (**in**) 互斥锁对象强指针

@usage
print(mtxsp)
]]


--[[--------
等待上锁互斥锁



如果互斥锁无法上锁，就会阻塞当前线程。

@tparam userdata mtxsp (**in**) 互斥锁对象强指针

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EINTR** 等待被中断<br>
　● **-ENOTTHDCTX** 不在线程上下文中

@usage
lock = xwos.mtx.new()
rc = lock:lock()
if (rc == 0) then
  -- 临界区
  lock:unlock()
end
]]
function mtxsp:lock(mtxsp)
end


--[[--------
尝试上锁互斥锁



如果互斥锁无法上锁，不会阻塞调用线程。

@tparam userdata mtxsp (**in**) 互斥锁对象强指针

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EINTR** 等待被中断<br>
　● **-ENOTTHDCTX** 不在线程上下文中<br>
　● **-ENODATA** 尝试上锁失败

@usage
lock = xwos.mtx.new()
rc = lock:trylock() -- 尝试获取互斥锁，失败直接返回-ENODATA
if (rc == 0) then
  -- 临界区
  lock:unlock()
elseif (rc == -61) then
  print("Can't lock!")
end
]]
function mtxsp:trylock(mtxsp)
end


--[[--------
限时等待上锁互斥锁



如果互斥锁无法上锁，就会阻塞当前线程。

@tparam userdata mtxsp (**in**) 互斥锁对象强指针

@tparam number to (**in**) 期望唤醒的时间点

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EINTR** 等待被中断<br>
　● **-ENOTTHDCTX** 不在线程上下文中<br>
　● **-ETIMEDOUT** 超时

@usage
lock = xwos.mtx.new()
rc = lock:lock_to(xwtm.ft(xwtm.s(1))) -- 最多等待互斥锁1s，超时返回-ETIMEDOUT
if (rc == 0) then
  -- 临界区
  lock:unlock()
elseif (rc == -116) then
  print("Timeout!")
end
]]
function mtxsp:lock_to(mtxsp, to)
end


--[[--------
解锁互斥锁



@tparam userdata mtxsp (**in**) 互斥锁对象强指针

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EOWNER** 线程并没有锁定此互斥锁<br>
　● **-ENOTTHDCTX** 不在线程上下文中

@usage
lock = xwos.mtx.new()
rc = lock:lock()
if (rc == 0) then
  -- 临界区
  lock:unlock()
end
]]
function mtxsp:unlock(mtxsp)
end


--[[--------
获取互斥锁状态



@tparam userdata mtxsp (**in**) 互斥锁对象强指针

@treturn number 锁状态<br>
　● **0** 未锁定<br>
　● **1** 锁定

@usage
lock = xwos.mtx.new()
rc = lock:lock()
if (rc == 0) then
  -- 临界区
  lkst = lock:status()
  lock:unlock()
end
]]
function mtxsp:status(mtxsp)
end
