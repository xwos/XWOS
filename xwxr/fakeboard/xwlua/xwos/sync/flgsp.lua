--[[--------
XWLUA类：事件标志对象强指针

事件标志对象强指针是Lua语言中的一种 `userdata` ，用于 **强引用** XWOS的事件标志对象。

**强引用** 表示会增加XWOS的事件标志对象的 **引用计数** 。

@classmod flgsp
]]


--[[--------
元方法：__copy



将事件标志对象强指针拷贝到全局导出表 `xwxt` 中。

此元方法意味着事件标志对象的强引用多了一个， **引用计数** 加 **1** 。

@within MetaMethods
@function flgsp:metatable.__copy

@tparam userdata flgsp (**in**) 事件标志对象强指针

@tparam userdata vm (**in**) 目标虚拟机

@usage
flgsp = ... -- 创建事件标志的代码（省略）
xwxt.someflg = flgsp -- 事件标志对象的引用计数加1
flgsp2 = xwxt.someflg -- 事件标志对象的引用计数加1
xwxt.someflg = nil -- 事件标志对象的引用计数减1
]]


--[[--------
元方法：__gc



事件标志对象强指针的垃圾回收方法。

此元方法意味着事件标志对象的强引用少了一个， **引用计数** 减 **1** 。

@within MetaMethods
@function flgsp:metatable.__gc

@tparam userdata flgsp (**in**) 事件标志对象强指针

@usage
flgsp = nil -- 删除引用
collectgarbage() -- 强制垃圾回收，将调用__gc()函数
]]


--[[--------
元方法：__tostring



将事件标志对象强指针格式化成字符串，可用于终端打印调试。

@within MetaMethods
@function flgsp:metatable.__tostring

@tparam userdata flgsp (**in**) 事件标志对象强指针

@usage
print(flgsp)
]]


--[[--------
绑定事件标志对象到信号选择器



@tparam userdata flgsp (**in**) 事件标志对象强指针

@tparam userdata sel (**in**) 信号选择器对象的强指针

@tparam number pos (**in**) 事件标志对象映射到信号选择器位图中的位置，位置从 **0** 开始编号

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-ECHRNG** 位置超出范围<br>
　● **-EALREADY** 同步对象已经绑定到事件对象<br>
　● **-EBUSY** 通道已经被其他同步对象独占

@usage
flg = xwos.flg.new(32)
-- ...省略...
rc = flg:bind(sel, 3) -- 绑定到信号选择器的位置3
if (rc == 0) then
  -- 绑定成功
else
  -- 绑定失败
end
]]
function flgsp:bind(flgsp, sel, pos)
end


--[[--------
从信号选择器上解绑事件标志对象



@tparam userdata flgsp (**in**) 事件标志对象强指针

@tparam userdata sel (**in**) 信号选择器对象的强指针

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-ENOTCONN** 同步对象没有绑定到事件对象上

@usage
flg:unbind(sel)
]]
function flgsp:unbind(flgsp, sel)
end


--[[--------
新建位图，位的数量与事件标志中的事件数量相等



@tparam userdata flgsp (**in**) 事件标志对象强指针

@treturn userdata 位图

@usage
bmp = flg:bmp()
]]
function flgsp:bmp(flgsp)
end


--[[--------
获取事件标志中事件（位）的数量



@tparam userdata flgsp (**in**) 事件标志对象强指针

@treturn number 数量

@usage
num = flg:num()
]]
function flgsp:num(flgsp)
end


--[[--------
读取事件标志



@tparam userdata flgsp (**in**) 事件标志对象强指针

@treturn number 错误码<br>
　● **0** 没有错误

@treturn userdata 事件标志位图

@usage
rc, bmp = flg:read()
]]
function flgsp:read(flgsp)
end


--[[--------
同时设置多个事件标志位



@tparam userdata flgsp (**in**) 事件标志对象强指针

@tparam userdata msk (**in**) 事件的位图掩码

@treturn number 错误码<br>
　● **0** 没有错误

@usage
msk = flg:bmp()
msk:zero()
msk:s1i(0, 1, 2, 3, 4, 5, 6, 7) -- msk == 0xFF
flg:s1m(msk) -- 设置事件1,2,3,4,5,6,7,8
]]
function flgsp:s1m(flgsp, msk)
end


--[[--------
设置单个事件标志位



@tparam userdata flgsp (**in**) 事件标志对象强指针

@tparam number pos (**in**) 事件的序号

@treturn number 错误码<br>
　● **0** 没有错误

@usage
flg:s1i(1) -- 设置事件1
]]
function flgsp:s1i(flgsp, pos)
end


