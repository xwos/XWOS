--[[--------
XWLUA类：事件标志对象强指针

事件标志对象强指针是Lua语言中的一种 `userdata` ，用于**强引用**XWOS的事件标志对象。

**强引用**表示会增加XWOS的事件标志对象的**引用计数**。

@classmod flgsp
]]


--[[--------
元方法：__copy<br>
<br>

将事件标志对象强指针拷贝到全局导出表 `xwxt` 中。

此元方法意味着事件标志对象的强引用多了一个，**引用计数**加1。

@within MetaMethods
@function flgsp:metatable.__copy

@tparam userdata flgsp (**in**) 事件标志对象强指针<br>
<br>

@tparam userdata vm (**in**) 目标虚拟机<br>
<br>

@usage
flgsp = ... -- 创建事件标志的代码（省略）
xwxt.someflg = flgsp -- 事件标志对象的引用计数加1
flgsp2 = xwxt.someflg -- 事件标志对象的引用计数加1
xwxt.someflg = nil -- 事件标志对象的引用计数减1
]]


--[[--------
元方法：__gc<br>
<br>

事件标志对象强指针的垃圾回收方法。

此元方法意味着事件标志对象的强引用少了一个，**引用计数**减1。

@within MetaMethods
@function flgsp:metatable.__gc

@tparam userdata flgsp (**in**) 事件标志对象强指针<br>
<br>

@usage
flgsp = nil -- 删除引用
collectgarbage() -- 强制垃圾回收，将调用__gc()函数
]]


--[[--------
元方法：__tostring<br>
<br>

将事件标志对象强指针格式化成字符串，可用于终端打印调试。

@within MetaMethods
@function flgsp:metatable.__tostring

@tparam userdata flgsp (**in**) 事件标志对象强指针<br>
<br>

@usage
print(flgsp)
]]


--[[--------
绑定事件标志对象到信号选择器<br>
<br>

@tparam userdata flgsp (**in**) 事件标志对象强指针<br>
<br>

@tparam userdata sel (**in**) 信号选择器对象的强指针<br>
<br>

@tparam number pos (**in**) 事件标志对象映射到信号选择器位图中的位置，位置从0开始编号<br>
<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误
  + ○ **-ECHRNG** 位置超出范围
  + ○ **-EALREADY** 同步对象已经绑定到事件对象
  + ○ **-EBUSY** 通道已经被其他同步对象独占

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
从信号选择器上解绑事件标志对象<br>
<br>

@tparam userdata flgsp (**in**) 事件标志对象强指针<br>
<br>

@tparam userdata sel (**in**) 信号选择器对象的强指针<br>
<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误
  + ○ **-ENOTCONN** 同步对象没有绑定到事件对象上

@usage
flg:unbind(sel)
]]
function flgsp:unbind(flgsp, sel)
end


--[[--------
新建位图，位的数量与事件标志中的事件数量相等<br>
<br>

@tparam userdata flgsp (**in**) 事件标志对象强指针<br>
<br>

@treturn userdata 位图

@usage
bmp = flg:bmp()
]]
function flgsp:bmp(flgsp)
end


--[[--------
获取事件标志中事件（位）的数量<br>
<br>

@tparam userdata flgsp (**in**) 事件标志对象强指针<br>
<br>

@treturn number 数量

@usage
num = flg:num()
]]
function flgsp:num(flgsp)
end


--[[--------
读取事件标志<br>
<br>

@tparam userdata flgsp (**in**) 事件标志对象强指针<br>
<br>

@treturn {number,userdata}
+ ● **rc** 返回值
  + ○ **0** 没有错误
+ ● **bmp** 事件标志位图

@usage
rc, bmp = flg:read()
]]
function flgsp:read(flgsp)
end


--[[--------
中断阻塞在事件标志中的所有线程<br>
<br>

@tparam userdata flgsp (**in**) 事件标志对象强指针<br>
<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误

@usage
rc = flg:intr_all()
]]
function flgsp:intr_all(flgsp)
end


