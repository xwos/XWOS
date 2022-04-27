--[[--------
XWLUA模块：UART

@classmod xwds.uart
]]


--[[--------
注册UART对象

在C代码中为XWLUA注册UART对象

@within CAPI
@function xwds.uart.xwlua_uart_register

@param L **(lua_State *)** 虚拟机指针（向哪个虚拟机注册）<br>
<br>

@param name **(const char *)** 在Lua语言中的符号名<br>
<br>

@param dmauartc **(struct xwds_dmauartc *)** UART对象<br>
<br>

@usage
void xwlua_uart_register(lua_State * L, const char * name, struct xwds_dmauartc * dmauartc);

@usage
xwlua_uart_register(L, "uart1", &stm32cube_usart1_cb);

]]

--[[--------
删除UART对象

在C代码中删除UART对象


@within CAPI
@function xwds.uart.xwlua_uart_unregister

@param L **(lua_State *)** 虚拟机指针（从哪个虚拟机删除）<br>
<br>

@param name **(const char *)** 在Lua语言中的符号名<br>
<br>

@usage
void xwlua_uart_unregister(lua_State * L, const char * name);
]]


--[[--------
元方法：__tostring<br>
<br>

将UART对象格式化成字符串，可用于终端打印调试。

@within MetaMethods
@function thdsp:metatable.__tostring

@tparam userdata uart (**in**) C代码中注册的UART控制器<br>
<br>
@usage
print(thdsp)
]]


--[[--------
从接收队列中获取数据<br>
<br>

@tparam userdata uart (**in**) C代码中注册的UART控制器<br>
<br>

@tparam number size (**in**) 申请的缓冲区大小<br>
<br>

@tparam number time (**in**) 期望的阻塞等待时间<br>
<br>

@treturn {number,number,string}
+ ● **rc** 此方法的返回值
  + ○ **0** 没有错误
  + ○ **-EINTR** 等待被中断
  + ○ **-ENOTTHDCTX** 不在线程上下文中
  + ○ **-ETIMEDOUT** 超时
+ ● **rxsize** 实际接受的大小
+ ● **rxd** 接收的数据

@usage
-- 从uart3接收数据，接收10个字节，最多等待时间2s
rc, rxsize, rxd = uart3:rx(10, xwtm.ft(xwtm.s(2)))
]]
function rx(uart, size, time)
end


--[[--------
尝试从接收队列中获取数据<br>
<br>

@tparam userdata uart (**in**) C代码中注册的UART控制器<br>
<br>

@tparam number size (**in**) 申请的缓冲区大小<br>
<br>

@treturn {number,number,string}
+ ● **rc** 此方法的返回值
  + ○ **0** 没有错误
  + ○ **-EINTR** 等待被中断
  + ○ **-ENOTTHDCTX** 不在线程上下文中
  + ○ **-ENODATA** 尝试失败
+ ● **rxsize** 实际接受的大小
+ ● **rxd** 接收的数据

@usage
-- 尝试从uart3接收数据，接收10个字节
rc, rxsize, rxd = uart3:try_rx(10)
]]
function try_rx(uart, size)
end


--[[--------
发送数据<br>
<br>

@tparam userdata uart (**in**) C代码中注册的UART控制器<br>
<br>

@tparam string data (**in**) 待发送的数据<br>
<br>

@tparam number time (**optional**) (**in**) 期望的阻塞等待时间<br>
<br>

@treturn {number,number,string}
+ ● **rc** 此方法的返回值
  + ○ **0** 没有错误
  + ○ **-EINTR** 等待被中断
  + ○ **-ENOTTHDCTX** 不在线程上下文中
  + ○ **-ETIMEDOUT** 超时
+ ● **txsize** 实际发送的大小

@usage
-- 打包{88, 87, 79, 83}
txd = string.pack("BBBB", 88, 87, 79, 83)
-- 发送打包数据，期望等待时间2s
rc, txsize = uart3:tx(txd, xwtm.ft(xwtm.s(2)))
]]
function tx(uart, data, time)
end