--[[--------
同时清除多个事件标志位



@tparam userdata flgsp (**in**) 事件标志对象强指针

@tparam userdata msk (**in**) 事件的位图掩码

@treturn number 错误码<br>
　● **0** 没有错误

@usage
msk = flg:bmp()
msk:zero()
msk:s1i(0, 1, 2, 3, 4, 5, 6, 7) -- msk == 0xFF
flg:c0m(msk) -- 清除事件1,2,3,4,5,6,7,8
]]
function flgsp:c0m(flgsp, msk)
end


--[[--------
清除单个事件标志位



@tparam userdata flgsp (**in**) 事件标志对象强指针

@tparam number pos (**in**) 事件的序号

@treturn number 错误码<br>
　● **0** 没有错误

@usage
flg:c0i(1) -- 清除事件1
]]
function flgsp:c0i(flgsp, pos)
end


--[[--------
同时翻转多个事件标志位



@tparam userdata flgsp (**in**) 事件标志对象强指针

@tparam userdata msk (**in**) 事件的位图掩码

@treturn number 错误码<br>
　● **0** 没有错误

@usage
msk = flg:bmp()
msk:zero()
msk:s1i(0, 1, 2, 3, 4, 5, 6, 7) -- msk == 0xFF
flg:x1m(msk) -- 翻转事件1,2,3,4,5,6,7,8
]]
function flgsp:x1m(flgsp, msk)
end


--[[--------
翻转单个事件标志位



@tparam userdata flgsp (**in**) 事件标志对象强指针

@tparam number pos (**in**) 事件的序号

@treturn number 错误码<br>
　● **0** 没有错误

@usage
flg:x1i(1) -- 翻转事件1
]]
function flgsp:x1i(flgsp, pos)
end


--[[--------
等待触发事件



@tparam userdata flgsp (**in**) 事件标志对象强指针

@tparam string trigger (**in**) 事件触发条件<br>
　● **"sa"** 所有事件位被置 **1** 触发<br>
　● **"so"** 任意事件位被置 **1** 触发<br>
　● **"ca"** 所有事件位被清 **0** 触发<br>
　● **"co"** 任意事件位被清 **0** 触发<br>
　● **"ta"** 所有事件位发生翻转触发<br>
　● **"to"** 任意事件位发生翻转触发

@tparam boolean consumption (**in**) 是否消费事件<br>
　● **消费** 事件是指，当线程等到事件被唤醒，可以选择是否 **清除** 事件标志。<br>
　● **清除** 的含义是：<br>
　　○ 当线程等待的是位图中的事件位被置 **1** ，**清除** 是指将这些位清 **0** ；<br>
　　○ 当线程等待的是位图中的事件位被清 **0** ，**清除** 是指将这些位置 **1** ；<br>
　● 此参数对 **trigger** 取值 **"sa"** ， **"so"** ， **"ca"** ， **"co"** 有效；<br>
　● 此参数对 **trigger** 取值 **"ta"** ， **"to"** ，无意义。

@tparam userdata origin (**in,out**) 事件的状态位图<br>
　● 当参数 **trigger** 取值 **"sa"** ， **"so"** ， **"ca"** ， **"co"** 时：<br>
　　○ (**out**) 返回事件触发 **之前** 的位图状态<br>
　● 当参数 **trigger** 取值 **"ta"** ， **"to"** 时：<br>
　　○ (**in**) 输入时，作为评估事件位图是否发生变化的初始状态<br>
　　○ (**out**) 输出时，返回触发后的事件标志位图状态（可作为下一次调用的初始值）

@tparam userdata msk (**in**) 事件的位图掩码，表示只关注掩码部分的事件

@treturn[1] number 返回值<br>
　● **0** 没有错误<br>
　● **-EINVAL** 参数错误<br>
　● **-EINTR** 等待被中断<br>
　● **-ENOTTHDCTX** 不在线程上下文中

@usage
flg = xwos.flg.new(32)
origin = flg:bmp();
msk = flg:bmp();
msk:zero();
msk:s1i(0, 1, 2, 3, 4, 5, 6, 7) -- 事件掩码：0xFF
rc = flg:wait("sa", true, origin, msk)
if (rc == 0) then
  -- 获取所有的事件1,2,3,4,5,6,7,8
  -- 消费事件，事件1,2,3,4,5,6,7,8会被清除
else
  -- 错误发生
end

@usage
flg = xwos.flg.new(32)
origin = flg:bmp();
msk = flg:bmp();
msk:zero();
msk:s1i(0, 1, 2, 3, 4, 5, 6, 7) -- 事件掩码：0xFF
rc = flg:wait("so", true, origin, msk)
if (rc == 0) then
  -- 获取事件1,2,3,4,5,6,7,8中的一个或多个
  -- 消费事件，事件1,2,3,4,5,6,7,8会被清除
else
  -- 错误发生
end

@usage
flg = xwos.flg.new(32)
origin = flg:bmp();
msk = flg:bmp();
msk:zero();
msk:s1i(0, 1, 2, 3, 4, 5, 6, 7) -- 事件掩码：0xFF
rc = flg:wait("ca", true, origin, msk)
if (rc == 0) then
  -- 事件1,2,3,4,5,6,7,8全部被清除
  -- 消费事件，事件1,2,3,4,5,6,7,8全被设置
else
  -- 错误发生
end

@usage
flg = xwos.flg.new(32)
origin = flg:bmp();
msk = flg:bmp();
msk:zero();
msk:s1i(0, 1, 2, 3, 4, 5, 6, 7) -- 事件掩码：0xFF
rc = flg:wait("co", true, origin, msk)
if (rc == 0) then
  -- 事件1,2,3,4,5,6,7,8中的一个或多个被清除
  -- 消费事件，事件1,2,3,4,5,6,7,8全被设置
else
  -- 错误发生
end

@usage
flg = xwos.flg.new(32)
origin = flg:bmp();
origin:zero();
origin:s1i(2, 4, 6, 8); -- 事件初始值：0xAA
msk = flg:bmp();
msk:zero();
msk:s1i(0, 1, 2, 3, 4, 5, 6, 7) -- 事件掩码：0xFF
rc = flg:wait("ta", true, origin, msk)
if (rc == 0) then
  -- 事件1,2,3,4,5,6,7,8全部发生了翻转
  -- consumption对此种类型的等待不起作用
  -- origin为翻转后的事件状态（0x55）
else
  -- 错误发生
end

@usage
flg = xwos.flg.new(32)
origin = flg:bmp();
origin:zero();
origin:s1i(2, 4, 6, 8); -- 事件初始值：0xAA
msk = flg:bmp();
msk:zero();
msk:s1i(0, 1, 2, 3, 4, 5, 6, 7) -- 事件掩码：0xFF
rc = flg:wait("to", true, origin, msk)
if (rc == 0) then
  -- 事件1,2,3,4,5,6,7,8部分发生了翻转
  -- consumption对此种类型的等待不起作用
  -- origin为翻转后的事件状态
else
  -- 错误发生
end
]]
function flgsp:wait(flgsp, trigger, consumption, origin, msk)
end


