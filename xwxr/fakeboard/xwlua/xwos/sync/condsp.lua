--[[--------
XWLUA类：条件量对象强指针

条件量对象强指针是Lua语言中的一种 `userdata` ，用于**强引用**XWOS的条件量对象。

**强引用**表示会增加XWOS的条件量对象的**引用计数**。

@classmod condsp
]]


--[[--------
元方法：__copy<br>
<br>

将条件量对象强指针拷贝到全局导出表 `xwxt` 中。

此元方法意味着条件量对象的强引用多了一个，**引用计数**加1。

@within MetaMethods
@function condsp:metatable.__copy

@tparam userdata condsp (**in**) 条件量对象强指针<br>
<br>

@tparam userdata vm (**in**) 目标虚拟机<br>
<br>

@usage
condsp = ... -- 创建条件量的代码（省略）
xwxt.somecond = condsp -- 条件量对象的引用计数加1
condsp2 = xwxt.somecond -- 条件量对象的引用计数加1
xwxt.somecond = nil -- 条件量对象的引用计数减1
]]


--[[--------
元方法：__gc<br>
<br>

条件量对象强指针的垃圾回收方法。

此元方法意味着条件量对象的强引用少了一个，**引用计数**减1。

@within MetaMethods
@function condsp:metatable.__gc

@tparam userdata condsp (**in**) 条件量对象强指针<br>
<br>

@usage
condsp = nil -- 删除引用
collectgarbage() -- 强制垃圾回收，将调用__gc()函数
]]


--[[--------
元方法：__tostring<br>
<br>

将条件量对象强指针格式化成字符串，可用于终端打印调试。

@within MetaMethods
@function condsp:metatable.__tostring

@tparam userdata condsp (**in**) 条件量对象强指针<br>
<br>

@usage
print(condsp)
]]


--[[--------
绑定条件量对象到信号选择器<br>
<br>

@tparam userdata condsp (**in**) 条件量对象强指针<br>
<br>

@tparam userdata sel (**in**) 信号选择器对象的强指针<br>
<br>

@tparam number pos (**in**) 条件量对象映射到信号选择器位图中的位置，位置从0开始编号<br>
<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误
  + ○ **-ECHRNG** 位置超出范围
  + ○ **-EALREADY** 同步对象已经绑定到事件对象
  + ○ **-EBUSY** 通道已经被其他同步对象独占

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
从信号选择器上解绑条件量对象<br>
<br>

@tparam userdata condsp (**in**) 条件量对象强指针<br>
<br>

@tparam userdata sel (**in**) 信号选择器对象的强指针<br>
<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误
  + ○ **-ENOTCONN** 同步对象没有绑定到事件对象上

@usage
cond:unbind(sel)
]]
function condsp:unbind(condsp, sel)
end


--[[--------
冻结条件量<br>
<br>

@tparam userdata condsp (**in**) 条件量对象强指针<br>
<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误
  + ○ **-EALREADY** 条件量对象已被冻结
@usage
cond:freeze()
]]
function condsp:freeze(condsp)
end


--[[--------
解冻条件量<br>
<br>

@tparam userdata condsp (**in**) 条件量对象强指针<br>
<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误
  + ○ **-EALREADY** 条件量对象已被冻结
@usage
cond:thaw()
]]
function condsp:thaw(condsp)
end


--[[--------
单播条件量对象，只会唤醒第一个线程<br>
<br>

@tparam userdata condsp (**in**) 条件量对象强指针<br>
<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误
  + ○ **-ENEGATIVE** 条件量对象已被冻结
@usage
cond:unicast()
]]
function condsp:unicast(condsp)
end


--[[--------
广播条件量对象，等待队列中的所有线程都会被唤醒<br>
<br>

@tparam userdata condsp (**in**) 条件量对象强指针<br>
<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误
  + ○ **-ENEGATIVE** 条件量对象已被冻结
@usage
cond:broadcast()
]]
function condsp:broadcast(condsp)
end


--[[--------
等待并获取条件量<br>
<br>

+ 当等待成功返回0时，将自动获得锁；
+ 当等待失败返回错误码时，将不会获得锁。

@tparam userdata condsp (**in**) 条件量对象强指针<br>
<br>

@tparam userdata lock (**in**) 锁对象的强引用指针<br>
..● **splksp** 自旋锁<br>
..● **sqlksp** 顺序锁<br>
..● **mtxsp** 互斥锁<br>
<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误
  + ○ **-EINVAL** 参数错误
  + ○ **-EINTR** 等待被中断
  + ○ **-ENOTTHDCTX** 不在线程上下文中

@usage
cond = xwos.cond.new()
rc = cond:wait()
if (rc == 0) then
  -- 获取到一个信号
else
  -- 错误发生
end
]]
function condsp:wait(condsp, lock)
end


--[[--------
限时阻塞当前线程，直到被条件量唤醒<br>
<br>

+ 当等待成功返回0时，将自动获得锁；
+ 当等待失败返回错误码时，将不会获得锁。

@tparam userdata condsp (**in**) 条件量对象强指针<br>
<br>

@tparam userdata lock (**in**) 锁对象的强引用指针<br>
..● **splksp** 自旋锁<br>
..● **sqlksp** 顺序锁<br>
..● **mtxsp** 互斥锁<br>
<br>

@tparam number to (**in**) 期望的阻塞等待时间<br>
<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误
  + ○ **-EINVAL** 参数错误
  + ○ **-EINTR** 等待被中断
  + ○ **-ENOTTHDCTX** 不在线程上下文中
  + ○ **-ETIMEDOUT** 超时

@usage
cond = xwos.cond.new()
rc = cond:wait(xwtm.ft(xwt,.s(1))) -- 最多等待1s
if (rc == 0) then
  -- 获取到一个信号
elseif (rc == -116) then
  -- 超时
else
  -- 错误发生
end
]]
function condsp:wait_to(condsp, lock, to)
end


--[[--------
阻塞当前线程，直到被条件量唤醒，且阻塞不可被中断<br>
<br>

+ 当等待成功返回0时，将自动获得锁；
+ 当等待失败返回错误码时，将不会获得锁。

@tparam userdata condsp (**in**) 条件量对象强指针<br>
<br>

@tparam userdata lock (**in**) 锁对象的强引用指针<br>
..● **splksp** 自旋锁<br>
..● **sqlksp** 顺序锁<br>
..● **mtxsp** 互斥锁<br>
<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误
  + ○ **-EINVAL** 参数错误
  + ○ **-ENOTTHDCTX** 不在线程上下文中

@usage
cond = xwos.cond.new()
rc = cond:wait(xwtm.ft(xwt,.s(1))) -- 最多等待1s
if (rc == 0) then
  -- 获取到一个信号
elseif (rc == -116) then
  -- 超时
else
  -- 错误发生
end
]]
function condsp:wait_unintr(condsp, lock)
end