--[[--------
同时设置多个事件标志位<br>
<br>

@tparam userdata flgsp (**in**) 事件标志对象强指针<br>
<br>

@tparam userdata msk (**in**) 事件的位图掩码<br>
<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误

@usage
msk = flg:bmp()
msk:zero()
msk:s1i(0, 1, 2, 3, 4, 5, 6, 7) -- msk == 0xFF
flg:s1m(msk) -- 设置事件1,2,3,4,5,6,7,8
]]
function flgsp:s1m(flgsp, msk)
end


--[[--------
设置单个事件标志位<br>
<br>

@tparam userdata flgsp (**in**) 事件标志对象强指针<br>
<br>

@tparam number pos (**in**) 事件的序号<br>
<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误

@usage
flg:s1i(1) -- 设置事件1
]]
function flgsp:s1i(flgsp, pos)
end


--[[--------
同时清除多个事件标志位<br>
<br>

@tparam userdata flgsp (**in**) 事件标志对象强指针<br>
<br>

@tparam userdata msk (**in**) 事件的位图掩码<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误

@usage
msk = flg:bmp()
msk:zero()
msk:s1i(0, 1, 2, 3, 4, 5, 6, 7) -- msk == 0xFF
flg:c0m(msk) -- 清除事件1,2,3,4,5,6,7,8
]]
function flgsp:c0m(flgsp, msk)
end


--[[--------
清除单个事件标志位<br>
<br>

@tparam userdata flgsp (**in**) 事件标志对象强指针<br>
<br>

@tparam number pos (**in**) 事件的序号<br>
<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误

@usage
flg:c0i(1) -- 清除事件1
]]
function flgsp:c0i(flgsp, pos)
end


--[[--------
同时翻转多个事件标志位<br>
<br>

@tparam userdata flgsp (**in**) 事件标志对象强指针<br>
<br>

@tparam userdata msk (**in**) 事件的位图掩码<br>
<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误

@usage
msk = flg:bmp()
msk:zero()
msk:s1i(0, 1, 2, 3, 4, 5, 6, 7) -- msk == 0xFF
flg:x1m(msk) -- 翻转事件1,2,3,4,5,6,7,8
]]
function flgsp:x1m(flgsp, msk)
end


--[[--------
翻转单个事件标志位<br>
<br>

@tparam userdata flgsp (**in**) 事件标志对象强指针<br>
<br>

@tparam number pos (**in**) 事件的序号<br>
<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误

@usage
flg:x1i(1) -- 翻转事件1
]]
function flgsp:x1i(flgsp, pos)
end


