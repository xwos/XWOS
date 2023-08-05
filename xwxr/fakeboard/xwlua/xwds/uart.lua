--[[--------
XWLUA模块：UART


## CAPI

[**CAPI**](../../capi/group__xwlua__xwds__i2cm.html)

@classmod xwds.uart
]]


--[[--------
元方法：__tostring



将UART对象格式化成字符串，可用于终端打印调试。

@within MetaMethods
@function xwds.uart:metatable.__tostring

@tparam userdata uart (**in**) C代码中注册的UART控制器
]]


--[[--------
从接收队列中获取数据



@tparam userdata uart (**in**) C代码中注册的UART控制器

@tparam number size (**in**) 申请的缓冲区大小

@tparam number time (**in**) 期望的阻塞等待时间

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EINTR** 等待被中断<br>
　● **-ENOTTHDCTX** 不在线程上下文中<br>
　● **-ETIMEDOUT** 超时

@treturn number 实际接受的大小

@treturn string 接收的数据

@usage
-- 从uart3接收数据，接收10个字节，最多等待时间2s
rc, rxsize, rxd = uart3:rx(10, xwtm.ft(xwtm.s(2)))
]]
function rx(uart, size, time)
end


--[[--------
尝试从接收队列中获取数据



@tparam userdata uart (**in**) C代码中注册的UART控制器

@tparam number size (**in**) 申请的缓冲区大小

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EINTR** 等待被中断<br>
　● **-ENOTTHDCTX** 不在线程上下文中<br>
　● **-ENODATA** 尝试失败

@treturn number 实际接受的大小

@treturn string 接收的数据

@usage
-- 尝试从uart3接收数据，接收10个字节
rc, rxsize, rxd = uart3:try_rx(10)
]]
function try_rx(uart, size)
end


--[[--------
发送数据



@tparam userdata uart (**in**) C代码中注册的UART控制器

@tparam string data (**in**) 待发送的数据

@tparam number time (**optional**) (**in**) 期望的阻塞等待时间

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EINTR** 等待被中断<br>
　● **-ENOTTHDCTX** 不在线程上下文中<br>
　● **-ETIMEDOUT** 超时

@treturn number 实际发送的大小

@usage
-- 打包{88, 87, 79, 83}
txd = string.pack("BBBB", 88, 87, 79, 83)
-- 发送打包数据，期望等待时间2s
rc, txsize = uart3:tx(txd, xwtm.ft(xwtm.s(2)))
]]
function tx(uart, data, time)
end
