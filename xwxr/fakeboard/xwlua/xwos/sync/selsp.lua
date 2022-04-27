--[[--------
XWLUA类：信号选择器对象强指针

信号选择器对象强指针是Lua语言中的一种 `userdata` ，用于**强引用**XWOS的信号选择器对象。

**强引用**表示会增加XWOS的信号选择器对象的**引用计数**。

@classmod selsp
]]


--[[--------
元方法：__copy<br>
<br>

将信号选择器对象强指针拷贝到全局导出表 `xwxt` 中。

此元方法意味着信号选择器对象的强引用多了一个，**引用计数**加1。

@within MetaMethods
@function selsp:metatable.__copy

@tparam userdata selsp (**in**) 信号选择器对象强指针<br>
<br>

@tparam userdata vm (**in**) 目标虚拟机<br>
<br>

@usage
selsp = ... -- 创建信号选择器的代码（省略）
xwxt.somesel = selsp -- 信号选择器对象的引用计数加1
selsp2 = xwxt.somesel -- 信号选择器对象的引用计数加1
xwxt.somesel = nil -- 信号选择器对象的引用计数减1
]]


--[[--------
元方法：__gc<br>
<br>

信号选择器对象强指针的垃圾回收方法。

此元方法意味着信号选择器对象的强引用少了一个，**引用计数**减1。

@within MetaMethods
@function selsp:metatable.__gc

@tparam userdata selsp (**in**) 信号选择器对象强指针<br>
<br>

@usage
selsp = nil -- 删除引用
collectgarbage() -- 强制垃圾回收，将调用__gc()函数
]]


--[[--------
元方法：__tostring<br>
<br>

将信号选择器对象强指针格式化成字符串，可用于终端打印调试。

@within MetaMethods
@function selsp:metatable.__tostring

@tparam userdata selsp (**in**) 信号选择器对象强指针<br>
<br>

@usage
print(selsp)
]]


--[[--------
绑定**源**信号选择器对象到**目的**信号选择器<br>
<br>

@tparam userdata selsp (**in**) **源**信号选择器对象强指针<br>
<br>

@tparam userdata dst (**in**) **目的**信号选择器对象的强指针<br>
<br>

@tparam number pos (**in**) 信号选择器对象映射到目的信号选择器位图中的位置，位置从0开始编号<br>
<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误
  + ○ **-ECHRNG** 位置超出范围
  + ○ **-EALREADY** 同步对象已经绑定到事件对象
  + ○ **-EBUSY** 通道已经被其他同步对象独占

@usage
sel = xwos.sel.new(32)
-- ...省略...
rc = sel:bind(dst, 3) -- 绑定到信号选择器的位置3
if (rc == 0) then
  -- 绑定成功
else
  -- 绑定失败
end
]]
function selsp:bind(selsp, dst, pos)
end


--[[--------
从**目的**信号选择器上解绑**源**信号选择器对象<br>
<br>

@tparam userdata selsp (**in**) **源**信号选择器对象强指针<br>
<br>

@tparam userdata dst (**in**) **目的**信号选择器对象的强指针<br>
<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误
  + ○ **-ENOTCONN** 同步对象没有绑定到事件对象上

@usage
sel:unbind(dst)
]]
function selsp:unbind(selsp, dst)
end


--[[--------
中断阻塞在信号选择器中的所有线程<br>
<br>

@tparam userdata selsp (**in**) 信号选择器对象强指针<br>
<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误

@usage
rc = flg:intr_all()
]]
function selsp:intr_all(selsp)
end


--[[--------
新建位图，位的数量与信号选择器中的信号槽数量相等<br>
<br>

@tparam userdata selsp (**in**) 信号选择器对象强指针<br>
<br>

@treturn userdata 位图

@usage
bmp = sel:bmp()
]]
function selsp:bmp(selsp)
end


--[[--------
获取信号选择器中信号槽（位）的数量<br>
<br>

@tparam userdata selsp (**in**) 信号选择器对象强指针<br>
<br>

@treturn number 数量

@usage
num = sel:num()
]]
function selsp:num(selsp)
end


--[[--------
等待信号选择器中的触发信号<br>
<br>

@tparam userdata selsp (**in**) 信号选择器对象强指针<br>
<br>

@tparam userdata msk (**in**) 待触发的同步对象位图掩码<br>
<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误
  + ○ **-EINVAL** 参数错误
  + ○ **-EINTR** 等待被中断
  + ○ **-ENOTTHDCTX** 不在线程上下文中

@usage
sel = xwos.sel.new(32)
-- 绑定操作...
msk = sel:bmp();
msk:zero();
msk:s1i(0, 1, 2, 3, 4, 5, 6, 7) -- 掩码：0xFF
rc = sel:select(msk)
if (rc == 0) then
  -- 同步对象1,2,3,4,5,6,7,8中至少有一个触发了同步信号
else
  -- 错误发生
end
]]
function selsp:select(selsp, msk)
end


--[[--------
检测信号选择器中是否有同步信号触发<br>
<br>

若没有同步信号触发，立即返回，不会等待。

@tparam userdata selsp (**in**) 信号选择器对象强指针<br>
<br>

@tparam userdata msk (**in**) 待触发的同步对象位图掩码<br>
<br>

@treturn number
+ ● **rc** 返回值
  + ○ **0** 没有错误
  + ○ **-EINVAL** 参数错误
  + ○ **-EINTR** 等待被中断
  + ○ **-ENOTTHDCTX** 不在线程上下文中
  + ○ **-ENODATA** 尝试失败

@usage
sel = xwos.sel.new(32)
-- 绑定操作...
msk = sel:bmp();
msk:zero();
msk:s1i(0, 1, 2, 3, 4, 5, 6, 7) -- 掩码：0xFF
rc = sel:tryselect(msk) -- 检测信号选择器中是否有同步信号触发，立即返回，不会等待
if (rc == 0) then
  -- 同步对象1,2,3,4,5,6,7,8中至少有一个触发了同步信号
elseif (rc == -61)
  -- 没有检测到
else
  -- 错误发生
end
]]
function selsp:tryselect(selsp, msk)
end


--[[--------
限时等待信号选择器中的同步信号<br>
<br>

@tparam userdata selsp (**in**) 信号选择器对象强指针<br>
<br>

@tparam userdata msk (**in**) 待触发的同步对象位图掩码<br>
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
sel = xwos.sel.new(32)
-- 绑定操作...
msk = sel:bmp();
msk:zero();
msk:s1i(0, 1, 2, 3, 4, 5, 6, 7) -- 掩码：0xFF
rc = sel:select_to(msk, xwtm.ft(xwtm.s(1))) -- 最多等待1s
if (rc == 0) then
  -- 同步对象1,2,3,4,5,6,7,8中至少有一个触发了同步信号
elseif (rc == -116)
  -- 超时
else
  -- 错误发生
end
]]
function selsp:select_to(selsp, msk, to)
end
