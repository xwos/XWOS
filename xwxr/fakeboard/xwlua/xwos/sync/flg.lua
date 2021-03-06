--[[--------
XWLUA模块：事件标志

事件标志使用位图来管理一组事件，位图中的每个位代表一个事件，
当一个或多个事件状态发生变化时，事件对应的位也会发生变化，并唤醒正在等待的线程。

XWOS的事件标志：

+ 线程可以等待位图中的事件位被置1，也可以等待事件位被清0；
+ 线程可以等待位图中的事件位同时被置1（事件与事件之间是逻辑**与**的关系），
  也可以等待其中任意一个位被置1（事件与事件之间是逻辑**或**的关系）；
+ 线程可以等待位图中的事件位同时被清0（事件与事件之间是逻辑**与**的关系），
  也可以等待其中任意一个位被清0（事件与事件之间是逻辑**或**的关系）；
+ 线程可以选择是否**消费**事件，**“消费”**事件是指，当线程等到事件被
  唤醒，可以选择是否**清除**事件标志。**清除**的含义是：
  + 当线程等待的是位图中的事件位被置1，**清除**是指将这些位清0；
  + 当线程等待的是位图中的事件位被清0，**清除**是指将这些位置1；
+ 线程可以等待事件标志位发生**翻转**，**翻转**是指事件标志位由1变为0，或
  由0变为1，当线程等待的是标志位发生**翻转**，不可**清除**事件标志；
+ 事件位图中的位数可在创建时指定。


@module xwos.sync.flg
]]

--[[--------
新建事件标志<br>
<br>

@tparam number num 事件的数量<br>
<br>

@treturn userdata 新的事件标志对象的强指针 `flgsp`

@usage
flg = xwos.flg.new(num) -- 创建事件标志
]]
function new(num)
end
