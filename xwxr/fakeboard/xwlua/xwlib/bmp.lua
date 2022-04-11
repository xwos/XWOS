--[[--------
XWLUA模块：位图

位的序号从0开始。

@classmod xwlib.bmp
]]

--[[--------
新建位图<br>
<br>

@within StaticMethods
@function xwlib.bmp.new
@tparam number bits (**in**) 位图中位的数量<br>

@treturn userdata 新的位图
@usage
mybmp = xwlib.bmp.new(64) -- 创建包含64个位的位图
]]


--[[--------
元方法：__copy<br>
<br>

将位图导出到全局导出表 `xwxt` 中，此方法会逐位拷贝。

@within MetaMethods
@function bmp:metatable.__copy
@tparam userdata bmp (**in**) 位图对象
@tparam userdata vm (**in**) 目标虚拟机
@usage
mybmp = ... -- 创建（省略）
xwxt.somebmp = mybmp -- 复制到全局导出表
]]


--[[--------
元方法：__tostring<br>
<br>

将位图格式化成字符串，可用于终端打印调试。

@within MetaMethods
@function bmp:metatable.__tostring
@tparam userdata bmp (**in**) 位图对象
@usage
mybmp = ... -- 创建（省略）
print(mybmp)
]]


--[[--------
元方法：__band<br>
<br>

运算符：**位与 &**，此运算符要求两个操作数的位数一致，
会将两个位图对象按位进行与运算，并产生一个新的位图对象返回结果。

@within MetaMethods
@function bmp:metatable.__band

@tparam userdata opd1 (**in**) 位图对象1<br>
<br>

@tparam userdata opd2 (**in**) 位图对象2<br>
<br>

@treturn userdata 结果

@usage
mybmp1 = xwlib.bmp.new(64) -- 创建一个包含64个位的位图1
mybmp1:fill() -- 全部填充1
mybmp2 = xwlib.bmp.new(64) -- 创建一个包含64个位的位图2
mybmp2:zero()
mybmp2:s1i(0, 1, 2, 3, 4, 5, 6, 7) -- 0xFF
mybmp3 = mybmp1 & mybmp2
print(mybmp3)

@usage
`终端输出`
`00000000000000FF`
]]


--[[--------
元方法：__bor<br>
<br>

运算符：**位或 |**，此运算符要求两个操作数的位数一致，
会将两个位图对象按位进行或运算，并产生一个新的位图对象返回结果。

@within MetaMethods
@function bmp:metatable.__bor

@tparam userdata opd1 (**in**) 位图对象1<br>
<br>

@tparam userdata opd2 (**in**) 位图对象2<br>
<br>

@treturn userdata 结果

@usage
mybmp1 = xwlib.bmp.new(64) -- 创建一个包含64个位的位图1
mybmp1:zero() -- 全部填清0
mybmp2 = xwlib.bmp.new(64) -- 创建一个包含64个位的位图2
mybmp2:zero()
mybmp2:s1i(0, 1, 2, 3, 4, 5, 6, 7) -- 0xFF
mybmp3 = mybmp1 | mybmp2
print(mybmp3)

@usage
`终端输出`
`00000000000000FF`
]]


--[[--------
元方法：__bxor<br>
<br>

运算符：**位异或 ~**，此运算符要求两个操作数的位数一致，
会将两个位图对象按位进行异或运算，并产生一个新的位图对象返回结果。

@within MetaMethods
@function bmp:metatable.__bxor

@tparam userdata opd1 (**in**) 位图对象1<br>
<br>

@tparam userdata opd2 (**in**) 位图对象2<br>
<br>

@treturn userdata 结果

@usage
mybmp1 = xwlib.bmp.new(64) -- 创建一个包含64个位的位图1
mybmp1:fill() -- 全部填充1
mybmp2 = xwlib.bmp.new(64) -- 创建一个包含64个位的位图2
mybmp2:zero()
mybmp2:s1i(0, 1, 2, 3, 4, 5, 6, 7) -- 0xFF
mybmp3 = mybmp1 ~ mybmp2
print(mybmp3)

@usage
`终端输出`
`FFFFFFFFFFFFFF00`
]]


--[[--------
元方法：__bnot<br>
<br>

运算符：**位取反 ~**，此运算符会将位图对象按位进行取反运算，并产生一个新的位图对象返回结果。

@within MetaMethods
@function bmp:metatable.__bnot

@tparam userdata opd1 (**in**) 位图对象1<br>
<br>

@treturn userdata 结果

@usage
mybmp1 = xwlib.bmp.new(64) -- 创建一个包含64个位的位图1
mybmp1:fill() -- 全部填充1
mybmp2 = ~mybmp1
print(mybmp1)
print(mybmp2)

@usage
`终端输出`
`FFFFFFFFFFFFFFFF`
`0000000000000000`
]]


