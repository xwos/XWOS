# Role

+ 你是经验丰富的嵌入式软件工程师，精通C语言，并熟悉ARMv7 Cortex-M的MPU。


# Task

实现ARMv7 Cortex-M的MPU驱动。

+ 路径： `xwcd/soc/arm/v7m`
+ 头文件： `xwcd/soc/arm/v7m/armv7m_mpu.h`
+ 源文件： `xwcd/soc/arm/v7m/armv7m_mpu.c`
+ 类型
  + `struct armv7m_mpu_config` MPU配置结构体，用户填充此结构体后通过 `armv7m_mpu_config()` 设置MPU
    + 提供完整的 TEX, S, C, B 的属性宏
+ API
  + `armv7m_mpu_get_type()` 获取MPU的type寄存器
  + `armv7m_mpu_set(struct armv7m_mpu_config * cfg)` 配置MPU的Region，支持一次设置多个Region
  + `armv7m_mpu_reset()` 取消MPU的region配置
  + `armv7m_mpu_disable()` 关闭MPU
  + `armv7m_mpu_enable()` 启用MPU


# Context

+ `xwcd/soc/arm/v7m/armv7m_isa.h` 中定义了MPU寄存器，可通过 `armv7m_scs.scb.mpu` 访问
+ 用户使用流程：
  + 1. 调用 `armv7m_mpu_disable()` 关闭MPU
  + 2. 设置 `struct armv7m_mpu_config`
  + 3. 通过 `armv7m_mpu_set()` 设置Region
  + 4. 调用 `armv7m_mpu_enable()` 启用MPU

+ 编译开关 `xwcd/soc/arm/v7m/arch.mk` 增加

```Makefile
ifeq ($(ARCHCFG_MPU),y)
  ARCH_CSRCS += armv7m_mpu.c
endif
```

+ 编译开关 `xwbd/WeActMiniStm32H750/cfg/arch.h` : `#define ARCHCFG_MPU 1`

+ 编译验证：切换路径 `xwbd/WeActMiniStm32H750` 执行 `make -j22`
  + 编译输出 `xwbd/WeActMiniStm32H750/wkspc`
  + 输出的是嵌入式固件，不可在本机执行

+ `xwcd/soc/arm/v7m/xwosimpl_soc_isa.h` 提供了内存屏障的指令



# Constraints

+ 不需要支持RBAR_A1/A2/A3, RASR_A1/A2/A3寄存器
+ 错误码: `xwos/lib/errno.h`
+ Region数量从MPU.type读取
+ 函数至多在尾部拥有两个 `return`
  + 一个无错误 `return`
  + 一个发生错误通过 `goto` 跳转过来 `return`
+ 尽量不使用 `continue`