--[[--------
限时等待事件标志对象的触发事件



@tparam userdata flgsp (**in**) 事件标志对象强指针

@tparam string trigger (**in**) 事件触发条件<br>
　● **"sa"** 所有事件位被置 **1** 触发<br>
　● **"so"** 任意事件位被置 **1** 触发<br>
　● **"ca"** 所有事件位被清 **0** 触发<br>
　● **"co"** 任意事件位被清 **0** 触发<br>
　● **"ta"** 所有事件位发生翻转触发<br>
　● **"to"** 任意事件位发生翻转触发

@tparam boolean consumption (**in**) 是否消费事件<br>
　● **消费** 事件是指，当线程等到事件被唤醒，可以选择是否 **清除** 事件标志。<br>
　● **清除** 的含义是：<br>
　　○ 当线程等待的是位图中的事件位被置 **1** ， **清除** 是指将这些位清 **0** ；<br>
　　○ 当线程等待的是位图中的事件位被清 **0** ， **清除** 是指将这些位置 **1** ；<br>
　● 此参数对 **trigger** 取值 **"sa"** ， **"so"** ， **"ca"** ， **"co"** 有效；<br>
　● 此参数对 **trigger** 取值 **"ta"** ， **"to"** ，无意义。

@tparam userdata origin (**in,out**) 事件的状态位图<br>
　● 当参数 **trigger** 取值 **"sa"** ， **"so"** ， **"ca"** ， **"co"** 时：<br>
　　○ (**out**) 返回事件触发 **之前** 的位图状态<br>
　● 当参数 **trigger** 取值 **"ta"** ， **"to"** 时：<br>
　　○ (**in**) 输入时，作为评估事件位图是否发生变化的初始状态<br>
　　○ (**out**) 输出时，返回触发后的事件标志位图状态（可作为下一次调用的初始值）

@tparam userdata msk (**in**) 事件的位图掩码，表示只关注掩码部分的事件

@tparam number to (**in**) 期望的阻塞等待时间

@treturn number 返回值<br>
　● **0** 没有错误<br>
　● **-EINVAL** 参数错误<br>
　● **-EINTR** 等待被中断<br>
　● **-ENOTTHDCTX** 不在线程上下文中<br>
　● **-ETIMEDOUT** 超时

@usage
flg = xwos.flg.new(32)
origin = flg:bmp();
msk = flg:bmp();
msk:zero();
msk:s1i(0, 1, 2, 3, 4, 5, 6, 7) -- 事件掩码：0xFF
rc = flg:wait_to("ca", true, origin, msk, xwtm.ft(xwtm.s(1)))
if (rc == 0) then
  -- 事件1,2,3,4,5,6,7,8全部被清除
  -- 消费事件，事件1,2,3,4,5,6,7,8全被设置
else if (rc == -116)
  -- 超时
else
  -- 错误发生
end
]]
function flgsp:wait_to(flgsp, trigger, consumption, origin, msk, to)
end


