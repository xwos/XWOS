--[[--------
XWLUA模块：SOC


## CAPI

[**CAPI**](../../capi/group__xwlua__xwds__i2cm.html)

@classmod xwds.soc
]]


--[[--------
元方法：__tostring



将SOC对象格式化成字符串，可用于终端打印调试。

@within MetaMethods
@function xwds.soc:metatable.__tostring

@tparam userdata soc (**in**) C代码中注册的SOC
]]


--[[--------
返回指定序号的GPIO PIN的掩码



注意：序号是从0开始。

@tparam userdata soc (**in**) C代码中注册的SOC

@tparam number pin (**in**) GPIO PIN的序号

@treturn number pin的掩码

@usage
msk = stm32:pin(4) -- msk == 0x8
]]
function pin(soc, pin)
end


--[[--------
申请GPIO



注意：序号是从0开始编号。

@tparam userdata soc (**in**) C代码中注册的SOC

@tparam number port (**in**) GPIO PORT的序号

@tparam number pinmsk (**in**) GPIO PIN的掩码

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-ERANGE** GPIO PORT错误<br>
　● **-EBUSY** GPIO PIN已被使用

@usage
rc = stm32:req(4, stm32:pin(7)) -- GPIOE PIN7
]]
function req(soc, port, pinmsk)
end


--[[--------
释放GPIO



注意：序号是从0开始编号。

@tparam userdata soc (**in**) C代码中注册的SOC

@tparam number port (**in**) GPIO PORT的序号

@tparam number pinmsk (**in**) GPIO PIN的掩码

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-ERANGE** GPIO PORT错误<br>
　● **-EPERM** GPIO PIN未被使用

@usage
rc = stm32:rls(3, stm32:pin(7)) -- PORTD, PIN7
]]
function rls(soc, port, pinmsk)
end


--[[--------
设置GPIO为高电平



注意：序号是从0开始编号。

@tparam userdata soc (**in**) C代码中注册的SOC

@tparam number port (**in**) GPIO PORT的序号

@tparam number pinmsk (**in**) GPIO PIN的掩码

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-ERANGE** GPIO PORT错误<br>
　● **-EPERM** GPIO PIN未被使用

@usage
rc = stm32:set(0, stm32:pin(7)) -- PORTA, PIN7
]]
function set(soc, port, pinmsk)
end


--[[--------
设置GPIO为低电平



注意：序号是从0开始编号。

@tparam userdata soc (**in**) C代码中注册的SOC

@tparam number port (**in**) GPIO PORT的序号

@tparam number pinmsk (**in**) GPIO PIN的掩码

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-ERANGE** GPIO PORT错误<br>
　● **-EPERM** GPIO PIN未被使用

@usage
rc = stm32:reset(3, stm32:pin(7)) -- PORTD, PIN7
]]
function reset(soc, port, pinmsk)
end


--[[--------
翻转GPIO的电平



注意：序号是从0开始编号。

@tparam userdata soc (**in**) C代码中注册的SOC

@tparam number port (**in**) GPIO PORT的序号

@tparam number pinmsk (**in**) GPIO PIN的掩码

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-ERANGE** GPIO PORT错误<br>
　● **-EPERM** GPIO PIN未被使用

@usage
rc = stm32:toggle(1, stm32:pin(7)) -- PORTB, PIN7
]]
function toggle(soc, port, pinmsk)
end


--[[--------
同时输出多个GPIO PIN



注意：序号是从0开始编号。

@tparam userdata soc (**in**) C代码中注册的SOC

@tparam number port (**in**) GPIO PORT的序号

@tparam number pinmsk (**in**) GPIO PIN的掩码

@tparam number out GPIO 输出值

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-ERANGE** GPIO PORT错误<br>
　● **-EPERM** GPIO PIN未被使用

@usage
-- 输出GPIOE，掩码0xFFFFFFFF，输出值0x3
rc = stm32:output(stm32, 4, 0xFFFFFFFF, 0x3)
]]
function output(soc, port, pinmsk, out)
end


--[[--------
同时从GPIO读取多个PIN的输入值



注意：序号是从0开始编号。

@tparam userdata soc (**in**) C代码中注册的SOC

@tparam number port (**in**) GPIO PORT的序号

@tparam number pinmsk (**in**) GPIO PIN的掩码

@treturn[2] number 错误码<br>
　● **0** 没有错误<br>
　● **-ERANGE** GPIO PORT错误<br>
　● **-EPERM** GPIO PIN未被使用

@treturn[2] number GPIO的输入值

@usage
-- 读取GPIOC，掩码0xFFFFFFFF
rc, in = stm32:input(stm32, 2, 0xFFFFFFFF)
]]
function input(soc, port, pinmsk)
end
