## 上下文

当前 `dma_cfg` 的实现将 `void *cfg` 直接作为裸 CB 指针写入 CONBLK_AD 寄存器，
不配置 CS 寄存器。BCM2711 DMA CS 寄存器包含四个可配置字段——
`priority` (bits 19:16)、`panic_priority` (bits 23:20)、`disdebug` (bit 29)、
`wait_for_outstanding_writes` (bit 28)——这些字段跨三种通道类型
（普通 DMA、DMA Lite、DMA4）使用完全相同的位域名称和位置。

GPIO 驱动已有先例：`struct rpi4bxwds_gpio_cfg` 定义在 `soc.h` 中，作为
`gpio_cfg` 的配置载体。DMA 遵循同一模式。

## 目标 / 非目标

**目标：**
- 定义 `struct rpi4bxwds_dma_cfg`，union 内嵌三种 CB 类型 + 四个 CS 配置字段
- 修改 `dma_cfg` 的 BSP 实现，写入 CONBLK_AD 的同时配置 CS 寄存器
- 配置结构体 32 字节对齐（`__xwcc_aligned(32)`），`&cfg == &cfg.cb`

**非目标：**
- 不修改 `soc_dma.h` 中的寄存器或 CB 结构体定义
- 不修改框架层 `xwds_dma_cfg` API 签名
- 不改变 `dma_req/rls/enable/disable/start/stop` 的行为

## 决策

### 决策1: 配置结构体内嵌 CB（非指针）

**选择**: union 直接嵌入三种 CB 结构体，`cfg` 指针即 CB 地址（offset 0）

**理由**: CB 必须 32 字节对齐且在 DMA 传输期间保持有效。结构体内嵌保证了
CB 内存由调用方管理的结构体自然提供，无间接引用，零额外分配。
`&cfg == &cfg.cb` 使 BSP 中 CONBLK_AD 地址计算极为简单。

**替代方案**:
- `void *cb` 指针：需额外间接引用，每次 dma_cfg 时解引用取值
- union 内嵌指针：仍然需要调用方管理 CB 内存，且多了指针字段

### 决策2: union 使用三种命名成员

**选择**: union 包含 `.dma` (soc_dma_cb)、`.lite` (soc_dma_lite_cb)、
`.dma4` (soc_dma4_cb) 三个命名成员，不做统一命名

**理由**: 调用方根据目标通道类型选择对应成员，类型系统约束正确性。
BSP 取地址时用任一成员均可（所有成员同为 offset 0 的 32 字节），
统一用 `.cb.dma` 取地址。

### 决策3: CS 字段使用 xwu32_t 类型

**选择**: `cs_priority`, `cs_panic_priority`, `cs_disdebug`,
`cs_wait_for_outstanding_writes` 均为 `xwu32_t`

**理由**: 遵循 GPIO 配置结构体使用 `xwu32_t` 的惯例。
`cs_disdebug` 和 `cs_wait_for_outstanding_writes` 虽为 1 位值，
使用 `xwu32_t` 与位域赋值兼容（`xwu32_t` → 位域自动截断）。

### 决策4: dma_cfg 中通过位域写入 CS

**选择**: 在 switch/case 分发后用 `cs.b.priority = cfg->cs_priority` 等位域赋值

**理由**: 编译器生成 RMW 操作，仅修改目标位域，不影响 ACTIVE / END / INT 等
状态位。三种通道类型的 CS 寄存器位域名称完全一致，同一代码适用于所有类型。

### 决策5: 对齐使用 `__xwcc_aligned(32)`

**选择**: `struct rpi4bxwds_dma_cfg` 使用 `__xwcc_aligned(32)` 声明

**理由**: CB 硬件要求 256-bit (32 字节) 对齐。`__xwcc_aligned` 是项目标准对齐宏，
可跨编译器移植。`__attribute__((aligned(32)))` 仅在 GCC 下有效。

## 风险 / 权衡

- **API 破坏性变更**: `dma_cfg` 的 `cfg` 参数语义改变，旧调用方传裸 CB 指针会出错。
但 DMA 驱动刚实现（一个会话前完成），无存量调用方。→ 无实际影响。
- **soc.h 新增 soc_dma.h 依赖**: board `soc.h` 原本只依赖 `board/std.h`，
新增对芯片级 `soc_dma.h` 的依赖。无循环依赖问题（`soc_dma.h` 不依赖 board `soc.h`）。
→ 合理依赖。
- **结构体大小 48 字节**: 32B CB + 16B CS 字段。相比裸 CB 指针（8 字节）增加了 40 字节
栈/静态内存占用。嵌入式场景可接受。→ 无实际影响。