--[[--------
检查事件标志



若事件没有触发，立即返回，不会等待。

@tparam userdata flgsp (**in**) 事件标志对象强指针

@tparam string trigger (**in**) 事件触发条件<br>
　● **"sa"** 所有事件位被置 **1** 触发<br>
　● **"so"** 任意事件位被置 **1** 触发<br>
　● **"ca"** 所有事件位被清 **0** 触发<br>
　● **"co"** 任意事件位被清 **0** 触发<br>
　● **"ta"** 所有事件位发生翻转触发<br>
　● **"to"** 任意事件位发生翻转触发

@tparam boolean consumption (**in**) 是否消费事件<br>
　● **消费** 事件是指，当线程等到事件被唤醒，可以选择是否 **清除** 事件标志。<br>
　● **清除** 的含义是：<br>
　　○ 当线程等待的是位图中的事件位被置 **1** ， **清除** 是指将这些位清 **0** ；<br>
　　○ 当线程等待的是位图中的事件位被清 **0** ， **清除** 是指将这些位置 **1** ；<br>
　● 此参数对 **trigger** 取值 **"sa"** ， **"so"** ， **"ca"** ， **"co"** 有效；<br>
　● 此参数对 **trigger** 取值 **"ta"** ， **"to"** ，无意义。

@tparam userdata origin (**in,out**) 事件的状态位图<br>
　● 当参数 **trigger** 取值 **"sa"** ， **"so"** ， **"ca"** ， **"co"** 时：<br>
　　○ (**out**) 返回事件触发 **之前** 的位图状态<br>
　● 当参数 **trigger** 取值 **"ta"** ， **"to"** 时：<br>
　　○ (**in**) 输入时，作为评估事件位图是否发生变化的初始状态<br>
　　○ (**out**) 输出时，返回触发后的事件标志位图状态（可作为下一次调用的初始值）

@tparam userdata msk (**in**) 事件的位图掩码，表示只关注掩码部分的事件

@treturn[1] number 返回值<br>
　● **0** 没有错误<br>
　● **-EINVAL** 参数错误<br>
　● **-EINTR** 等待被中断<br>
　● **-ENOTTHDCTX** 不在线程上下文中<br>
　● **-ENODATA** 尝试失败

@usage
flg = xwos.flg.new(32)
origin = flg:bmp();
msk = flg:bmp();
msk:zero();
msk:s1i(0, 1, 2, 3, 4, 5, 6, 7) -- 事件掩码：0xFF
rc = flg:trywait("sa", true, origin, msk)
if (rc == 0) then
  -- 获取所有的事件1,2,3,4,5,6,7,8
  -- 消费事件，事件1,2,3,4,5,6,7,8会被清除
else if (rc == -61)
  -- 检查失败
else
  -- 错误发生
end
]]
function flgsp:trywait(flgsp, trigger, consumption, origin, msk)
end


