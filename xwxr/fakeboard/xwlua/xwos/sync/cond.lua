--[[--------
XWLUA模块：条件量

条件量属于操作系统比较底层的同步机制，可以同时阻塞多个线程，直到其他上下文使得**条件成立**，
并通知一个或所有正在等待的线程。


@module xwos.sync.cond
]]

--[[--------
新建条件量<br>
<br>

@treturn userdata 新的条件量对象的强指针 `condsp`

@usage
cond = xwos.cond.new() -- 创建条件量
]]
function new()
end
