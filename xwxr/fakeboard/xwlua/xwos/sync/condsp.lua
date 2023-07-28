--[[--------
XWLUA类：条件量对象强指针

条件量对象强指针是Lua语言中的一种 `userdata` ，用于 **强引用** XWOS的条件量对象。

**强引用** 表示会增加XWOS的条件量对象的 **引用计数** 。

@classmod condsp
]]


--[[--------
元方法：__copy



将条件量对象强指针拷贝到全局导出表 `xwxt` 中。

此元方法意味着条件量对象的强引用多了一个， **引用计数** 加 **1** 。

@within MetaMethods
@function condsp:metatable.__copy

@tparam userdata condsp (**in**) 条件量对象强指针

@tparam userdata vm (**in**) 目标虚拟机

@usage
condsp = ... -- 创建条件量的代码（省略）
xwxt.somecond = condsp -- 条件量对象的引用计数加1
condsp2 = xwxt.somecond -- 条件量对象的引用计数加1
xwxt.somecond = nil -- 条件量对象的引用计数减1
]]


--[[--------
元方法：__gc



条件量对象强指针的垃圾回收方法。

此元方法意味着条件量对象的强引用少了一个， **引用计数** 减 **1** 。

@within MetaMethods
@function condsp:metatable.__gc

@tparam userdata condsp (**in**) 条件量对象强指针

@usage
condsp = nil -- 删除引用
collectgarbage() -- 强制垃圾回收，将调用__gc()函数
]]


--[[--------
元方法：__tostring



将条件量对象强指针格式化成字符串，可用于终端打印调试。

@within MetaMethods
@function condsp:metatable.__tostring

@tparam userdata condsp (**in**) 条件量对象强指针

@usage
print(condsp)
]]


--[[--------
绑定条件量对象到信号选择器



@tparam userdata condsp (**in**) 条件量对象强指针

@tparam userdata sel (**in**) 信号选择器对象的强指针

@tparam number pos (**in**) 条件量对象映射到信号选择器位图中的位置，位置从 **0** 开始编号

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-ECHRNG** 位置超出范围<br>
　● **-EALREADY** 同步对象已经绑定到事件对象<br>
　● **-EBUSY** 通道已经被其他同步对象独占

@usage
cond = xwos.cond.new(0, 100)
rc = cond:bind(sel, 3) -- 绑定到信号选择器的位置3
if (rc == 0) then
  -- 绑定成功
else
  -- 绑定失败
end
]]
function condsp:bind(condsp, sel, pos)
end


--[[--------
从信号选择器上解绑条件量对象



@tparam userdata condsp (**in**) 条件量对象强指针

@tparam userdata sel (**in**) 信号选择器对象的强指针

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-ENOTCONN** 同步对象没有绑定到事件对象上

@usage
cond:unbind(sel)
]]
function condsp:unbind(condsp, sel)
end


--[[--------
冻结条件量



@tparam userdata condsp (**in**) 条件量对象强指针

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EALREADY** 条件量对象已被冻结

@usage
cond:freeze()
]]
function condsp:freeze(condsp)
end


--[[--------
解冻条件量



@tparam userdata condsp (**in**) 条件量对象强指针

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EALREADY** 条件量对象已被冻结

@usage
cond:thaw()
]]
function condsp:thaw(condsp)
end


--[[--------
单播条件量对象，只会唤醒第一个线程



@tparam userdata condsp (**in**) 条件量对象强指针

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-ENEGATIVE** 条件量对象已被冻结

@usage
cond:unicast()
]]
function condsp:unicast(condsp)
end


--[[--------
广播条件量对象，等待队列中的所有线程都会被唤醒



@tparam userdata condsp (**in**) 条件量对象强指针

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-ENEGATIVE** 条件量对象已被冻结

@usage
cond:broadcast()
]]
function condsp:broadcast(condsp)
end


