--[[--------
XWLUA模块：XWLUA I2C主机消息


## CAPI

@classmod xwds.i2cm.msg
]]


--[[--------
设置XWLUA I2C主机消息的外设地址

@tparam number idx (**in**) 消息的索引
@tparam number addr (**in**) 外设地址
@tparam boolean addr10bit (**in**) 是否为10位地址模式<br>
]]
function addr(idx, addr, addr10bit)
end


--[[--------
设置XWLUA I2C主机消息的读标志

@tparam number idx (**in**) 消息的索引
@tparam boolean flag (**in**) 是否为读模式<br>
]]
function read(idx, flag)
end


--[[--------
设置XWLUA I2C主机消息的开始标志

@tparam number idx (**in**) 消息的索引
@tparam boolean flag (**in**) 是否包含START标志<br>
]]
function start(idx, flag)
end


--[[--------
设置XWLUA I2C主机消息的停止标志

@tparam number idx (**in**) 消息的索引
@tparam boolean flag (**in**) 是否包含STOP标志<br>
]]
function stop(idx, flag)
end


--[[--------
设置XWLUA I2C主机消息的数据（用于发送）

@tparam number idx (**in**) 消息的索引
@tparam string data (**in**) 待发送的数据<br>
]]
function data(idx, data)
end


--[[--------
为XWLUA I2C主机消息申请数据缓冲区（用于接收）

@tparam number idx (**in**) 消息的索引
@tparam number size (**in**) 缓冲区大小
@treturn string 用于接收数据的缓冲区<br>
]]
function buffer(idx, size)
end
