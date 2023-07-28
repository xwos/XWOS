--[[--------
XWLUA模块：线程栅栏

线程栅栏是用于协调多个线程并行工作的同步机制。
当线程到达线程栅栏时会暂停运行并等待，直到所有合作的线程都达到线程栅栏，所有线程从线程栅栏开始同时执行。

@module xwos.sync.br
]]

--[[--------
新建线程栅栏



@tparam number num (**in**) 线程槽的数量

@treturn userdata 新的线程栅栏对象的强指针 `brsp`

@usage
br = xwos.br.new(32) -- 创建拥有32个槽的线程栅栏
]]
function new(num)
end
