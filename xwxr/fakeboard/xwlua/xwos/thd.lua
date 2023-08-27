--[[--------
XWLUA模块：线程

原生的Lua语言不支持多线程（Lua中的类型 **thread** 并不是真正的线程，而是协程。），
脚本中的全局变量只能由执行脚本的线程自身访问。
XWLUA在C语言层面开发了多线程的功能，每个线程都拥有一个 **独立的Lua虚拟机** 。

XWLUA线程分为

+ Detached线程：线程结束后由系统自动回收资源，抛出的返回值不可被获取；
+ Joinable线程：线程结束后需要由另一个线程进行 `thdsp::join()` ， `thdsp::join()` 后可回收其资源，并捕获其返回值。

XWLUA线程的三种形式：

+ 文件线程：基于脚本文件创建线程
+ 字符串线程：基于脚本字符串创建线程
+ 函数线程：基于函数创建线程


## XWLUA线程的返回值

Joinable线程可以 `return` 多个值，这些返回值可被另一个线程 `thdsp::join()` 捕获。
值的类型支持：

+ boolean
+ lightuserdata
+ userdata
+ number
+ string
+ function
+ table

Lua中的协程类型 `LUA_TTHREAD` **不支持** 在线程之间传递。

@module xwos.thd
]]


--------
-- 从文件系统中读取脚本文件 **file** ，然后新建一个线程运行脚本
--
--
--
-- @tparam string file (**in**) lua脚本文件的路径
--
-- @tparam boolean joinable (**in** , **optional**) 是否创建 Joinable 的线程，默认值：false
--
-- @tparam boolean name (**in** , **optional**) 线程名，默认值：lua脚本文件的路径
--
-- @tparam boolean unprivileged (**in** , **optional**) 是否创建不具有系统权限的线程，默认值：false
--
-- @treturn userdata 新的线程对象的强指针 `thdsp` 或 `nil`<br>
-- ● 当 `joinable` 为 `true` 时，此函数一定返回线程对象的强指针 `thdsp` 。<br>
-- ● 当 `joinable` 为 `false` 时，此函数不一定返回线程对象的强指针 `thdsp` 。因为 **Detached** 的线程可能先运行结束，此时将返回 `nil` 。
--
--
--
-- @usage
-- > xwos.thd.dofile("sd:/hw.lua") -- 创建 Detached 线程
--
-- `终端输出：`
-- `XWLUA Test Script
--
-- Thread strong pointer:	{0x20000880, 63}
-- pi =	3.1415926535898
-- 2^11 =	-2.5060358315688e-94
-- exp(10) =	22026.465794807
-- t1 = 5
-- t1[5] = 5
-- 1	1
-- 2	2
-- 3	false
-- 4	4
-- 5	5
-- 6	6
-- 7	7
-- c	c
-- b	b
-- a	a`
--
-- @usage
-- > childsp = xwos.thd.dofile("sd:/hw.lua", true)
-- > rc, rcmsg = xwxt.childsp:join()
-- > print(rcmsg)
-- > childsp = nil
-- > collectgarbage()
--
-- `终端输出：`
-- `XWLUA Test Script
--
-- Thread strong pointer:	{0x20000880, 63}
-- pi =	3.1415926535898
-- 2^11 =	-2.5060358315688e-94
-- exp(10) =	22026.465794807
-- t1 = 5
-- t1[5] = 5
-- 1	1
-- 2	2
-- 3	false
-- 4	4
-- 5	5
-- 6	6
-- 7	7
-- c	c
-- b	b
-- a	a`
--
-- > rc, rcmsg = xwxt.childsp:join()
-- > print(rcmsg)
-- `OK`
--
function dofile(file, joinable, name, unprivileged)
end


--------
-- 建立一个新的线程执行字符串
--
--
-- @tparam string str (**in**) 字符串
--
-- @tparam boolean joinable (**in** , **optional**) 是否创建 Joinable 的线程，默认值：false
--
-- @tparam boolean name (**in** , **optional**) 线程名，默认值：lua脚本文件的路径
--
-- @tparam boolean unprivileged (**in** , **optional**) 是否创建不具有系统权限的线程，默认值：false
--
--
-- @treturn userdata 新的线程对象的强指针 `thdsp`
--
-- @usage
-- script = [[
-- print("Child Thread:", xwos.cthd.sp())
-- rcfunc = function ()
--   print("OK")
-- end
-- return rcfunc, "Hi", 1
-- ]]
-- childsp = xwos.thd.dostring(script, true)
-- rc, rcfunc, msg, num = childsp:join()
-- rcfunc()
-- print(msg)
-- print(num)
-- childsp = nil
-- collectgarbage()
--
--
-- `终端输出：`
-- > script = [[
-- > print("Child Thread:", xwos.cthd.sp())
-- > rcfunc = function ()
-- >   print("OK")
-- > end
-- > return rcfunc, "Hi", 1
-- > ]]
-- > childsp = xwos.thd.dostring(script, true)
-- `Child Thread:	{0x20000660, 67}`
--
-- > rc, rcfunc, msg, num = childsp:join()
-- > rcfunc()
-- `OK`
-- > print(msg)
-- `Hi`
-- > print(num)
-- `1`
function dostring(str, joinable, name, unprivileged)
end


--------
-- 建立一个新的线程调用函数
--
--
--
-- @tparam function func (**in**) 函数
--
-- @tparam boolean joinable (**in**) 是否创建 Joinable 的线程，默认值：false
--
-- @tparam boolean name (**in** , **optional**) 线程名，默认值：lua脚本文件的路径
--
-- @tparam boolean unprivileged (**in** , **optional**) 是否创建不具有系统权限的线程，默认值：false
--
-- @treturn userdata 新的线程对象的强指针 `thdsp`
--
-- @usage
-- function childthd_main ()
--   print("Child Thread:", xwos.cthd.sp())
--   result = {}
--   result.func = function ()
--     print("OK")
--   end
--   result.msg = "OK"
--   result.tab = {0, 1, 2}
--   result[{"0", "1", "2"}] = {0, 1, 2}
--   mt = {
--     ["__os"] = "XWOS"
--   }
--   setmetatable(result, mt);
--   return result
-- end
-- childsp = xwos.thd.call(childthd_main, true)
-- rc, rctab = childsp:join()
-- print(rctab.msg)
-- rctab.func()
-- mt = getmetatable(rctab)
-- print(mt.__os)
-- childsp = nil
-- collectgarbage()
--
-- `终端输出：`
-- > function childthd_main ()
-- >   print("Child Thread:", xwos.cthd.sp())
-- >   result = {}
-- >   result.func = function ()
-- >     print("OK")
-- >   end
-- >   result.msg = "OK"
-- >   result.tab = {0, 1, 2}
-- >   result[{"0", "1", "2"}] = {0, 1, 2}
-- >   mt = {
-- >     ["__os"] = "XWOS"
-- >   }
-- >   setmetatable(result, mt);
-- >   return result
-- > end
-- > childsp = xwos.thd.call(childthd_main, true)
-- `Child Thread:	{0x20000550, 69}`
--
-- > rc, rctab = childsp:join()
-- > print(rctab.msg)
-- `OK`
-- > rctab.func()
-- `OK`
-- > mt = getmetatable(rctab)
-- > print(mt.__os)
-- `XWOS`
function call(func, joinable, name, unprivileged)
end