--[[--------
等待触发事件<br>
<br>

@tparam userdata flgsp (**in**) 事件标志对象强指针<br>
<br>

@tparam string trigger (**in**) 事件触发条件<br>
..● **"sa"** 所有事件位被置1触发<br>
..● **"so"** 任意事件位被置1触发<br>
..● **"ca"** 所有事件位被清0触发<br>
..● **"co"** 任意事件位被清0触发<br>
..● **"ta"** 所有事件位发生翻转触发<br>
..● **"to"** 任意事件位发生翻转触发<br>
<br>

@tparam boolean consumption (**in**) 是否消费事件<br>
..● **消费**事件是指，当线程等到事件被唤醒，可以选择是否**清除**事件标志。<br>
..● **清除**的含义是：<br>
....○ 当线程等待的是位图中的事件位被置1，**清除**是指将这些位清0；<br>
....○ 当线程等待的是位图中的事件位被清0，**清除**是指将这些位置1；<br>
..● 此参数对**trigger**取值 **"sa"**，**"so"**，**"ca"**，**"co"** 有效；<br>
..● 此参数对**trigger**取值 **"ta"**，**"to"**，无意义。<br>
<br>

@tparam userdata origin (**in,out**) 事件的状态位图<br>
..● 当参数trigger取值**"sa"**，**"so"**，**"ca"**，**"co"**时：(**out**) 返回触发时的事件状态位图<br>
..● 当参数trigger取值**"ta"**，**"to"**时：<br>
....○ (**in**) 用于比较的初始值<br>
....○ (**out**) 返回触发时的事件状态位图<br>
<br>

@tparam userdata msk (**in**) 事件的位图掩码，表示只关注掩码部分的事件<br>
<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误
  + ○ **-EINVAL** 参数错误
  + ○ **-EINTR** 等待被中断
  + ○ **-ENOTTHDCTX** 不在线程上下文中

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
检查事件标志<br>
<br>

若事件没有触发，立即返回，不会等待。

@tparam userdata flgsp (**in**) 事件标志对象强指针<br>
<br>

@tparam string trigger (**in**) 事件触发条件<br>
..● **"sa"** 所有事件位被置1触发<br>
..● **"so"** 任意事件位被置1触发<br>
..● **"ca"** 所有事件位被清0触发<br>
..● **"co"** 任意事件位被清0触发<br>
..● **"ta"** 所有事件位发生翻转触发<br>
..● **"to"** 任意事件位发生翻转触发<br>
<br>

@tparam boolean consumption (**in**) 是否消费事件<br>
..● **消费**事件是指，当线程等到事件被唤醒，可以选择是否**清除**事件标志。<br>
..● **清除**的含义是：<br>
....○ 当线程等待的是位图中的事件位被置1，**清除**是指将这些位清0；<br>
....○ 当线程等待的是位图中的事件位被清0，**清除**是指将这些位置1；<br>
..● 此参数对**trigger**取值 **"sa"**，**"so"**，**"ca"**，**"co"** 有效；<br>
..● 此参数对**trigger**取值 **"ta"**，**"to"**，无意义。<br>
<br>

@tparam userdata origin (**in,out**) 事件的状态位图<br>
..● 当参数trigger取值**"sa"**，**"so"**，**"ca"**，**"co"**时：(**out**) 返回触发时的事件状态位图<br>
..● 当参数trigger取值**"ta"**，**"to"**时：<br>
....○ (**in**) 用于比较的初始值<br>
....○ (**out**) 返回触发时的事件状态位图<br>
<br>

@tparam userdata msk (**in**) 事件的位图掩码，表示只关注掩码部分的事件<br>
<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误
  + ○ **-EINVAL** 参数错误
  + ○ **-EINTR** 等待被中断
  + ○ **-ENOTTHDCTX** 不在线程上下文中
  + ○ **-ENODATA** 尝试失败

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
限时等待事件标志对象的触发事件<br>
<br>

@tparam userdata flgsp (**in**) 事件标志对象强指针<br>
<br>

@tparam string trigger (**in**) 事件触发条件<br>
..● **"sa"** 所有事件位被置1触发<br>
..● **"so"** 任意事件位被置1触发<br>
..● **"ca"** 所有事件位被清0触发<br>
..● **"co"** 任意事件位被清0触发<br>
..● **"ta"** 所有事件位发生翻转触发<br>
..● **"to"** 任意事件位发生翻转触发<br>
<br>

@tparam boolean consumption (**in**) 是否消费事件<br>
..● **消费**事件是指，当线程等到事件被唤醒，可以选择是否**清除**事件标志。<br>
..● **清除**的含义是：<br>
....○ 当线程等待的是位图中的事件位被置1，**清除**是指将这些位清0；<br>
....○ 当线程等待的是位图中的事件位被清0，**清除**是指将这些位置1；<br>
..● 此参数对**trigger**取值 **"sa"**，**"so"**，**"ca"**，**"co"** 有效；<br>
..● 此参数对**trigger**取值 **"ta"**，**"to"**，无意义。<br>
<br>

@tparam userdata origin (**in,out**) 事件的状态位图<br>
..● 当参数trigger取值**"sa"**，**"so"**，**"ca"**，**"co"**时：(**out**) 返回触发时的事件状态位图<br>
..● 当参数trigger取值**"ta"**，**"to"**时：<br>
....○ (**in**) 用于比较的初始值<br>
....○ (**out**) 返回触发时的事件状态位图<br>
<br>

@tparam userdata msk (**in**) 事件的位图掩码，表示只关注掩码部分的事件<br>
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