--[[--------
等待触发事件，且等待不可被中断



@tparam userdata flgsp (**in**) 事件标志对象强指针

@tparam string trigger (**in**) 事件触发条件<br>
　● **"sa"** 所有事件位被置 **1** 触发<br>
　● **"so"** 任意事件位被置 **1** 触发<br>
　● **"ca"** 所有事件位被清 **0** 触发<br>
　● **"co"** 任意事件位被清 **0** 触发<br>
　● **"ta"** 所有事件位发生翻转触发<br>
　● **"to"** 任意事件位发生翻转触发

@tparam boolean consumption (**in**) 是否消费事件<br>
　● **消费** 事件是指，当线程等到事件被唤醒，可以选择是否 **清除** 事件标志。<br>
　● **清除** 的含义是：<br>
　　○ 当线程等待的是位图中的事件位被置 **1** ，**清除** 是指将这些位清 **0** ；<br>
　　○ 当线程等待的是位图中的事件位被清 **0** ，**清除** 是指将这些位置 **1** ；<br>
　● 此参数对 **trigger** 取值 **"sa"** ， **"so"** ， **"ca"** ， **"co"** 有效；<br>
　● 此参数对 **trigger** 取值 **"ta"** ， **"to"** ，无意义。

@tparam userdata origin (**in,out**) 事件的状态位图<br>
　● 当参数 **trigger** 取值 **"sa"** ， **"so"** ， **"ca"** ， **"co"** 时：<br>
　　○ (**out**) 返回事件触发 **之前** 的位图状态<br>
　● 当参数 **trigger** 取值 **"ta"** ， **"to"** 时：<br>
　　○ (**in**) 输入时，作为评估事件位图是否发生变化的初始状态<br>
　　○ (**out**) 输出时，返回触发后的事件标志位图状态（可作为下一次调用的初始值）

@tparam userdata msk (**in**) 事件的位图掩码，表示只关注掩码部分的事件

@treturn[1] number 返回值<br>
　● **0** 没有错误<br>
　● **-EINVAL** 参数错误<br>
　● **-EINTR** 等待被中断<br>
　● **-ENOTTHDCTX** 不在线程上下文中

@usage
flg = xwos.flg.new(32)
origin = flg:bmp();
msk = flg:bmp();
msk:zero();
msk:s1i(0, 1, 2, 3, 4, 5, 6, 7) -- 事件掩码：0xFF
rc = flg:wait_unintr("sa", true, origin, msk)
if (rc == 0) then
  -- 获取所有的事件1,2,3,4,5,6,7,8
  -- 消费事件，事件1,2,3,4,5,6,7,8会被清除
else
  -- 错误发生
end

@usage
flg = xwos.flg.new(32)
origin = flg:bmp();
msk = flg:bmp();
msk:zero();
msk:s1i(0, 1, 2, 3, 4, 5, 6, 7) -- 事件掩码：0xFF
rc = flg:wait_unintr("so", true, origin, msk)
if (rc == 0) then
  -- 获取事件1,2,3,4,5,6,7,8中的一个或多个
  -- 消费事件，事件1,2,3,4,5,6,7,8会被清除
else
  -- 错误发生
end

@usage
flg = xwos.flg.new(32)
origin = flg:bmp();
msk = flg:bmp();
msk:zero();
msk:s1i(0, 1, 2, 3, 4, 5, 6, 7) -- 事件掩码：0xFF
rc = flg:wait_unintr("ca", true, origin, msk)
if (rc == 0) then
  -- 事件1,2,3,4,5,6,7,8全部被清除
  -- 消费事件，事件1,2,3,4,5,6,7,8全被设置
else
  -- 错误发生
end

@usage
flg = xwos.flg.new(32)
origin = flg:bmp();
msk = flg:bmp();
msk:zero();
msk:s1i(0, 1, 2, 3, 4, 5, 6, 7) -- 事件掩码：0xFF
rc = flg:wait_unintr("co", true, origin, msk)
if (rc == 0) then
  -- 事件1,2,3,4,5,6,7,8中的一个或多个被清除
  -- 消费事件，事件1,2,3,4,5,6,7,8全被设置
else
  -- 错误发生
end

@usage
flg = xwos.flg.new(32)
origin = flg:bmp();
origin:zero();
origin:s1i(2, 4, 6, 8); -- 事件初始值：0xAA
msk = flg:bmp();
msk:zero();
msk:s1i(0, 1, 2, 3, 4, 5, 6, 7) -- 事件掩码：0xFF
rc = flg:wait_unintr("ta", true, origin, msk)
if (rc == 0) then
  -- 事件1,2,3,4,5,6,7,8全部发生了翻转
  -- consumption对此种类型的等待不起作用
  -- origin为翻转后的事件状态（0x55）
else
  -- 错误发生
end

@usage
flg = xwos.flg.new(32)
origin = flg:bmp();
origin:zero();
origin:s1i(2, 4, 6, 8); -- 事件初始值：0xAA
msk = flg:bmp();
msk:zero();
msk:s1i(0, 1, 2, 3, 4, 5, 6, 7) -- 事件掩码：0xFF
rc = flg:wait_unintr("to", true, origin, msk)
if (rc == 0) then
  -- 事件1,2,3,4,5,6,7,8部分发生了翻转
  -- consumption对此种类型的等待不起作用
  -- origin为翻转后的事件状态
else
  -- 错误发生
end
]]
function flgsp:wait_unintr(flgsp, trigger, consumption, origin, msk)
end
