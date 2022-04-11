--[[--------
XWLUA模块：信号量

信号量通常用于不同代码上下文间的同步，使用PV原语进行操作，属于操作系统比较底层的同步机制。


@module xwos.sync.sem
]]

--[[--------
新建信号量<br>
<br>

@tparam number val (**in**) 信号量的初始值<br>
<br>

@tparam number max (**in**) 信号量的最大值，0表示使用最大值<br>
<br>

@treturn userdata 新的信号量对象的强指针 `semsp`

@usage
sem = xwos.sem.new(0, 100) -- 创建信号量，初始值0，最大值100
]]
function new(val, max)
end
