## 上下文

RPi4B/BCM2711 的 SOC 驱动位于 `xwbd/RPi4B/board/xwac/xwds/soc.c`，已实现 GPIO 的输入输出操作，但外部中断（EIRQ）部分为空。`cfg/xwcd.h` 中 `XWCDCFG_ds_SOC_EIRQ=1` 已启用，通用层 `xwcd/ds/soc/eirq.c` 提供了 `xwds_eirq_req()` 和 `xwds_eirq_rls()` API，负责 ISR 表管理和引用计数。板级驱动只需实现 `struct xwds_soc_driver` 中的两个函数指针 `eirq_req` 和 `eirq_rls`，以及对应的中断分发逻辑。

参考实现：STM32H750 的 `xwbd/WeActMiniStm32H750/bm/Stm32Hal/xwds/soc.c`；GIC ISR 注册参考：`xwbd/RPi4B/board/xwac/xwds/miniuart.c` 的 `rpi4bxwds_miniuart_drv_start()`。

BCM2711 GPIO 模块特性：
- 58 根 GPIO 引脚（0~57），分为 3 个中断 Bank：Bank 0（pin 0~27）、Bank 1（pin 28~45）、Bank 2（pin 46~57）
- 检测寄存器按 32-pin 分组：`gparen0`/`gpafen0`/`gphen0`/`gplen0`（pin 0~31）、`gparen1`/`gpafen1`/`gphen1`/`gplen1`（pin 32~57）
- 统一事件状态寄存器：`gpeds0`（pin 0~31）、`gpeds1`（pin 32~57），均为 write-1-to-clear
- 4 条中断线到 VC→GIC：`SOC_VC_IRQ_GPIO0`(96) → Bank 0、`GPIO1`(97) → Bank 1、`GPIO2`(98) → Bank 2、`GPIO3`(99) → 所有 Bank 的 OR

## 目标 / 非目标

**目标：**
- 实现 `xwds_eirq_req()`/`xwds_eirq_rls()` 的板级驱动，使上层可通过标准 API 使用 GPIO 外部中断
- 全部使用异步检测寄存器，不依赖外设时钟
- 支持 4 种触发模式：上升沿、下降沿、高电平、低电平
- 4 路 bank ISR 在 probe 阶段静态注册到 GIC，无需动态管理
- 在 bank ISR 中查询 gpeds 并分发到用户 ISR，保证多个引脚同时触发时不会丢失事件

**非目标：**
- 不实现 DMA 触发（`XWDS_SOC_EIF_DMA`）
- 不实现跨 CPU 的 bank ISR 亲和性分发（本地 CPU 即可）
- 不修改通用层 EIRQ API 或数据结构

## 决策

### 决策1: EIRQ ID 模型 — pin 级

每个 GPIO 引脚映射一个独立的 EIRQ ID（0~57），`rpi4bxwds_soc.eirq.num = 58`。

**理由**：与 STM32 参考实现和现有上层使用者（如 `button.rs`）保持一致。用户通过 `eiid` 直接对应 GPIO 引脚号，每个引脚可独立注册 ISR。

**替代方案**：Bank 级 EIRQ（num=4），强制用户在 ISR 中自行判断引脚。被否决——破坏现有 API 约定，增加上层使用复杂度。

### 决策2: 全部使用异步检测寄存器

边沿触发使用 `gparen`/`gpafen`，电平触发使用 `gphen`/`gplen`。

**理由**：异步寄存器不需要外设时钟，可在休眠状态下保持中断唤醒能力。用户要求统一使用异步模式。

**替代方案**：使用同步寄存器 `gpren`/`gpfen`——需要外设时钟运行，低功耗场景下无法唤醒。

### 决策3: Bank ISR 在 probe 阶段注册

在 `rpi4bxwds_soc_drv_probe()` 中注册 4 个 bank ISR 到 GIC。

**理由**：bank ISR 是 SOC 级别的基础设施，不绑定特定设备实例。probe 阶段注册保证在任何用户调用 `eirq_req()` 之前 ISR 已就绪。GIC 中断在 GPIO 侧不使能检测位的情况下不会触发，因此提前使能是安全的。

**替代方案**：在首次 `eirq_req()` 时动态注册——增加复杂度（需要引用计数决定何时 unregister），且与 probe 在同一个初始化路径上没有明显收益。

### 决策4: Bank ISR 实现策略

4 个 bank ISR 分别实现（而非一个通用 ISR 判断 bank）。

**理由**：GIC ISR 的 `xwisr_f` 签名是 `void (*)(void)`，无法传递参数。4 个独立 ISR 读取对应的 `gpeds0/1` 寄存器，代码路径更短、更确定。

**替代方案**：使用 `xwospl_irq_get_id()` 在 ISR 中获取当前 IRQ 号判断 bank——增加运行时开销，对实时性无益。

### 决策5: eirq_req 中 GPIO 引脚号验证

`eirq_req` 中不验证 `pinmask` 与 `eiid` 的关系（`XWBOP_BIT(eiid) == pinmask`），由调用者保证正确性。

**理由**：STM32 参考实现中有此验证，但 BCM2711 的 EIRQ 模型是 pin 级直接映射，`eiid` 与 `pinmask` 的关系天然一一对应。多余的验证会增加代码路径而不提升安全性。与 GPIO 操作函数风格保持一致（gpio_set/reset 等也不验证 pinmask）。

## 风险 / 权衡

- **电平触发模式在 GIC 侧使用 LEVEL 触发类型**：电平触发的 GPIO 中断会在条件满足期间持续触发 bank ISR。需要在用户 ISR 中处理重入问题。用户应尽快在 ISR 中清除触发条件或释放中断。
  → 缓解措施：文档中说明电平触发模式的注意事项；bank ISR 在调用用户 ISR 前清除 gpeds，但电平保持时会立即重新置位。

- **gpeds 的 write-1-to-clear 语义**：写入 0 的位不受影响。bank ISR 需写入读取到的完整 gpeds 值以清除所有已触发的事件，而非仅清除当前处理的位。这安全，因为 bank ISR 在 ISR 上下文中不会被同一个 bank IRQ 重入（GIC 同一 IRQ 线保持激活状态时不会再次触发）。

- **Bank 3 ISR 为所有 Bank 的 OR 线**：datasheet 指出第四条中断线在任意 Bank 有事件时触发。实现为同时调用 bank0/1/2 的 ISR，保证 pin 46~57 的中断在常规 bank 2 ISR 之外也能被处理。这种冗余带来确定的安全性：每个 bank ISR 只在各自事件范围内分发。但如果 bank3 ISR 和 bank0/1/2 ISR 同时触发（由于不同 CPU 或中断抢占），可能出现重复分发。实际使用中 GIC 在同一中断线上不会重入，且 bank ISR 清除 gpeds 后才分发，因此重复分发风险极低。
