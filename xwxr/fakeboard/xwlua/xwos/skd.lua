--[[--------
XWLUA模块：调度器

与调度器相关的功能

@module xwos.skd
]]


--[[--------
获取当前CPU的ID<br>
<br>

@treturn number CPU的ID

@usage id = xwos.skd.id()
]]
function id()
end

--[[--------
获取当前CPU调度器的系统滴答时间<br>
<br>

XWOS的系统滴答时间是一个有符号的64位整数，以纳秒为单位。

@treturn number 系统滴答时间

@usage time = xwos.skd.tt()
]]
function tt()
end

--[[--------
获取当前CPU调度器的系统滴答计数<br>
<br>

XWOS的系统滴答计数表示滴答定时器中断了多少次。

@treturn number 系统滴答计数

@usage count = xwos.skd.tc()
]]
function tc()
end

--[[--------
获取当前CPU调度器的系统时间戳<br>
<br>

XWOS的系统时间戳 = XWOS的系统滴答时间 + 滴答定时器中数值换算成纳秒的时间。

@treturn number 系统时间戳

@usage timestamp = xwos.skd.ts()
]]
function ts()
end

--[[--------
关闭本地CPU调度器的抢占<br>
<br>

@usage xwos.skd.dspmpt()
]]
function dspmpt()
end

--[[--------
开启本地CPU调度器的抢占<br>
<br>

@usage xwos.skd.enpmpt()
]]
function enpmpt()
end
