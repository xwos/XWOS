--[[--------
XWLUA类：线程对象强指针

线程对象强指针是Lua语言中的一种 `userdata` ，用于 **强引用** XWOS的线程对象。

**强引用** 表示会增加XWOS的线程对象的 **引用计数** 。

@classmod thdsp
]]


--[[--------
元方法：__copy



将线程对象强指针拷贝到全局导出表 `xwxt` 中。

此元方法意味着线程对象的强引用多了一个， **引用计数** 加 **1** 。

@within MetaMethods
@function thdsp:metatable.__copy

@tparam userdata thdsp (**in**) 线程对象强指针

@tparam userdata vm (**in**) 目标虚拟机

@usage
thdsp = ... -- 创建线程的代码（省略）
xwxt.somethd = thdsp -- 线程对象的引用计数加1
thdsp2 = xwxt.somethd -- 线程对象的引用计数加1
xwxt.somethd = nil -- 线程对象的引用计数减1
]]


--[[--------
元方法：__gc



线程对象强指针的垃圾回收方法。

此元方法意味着线程对象的强引用少了一个， **引用计数** 减 **1** 。

@within MetaMethods
@function thdsp:metatable.__gc

@tparam userdata thdsp (**in**) 线程对象强指针

@usage
thdsp = nil -- 删除引用
collectgarbage() -- 强制垃圾回收，将调用__gc()函数
]]


--[[--------
元方法：__tostring



将线程对象强指针格式化成字符串，可用于终端打印调试。

@within MetaMethods
@function thdsp:metatable.__tostring

@tparam userdata thdsp (**in**) 线程对象强指针
@usage
print(thdsp)
]]


--[[--------
中断线程的阻塞态和睡眠态



　● 此LuaAPI会中断线程的 **阻塞状态** 和 **睡眠状态** 。
　● 阻塞和睡眠的函数将以错误码 `-EINTR` (`-4`) 退出。
　● 如果线程的 **阻塞状态** 是不可被中断的，中断将不会发生。

@tparam userdata thdsp (**in**) 线程对象强指针

@treturn number 错误码<br>
　● **0** 没有错误

@usage
rc = thdsp:intr()
]]
function thdsp:intr(thdsp)
end


--[[--------
通知线程退出



此方法用于线程 **A** 向另一个线程 **B** 设置 **退出** 状态。<br>
　● 线程 **B** 是否要退出，什么时候退出，由线程 **B** 自己决定；<br>
　● 调用此方法的线程 **A** 也不会等待子线程退出；<br>
　● 线程 **B** 可以是 **Joinable** 的，也可以是 **Detached** 的；<br>
　● 此方法可被重复调用，线程 **B** 的 **退出** 状态一旦被设置，不可被清除。

@tparam userdata thdsp (**in**) 线程对象强指针

@treturn number 错误码<br>
　● **0** 没有错误

@usage
rc = thdsp:quit()
]]
function thdsp:quit(thdsp)
end


--[[--------
等待线程结束，回收线程内存资源，抛出它的返回值



此方法类似于POSIX线程库的 `pthread_join()` ，用于等待另一个线程退出。
线程 **A** 调用此方法时，会阻塞等待线程 **B** 退出。线程 **B** 退出后，会释放线程 **B** 的内存资源，然后抛出线程 **B** 的返回值。

此方法只能对 **Joinable** 的线程 **B** 使用。

@tparam userdata thdsp (**in**) 线程对象强指针

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EINVAL** 线程不是 **Joinable** 的

@treturn number 子线程抛出的返回值

@usage
rc, childrc = thdsp:join()
]]
function thdsp:join(thdsp)
end


--[[--------
终止线程并等待它退出，回收线程内存资源，并抛出线程的返回值



此方法用于线程 **A** 停止另一个线程 **B** ，并等待线程 **B** 结束。<br>
　● 此方法只能对 **Joinable** 的线程 **B** 使用。<br>
　● 线程 **A** 调用此方法时，会对线程 **B** 设置一个 **退出** 状态，然后一直阻塞等待线程 **B** 退出。<br>
　● 线程 **B** 是否要退出，什么时候退出，由线程 **B** 自己决定。<br>
　● 当线程 **B** 退出后，此方法会释放线程 **B** 的内存资源，然后抛出线程 **B** 的返回值。<br>
　● 此方法等价于 `thdsp:quit`() + `thdsp:join`() 的组合。

@tparam userdata thdsp (**in**) 线程对象强指针

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EINVAL** 线程不是Joinable的

@treturn number 子线程抛出的返回值

@usage
rc, childrc = thdsp:stop()
]]
function thdsp:stop(thdsp)
end


--[[--------
分离线程



此方法功能类似于 `pthread_detach()` ，将线程设置为 **分离态** 。
处于 **分离态** 的线程退出后，系统将自动回收其内存资源。

@tparam userdata thdsp (**in**) 线程对象强指针

@treturn number 错误码<br>
　● **0** 没有错误

@usage
rc = thdsp:detach()
]]
function thdsp:detach(thdsp)
end


--[[--------
将线程迁移到目标CPU



此方法在多核系统中，用于将线程迁移到另一个CPU上。

@tparam userdata thdsp (**in**) 线程对象强指针

@tparam number cpu (**in**) 目标CPU的ID

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-ENODEV** CPU序号不存在

@usage
rc = thdsp:migrate(cpu)
]]
function thdsp:migrate(thdsp, cpu)
end
