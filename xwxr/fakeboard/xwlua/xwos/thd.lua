--[[--------
XWLUA模块：线程

原生的Lua语言不支持多线程（Lua中的类型 **thread** 并不是真正的线程，而是协程。），
脚本中的全局变量只能由执行脚本的线程自身访问。
XWLUA在C语言层面开发了多线程的功能，每个线程都拥有一个独立的Lua虚拟机。

@module xwos.thd
]]


--------
-- 从文件系统中读取脚本文件 **file** ，然后新建一个线程运行脚本
--
--
--
-- @tparam string file (**in** ) lua脚本文件的路径
--
-- @treturn userdata 新的线程对象的强指针 `thdsp`
--
-- @usage
-- childthdsp = xwos.thd.dofile("sd:/hw.lua")
-- childthdsp:stop() -- 停止线程并等待线程退出，然后回收其内存资源
--
-- @usage
-- `终端输出：`
-- > childthdsp = xwos.thd.dofile("sd:/hw.lua")
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
-- > childthdsp:stop()
-- `0	0`
function dofile(file)
end


--------
-- 建立一个新的线程执行字符串
--
--
-- @tparam string str (**in**) 字符串
--
--
-- @treturn userdata 新的线程对象的强指针 `thdsp`
--
-- @usage
-- script = [[
-- mythdsp = xwos.cthd.sp()
-- print("Thread strong pointer:", mythdsp)
-- ]]
-- strthdsp = xwos.thd.dostring(script)
-- strthdsp
-- strthdsp:stop()
--
-- @usage
-- `终端输出：`
-- > script = [[
-- >> mythdsp = xwos.cthd.sp()
-- >> print("Thread strong pointer:", mythdsp)
-- >> ]]
-- > strthdsp = xwos.thd.dostring(script)
-- `Thread strong pointer:	{0x20000660, 67}`
--
-- > strthdsp
-- `{0x20000660, 67}`
-- > strthdsp:stop()
-- `0	0`
function dostring(str)
end


--------
-- 建立一个新的线程调用函数
--
--
--
-- @tparam function func (**in**) 函数
--
-- @treturn userdata 新的线程对象的强指针 `thdsp`
--
-- @usage
-- function childthd_main ()
--   mythdsp = xwos.cthd.sp()
--   print("Thread strong pointer:", mythdsp)
-- end
-- cthdsp = xwos.thd.call(childthd_main)
-- cthdsp
-- cthdsp:stop()
-- @usage
-- `终端输出：`
-- > function childthd_main ()
-- >>   mythdsp = xwos.cthd.sp()
-- >>   print("Thread strong pointer:", mythdsp)
-- >> end
-- > cthdsp = xwos.thd.call(childthd_main)
-- > Thread strong pointer:	{0x20000550, 69}
--
-- > cthdsp
-- `{0x20000550, 69}`
-- > cthdsp:stop()
-- `0	0`
function call(func)
end
