--[[--------
XWLUA模块：调度器

与调度器相关的功能

@module xwos.skd
]]


--[[--------
获取当前CPU的ID



@treturn number CPU的ID

@usage id = xwos.skd.id()
]]
function id()
end


--[[--------
关闭本地CPU调度器的抢占



@usage xwos.skd.dspmpt()
]]
function dspmpt()
end

--[[--------
开启本地CPU调度器的抢占



@usage xwos.skd.enpmpt()
]]
function enpmpt()
end
