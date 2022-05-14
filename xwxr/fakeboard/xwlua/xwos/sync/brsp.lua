--[[--------
XWLUA类：线程栅栏对象强指针

线程栅栏对象强指针是Lua语言中的一种 `userdata` ，用于**强引用**XWOS的线程栅栏对象。

**强引用**表示会增加XWOS的线程栅栏对象的**引用计数**。

@classmod brsp
]]


--[[--------
元方法：__copy<br>
<br>

将线程栅栏对象强指针拷贝到全局导出表 `xwxt` 中。

此元方法意味着线程栅栏对象的强引用多了一个，**引用计数**加1。

@within MetaMethods
@function brsp:metatable.__copy

@tparam userdata brsp (**in**) 线程栅栏对象强指针<br>
<br>

@tparam userdata vm (**in**) 目标虚拟机<br>
<br>

@usage
brsp = ... -- 创建线程栅栏的代码（省略）
xwxt.somebr = brsp -- 线程栅栏对象的引用计数加1
brsp2 = xwxt.somebr -- 线程栅栏对象的引用计数加1
xwxt.somebr = nil -- 线程栅栏对象的引用计数减1
]]


--[[--------
元方法：__gc<br>
<br>

线程栅栏对象强指针的垃圾回收方法。

此元方法意味着线程栅栏对象的强引用少了一个，**引用计数**减1。

@within MetaMethods
@function brsp:metatable.__gc

@tparam userdata brsp (**in**) 线程栅栏对象强指针<br>
<br>

@usage
brsp = nil -- 删除引用
collectgarbage() -- 强制垃圾回收，将调用__gc()函数
]]


--[[--------
元方法：__tostring<br>
<br>

将线程栅栏对象强指针格式化成字符串，可用于终端打印调试。

@within MetaMethods
@function brsp:metatable.__tostring

@tparam userdata brsp (**in**) 线程栅栏对象强指针<br>
<br>

@usage
print(brsp)
]]


--[[--------
绑定线程栅栏对象到信号选择器<br>
<br>

@tparam userdata brsp (**in**) 线程栅栏对象强指针<br>
<br>

@tparam userdata sel (**in**) 信号选择器对象的强指针<br>
<br>

@tparam number pos (**in**) 线程栅栏对象映射到信号选择器位图中的位置，位置从0开始编号<br>
<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误
  + ○ **-ECHRNG** 位置超出范围
  + ○ **-EALREADY** 同步对象已经绑定到事件对象
  + ○ **-EBUSY** 通道已经被其他同步对象独占

@usage
br = xwos.br.new(32)
-- ...省略...
rc = br:bind(sel, 3) -- 绑定到线程栅栏的位置3
if (rc == 0) then
  -- 绑定成功
else
  -- 绑定失败
end
]]
function brsp:bind(brsp, sel, pos)
end


--[[--------
从信号选择器上解绑线程栅栏对象<br>
<br>

@tparam userdata brsp (**in**) 线程栅栏对象强指针<br>
<br>

@tparam userdata sel (**in**) 信号选择器对象的强指针<br>
<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误
  + ○ **-ENOTCONN** 同步对象没有绑定到事件对象上

@usage
br:unbind(sel)
]]
function brsp:unbind(brsp, sel)
end


--[[--------
新建位图，位的数量与线程栅栏中的线程槽数量相等<br>
<br>

@tparam userdata brsp (**in**) 线程栅栏对象强指针<br>
<br>

@treturn userdata 位图

@usage
bmp = br:bmp()
]]
function brsp:bmp(brsp)
end


--[[--------
获取线程栅栏中线程槽（位）的数量<br>
<br>

@tparam userdata brsp (**in**) 线程栅栏对象强指针<br>
<br>

@treturn number 数量

@usage
num = br:num()
]]
function brsp:num(brsp)
end


--[[--------
等待所有线程到达栅栏<br>
<br>

@tparam userdata brsp (**in**) 线程栅栏对象强指针<br>
<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误
  + ○ **-EINVAL** 参数错误
  + ○ **-EINTR** 等待被中断
  + ○ **-ENOTTHDCTX** 不在线程上下文中

@usage
br = xwos.br.new(4) -- 同步4个线程
-- ...省略...
rc = br:wait()
if (rc == 0) then
  -- 线程全部到达线程栅栏
else
  -- 错误发生
end
]]
function brsp:wait(brsp)
end


--[[--------
限时等待所有线程到达栅栏<br>
<br>

@tparam userdata brsp (**in**) 线程栅栏对象强指针<br>
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
br = xwos.br.new(4) -- 同步4个线程
-- ...省略...
rc = br:wait_to(xwtm.ft(xwtm.s(1))) -- 最多等待1s
if (rc == 0) then
  -- 线程全部到达线程栅栏
elseif (rc == -116)
  -- 超时
else
  -- 错误发生
end
]]
function brsp:wait_to(brsp, to)
end
