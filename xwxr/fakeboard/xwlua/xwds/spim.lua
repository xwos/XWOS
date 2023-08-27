--[[--------
XWLUA模块：SPI主机模式控制器


## CAPI

[**CAPI**](../../capi/group__xwlua__xwds__i2cm.html)

@classmod xwds.spim
]]


--[[--------
元方法：__tostring



将SPI主机模式控制器对象格式化成字符串，可用于终端打印调试。

@within MetaMethods
@function xwds.spim:metatable.__tostring

@tparam userdata spim (**in**) C代码中注册的SPI主机模式控制器控制器
]]


--[[--------
接收数据



@tparam userdata spim (**in**) C代码中注册的SPI主机模式控制器控制器

@tparam string txd (**in**) 待发送的数据

@tparam boolean rx (**in**) 是否接收数据

@tparam number size (**in**) 本次传输的数据大小

@tparam number time (**in** , **optional**) 期望的阻塞等待时间

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EINTR** 等待被中断<br>
　● **-ENOTTHDCTX** 不在线程上下文中<br>
　● **-ETIMEDOUT** 超时，仅当存在可选参数 **time** 时才会出现此错误值

@treturn number 实际接受的大小

@treturn string 接收的数据

@usage
-- 打包{88, 87, 79, 83}
txd = string.pack("BBBB", 88, 87, 79, 83)
-- 发送数据，返回接收到的数据，期望等待时间2s
rc, size, rxd = spim1:xfer(spi1m, txd, true, #txd, xwtm.ft(xwtm.s(2)))

@usage
-- 打包{88, 87, 79, 83}
txd = string.pack("BBBB", 88, 87, 79, 83)
-- 发送数据，不接收数据，期望等待时间2s
rc, size = spim1:xfer(spi1m, txd, false, #txd, xwtm.ft(xwtm.s(2)))

@usage
-- 不发送数据，接收数据，接收缓冲区8字节，期望等待时间2s
rc, size, rxd = spim1:xfer(spi1m, nil, true, 8, xwtm.ft(xwtm.s(2)))
]]
function xfer(spim, txd, rx, size, time)
end
