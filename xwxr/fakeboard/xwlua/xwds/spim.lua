--[[--------
XWLUA模块：SPI主机模式控制器

@classmod xwds.spim
]]


--[[--------
注册SPI主机模式控制器对象

在C代码中为XWLUA注册SPI主机模式控制器对象

@within CAPI
@function xwds.spim.xwlua_spim_register

@param L **(lua_State *)** 虚拟机指针（向哪个虚拟机注册）<br>
<br>

@param name **(const char *)** 在Lua语言中的符号名<br>
<br>

@param spim **(struct xwds_spim *)** SPI主机模式控制器对象<br>
<br>

@usage
void xwlua_spim_register(lua_State * L, const char * name, struct xwds_spim * spim);

@usage
xwlua_spim_register(L, "spim1", &stm32cube_usart1_cb);

]]

--[[--------
删除SPI主机模式控制器对象

在C代码中删除SPI主机模式控制器对象


@within CAPI
@function xwds.spim.xwlua_spim_unregister

@param L **(lua_State *)** 虚拟机指针（从哪个虚拟机删除）<br>
<br>

@param name **(const char *)** 在Lua语言中的符号名<br>
<br>

@usage
void xwlua_spim_unregister(lua_State * L, const char * name);
]]


--[[--------
元方法：__tostring<br>
<br>

将SPI主机模式控制器对象格式化成字符串，可用于终端打印调试。

@within MetaMethods
@function thdsp:metatable.__tostring

@tparam userdata spim (**in**) C代码中注册的SPI主机模式控制器控制器<br>
<br>
@usage
print(thdsp)
]]


--[[--------
接收数据<br>
<br>

@tparam userdata spim (**in**) C代码中注册的SPI主机模式控制器控制器<br>
<br>

@tparam string txd (**in**) 待发送的数据<br>
<br>

@tparam boolean rx (**in**) 是否接收数据<br>
<br>

@tparam number size (**in**) 本次传输的数据大小<br>
<br>

@tparam number time (**optional**) (**in**) 期望的阻塞等待时间<br>
<br>

@treturn {number,number,string}
+ ● **rc** 此方法的返回值
  + ○ **0** 没有错误
  + ○ **-EINTR** 等待被中断
  + ○ **-ENOTINTHD** 不在线程上下文中
  + ○ **-ETIMEDOUT** 超时，仅当存在可选参数 **time** 时才会出现此错误值
+ ● **rxsize** 实际接受的大小
+ ● **rxd** 接收的数据

@usage
-- 打包{88, 87, 79, 83}
txd = string.pack("BBBB", 88, 87, 79, 83)
-- 发送数据，返回接收到的数据，期望等待时间2s
rc, size, rxd = spim1:xfer(spi1m, txd, true, #txd, 2000000000)

@usage
-- 打包{88, 87, 79, 83}
txd = string.pack("BBBB", 88, 87, 79, 83)
-- 发送数据，不接收数据，期望等待时间2s
rc, size = spim1:xfer(spi1m, txd, false, #txd, 2000000000)

@usage
-- 不发送数据，接收数据，接收缓冲区8字节，期望等待时间2s
rc, size, rxd = spim1:xfer(spi1m, nil, true, 8, 2000000000)
]]
function xfer(spim, txd, rx, size, time)
end