--[[--------
元方法：__len<br>
<br>

运算符：**计算长度 #**，此运算符会返回位图中位数。

@within MetaMethods
@function bmp:metatable.__len

@tparam userdata opd1 (**in**) 位图对象1<br>
<br>

@treturn userdata 结果

@usage
mybmp = xwlib.bmp.new(64) -- 创建一个包含64个位的位图1
print(#mybmp)

@usage
`终端输出`
`64`
]]


--[[--------
元方法：__eq<br>
<br>

运算符：**比较相等 ==**，此运算符会返回位图中位数。

@within MetaMethods
@function bmp:metatable.__eq

@tparam userdata opd1 (**in**) 位图对象1<br>
<br>

@tparam userdata opd2 (**in**) 位图对象2<br>
<br>

@treturn boolean 是否相等

@usage
mybmp1 = xwlib.bmp.new(64) -- 创建一个包含64个位的位图1
mybmp1:fill() -- 全部填充1
mybmp2 = xwlib.bmp.new(64) -- 创建一个包含64个位的位图2
mybmp2:fill() -- 全部填充1
if (mybmp1 == mybmp2) then
  print("equal")
end

@usage
`终端输出`
`equal`
]]


--[[--------
将位图全部填1<br>
<br>

@tparam userdata bmp (**in**) 位图对象<br>
<br>

@usage
mybmp = xwlib.bmp.new(64) -- 创建一个包含64个位的位图
mybmp:fill() -- 全部填充1
print(mybmp)

@usage
`终端输出`
`FFFFFFFFFFFFFFFF`

]]
function bmp:fill(bmp)
end


--[[--------
将位图全部清0<br>
<br>

@tparam userdata bmp (**in**) 位图对象<br>
<br>

@usage
mybmp = xwlib.bmp.new(64) -- 创建一个包含64个位的位图
mybmp:zero() -- 全部填充0
print(mybmp)

@usage
`终端输出`
`0000000000000000`

]]
function bmp:zero(bmp)
end


--[[--------
赋值<br>
<br>

此函数与`bmp = opd`不同，`bmp = opd`只是将引用bmp指向opd，
bmp原来的对象如果没有被别的引用持有，将会被虚拟机回收掉，属于**浅拷贝**。

@tparam userdata bmp (**in**) 目的位图对象<br>
<br>

@tparam userdata opd (**in**) 源位图对象<br>
<br>

@usage
mybmp1 = xwlib.bmp.new(64) -- 创建一个包含64个位的位图1
mybmp1:fill() -- 全部填充1
mybmp2 = xwlib.bmp.new(64) -- 创建一个包含64个位的位图2
mybmp2:assign(mybmp1)
print(mybmp1)
print(mybmp2)

@usage
`终端输出`
`FFFFFFFFFFFFFFFF`
`FFFFFFFFFFFFFFFF`
]]
function bmp:assign(bmp, opd)
end


--[[--------
计算位图中1的个数<br>
<br>

@tparam userdata bmp (**in**) 目的位图对象<br>
<br>

@treturn number 位图中1的个数

@usage
mybmp = xwlib.bmp.new(64) -- 创建一个包含64个位的位图
mybmp:zero() -- 清0
mybmp:s1i(1, 10) -- 第1, 10位填充1
print(mybmp:weight())

@usage
`终端输出`
`2`
]]
function bmp:weight(bmp)
end


--[[--------
从位图的最低位开始，查找第一个被置1的位的序号<br>
<br>

@tparam userdata bmp (**in**) 目的位图对象<br>
<br>

@treturn number 序号

@usage
mybmp = xwlib.bmp.new(64) -- 创建一个包含64个位的位图
mybmp:zero() -- 清0
mybmp:s1i(10) -- 第10位填充1
mybmp:s1i(11) -- 第11位填充1
print(mybmp:ffs())

@usage
`终端输出`
`10`
]]
function bmp:ffs(bmp)
end


--[[--------
从位图的最低位开始，查找第一个被清0的位的序号<br>
<br>

@tparam userdata bmp (**in**) 目的位图对象<br>
<br>

@treturn number 序号

@usage
mybmp = xwlib.bmp.new(64) -- 创建一个包含64个位的位图
mybmp:zero() -- 清0
mybmp:s1i(10) -- 第10位填充1
mybmp:s1i(11) -- 第11位填充1
print(mybmp:ffz())

@usage
`终端输出`
`0`
]]
function bmp:ffz(bmp)
end


--[[--------
从位图的最高位开始，查找第一个被置1的位的序号<br>
<br>

@tparam userdata bmp (**in**) 目的位图对象<br>
<br>

@treturn number 序号

@usage
mybmp = xwlib.bmp.new(64) -- 创建一个包含64个位的位图
mybmp:zero() -- 清0
mybmp:s1i(10) -- 第10位填充1
mybmp:s1i(11) -- 第11位填充1
print(mybmp:fls())

@usage
`终端输出`
`11`
]]
function bmp:fls(bmp)
end


--[[--------
从位图的最高位开始，查找第一个被清0的位的序号<br>
<br>

@tparam userdata bmp (**in**) 目的位图对象<br>
<br>

@treturn number 序号

@usage
mybmp = xwlib.bmp.new(64) -- 创建一个包含64个位的位图
mybmp:zero() -- 清0
mybmp:s1i(10) -- 第10位填充1
mybmp:s1i(11) -- 第11位填充1
print(mybmp:flz())

@usage
`终端输出`
`63`
]]
function bmp:flz(bmp)
end


--[[--------
依次将位图中的位置1<br>
<br>

@tparam userdata bmp (**in**) 目的位图对象<br>
<br>

@tparam userdata pos1 (**in**) 位的序号1<br>
<br>

@tparam userdata pos2 (**in**) (**optional**) 位的序号2<br>
<br>

@tparam userdata ... (**in**) (**optional**) 位的序号n<br>
<br>

@usage
mybmp = xwlib.bmp.new(64) -- 创建一个包含64个位的位图
mybmp:zero() -- 清0
mybmp:s1i(0, 1, 3, 9) -- 第0, 1, 3, 9位填充1
print(mybmp)

@usage
`终端输出`
`000000000000020B`
]]
function bmp:s1i(bmp, pos1, pos2, ...)
end


--[[--------
将位图中的掩码部分全部置1<br>
<br>

@tparam userdata bmp (**in**) 目的位图对象<br>
<br>

@tparam userdata msk (**in**) 掩码位图<br>
<br>

@usage
mybmp = xwlib.bmp.new(64) -- 创建一个包含64个位的位图
mymsk = xwlib.bmp.new(64) -- 创建一个包含64个位的位图掩码
mymsk:zero()
mymsk:s1i(0, 1, 3, 9) -- 第0, 1, 3, 9位填1
mybmp:zero()
mybmp:s1m(mymsk)
print(mybmp)

@usage
`终端输出`
`000000000000020B`
]]
function bmp:s1m(bmp, msk)
end


--[[--------
依次将位图中的位清0<br>
<br>

@tparam userdata bmp (**in**) 目的位图对象<br>
<br>

@tparam userdata pos1 (**in**) 位的序号1<br>
<br>

@tparam userdata pos2 (**in**) (**optional**) 位的序号2<br>
<br>

@tparam userdata ... (**in**) (**optional**) 位的序号n<br>
<br>

@usage
mybmp = xwlib.bmp.new(64) -- 创建一个包含64个位的位图
mybmp:fill()
mybmp:c0i(0, 1, 3, 9) -- 第0, 1, 3, 9位清0
print(mybmp)

@usage
`终端输出`
`FFFFFFFFFFFFFDF4`
]]
function bmp:c0i(bmp, pos1, pos2, ...)
end


--[[--------
将位图中的掩码部分全部清0<br>
<br>

@tparam userdata bmp (**in**) 目的位图对象<br>
<br>

@tparam userdata msk (**in**) 掩码位图<br>
<br>

@usage
mybmp = xwlib.bmp.new(64) -- 创建一个包含64个位的位图
mymsk = xwlib.bmp.new(64) -- 创建一个包含64个位的位图掩码
mymsk:zero()
mymsk:s1i(0, 1, 3, 9) -- 第0, 1, 3, 9位填1
mybmp:fill()
mybmp:c0m(mymsk)
print(mybmp)

@usage
`终端输出`
`FFFFFFFFFFFFFDF4`
]]
function bmp:c0m(bmp, msk)
end


--[[--------
依次将位图中的位翻转<br>
<br>

@tparam userdata bmp (**in**) 目的位图对象<br>
<br>

@tparam userdata pos1 (**in**) 位的序号1<br>
<br>

@tparam userdata pos2 (**in**) (**optional**) 位的序号2<br>
<br>

@tparam userdata ... (**in**) (**optional**) 位的序号n<br>
<br>

@usage
mybmp = xwlib.bmp.new(64) -- 创建一个包含64个位的位图
mybmp:zero()
mybmp:x1i(0, 1, 3, 9) -- 第0, 1, 3, 9位翻转
print(mybmp)

@usage
`终端输出`
`000000000000020B`
]]
function bmp:x1i(bmp, pos1, pos2, ...)
end


--[[--------
将位图中的掩码部分全部翻转<br>
<br>

@tparam userdata bmp (**in**) 目的位图对象<br>
<br>

@tparam userdata msk (**in**) 掩码位图<br>
<br>

@usage
mybmp = xwlib.bmp.new(64) -- 创建一个包含64个位的位图
mymsk = xwlib.bmp.new(64) -- 创建一个包含64个位的位图掩码
mymsk:zero()
mymsk:s1i(0, 1, 3, 9) -- 第0, 1, 3, 9位填1
mybmp:zero()
mybmp:x1m(mymsk)
print(mybmp)

@usage
`终端输出`
`000000000000020B`
]]
function bmp:x1m(bmp, msk)
end


--[[--------
测试位图中的某个位<br>
<br>

@tparam userdata bmp (**in**) 目的位图对象<br>
<br>

@tparam number pos (**in**) 位的序号<br>
<br>

@treturn boolean

@usage
mybmp = xwlib.bmp.new(64) -- 创建一个包含64个位的位图
mybmp:fill()
mybmp:c0i(0, 1, 3, 9)
print(mybmp:t1i(1))
print(mybmp:t1i(11))

@usage
`终端输出`
`false`
`true`
]]
function bmp:t1i(bmp, pos)
end


--[[--------
测试位图中的掩码部分的位是否全部置1<br>
<br>

@tparam userdata bmp (**in**) 目的位图对象<br>
<br>

@tparam userdata msk (**in**) 掩码位图<br>
<br>

@treturn boolean

@usage
mybmp = xwlib.bmp.new(64)
mymsk = xwlib.bmp.new(64)
mymsk:zero()
mymsk:s1i(0, 1, 3, 9)
mybmp:zero()
mybmp:s1i(0, 1)
print(mybmp:t1ma(mymsk))
mybmp:s1i(3, 9)
print(mybmp:t1ma(mymsk))

@usage
`终端输出`
`false`
`true`
]]
function bmp:t1ma(bmp, msk)
end


--[[--------
测试位图中的掩码部分的位是否至少有一位置1<br>
<br>

@tparam userdata bmp (**in**) 目的位图对象<br>
<br>

@tparam userdata msk (**in**) 掩码位图<br>
<br>

@treturn boolean

@usage
mybmp = xwlib.bmp.new(64) -- 创建一个包含64个位的位图
mymsk = xwlib.bmp.new(64) -- 创建一个包含64个位的位图掩码
mymsk:zero()
mymsk:s1i(0, 1, 3, 9)
mybmp:zero()
mybmp:s1i(0, 1)
print(mybmp:t1mo(mymsk))
mybmp:s1i(3, 9)
print(mybmp:t1mo(mymsk))

@usage
`终端输出`
`true`
`true`
]]
function bmp:t1mo(bmp, msk)
end


--[[--------
测试位图中的掩码部分的位是否全部清0<br>
<br>

@tparam userdata bmp (**in**) 目的位图对象<br>
<br>

@tparam userdata msk (**in**) 掩码位图<br>
<br>

@treturn boolean

@usage
mybmp = xwlib.bmp.new(64)
mymsk = xwlib.bmp.new(64)
mymsk:zero()
mymsk:s1i(0, 1, 3, 9)
mybmp:fill()
mybmp:c0i(0, 1)
print(mybmp:t0ma(mymsk))
mybmp:c0i(3, 9)
print(mybmp:t0ma(mymsk))

@usage
`终端输出`
`false`
`true`
]]
function bmp:t0ma(bmp, msk)
end


--[[--------
测试位图中的掩码部分的位是否至少有一位清0<br>
<br>

@tparam userdata bmp (**in**) 目的位图对象<br>
<br>

@tparam userdata msk (**in**) 掩码位图<br>
<br>

@treturn boolean

@usage
mybmp = xwlib.bmp.new(64)
mymsk = xwlib.bmp.new(64)
mymsk:zero()
mymsk:s1i(0, 1, 3, 9)
mybmp:fill()
mybmp:c0i(0, 1)
print(mybmp:t0mo(mymsk))
mybmp:c0i(3, 9)
print(mybmp:t0mo(mymsk))

@usage
`终端输出`
`true`
`true`
]]
function bmp:t0mo(bmp, msk)
end
