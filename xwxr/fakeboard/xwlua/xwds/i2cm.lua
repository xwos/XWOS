--[[--------
XWLUA模块：I2C主机模式控制器


## CAPI

[**CAPI**](../../capi/group__xwlua__xwds__i2cm.html)

@classmod xwds.i2cm
]]


--[[--------
元方法：__tostring



将I2C主机模式控制器对象格式化成字符串，可用于终端打印调试。

@within MetaMethods
@function xwds.i2cm:metatable.__tostring
]]


--[[--------
创建XWLUA I2C主机消息

@tparam number num (**in**) 消息的数量
@treturn userdata XWLUA I2C主机消息<br>
]]
function xwds.i2cm.msg(num)
end


--[[--------
传输I2C消息


@tparam userdata msg (**in**) XWLUA I2C主机消息，由 `xwds.i2cm.msg()` 创建

@tparam number to (**in** , **optional**) 期望的阻塞等待时间

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EINTR** 等待被中断<br>
　● **-ENOTTHDCTX** 不在线程上下文中<br>
　● **-ETIMEDOUT** 超时，仅当存在可选参数 **to** 时才会出现此错误值

@usage
-- 向EEPROM的0地址写入4字节数据
txd = string.pack(">HBBBB", 0x0000, 88, 87, 79, 83) -- 打包写数据{地址0, 88, 87, 79, 83}
msg = xwds.i2cm.msg(1) -- 创建1个I2C消息
msg:addr(0, 0x50, false) -- 设置msg[0]的外设地址为0x50，7位地址模式
msg:start(0, true) -- 设置msg[0]的START标志
msg:read(0, false) -- 清除msg[0]的READ标志
msg:stop(0, true) -- 设置msg[0]的STOP标志
msg:data(0, txd) -- 设置msg[0]的data
-- 写入数据，最多等待2s
rc = i2cm1:xfer(msg, xwtm.ft(xwtm.s(2)))

@usage
-- 从EEPROM的0地址读出16字节数据，
txd = string.pack(">H", 0x0000) -- 打包写数据{地址0x0000}
msg = xwds.i2cm.msg(2) -- 创建2个I2C消息
msg:addr(0, 0x50, false) -- 设置msg[0]的外设地址为0x50，7位地址模式
msg:start(0, true) -- 设置msg[0]的START标志
msg:read(0, false) -- 清除msg[0]的READ标志
msg:stop(0, false) -- 清除msg[0]的STOP标志
msg:data(0, txd) -- 设置msg[0]的data
msg:addr(1, 0x50, false) -- 设置msg[1]的外设地址为0x50，7位地址模式
msg:start(1, true) -- 设置msg[1]的START标志
msg:read(1, true) -- 设置msg[1]的READ标志
msg:stop(1, true) -- 设置msg[1]的STOP标志
rxd = msg:buffer(1, 4) -- 给msg[1]申请4字节读buffer
-- 读取数据，最多等待2s
rc = i2cm1:xfer(msg, xwtm.ft(xwtm.s(2)))
print(rc)
print(rxd)
]]
function xfer(msg, to)
end
