--[[--------
XWLUA模块：当前线程

操作线程自己的方法集合

@module xwos.cthd
]]


--[[--------
返回当前线程的 **线程对象强指针** `thdsp`



@treturn userdata 当前线程的 **线程对象强指针** `thdsp`

@usage
me = xwos.cthd.sp()

@usage
`终端输出：`
> me = xwos.cthd.sp()
> me
`{0x20000660, 67}`
]]
function sp()
end


--[[--------
当前线程睡眠一段时间



调用此函数的线程会睡眠 **xwtm** ，线程会在 **当前时间点 + xwtm** 时被唤醒。

@tparam number xwtm (**in**) 需要睡眠的时间

@treturn number 错误码<br>
　● 0 没有错误<br>
　● -EINTR 睡眠过程被中断<br>
　● -ETIMEDOUT 输入的时间为负数

@usage
rc = xwos.cthd.sleep(xwtm.s(1)) -- 睡眠1s
]]
function sleep(xwtm)
end


--[[--------
当前线程睡眠到一个时间点



@tparam number to (**in**) 期望唤醒的时间点

@treturn number 错误码<br>
　● 0 没有错误<br>
　● -EINTR 睡眠过程被中断<br>
　● -ETIMEDOUT 输入的时间为负数

@usage
rc = xwos.cthd.sleep_to(xwtm.ft(xwtm.s(1))) -- 从现在开始1s后唤醒
]]
function sleep_to(to)
end


--[[--------
当前线程从一个时间起点睡眠到另一个时间点



第一次调用此方法时，需要确定一个时间起点，可以通过 `xwtm.now`() 获取当前CPU调度器的系统时间。


此方法每次返回时都会将当前的系统时间返回，可以用作下一次调用的时间起点 `origin` 。
若时间增量 `inc` 保持不变，不断循环调用此方法，可形成较精确的周期性唤醒。

@tparam number origin (**in**) 时间起点

@tparam number inc (**in**) 期望被唤醒的时间相对于起点的增量，单位：纳秒

@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EINTR** 睡眠过程被中断

@treturn userdata 下一次调用的时间起点

@usage
origin = xwos.skd.tt()
cnt = 30 -- 重复30次
repeat
  rc, origin = xwos.cthd.sleep_from(origin, xwtm.s(1)) -- 周期性每1s唤醒一次
  cnt = cnt - 1
until (cnt == 0)
]]
function sleep_from(origin, inc)
end


--[[--------
通知调度器重新选择线程



@usage
xwos.cthd.yield()
]]
function yield()
end


--[[--------
退出当前线程



@tparam number rc (**in**) 线程退出时的返回值

@usage
xwos.cthd.exit(0) -- 线程退出时抛出返回值0
]]
function exit(rc)
end


--[[--------
判断当前线程能否退出



@treturn boolean 当前线程能否退出

@usage
repeat
  threadLoop() -- 线程的功能逻辑
  shldstop = xwos.cthd.shld_stop()
until (shldstop)
]]
function shld_stop()
end


--[[--------
判断当前线程是否可被冻结，如果是，就冻结线程，之后再判断线程是否可以退出



@treturn boolean 当前线程能否退出

@treturn boolean 当前线程是否被冻结过

@usage
repeat
  threadLoop() -- 线程的功能逻辑
  shldstop, froze = xwos.cthd.frz_shld_stop()
until (shldstop)
]]
function frz_shld_stop()
end


--[[--------
判断当前线程是否可被冻结



@treturn boolean 是否可冻结当前线程

@usage
repeat
  if (xwos.cthd.shld_frz()) then
    clean_before_freezing() -- 冻结前清理资源
    xwos.cthd.frz() -- 冻结
    setup_after_frozen() -- 解冻后重新获取资源
  end
  threadLoop() -- 线程的功能逻辑
  shldstop = xwos.cthd.shld_stop()
until (shldstop)
]]
function shld_frz()
end

--[[--------
冻结当前线程



@treturn number 错误码<br>
　● **0** 没有错误<br>
　● **-EPERM** 当前不需要冻结线程<br>
　● **-EINVAL** 线程不是 **Joinable** 的

@usage
repeat
  if (xwos.cthd.shld_frz()) then
    clean_before_freezing() -- 冻结前清理资源
    xwos.cthd.frz() -- 冻结
    setup_after_frozen() -- 解冻后重新获取资源
  end
  threadLoop() -- 线程的功能逻辑
  shldstop = xwos.cthd.shld_stop()
until (shldstop)
]]
function frz()
end
