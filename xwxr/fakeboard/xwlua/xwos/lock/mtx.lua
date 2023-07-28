--[[--------
XWLUA模块：互斥锁

互斥锁是用来保证不同线程正确访问共享数据的机制。访问共享数据的代码片段被称为临界区。

互斥锁 **不可** 用在 **除线程以外** 的其他上下文(Context)。

等待互斥锁的线程会被阻塞，并让出CPU的使用权。


## **to-be-closed** 特性

**to-be-closed** 特性是 **Lua-5.4** 新引入的特性。拥有 **to-be-closed** 特性的变量，在离开其作用域时，会自动被 **关闭** 。
**关闭** 的行为是由元方法 **__close()** 来定义的。


XWLUA的互斥锁对象强指针具有 **to-be-closed** 特性，并提供了 **Scope Lock** 模式 。
当互斥锁对象强指针离开作用域时，会自动解锁关联的互斥锁。


## 可重入锁

XWLUA的互斥锁是可重入锁，同一个Lua线程可以反复上锁，但也需要同样次数的解锁才能将锁解开。

@module xwos.lock.mtx
]]

--[[--------
新建互斥锁

@treturn userdata 新的互斥锁对象的强指针 `mtxsp`

@usage
mymtx = xwos.mtx.new()
]]
function new()
end
