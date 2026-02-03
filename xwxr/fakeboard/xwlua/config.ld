-- ldoc configuration file
title = "XWLUA参考手册"
project = 'XWLUA'
format = 'markdown'
description = [[
XWOS LUA API参考手册
]]
full_description = [[

## XWLUA的扩展

### 多线程

原生的Lua语言不支持多线程（Lua中的类型 `thread` 并不是真正的线程，而是协程。），脚本中的全局变量只能由执行脚本的线程自身访问。
XWLUA在C语言层面开发了多线程的功能，每个线程都拥有一个 **独立的Lua虚拟机** 。

### 全局导出表

为了实现多线程共享数据，XWLUA中实现了一个 **全局导出表(xwxt)** ，全局导出表中的数据对所有线程可见，并且是线程安全的。
全局导出表也是Lua中的一个独立的虚拟机。

### 新的Metamethod：`__copy`

元方法 `__copy()` 用于将用户数据从一个虚拟机（全局导出表、线程） **深拷贝** 到另一个虚拟机（线程、全局导出表）中。
只有 **userdata** 可以定义 `__copy()` 元方法。

元方法 `__copy()` 有两个参数，第一个参数是 **数据** ，第二参数是 **目的虚拟机** ，无返回值。
元方法 `__copy()` 运行在 **保护模式** ，可以 **抛出错误** 。

实现元方法 `__copy()` 时，若拷贝失败，应该在 **目的虚拟机** 中压入一个 **nil** 。


XWLUA的对象都提供了元方法 `__copy()` 。


### 对象强指针

XWLUA使用 **对象强指针** 管理XWOS内核中的对象（线程、信号量、条件量、事件标志、信号选择器、线程栅栏、互斥锁、自旋锁、顺序锁）。
**对象强指针** 是一种 **userdata** 。 **对象强指针** 就是对XWOS中的 [对象描述符](../docs/TechRefManual/XwosStd#对象描述符) 的封装。
再借助于Lua虚拟机的GC机制，可实现对象的生命周期自动管理：

+ Lua虚拟机每引用一个 **对象强指针** ，都会增加相关联对象的引用计数；
+ Lua虚拟机每 **GC** 一个 **对象强指针** ，都会减少相关联对象的引用计数；
+ 当相关联对象的引用计数为 **0** 时，就会执行C语言层面对象的 `GC` 函数 。

**对象强指针** 的概念源于Lua语言的 **强引用** 。

[**返回首页**](/)
]]
format = 'discount'
backtick_references = true
file = {
'xwtm.lua',
'xwvm.lua',
'xwxt.lua',
'xwlib/bmp.lua',
'xwos/skd.lua', 'xwos/thd.lua', 'xwos/cthd.lua', 'xwos/thdsp.lua',
'xwos/lock/mtx.lua', 'xwos/lock/mtxsp.lua',
'xwos/lock/splk.lua', 'xwos/lock/splksp.lua',
'xwos/lock/sqlk.lua', 'xwos/lock/sqlksp.lua',
'xwos/sync/sem.lua', 'xwos/sync/semsp.lua',
'xwos/sync/cond.lua', 'xwos/sync/condsp.lua',
'xwos/sync/flg.lua', 'xwos/sync/flgsp.lua',
'xwos/sync/br.lua', 'xwos/sync/brsp.lua',
'xwos/sync/sel.lua', 'xwos/sync/selsp.lua',
'xwos/pm.lua',
'xwds/soc.lua',
'xwds/uart.lua',
'xwds/spim.lua',
'xwds/i2cm.lua', 'xwds/i2cm_msg.lua'
}
style = '!new'
dir = '../../wkspc/luadoc'
