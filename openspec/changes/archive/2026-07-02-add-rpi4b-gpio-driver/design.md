## 上下文

RPi4B 使用 BCM2711 芯片，GPIO 控制器为单平面设计（0~57 共 58 个 PIN），无 Port 概念。XWOS SOC 设备驱动框架（`xwcd/ds/soc/chip.h`）提供了 `struct xwds_soc_driver` 函数表的 BSP 填充模式。STM32 参考实现 `xwbd/WeActMiniStm32H750/bm/Stm32Hal/xwds/soc.c` 展示了多端口 GPIO 驱动的完整范例，RPi4B 需在此基础上适配 BCM2711 的单平面特性。

配置文件中 `XWCDCFG_ds_SOC_GPIO=1`、`XWCDCFG_ds_SOC_EIRQ=1` 已启用，系统为 SMP（4 核 ARM Cortex-A72），需使用自旋锁替代关中断保护。

BCM2711 GPIO 寄存器通过 `soc_gpio` 宏（`volatile struct soc_gpio_regs`）访问，文件位于 `xwcd/soc/arm64/v8a/a72/bcm2711/soc_gpio.h`。现有 UART 驱动已直接使用该宏配置引脚（`board/xwac/xwds/uart.c`）。

## 目标 / 非目标

**目标：**
- 实现 `struct xwds_soc_driver` 中所有 GPIO 相关函数指针（除 `gpio_read_output`）
- 通过 `rpi4bxwds_soc_init/fini` 完成 SOC 设备的构造 → probe → start 生命周期
- 使用自旋锁（`struct xwos_splk`）保护多核并发访问

**非目标：**
- 不实现 `gpio_read_output`（设为 NULL，框架返回 `-ENOSYS`）
- 不实现 EIRQ 外部中断（`eirq_req`/`eirq_rls` 设为 NULL）
- 不实现 CLK、PWR、DMA 功能
- 不修改 `soc_gpio.h` 寄存器定义

## 决策

### 决策1: 寄存器访问方式 — switch/case

**选择**: 使用 switch/case 分发到各 `gpfselN` 和 `pup_pdn_cntrl_regN` 寄存器

**理由**: BCM2711 寄存器在结构体中为独立命名字段（`gpfsel0` ~ `gpfsel5`），非数组。只有 6 个 fsel 和 4 个 pupd 寄存器，switch/case 代码量小（约 10 行/函数），避免指针运算的 MISRA 风险，无需额外配置结构体。

**替代方案**:
- 指针数组（`volatile xwu32_t *regs[6]`）: 需要在配置结构体中预存地址，增加间接引用，且有跨结构体成员指针运算的 MISRA 顾虑

### 决策2: set/reset/lev 寄存器 — 直接掩码写入

**选择**: `gpset0/1`、`gpclr0/1`、`gplev0/1` 使用 `u32` 成员做掩码操作

**理由**: 这些寄存器每位对应一个 PIN，可整体写入。低 32 位写 `gpset0`，高 26 位写 `gpset1`。set/clr 为写1生效模式，无需 RMW。

### 决策3: gpio_output 实现 — 读电平 + 差分写入

**选择**: 在自旋锁内读 `gplev` 获取当前物理电平，计算差分（需置高的位和需置低的位），分别写入 `gpset` 和 `gpclr`

**理由**: BCM2711 无统一输出寄存器（仅有分立的 set/clr 写1寄存器）。自旋锁保证读-修改-写的原子性。

### 决策4: GPIO 配置结构体

**选择**: 定义 `struct rpi4bxwds_gpio_cfg { xwu32_t function; xwu32_t pud; }` 作为 `gpio_cfg` 的 cfg 参数

**理由**: BCM2711 GPIO 的基本配置仅需功能选择（GPI/GPO/ALT）和上下拉两项。结构体简洁，与框架 `void *cfg` 参数兼容。

### 决策5: 驱动数据与自旋锁

**选择**: 定义 `struct rpi4bxwds_soc_driver_data { struct xwos_splk splk; }` 作为 SOC 设备的 `.dev.data`

**理由**: 遵循现有 UART 驱动的模式（`rpi4bxwds_uartc_driver_data`）。自旋锁在 probe 阶段初始化，在所有 GPIO 操作中通过 `xwos_splk_lock_cpuirqsv` / `xwos_splk_unlock_cpuirqrs` 持锁。

### 决策6: EIRQ 空壳

**选择**: `soc->eirq.num = 0`，`eirq_req`/`eirq_rls` 驱动指针设为 NULL

**理由**: 配置要求 `XWCDCFG_ds_SOC_EIRQ=1`，结构体中 eirq 子结构必须存在。num=0 使所有 EIRQ ID 的 `-ERANGE` 检查立即失败，NULL 驱动指针由框架返回 `-ENOSYS`。后续可单独实现。

## 风险 / 权衡

- **gpio_output 读取物理电平而非输出状态**: 对于开漏输出或输入模式，`gplev` 可能不等于期望的输出值。对于标准推挽输出，物理电平等值于输出值，无影响。→ 如果未来需要精确的输出状态跟踪，可通过软件缓存扩展。

- **无 gpio_read_output**: 上层代码若调用此 API 将收到 `-ENOSYS`。当前无已知调用方依赖此 API。→ 如果未来需要，可后续添加。

- **Port 参数被忽略**: 框架通过 `port < soc->gpio.port_num` 校验，只有 PortA (0) 可通过。调用方传入非零 Port 会收到 `-ERANGE` 。→ 符合 prompt.md 要求。

- **pin_num=58 超出 64位平台枚举** : `xwds_gpio_pin_em` 在 64 位平台上枚举到 Pin 63。58 在范围内，无问题。
