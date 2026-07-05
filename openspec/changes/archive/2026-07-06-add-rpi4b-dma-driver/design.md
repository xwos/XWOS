## 上下文

RPi4B 使用 BCM2711 芯片，拥有 16 个 DMA 通道，分属三种硬件类型：
- 普通 DMA（Ch 0~6, 15）：全功能，2D 传输，32 位寻址
- DMA Lite（Ch 7~10）：精简版，仅 1D，32 位寻址
- DMA4（Ch 11~14）：高性能，2D 传输，40 位寻址

其中 Ch15 被 VPU 独占，ARM 侧可用通道 0~14 共 15 个。每个通道间隔 0x100 字节，
寄存器定义在 `xwcd/soc/arm64/v8a/a72/bcm2711/soc_dma.h`。

XWOS SOC 设备驱动框架（`xwcd/ds/soc/dma.h`）提供了 7 个 DMA API 函数，对应的
BSP 驱动函数表在 `struct xwds_soc_driver` 中。现有 `soc.c` 已实现 GPIO 和 EIRQ
的 BSP 函数，DMA 的 7 个函数指针为空。

配置文件 `XWCDCFG_ds_SOC_DMA=1` 已启用。中断通过 ARM GIC 注册，
现有 EIRQ 驱动已展示了 `armv8a_gic_irq_set_isr` 的标准注册模式。

## 目标 / 非目标

**目标：**
- 实现 `struct xwds_soc_driver` 中全部 7 个 DMA 函数指针
- 在 `probe()` 中注册 12 个 DMA ISR 到 GIC
- 在 `rpi4bxwds_soc` 对象中添加 `.dma` 子结构体（`ch_num=15`，回调表）
- 使用与 GPIO/EIRQ 相同的自旋锁保护 DMA 寄存器访问
- 三种通道类型通过 switch/case 分发到对应寄存器宏

**非目标：**
- 不驱动 Ch15（VPU 独占）
- 不实现 DMA 控制块（CB）的内存分配管理（由调用方负责）
- 不修改 `soc_dma.h` 寄存器定义
- 不实现 `dma_enable` / `dma_disable` 的寄存器操作（直接返回 `XWOK`）

## 决策

### 决策1: dma_req — 通道复位

**选择**: `dma_req` 执行通道复位操作：普通/Lite 写入 CS.RESET=1，DMA4 写入 DEBUG.RESET=1

**理由**: 复位确保通道从干净状态开始，清除上一次传输可能残留的状态。不同通道类型的
RESET 位位于不同寄存器的不同位（普通/Lite 在 CS bit 31，DMA4 在 DEBUG bit 23）。

**替代方案**:
- 不复位通道：可能残留上一次传输的错误状态，导致后续传输异常
- 在 `dma_rls` 时复位：释放时不影响当前用户，但无法保证下一次申请时的清洁状态

### 决策2: dma_rls — 清除 CONBLK_AD

**选择**: `dma_rls` 清除 CONBLK_AD 寄存器为 0，不执行复位

**理由**: 清理 CB 地址防止误启动，但保留通道其他状态供下次使用。复位已在 `dma_req` 中完成。

### 决策3: dma_start / dma_stop — 简化语义

**选择**: `dma_start` 置 CS.ACTIVE=1，`dma_stop` 清 CS.ACTIVE=0

**理由**: ACTIVE 位是 DMA 传输的开关。清 ACTIVE 是暂停而非彻底终止，
CB 地址保留，可通过重新置 ACTIVE 恢复。终止传输需配合 `dma_cfg` 更新新的 CB。

**约束**: `dma_start` 需校验通道已完成 `dma_cfg` 配置（CONBLK_AD 非零或 cfg 标记已设置）。

**替代方案**:
- stop 时执行 ABORT：但 ABORT 会跳到下一个 CB（如果链式），语义更复杂，且 prompt 指定只操作 active 位

### 决策4: dma_cfg — 通道类型感知的地址写入

**选择**: 根据通道号分发到对应寄存器宏，考虑不同 CB 地址格式
- 普通 DMA / DMA Lite：CONBLK_AD = 完整 32 位地址
- DMA4：CB 寄存器 = `addr >> 5`

