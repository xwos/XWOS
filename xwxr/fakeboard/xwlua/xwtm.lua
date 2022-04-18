--[[--------
XWLUA模块：系统时间

XWOS使用64位有符号整数作为系统时间的类型，单位为纳秒。

@module xwtm
]]


--[[--------
获取当前CPU的**系统时间**点<br>
<br>

XWOS中，每个CPU都有一个私有的滴答定时器，产生周期性的定时中断。**系统时间**在每次中断时都会增加一次。

**系统时间**是个每CPU私有变量。代码运行在哪个CPU，访问的就是哪个私有变量。

XWOS的系统时间 = 滴答时间定时器中断的次数 * 中断的周期（以纳秒为单位）

@treturn number 当前系统滴答时间<br>
<br>

@usage
xwtm.now()
]]
function now()
end


--[[--------
获取当前CPU未来的**系统时间**点<br>
<br>

未来的时间点 = 但前时间点 + xwtm

@tparam number xwtm (**in**) 从现在开始到未来的时间<br>

@treturn number 未来时间点<br>
<br>

@usage
xwtm.ft(xwtm)
]]
function ft(xwtm)
end


--[[--------
获取当前CPU的**系统时间戳**<br>
<br>

**系统时间戳**类似于**系统时间**。但是**系统时间**只会在每次定时器中断时才增加一次，两次中断之间不会发生变化。

**系统时间戳**是通过把滴答定时器到下一次中断还剩多少时间计算出来，再累加到**系统时间**上获取的。

**系统时间戳**是个每CPU私有变量。代码运行在哪个CPU，访问的就是哪个私有变量。

XWOS的系统时间戳 = XWOS的系统时间 + 滴答时间定时器中数值换算成纳秒的时间

@treturn number 当前系统时间戳<br>
<br>

@usage
xwtm.nowts()
]]
function nowts()
end


--[[--------
获取当前CPU的未来**系统时间戳**<br>
<br>

未来时间戳 = 当前时间时间戳 + xwtm

@tparam number xwtm (**in**) 从现在开始到未来的时间<br>

@treturn number 未来时间点<br>
<br>

@usage
xwtm.fts(xwtm)
]]
function fts(xwtm)
end


--[[--------
获取当前CPU的**系统滴答计数**<br>
<br>

**系统滴答计数**表示滴答定时器中断了多少次。

**系统滴答计数**是个每CPU私有变量。代码运行在哪个CPU，访问的就是哪个私有变量。

@treturn number 滴答计数<br>
<br>

@usage
xwtm.nowtc()
]]
function nowtc()
end


--[[--------
返回以纳秒为单位的系统时间<br>
<br>

@tparam number ns (**in**) 纳秒<br>

@treturn number 系统时间<br>
<br>

@usage
xwtm.ns(ns)
]]
function ns(ns)
end


--[[--------
返回以微秒为单位的系统时间<br>
<br>

@tparam number us (**in**) 微秒<br>

@treturn number 系统时间<br>
<br>

@usage
xwtm.us(us)
]]
function us(us)
end


--[[--------
返回以毫秒为单位的系统时间<br>
<br>

@tparam number ms (**in**) 毫秒<br>

@treturn number 系统时间<br>
<br>

@usage
xwtm.ms(ms)
]]
function ms(ms)
end


--[[--------
返回以秒为单位的系统时间<br>
<br>

@tparam number s (**in**) 秒<br>

@treturn number 系统时间<br>
<br>

@usage
xwtm.s(s)
]]
function s(s)
end


--[[--------
返回以分为单位的系统时间<br>
<br>

@tparam number m (**in**) 分<br>

@treturn number 系统时间<br>
<br>

@usage
xwtm.m(m)
]]
function m(m)
end


--[[--------
返回以时为单位的系统时间<br>
<br>

@tparam number h (**in**) 时<br>

@treturn number 系统时间<br>
<br>

@usage
xwtm.h(h)
]]
function h(h)
end


--[[--------
返回以天为单位的系统时间<br>
<br>

@tparam number d (**in**) 天<br>

@treturn number 系统时间<br>
<br>

@usage
xwtm.d(d)
]]
function d(d)
end
