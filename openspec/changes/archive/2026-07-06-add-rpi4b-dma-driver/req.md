## Task

为工程 `xwbd/RPi4B` 增加DMA驱动：

+ 驱动路径: `xwbd/RPi4B/board/xwac/xwds/soc.c`


## Context

+ 系统驱动框架 `xwcd/ds/soc` 提供了统一的API，新增的DMA驱动需要匹配驱动框架
  + `xwds_dma_req()` , `xwds_dma_rls()` , `xwds_dma_enable()` , `xwds_dma_disable()` 没有具体的寄存器对应，返回 `XWOK`
  + `xwds_dma_cfg()` 对应 `struct soc_dma_cb` ， `struct soc_dma_lite_cb` ， `struct soc_dma4_cb`
  + `xwds_dma_start()` 与 ``xwds_dma_stop()`` 对应 `cs` 寄存器的 `active` 位
+ 芯片DMA的头文件: `xwcd/soc/arm64/v8a/a72/bcm2711/soc_dma.h` ,其中定义了GPIO寄存器 `soc_dma`
+ 芯片Datasheet: `xwbd/RPi4B/doc/bcm2711-peripherals-manual/RP-008248-DS-1-bcm2711-peripherals.md`
+ 电路板：树莓派4B
+ 芯片：BCM2711


# Constraints

+ 遵循MISRA-C:2012标准
  + 函数至多在尾部拥有两个 `return`
    + 一个无错误 `return`
    + 一个发生错误通过 `goto` 跳转过来 `return`
  + 尽量不使用 `continue`