**理由**: BCM2711 的 DMA4 通道使用不同的 CB 地址寄存器格式（低 5 位被压缩，
硬件实际使用 `addr[36:5]`）。写入前需校验地址 32 字节对齐（低 5 位为 0）。

**替代方案**:
- 统一使用完整地址由硬件自动处理：DMA4 无法正确处理，地址不匹配
- 不做地址对齐校验：未对齐地址会导致硬件错误或不可预期的 DMA 行为

### 决策5: 通道分发 — switch/case

**选择**: 在操作函数内使用 switch/case 分发到对应通道的寄存器宏（`soc_dma0` ~ `soc_dma14`）

**理由**: 与 GPIO 驱动的 `gpfsel` 分发模式一致。寄存器宏为独立的命名变量
（`soc_dma0`, `soc_dma1`, ...），非数组。switch/case 代码量可控
（15 个 case），避免指针运算的 MISRA 风险。

**替代方案**:
- 基地址 + 偏移计算：寄存器结构体非数组，无法通过 ch*0x100 直接索引；
  需预存 15 个基地址指针，增加存储和初始化开销

### 决策6: 中断架构 — 每 IRQ 线一个 ISR

**选择**: 为每个 IRQ 线注册一个 ISR，共 12 个 ISR
- 独立 IRQ（DMA0~6, DMA11~14）：各一个 ISR，直接检查对应通道的 CS.INT
- 共享 IRQ（DMA7_8, DMA9_10）：各一个 ISR，检查两个通道的 CS.INT

**理由**: 遵循 EIRQ 驱动的 ISR 模式。共享 IRQ 的 ISR 遍历其关联通道，
检查 CS.INT 标志，仅调用已触发中断的通道的回调。

**ISR 注册时机**: 在 `probe()` 中通过 `armv8a_gic_irq_set_isr` 注册，
与 EIRQ 的三 bank ISR 注册模式一致。

### 决策7: 通道使用追踪

**选择**: 在设备数据结构体中维护 `xwu16_t dmach_used` 位图（15 位对应 Ch0~14），
req 置位，rls 清除，cfg/start 校验

**理由**: 通道申请/释放需要追踪占用状态以处理 `-EBUSY` 和 `-EPERM`。
位图方案内存开销小（2 字节），操作高效。
`dma_cfg` 和 `dma_start` 需校验通道已申请（在框架层 `ch_num` 范围检查之外）。

### 决策8: CB 地址存储

**选择**: CB 地址不单独存储于驱动数据中，直接写入硬件 CONBLK_AD 寄存器。
`dma_start` 通过检查 CS.ACTIVE 或内部标记判断是否已配置。

**理由**: 硬件寄存器本身就是 CB 地址的权威存储。框架的 SOC 对象中未提供
CB 地址缓存字段，无需额外存储。

## 风险 / 权衡

- **DMA4 CB 地址移位**: 写入 `addr >> 5` 意味着地址空间为 `addr[36:5]`。
若上层传入的 CB 地址低 5 位非零，需提前返回 `-EINVAL`。
BCM2711 的 DMA4 支持 40 位寻址，而普通 DMA 仅 32 位。→ 限制：DMA4 通道的 CB 仅支持
低 32 位内分配（由 ARM 运行环境决定）。

- **共享 IRQ 的边界情况**: 通道 7 和 8、通道 9 和 10 共享 IRQ 线。
若其中一个通道未申请但被意外触发，ISR 仅检查 CS.INT，未申请的通道无回调注册，
不会执行回调，但也会清除 INT 位防止中断风暴。→ 设计上安全。

- **自旋锁粒度**: 所有 DMA 操作共用一个自旋锁。若高频率 DMA 操作与低频率配置操作冲突，
可能存在锁争用。但 DMA 配置（req/rls/cfg/start/stop）本身为低频控制操作，
数据传输由硬件完成，持锁时间极短。→ 无实际影响。

- **无 DMA 传输错误处理**: 当前 ISR 仅处理完成中断（CS.INT），
若 DMA 发生 AXI 读写错误（CS.ERROR），不会触发回调。
→ 若未来需要错误通知，可在 ISR 中检查 DEBUG 寄存器的错误位并通过 rc 参数传递。