--[[--------
等待并获取条件量



条件量的等待，可以附加 **锁** 操作：<br>
　● 当线程进入 **等待** 状态时，自动解锁；<br>
　● 当条件成立，成功返回 **0** 时，自动上锁；<br>
　● 当等待失败，返回错误码时，不会上锁。

@tparam userdata condsp (**in**) 条件量对象强指针

@tparam userdata lock (**optional**) (**in**) 锁对象的强引用指针<br>
　● **splksp** 自旋锁<br>
　● **sqlksp** 顺序锁<br>
　● **mtxsp** 互斥锁<br>
　● **nil** 或 **空** 无锁

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EINVAL** 参数错误<br>
　● **-EINTR** 等待被中断<br>
　● **-ENOTTHDCTX** 不在线程上下文中

@usage
mycond = xwos.cond.new()
rc = mycond:wait()
if (rc == 0) then
  -- 条件成立
elseif (rc == -4) then
  -- 等待被中断
else
  -- 其他错误发生
end

@usage
mylock = xwos.splk.new()
mycond = xwos.cond.new()
mylock:lock()
-- 临界区
rc = mycond:wait(mylock)
if (rc == 0) then
  -- 条件成立， `mylock` 已上锁
  -- 临界区
  mylock:unlock()
elseif (rc == -4) then
  -- 等待被中断， `mylock` 未上锁
else
  -- 其他错误发生， `mylock` 未上锁
end
]]
function condsp:wait(condsp, lock)
end


--[[--------
限时阻塞当前线程，直到被条件量唤醒



条件量的等待，可以附加 **锁** 操作：<br>
　● 当线程进入 **等待** 状态时，自动解锁；<br>
　● 当条件成立，成功返回 **0** 时，自动上锁；<br>
　● 当等待失败，返回错误码时，不会上锁。

@tparam userdata condsp (**in**) 条件量对象强指针

@tparam userdata lock (**in**) 锁对象的强引用指针<br>
　● **splksp** 自旋锁<br>
　● **sqlksp** 顺序锁<br>
　● **mtxsp** 互斥锁<br>
　● **nil** 无锁

@tparam number to (**in**) 期望的阻塞等待时间

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EINVAL** 参数错误<br>
　● **-EINTR** 等待被中断<br>
　● **-ENOTTHDCTX** 不在线程上下文中<br>
　● **-ETIMEDOUT** 超时

@usage
mycond = xwos.cond.new()
rc = mycond:wait_to(nil, xwtm.ft(xwt,.s(1))) -- 不包括锁操作，最多等待1s
if (rc == 0) then
  -- 条件成立
elseif (rc == -116) then
  -- 超时
elseif (rc == -4) then
  -- 等待被中断
else
  -- 其他错误发生
end

@usage
mymtx = xwos.mtx.new()
mycond = xwos.cond.new()
mymtx:lock()
-- 临界区
rc = mycond:wait_to(mymtx, xwtm.ft(xwt,.s(1))) -- 包括互斥锁操作，最多等待1s
if (rc == 0) then
  -- 条件成立， `mymtx` 已上锁
  -- 临界区
  mymtx:unlock()
elseif (rc == -4) then
  -- 等待被中断， `mymtx` 未上锁
elseif (rc == -116) then
  -- 超时， `mymtx` 未上锁
else
  -- 其他错误发生， `mymtx` 未上锁
end
]]
function condsp:wait_to(condsp, lock, to)
end


--[[--------
阻塞当前线程，直到被条件量唤醒，且阻塞不可被中断



条件量的等待，可以附加 **锁** 操作：<br>
　● 当线程进入 **等待** 状态时，自动解锁；<br>
　● 当条件成立，成功返回 **0** 时，自动上锁；<br>
　● 当等待失败，返回错误码时，不会上锁。

@tparam userdata condsp (**in**) 条件量对象强指针

@tparam userdata lock (**optional**) (**in**) 锁对象的强引用指针<br>
　● **splksp** 自旋锁<br>
　● **sqlksp** 顺序锁<br>
　● **mtxsp** 互斥锁<br>
　● **nil** 或 **空** 无锁

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EINVAL** 参数错误<br>
　● **-ENOTTHDCTX** 不在线程上下文中

@usage
mycond = xwos.cond.new()
rc = mycond:wait_unintr()
if (rc == 0) then
  -- 条件成立
else
  -- 其他错误发生
end

@usage
mylock = xwos.sqlk.new()
mycond = xwos.cond.new()
mylock:wr_lock()
-- 写临界区
rc = mycond:wait_unintr(mylock)
if (rc == 0) then
  -- 条件成立， `mylock` 已上写锁
  -- 写临界区
  mylock:wr_unlock()
else
  -- 其他错误发生， `mylock` 未写锁
end
]]
function condsp:wait_unintr(condsp, lock)
end
