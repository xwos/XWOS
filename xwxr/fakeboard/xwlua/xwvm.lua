--[[--------
XWLUA模块：虚拟机

XWLUA的虚拟机就是一个 **独立** 的 `struct Lua_State` ，是通过 `luaL_newstate()` 创建出来的。
XWLUA的虚拟机 **不能** 是通过 `coroutine.create()` （对应的CAPI `luaB_cocreate()` ）创建的 `struct Lua_State` 。



@module xwvm
]]



--[[--------
显示变量的内容



@param var (**in**) 变量

@usage
xwvm.dump(var)
]]
function dump(var)
end
