## Task

为工程 `xwbd/RPi4B` 增加GPIO驱动：

+ 驱动路径: `xwbd/RPi4B/board/xwac/xwds/soc.c`
+ 端口映射关系: `PortA Pin[0:57]` 对应芯片的 `Gpio[0:57]` ，
  实际代码中，忽略Port参数，不必关心是 PortA、PortB还是PortC。


## Context

+ 系统驱动框架 `xwcd/ds/soc` 提供了统一的API，新增的GPIO驱动需要匹配驱动框架
+ 其他芯片工程的参考代码：
  + STM32: `xwbd/WeActMiniStm32H750/bm/Stm32Hal/xwds/soc.c`
  + FC7300: `../XWKSPC/FCIDE/Fc7300DemoBoardXwos/Mcal/xwds/soc.c`
+ 芯片GPIO的头文件: `xwcd/soc/arm64/v8a/a72/bcm2711/soc_gpio.h` ,其中定义了GPIO寄存器 `soc_gpio`
+ 芯片Datasheet: `xwbd/RPi4B/doc/bcm2711-peripherals-manual/RP-008248-DS-1-bcm2711-peripherals.md`
+ OS是SMP系统，应该使用自旋锁来保护上下文：
  + 可以建立结构体 `struct rpi4bxwds_soc_driver_data` 其中包含 `struct xwos_splk splk;`
  + 自旋锁头文件 `xwos/osal/lock/spinlock.h`
+ 电路板：树莓派4B
+ 芯片：BCM2711


# Constraints

+ 遵循MISRA-C:2012标准
  + 函数至多在尾部拥有两个 `return`
    + 一个无错误 `return`
    + 一个发生错误通过 `goto` 跳转过来 `return`
  + 尽量不使用 `continue`
