--[[--------
XWLUA模块：全局导出表

为了实现多线程共享数据，XWLUA中实现了一个 **全局导出表(xwxt)** ，全局导出表中的数据对所有线程可见，并且是线程安全的。

```lua
xwxt.a = 3 -- 在xwxt中创建Number变量a，并赋值3，xwxt.a对所有线程可见
xwxt["a"] = 3 -- 与上一句等价

xwxt.f = function () print("HI, XWOS") end -- 在xwxt中创建函数f，xwxt.f可被所有线程访问
xwxt["f"] = function () print("HI, XWOS") end -- 与上一句等价

myfunc = xwxt.f -- 任意线程都可从全局导出表中获取函数f

xwxt.f = nil -- 删除xwxt中的函数f
xwxt.gc() -- 强制全局导出表做一次垃圾回收
```


支持在全局导出表中传递的类型：

+ boolean
+ lightuserdata
+ userdata
+ number
+ string
+ function
+ table


@module xwxt
]]

--[[--------
元方法：__newindex



将全局导出表 `xwxt` 的 **key** 赋值为 **value** 。其结果是将数据导出到全局导出表，数据将对所有线程可见。

全局导出表的 **索引** 只能是 **字符串** 。

Lua中的原生数据类型，导出到全局导出表时，会做一次 **深** 拷贝，这些数据类型是：

　● 数值类型：直接拷贝；<br>
　● 字符串：直接拷贝；<br>
　● 函数：dump()成字符串后拷贝；<br>
　● 布尔类型：直接拷贝；<br>
　● 表：<br>
　　○ 如果 **metatable["__copy"]** 是一个函数，则执行 **metatable.__copy()** 函数；<br>
　　○ 如果 **metatable["__copy"]** 为空或不是函数，则依次拷贝表中以及 **metatable** 中的每一个键值对，但如果键值对的 **索引** 不为字符串，就跳过。<br>
　● userdata：<br>
　　○ 如果 **metatable["__copy"]** 是一个函数，则执行 **metatable.__copy()** 函数；<br>
　　○ 如果 **metatable["__copy"]** 为空或不是函数，就略过。<br>
　● lightuserdata：同userdata。<br>

@within MetaMethods
@function __newindex

@tparam userdata xwxt (**in**) 全局导出表 `xwxt`

@tparam string key (**in**) 索引

@param value (**in**) 值

@usage
xwxt.key = value
xwxt["key"] = value
xwxt.key = nil -- 从全局导出表 `xwxt` 中删除"key"
]]

--[[--------
元方法：__index



将全局导出表 `xwxt` 中索引的值读取到线程。

@within MetaMethods
@function __index

@tparam userdata xwxt (**in**) 全局导出表 `xwxt`

@tparam string key (**in**) 索引

@return 全局导出表中索引的值

@usage
value = xwxt.key
value = xwxt["key"]
]]

--[[--------
将源虚拟机中的表元素导出到全局导出表 `xwxt`



表中的 **索引** 只能是字符串，其他将被忽略。

@tparam table table (**in**) 表

@usage
xwxt.append(table)
]]
function append(table)
end

--[[--------
强制全局导出表 `xwxt` 做一次垃圾回收



@usage
xwxt.gc()
]]
function gc()
end
