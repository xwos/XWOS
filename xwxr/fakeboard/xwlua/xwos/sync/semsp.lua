--[[--------
XWLUA类：信号量对象强指针

信号量对象强指针是Lua语言中的一种 `userdata` ，用于 **强引用** XWOS的信号量对象。

**强引用** 表示会增加XWOS的信号量对象的 **引用计数** 。

@classmod semsp
]]


--[[--------
元方法：__copy



将信号量对象强指针拷贝到全局导出表 `xwxt` 中。

此元方法意味着信号量对象的强引用多了一个， **引用计数** 加 **1** 。

@within MetaMethods
@function semsp:metatable.__copy

@tparam userdata semsp (**in**) 信号量对象强指针

@tparam userdata vm (**in**) 目标虚拟机

@usage
semsp = ... -- 创建信号量（代码省略）
xwxt.somesem = semsp -- 信号量对象的引用计数加1
semsp2 = xwxt.somesem -- 信号量对象的引用计数加1
xwxt.somesem = nil -- 信号量对象的引用计数减1
]]


--[[--------
元方法：__gc



信号量对象强指针的垃圾回收方法。

此元方法意味着信号量对象的强引用少了一个， **引用计数** 减 **1** 。

@within MetaMethods
@function semsp:metatable.__gc

@tparam userdata semsp (**in**) 信号量对象强指针

@usage
semsp = nil -- 删除引用
collectgarbage() -- 强制垃圾回收，将调用__gc()函数
]]


--[[--------
元方法：__tostring



将信号量对象强指针格式化成字符串，可用于终端打印调试。

@within MetaMethods
@function semsp:metatable.__tostring

@tparam userdata semsp (**in**) 信号量对象强指针

@usage
print(semsp)
]]


--[[--------
绑定信号量对象到信号选择器



@tparam userdata semsp (**in**) 信号量对象强指针

@tparam userdata sel (**in**) 信号选择器对象的强指针

@tparam number pos (**in**) 信号量对象映射到信号选择器位图中的位置，位置从 **0** 开始编号

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-ECHRNG** 位置超出范围<br>
　● **-EALREADY** 同步对象已经绑定到事件对象<br>
　● **-EBUSY** 通道已经被其他同步对象独占

@usage
sem = xwos.sem.new(0, 100)
-- ...省略...
rc = sem:bind(sel, 3) -- 绑定到信号选择器的位置3
if (rc == 0) then
  -- 绑定成功
else
  -- 绑定失败
end
]]
function semsp:bind(semsp, sel, pos)
end


--[[--------
从信号选择器上解绑信号量对象



@tparam userdata semsp (**in**) 信号量对象强指针

@tparam userdata sel (**in**) 信号选择器对象的强指针

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-ENOTCONN** 同步对象没有绑定到事件对象上

@usage
sem:unbind(sel)
]]
function semsp:unbind(semsp, sel)
end


--[[--------
冻结信号量



@tparam userdata semsp (**in**) 信号量对象强指针

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EALREADY** 信号量对象已被冻结

@usage
sem:freeze()
]]
function semsp:freeze(semsp)
end


--[[--------
解冻信号量



@tparam userdata semsp (**in**) 信号量对象强指针

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EALREADY** 信号量对象已被冻结

@usage
sem:thaw()
]]
function semsp:thaw(semsp)
end


--[[--------
发布信号量



@tparam userdata semsp (**in**) 信号量对象强指针

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-ENEGATIVE** 信号量对象已被冻结<br>
　● **-ERANGE** 信号量对象的值已经最大

@usage
sem:post()
]]
function semsp:post(semsp)
end


--[[--------
等待并获取信号



@tparam userdata semsp (**in**) 信号量对象强指针

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EINVAL** 参数错误<br>
　● **-EINTR** 等待被中断<br>
　● **-ENOTTHDCTX** 不在线程上下文中

@usage
sem = xwos.sem.new(0, 0)
rc = sem:wait()
if (rc == 0) then
  -- 获取到一个信号
else
  -- 错误发生
end
]]
function semsp:wait(semsp)
end


--[[--------
检查信号量对象



若没有检测到信号，立即返回，不会阻塞调用线程。

@tparam userdata semsp (**in**) 信号量对象强指针

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EINVAL** 参数错误<br>
　● **-EINTR** 等待被中断<br>
　● **-ENOTTHDCTX** 不在线程上下文中<br>
　● **-ENODATA** 尝试失败

@usage
sem = xwos.sem.new(0, 0)
rc = sem:trywait() -- 最多等待1s
if (rc == 0) then
  -- 获取到一个信号
elseif (rc == -61) then
  -- 没有检测到信号
else
  -- 错误发生
end
]]
function semsp:trywait(semsp)
end


--[[--------
限时等待并获取信号量



@tparam userdata semsp (**in**) 信号量对象强指针

@tparam number to (**in**) 期望的阻塞等待时间

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EINVAL** 参数错误<br>
　● **-EINTR** 等待被中断<br>
　● **-ENOTTHDCTX** 不在线程上下文中<br>
　● **-ETIMEDOUT** 超时

@usage
sem = xwos.sem.new(0, 0)
rc = sem:wait(xwtm.ft(xwtm.s(1))) -- 最多等待1s
if (rc == 0) then
  -- 获取到一个信号
elseif (rc == -116) then
  -- 超时
else
  -- 错误发生
end
]]
function semsp:wait_to(semsp, to)
end


--[[--------
获取信号量的值



@tparam userdata semsp (**in**) 自旋锁对象强指针

@treturn number 信号量的值

@usage
sem = xwos.sem.new(1, 0)
value = sem:get_value()
]]
function semsp:get_value(semsp)
end
