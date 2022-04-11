--[[--------
XWLUA类：线程对象强指针

线程对象强指针是Lua语言中的一种 `userdata` ，用于**强引用**XWOS的线程对象。

**强引用**表示会增加XWOS的线程对象的**引用计数**。

@classmod thdsp
]]


--[[--------
元方法：__copy<br>
<br>

将线程对象强指针拷贝到全局导出表 `xwxt` 中。

此元方法意味着线程对象的强引用多了一个，**引用计数**加1。

@within MetaMethods
@function thdsp:metatable.__copy

@tparam userdata thdsp (**in**) 线程对象强指针<br>
<br>

@tparam userdata vm (**in**) 目标虚拟机<br>
<br>

@usage
thdsp = ... -- 创建线程的代码（省略）
xwxt.somethd = thdsp -- 线程对象的引用计数加1
thdsp2 = xwxt.somethd -- 线程对象的引用计数加1
xwxt.somethd = nil -- 线程对象的引用计数减1
]]


--[[--------
元方法：__gc<br>
<br>

线程对象强指针的垃圾回收方法。

此元方法意味着线程对象的强引用少了一个，**引用计数**减1。

@within MetaMethods
@function thdsp:metatable.__gc

@tparam userdata thdsp (**in**) 线程对象强指针<br>
<br>

@usage
thdsp = nil -- 删除引用
collectgarbage() -- 强制垃圾回收，将调用__gc()函数
]]


--[[--------
元方法：__tostring<br>
<br>

将线程对象强指针格式化成字符串，可用于终端打印调试。

@within MetaMethods
@function thdsp:metatable.__tostring

@tparam userdata thdsp (**in**) 线程对象强指针<br>
<br>
@usage
print(thdsp)
]]


--[[--------
通知线程退出<br>
<br>

此方法用于用于线程A向另一个线程B设置**退出**状态。

+ ● 线程B是否要退出，什么时候退出，由线程B自己决定；
+ ● 调用此方法的线程A也不会等待子线程退出；
+ ● 线程B可以是Joinable的，也可以是Detached的；
+ ● 此方法可被重复调用，线程B的**退出**状态一旦被设置，不可被清除。

@tparam userdata thdsp (**in**) 线程对象强指针<br>
<br>

@treturn number
+ ● **rc** 此方法的返回值
  + ○ **0** 没有错误

@usage
rc = thdsp:quit()
]]
function thdsp:quit(thdsp)
end


--[[--------
等待线程结束，回收线程内存资源，抛出它的返回值<br>
<br>

此方法类似于POSIX线程库的`pthread_join()`，用于等待另一个线程退出。
线程A调用此方法时，会阻塞等待线程B退出，线程B退出后，会释放线程B的内存资源，然后返回线程B的返回值。

此方法只能对Joinable的线程B使用。

@tparam userdata thdsp (**in**) 线程对象强指针<br>
<br>

@treturn {number,number}
+ ● **rc** 此方法的返回值
  + ○ **0** 没有错误
  + ○ **-EINVAL** 线程不是Joinable的
+ ● **childrc** 子线程抛出的返回值

@usage
rc, childrc = thdsp:join()
]]
function thdsp:join(thdsp)
end


--[[--------
终止线程并等待它退出，回收线程内存资源，并抛出线程的返回值<br>
<br>

此方法用于线程A停止另一个线程B，并等待线程B结束。与 `thdsp:quit`() 不同，此方法只能对Joinable的线程B使用。

+ ● 线程A调用此方法时，会对线程B设置一个**退出**状态，然后一直阻塞等待线程B退出。
+ ● 线程B是否要退出，什么时候退出，由线程B自己决定。
+ ● 当线程B退出后，此方法会释放线程B的内存资源，然后返回线程B的返回值。
+ ● 此方法等价于 `thdsp:quit`() + `thdsp:join`() 的组合。

@tparam userdata thdsp (**in**) 线程对象强指针<br>
<br>

@treturn {numberm,number}
+ ● **rc** 此方法的返回值
  + ○ **0** 没有错误
  + ○ **-EINVAL** 线程不是Joinable的
+ ● **childrc** 子线程抛出的返回值

@usage
rc, childrc = thdsp:stop()
]]
function thdsp:stop(thdsp)
end


--[[--------
分离线程<br>
<br>

此方法功能类似于`pthread_detach()`，将线程设置为**分离态**。
处于**分离态**的线程退出后，系统将自动回收其内存资源。

@tparam userdata thdsp (**in**) 线程对象强指针<br>
<br>

@treturn number
+ ● **rc** 此方法的返回值
  + ○ **0** 没有错误

@usage
rc = thdsp:detach()
]]
function thdsp:detach(thdsp)
end


--[[--------
将线程迁移到目标CPU<br>
<br>

此方法在多核系统中，用于将线程迁移到另一个CPU上。

@tparam userdata thdsp (**in**) 线程对象强指针<br>
<br>

@tparam number cpu (**in**) 目标CPU的ID<br>
<br>

@treturn number
+ ● **rc** 此方法的返回值
  + ○ **0** 没有错误
  + ○ **-ENODEV** CPU序号不存在

@usage
rc = thdsp:migrate(cpu)
]]
function thdsp:migrate(thdsp, cpu)
end
