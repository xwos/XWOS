# 为rpi4b增加dma驱动

**Session ID:** ses_0ce3df698ffemHu8Fs5vmvdcs0
**Created:** 7/5/2026, 6:10:27 PM
**Updated:** 7/5/2026, 11:20:04 PM

---

## User

进入探索模式。深度思考。自由可视化。跟随对话走向任何方向。

**重要：探索模式是用于思考，而非实现。** 你可以读取文件、搜索代码和调查代码库，但绝对不能编写代码或实现功能。如果用户要求你实现某些东西，提醒他们先退出探索模式并创建变更提案。你可以创建 OpenSpec 产出物（提案、设计、规范），如果用户要求的话——那是记录思考，不是实现。

**这是一种态度，而非工作流。** 没有固定步骤，没有必需的顺序，没有必须的输出。你是帮助用户探索的思考伙伴。

**输入**：`/opsx-explore` 后面的参数是用户想要思考的内容。可以是：
- 一个模糊想法："实时协作"
- 一个具体问题："认证系统变得笨重了"
- 一个变更名称："add-dark-mode"（在该变更上下文中探索）
- 一个比较："postgres vs sqlite 对这个场景"
- 什么都没有（只是进入探索模式）

---

## 态度

- **好奇，不是指令式的** - 自然提出问题，不要照本宣科
- **开放线索，不是审问** - 展示多个有趣的方向，让用户跟随引起共鸣的方向。不要将他们限制在单一的提问路径上。
- **可视化** - 在有助于澄清思考时自由使用 ASCII 图表
- **自适应** - 跟随有趣的线索，当新信息出现时灵活调整
- **耐心** - 不要急于下结论，让问题的形状自然浮现
- **扎实** - 相关时探索实际代码库，不要只是理论化

---

## 你可能会做什么

根据用户带来的内容，你可能会：

**探索问题空间**
- 从他们所说的内容中自然提出澄清问题
- 挑战假设
- 重构问题
- 找到类比

**调查代码库**
- 映射与讨论相关的现有架构
- 找到集成点
- 识别已在使用的模式
- 揭示隐藏的复杂性

**比较选项**
- 头脑风暴多种方法
- 构建比较表
- 勾画权衡
- 推荐路径（如果被要求）

**可视化**
```
┌─────────────────────────────────────────┐
│     自由使用 ASCII 图表                  │
├─────────────────────────────────────────┤
│                                         │
│      ┌────────┐         ┌────────┐      │
│      │ 状态   │────────▶│ 状态   │      │
│      │   A    │         │   B    │      │
│      └────────┘         └────────┘      │
│                                         │
│   系统图、状态机、数据流、              │
│   架构草图、依赖图、比较表             │
│                                         │
└─────────────────────────────────────────┘
```

**揭示风险和未知项**
- 识别可能出错的地方
- 发现理解中的空白
- 建议探针或调查

---

## OpenSpec 感知

你完全了解 OpenSpec 系统。自然使用它，不要强迫。

### 检查上下文

开始时，快速检查存在什么：
```bash
openspec-cn list --json
```

这告诉你：
- 是否有活跃变更
- 它们的名称、Schema 和状态
- 用户可能在做什么

如果用户提到了特定的变更名称，读取其产出物获取上下文。

### 当没有变更存在时

自由思考。当洞察凝固时，你可能提议：

- "这已经足够清晰可以开始变更了。要我创建一个提案吗？"
- 或继续探索 - 不必急于形式化

### 当变更存在时

如果用户提到一个变更或你检测到一个相关的：

1. **解析并读取现有产出物获取上下文**
   - 运行 `openspec-cn status --change "<name>" --json`。
   - 使用状态 JSON 中的 `changeRoot`、`artifactPaths` 和 `actionContext`。
   - 从 `artifactPaths.<artifact>.existingOutputPaths` 读取现有文件。

2. **在对话中自然引用它们**
   - "你的设计提到使用 Redis，但我们刚意识到 SQLite 更合适..."
   - "提案将范围限定在高级用户，但我们现在考虑所有人..."

3. **当做出决策时提议记录**

    | 洞察类型               | 记录位置                        |
    |----------------------------|--------------------------------|
    | 发现新需求           | `specs/<capability>/spec.md` |
    | 需求变更            | `specs/<capability>/spec.md` |
    | 做出设计决策          | `design.md`                  |
    | 范围变更              | `proposal.md`                |
    | 识别新工作           | `tasks.md`                   |
    | 假设失效         | 相关产出物              |

   提议示例：
   - "这是一个设计决策。记录到 design.md 中？"
   - "这是一个新需求。添加到规范中？"
   - "这改变了范围。更新提案？"

4. **用户决定** - 提议然后继续。不要施压。不要自动记录。

---

## 你不必做的事

- 遵循脚本
- 每次都问相同的问题
- 产出特定的产出物
- 得出结论
- 如果有价值的岔路就保持话题
- 简短（这是思考时间）

---

## 结束探索

没有必需的结束方式。探索可能：

- **流入提案**："准备开始了？我可以创建一个变更提案。"
- **导致产出物更新**："用这些决策更新了 design.md"
- **只是提供清晰度**：用户得到了需要的东西，继续前进
- **以后继续**："我们可以随时继续"

当事情凝固时，你可能提供一个总结 - 但这是可选的。有时候思考本身就是价值。

---

## 护栏

- **不要实现** - 永远不要编写代码或实现功能。创建 OpenSpec 产出物可以，编写应用代码不行。
- **不要假装理解** - 如果不清楚，深入挖掘
- **不要急** - 探索是思考时间，不是任务时间
- **不要强加结构** - 让模式自然浮现
- **不要自动记录** - 提议保存洞察，不要直接做
- **要可视化** - 一个好的图表胜过多段文字
- **要探索代码库** - 将讨论扎根于现实
- **要质疑假设** - 包括用户的和你自己的

阅读req.md内容

---

## Assistant (Build · DeepSeek V4 Pro · 4.7s)

_Thinking:_

The user wants me to enter exploration mode. They're asking me to read the content of req.md. Let me first check what context we have - they mentioned `openspec-cn` commands but also just want to read `req.md`. Let me look for this file and also check if there's an OpenSpec context.

Let me first read the req.md file they're asking about, and also check the current directory structure.

**Tool: glob**

**Input:**
```json
{
  "pattern": "**/req.md"
}
```

**Output:**
```
/xuanwu/xwos.tech/XWOS/req.md
/xuanwu/xwos.tech/XWOS/openspec/changes/archive/2026-07-02-add-rpi4b-eirq-driver/req.md
/xuanwu/xwos.tech/XWOS/openspec/changes/archive/2026-07-02-add-rpi4b-gpio-driver/req.md
```

**Tool: bash**

**Input:**
```json
{
  "command": "ls .opencode/ 2>/dev/null; ls .agents/ 2>/dev/null"
}
```

**Output:**
```
commands
node_modules
package.json
package-lock.json
skills
skills

```

---

## Assistant (Build · DeepSeek V4 Pro · 5.5s)

_Thinking:_

Let me read the main req.md at the root, and also check if there's an active OpenSpec change.

**Tool: read**

**Input:**
```json
{
  "filePath": "/xuanwu/xwos.tech/XWOS/req.md"
}
```

**Output:**
```
<path>/xuanwu/xwos.tech/XWOS/req.md</path>
<type>file</type>
<content>
1: ## Task
2:
3: 为工程 `xwbd/RPi4B` 增加DMA驱动：
4:
5: + 驱动路径: `xwbd/RPi4B/board/xwac/xwds/soc.c`
6:
7:
8: ## Context
9:
10: + 系统驱动框架 `xwcd/ds/soc` 提供了统一的API，新增的DMA驱动需要匹配驱动框架
11:   + `xwds_dma_req()` , `xwds_dma_rls()` , `xwds_dma_enable()` , `xwds_dma_disable()` 没有具体的寄存器对应，返回 `XWOK`
12:   + `xwds_dma_cfg()` 对应 `struct soc_dma_cb` ， `struct soc_dma_lite_cb` ， `struct soc_dma4_cb`
13:   + `xwds_dma_start()` 与 ``xwds_dma_stop()`` 对应 `cs` 寄存器的 `active` 位
14: + 芯片DMA的头文件: `xwcd/soc/arm64/v8a/a72/bcm2711/soc_dma.h` ,其中定义了GPIO寄存器 `soc_dma`
15: + 芯片Datasheet: `xwbd/RPi4B/doc/bcm2711-peripherals-manual/RP-008248-DS-1-bcm2711-peripherals.md`
16: + 电路板：树莓派4B
17: + 芯片：BCM2711
18:
19:
20: # Constraints
21:
22: + 遵循MISRA-C:2012标准
23:   + 函数至多在尾部拥有两个 `return`
24:     + 一个无错误 `return`
25:     + 一个发生错误通过 `goto` 跳转过来 `return`
26:   + 尽量不使用 `continue`

(End of file - total 26 lines)
</content>
```

**Tool: bash**

**Input:**
```json
{
  "command": "which openspec-cn 2>/dev/null && openspec-cn list --json 2>/dev/null || echo \"no-openspec\""
}
```

**Output:**
```
/starsoul/bin/openspec-cn
{"changes":[]}

```

---

## Assistant (Build · DeepSeek V4 Pro · 146.8s)

_Thinking:_

Interesting. The user has a `req.md` at the root of the project that describes a task: add a DMA driver for the RPi4B board. There are no active OpenSpec changes. Let me explore the codebase to understand the landscape.

The task is:
- Add DMA driver for `xwbd/RPi4B` board
- Driver path: `xwbd/RPi4B/board/xwac/xwds/soc.c`
- Need to match the system driver framework `xwcd/ds/soc`
- DMA registers defined in `xwcd/soc/arm64/v8a/a72/bcm2711/soc_dma.h`
- Datasheet reference in `xwbd/RPi4B/doc/bcm2711-peripherals-manual/`

Let me explore the existing code structure to understand the driver framework, existing drivers (like GPIO), and the DMA register definitions. This is exploration mode - I should investigate broadly.

好，我看到根目录下有个 `req.md`——这是为 RPi4B 添加 DMA 驱动的任务描述。目前没有活跃的 OpenSpec 变更，但有两个已归档的相关变更（GPIO 驱动和 EIRQ 驱动）。

让我先探索一下现有格局：驱动程序框架、已有驱动实现、DMA 寄存器定义。

**Tool: task**

**Input:**
```json
{
  "description": "探索 DMA 驱动生态",
  "subagent_type": "explore",
  "prompt": "请深入探索以下内容，返回详细的结构信息：\n\n1. **系统驱动框架** `xwcd/ds/soc/` 目录结构，特别关注 DMA 相关的头文件（如 `soc_dma.h` 或类似文件）。读取 `xwcd/ds/soc/` 下的目录列表，找出 DMA 相关的 API 头文件内容。\n\n2. **已完成的 RPi4B 驱动** `xwbd/RPi4B/board/xwac/xwds/soc.c` 文件内容（如果存在）。也查看 `xwbd/RPi4B/board/xwac/xwds/` 目录下的所有文件。\n\n3. **芯片 DMA 寄存器定义** `xwcd/soc/arm64/v8a/a72/bcm2711/soc_dma.h` 文件内容。\n\n4. **已归档的参考变更** - 查看 `openspec/changes/archive/2026-07-02-add-rpi4b-gpio-driver/` 目录，看有哪些产出物（设计文档等），特别是 `design.md` 或 `proposal.md`。\n\n5. **其他 RPi4B 已有外设驱动** - 查看 `xwbd/RPi4B/board/xwac/` 目录结构，了解已有哪些外设驱动实现。\n\n6. **DMA 框架 API** - 搜索 `xwds_dma` 相关函数声明和结构体 `soc_dma_cb`、`soc_dma_lite_cb`、`soc_dma4_cb` 的定义。\n\n请返回每个发现的详细内容，特别是关键代码片段。这是研究任务，不需要写代码。"
}
```

**Output:**
```
<task id="ses_0ce3d9ecbffeIuB8mdWVhfhBgw" state="completed">
<task_result>
---

以下是每个研究领域的详细调查结果。

---

## 1. 系统驱动框架 `xwcd/ds/soc/` —— 目录结构与 DMA 相关 API

### 目录结构

**路径**：`/xuanwu/xwos.tech/XWOS/xwcd/ds/soc/`

```
xwcd/ds/soc/
├── chip.c          # struct xwds_soc 的实现（构造/析构/引用计数）
├── chip.h          # 核心框架头文件：struct xwds_soc, struct xwds_soc_driver
├── clock.c         # 时钟 API 实现
├── clock.h         # 时钟 API 声明
├── dma.c           # ★ DMA API 实现
├── dma.h           # ★ DMA API 声明
├── eirq.c          # 外部中断 API 实现
├── eirq.h          # 外部中断 API 声明
├── gpio.c          # GPIO API 实现
├── gpio.h          # GPIO API 声明
├── power.c         # 电源 API 实现
└── power.h         # 电源 API 声明
```

### DMA 框架 API (`dma.h`)

**路径**：`/xuanwu/xwos.tech/XWOS/xwcd/ds/soc/dma.h`

有 6 个 DMA API 函数：

```c
/* 申请 DMA 通道 */
xwer_t xwds_dma_req(struct xwds_soc * soc, xwid_t ch);

/* 释放 DMA 通道 */
xwer_t xwds_dma_rls(struct xwds_soc * soc, xwid_t ch);

/* 配置 DMA 通道 — 设置 cfg 并注册传输完成回调 cb */
xwer_t xwds_dma_cfg(struct xwds_soc * soc, xwid_t ch, void * cfg,
                    xwds_dma_f cb, xwds_dma_cbarg_t arg);

/* 使能 DMA 通道 */
xwer_t xwds_dma_enable(struct xwds_soc * soc, xwid_t ch);

/* 禁用 DMA 通道 */
xwer_t xwds_dma_disable(struct xwds_soc * soc, xwid_t ch);

/* 启动 DMA 传输 */
xwer_t xwds_dma_start(struct xwds_soc * soc, xwid_t ch);

/* 停止 DMA 传输 */
xwer_t xwds_dma_stop(struct xwds_soc * soc, xwid_t ch);
```

### DMA BSP 驱动函数表（`chip.h` 中）

**路径**：`/xuanwu/xwos.tech/XWOS/xwcd/ds/soc/chip.h`

`struct xwds_soc_driver` 内的 DMA 驱动函数表（条件编译 `XWCDCFG_ds_SOC_DMA`）：

```c
#if defined(XWCDCFG_ds_SOC_DMA) && (1 == XWCDCFG_ds_SOC_DMA)
        xwer_t (* dma_req)(struct xwds_soc *, xwid_t ch);
        xwer_t (* dma_rls)(struct xwds_soc *, xwid_t ch);
        xwer_t (* dma_cfg)(struct xwds_soc *, xwid_t ch, void * cfg);
        xwer_t (* dma_enable)(struct xwds_soc *, xwid_t ch);
        xwer_t (* dma_disable)(struct xwds_soc *, xwid_t ch);
        xwer_t (* dma_start)(struct xwds_soc *, xwid_t ch);
        xwer_t (* dma_stop)(struct xwds_soc *, xwid_t ch);
#endif
```

### DMA 回调函数类型 (`chip.h`)

```c
typedef void * xwds_dma_cbarg_t;

typedef void (* xwds_dma_f)(struct xwds_soc * soc,
                            xwid_t ch,
                            xwu32_t rc,
                            xwds_dma_cbarg_t arg);
```

### `struct xwds_soc` 内的 DMA 描述 (`chip.h`)

```c
#if defined(XWCDCFG_ds_SOC_DMA) && (1 == XWCDCFG_ds_SOC_DMA)
        struct {
                xwsz_t ch_num;                           /* DMA 通道数 */
                void * ccfg;                             /* SoC DMA 控制器配置 */
                xwds_dma_f * chcbs;                      /* 完成回调表 */
                xwds_dma_cbarg_t * chcbargs;             /* 回调参数表 */
        } dma;
#endif
```

### DMA 框架实现 (`dma.c`)

**路径**：`/xuanwu/xwos.tech/XWOS/xwcd/ds/soc/dma.c`

`xwds_dma_cfg()` 的关键逻辑是：它将回调函数和参数存储到 `soc->dma.chcbs[ch]` 和 `soc->dma.chcbargs[ch]` 中（除非配置了 `XWCDCFG_ds_SOC_DMA_ROCBT` 只读回调表），然后调用 BSP 驱动函数 `drv->dma_cfg(soc, ch, cfg)`。`dma_req` 也通过 `xwds_soc_grab` 递增 SOC 引用计数。

---

## 2. RPi4B 已完成驱动：`xwbd/RPi4B/board/xwac/xwds/`

### 目录结构

**路径**：`/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/board/xwac/xwds/`

```
xwbd/RPi4B/board/xwac/xwds/
├── device.c           # 设备初始化/销毁；SOC/UART/miniuart 的生命周期
├── device.h           # 全局变量和 init/fini 声明
├── miniuart.c         # Mini UART 驱动（AUX）
├── soc.c              # ★ SOC 驱动 — 当前仅实现了 GPIO + EIRQ
├── soc.h              # GPIO 配置结构体
└── uart.c             # UART5 驱动（PL011）
```

### `soc.c` — 当前 SOC 驱动实现

**路径**：`/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/board/xwac/xwds/soc.c`

关键观察：**当前的 SOC 驱动已为 GPIO 和 EIRQ 作了完全实现，但完全没有 DMA**。驱动函数表 `rpi4bxwds_soc_drv` 仅填充了以下函数：

```c
const struct xwds_soc_driver rpi4bxwds_soc_drv = {
        .base = { .name = "rpi4bxwds.soc", .probe/remove/start/stop 等 },
        .gpio_req = rpi4bxwds_soc_drv_gpio_req,
        .gpio_rls = rpi4bxwds_soc_drv_gpio_rls,
        .gpio_cfg = rpi4bxwds_soc_drv_gpio_cfg,
        .gpio_set / gpio_reset / gpio_toggle / gpio_output / gpio_input,
        .gpio_read_output = NULL,
        .eirq_req = rpi4bxwds_soc_drv_eirq_req,
        .eirq_rls = rpi4bxwds_soc_drv_eirq_rls,
        // ⚠️ 没有 DMA / CLK / PWR 回调
};
```

SOC 对象 `rpi4bxwds_soc` 为 GPIO 和 EIRQ 初始化了 .gpio 和 .eirq 字段，但 **缺少 `.dma` 子结构体**。

### `soc.h` — GPIO 配置结构体

**路径**：`/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/board/xwac/xwds/soc.h`

```c
struct rpi4bxwds_gpio_cfg {
        xwu32_t function; /* SOC_GPIO_ALT_GPI / GPO / ALT0~5 */
        xwu32_t pud;      /* SOC_GPIO_PUPDC_NO / PU / PD */
};
```

### `device.h` — 全局声明

**路径**：`/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/board/xwac/xwds/device.h`

声明了以下全局变量和函数：
- `rpi4bxwds`（xwds 实例）
- `rpi4bxwds_soc`（SOC 设备）、`rpi4bxwds_soc_init/fini`
- `rpi4bxwds_miniuart`（Mini UART）、`rpi4bxwds_miniuart_init/fini`
- `rpi4bxwds_uart5`（UART5）、`rpi4bxwds_uart_init/fini`

---

## 3. 芯片 DMA 寄存器定义

**路径**：`/xuanwu/xwos.tech/XWOS/xwcd/soc/arm64/v8a/a72/bcm2711/soc_dma.h`

### DMA 基地址（来自 `soc.h`）

```c
#define SOC_DMA_REGBASE        (SOC_MAIN_PERI_BASE + 0x02007000UL)
#define SOC_DMA_GLOBAL_REGBASE  (SOC_DMA_REGBASE + 0xF00UL)
#define SOC_DMA0_REGBASE        (SOC_DMA_REGBASE + 0x000UL)   /* Ch 0  */
#define SOC_DMA1_REGBASE        (SOC_DMA_REGBASE + 0x100UL)   /* Ch 1  */
/* ... 每个通道间隔 0x100 直至 Ch 14 ... */
#define SOC_DMA15_REGBASE       (SOC_MAIN_PERI_BASE + 0x02E05000UL) /* Ch 15 */
```

### DMA 中断号（来自 `soc.h`）

```c
SOC_VC_IRQ_DMA0,   SOC_VC_IRQ_DMA1,   SOC_VC_IRQ_DMA2,
SOC_VC_IRQ_DMA3,   SOC_VC_IRQ_DMA4,   SOC_VC_IRQ_DMA5,
SOC_VC_IRQ_DMA6,   SOC_VC_IRQ_DMA7_8, SOC_VC_IRQ_DMA9_10,
SOC_VC_IRQ_DMA11,  SOC_VC_IRQ_DMA12,  SOC_VC_IRQ_DMA13,
SOC_VC_IRQ_DMA14,  SOC_VC_IRQ_DMA15
```

### 三种 DMA 通道类型——结构体与寄存器

BCM2711 拥有 **3 种不同的 DMA 通道类型**，各有独立的硬件寄存器和控制块(CB)结构：

| 种类 | 通道 | 寄存器结构体 | 控制块(CB)结构体 | 寄存器宏 |
|------|------|-------------|---------------------|-----------|
| **普通 DMA** | 0~6, 15 | `struct soc_dma_regs` | `struct soc_dma_cb` | `soc_dma0` ~ `soc_dma6`，`soc_dma15` |
| **DMA Lite** | 7~10 | `struct soc_dma_lite_regs` | `struct soc_dma_lite_cb` | `soc_dma7` ~ `soc_dma10` |
| **DMA4** | 11~14 | `struct soc_dma4_regs` | `struct soc_dma4_cb` | `soc_dma11` ~ `soc_dma14` |

### 结构体详细字段

#### `struct soc_dma_cb`（普通 DMA 控制块——内存中的描述符）

```c
struct soc_dma_cb {
    struct {                /* +0x00 TI: Transfer Information */
        xwu32_t inten:1;          /* 中断使能 */
        xwu32_t tdmode:1;         /* 2D 模式 */
        xwu32_t wait_resp:1;      /* 等待响应 */
        xwu32_t dest_inc:1;       /* 目标地址自增 */
        xwu32_t dest_width:1;     /* 目标宽度 (0=32bit, 1=128bit) */
        xwu32_t dest_dreq:1;      /* 目标 DREQ */
        xwu32_t src_inc:1;        /* 源地址自增 */
        xwu32_t src_width:1;      /* 源宽度 (0=32bit, 1=128bit) */
        xwu32_t src_dreq:1;       /* 源 DREQ */
        xwu32_t src_ignore:1;     /* 忽略源读取 */
        xwu32_t burst_length:4;   /* 突发长度 */
        xwu32_t permap:5;         /* 外设映射 */
        xwu32_t waits:5;          /* 等待周期 */
        xwu32_t no_wide_bursts:1; /* 禁止宽突发 */
    } b;
    xwu32_t src;             /* +0x04 源地址 */
    xwu32_t dst;             /* +0x08 目标地址 */
    struct {                 /* +0x0C 传输长度 */
        xwu32_t xlength:16;  /*  X 方向长度 (最多 65535) */
        xwu32_t ylength:14;  /*  Y 方向长度 (2D) */
    } b;
    struct {                 /* +0x10 2D 步长 */
        xwu32_t s_stride:16; /* 源步长 */
        xwu32_t d_stride:16; /* 目标步长 */
    } b;
    xwu32_t next;            /* +0x14 下一个 CB 地址 */
    xwu32_t reserved0[2];    /* +0x18 ~ +0x1C */
};
```

#### `struct soc_dma_regs`（普通 DMA 通道寄存器）

关键寄存器：
- `cs`（+0x00 控制和状态）：`active, end, intr, dreq, error, priority, panic_priority, abort, reset`
- `cb`（+0x04 控制块地址）
- `ti`（+0x08 传输信息——启动时从此处复制）
- `src`，`dst`（+0x0C，+0x10 地址）
- `size`，`stride`（+0x14，+0x18）
- `next`（+0x1C）
- `debug`（+0x20）：`read_last_not_set_error, fifo_error, read_error, dma_id, dma_state, version, lite`

#### `struct soc_dma_lite_cb` / `struct soc_dma_lite_regs`（DMA Lite 通道）

相较于普通 DMA：
- **无 `tdmode`** —— 不支持 2D 传输
- **无 `stride`** —— `+0x10` 保留
- **无 `src_ignore`**
- `next` 偏移不同（CB: +0x14，寄存器: +0x1C）

#### `struct soc_dma4_cb` / `struct soc_dma4_regs`（DMA4 通道——40 位寻址）

相较于普通 DMA：
- `ti` 仅有 `inten, tdmode, wait_resp, permap, s_dreq, d_dreq, s_waits(8), d_waits(8)`
- 源和目标分别具有独立的 `addr_hi(8), burst_length(4), inc, size(2), ignore, stride(16)`
- 通过两个 32 位寄存器支持最高 40 位内存寻址
- 寄存器中有一个 `halt` 控制位（替代 `reset`）
- `debug` 字段特殊（无 DMA ID，但包含 `r_state, w_state, id(4)`）
- `debug2`：独立跟踪读写未完成计数

### 全局寄存器

```c
struct soc_dma_global_regs {
    /* ... reserved ... */
    xwu32_t int_status;   /* +0xFE0 中断状态 (每通道 1 位) */
    xwu32_t enable;       /* +0xFF0 全局使能 (en0~14 + page(4) + pagelite(4)) */
};
```

---

## 4. 已归档参考变更：`2026-07-02-add-rpi4b-gpio-driver`

**路径**：`/xuanwu/xwos.tech/XWOS/openspec/changes/archive/2026-07-02-add-rpi4b-gpio-driver/`

```
openspec/changes/archive/2026-07-02-add-rpi4b-gpio-driver/
├── .openspec.yaml        # schema: spec-driven, 创建日期 2026-07-02
├── design.md             # ★ 设计决策文档
├── req.md             # 原始任务提示
├── proposal.md           # ★ 变更提案
├── specs/
│   └── gpio-driver/
│       └── spec.md       # ★ 功能规格说明（含场景）
└── tasks.md              # ★ 任务分解与完成追踪
```

### `proposal.md` 摘要
- **变更内容**：创建 `board/xwac/xwds/soc.c`，实现 BCM2711 的 `struct xwds_soc_driver` 驱动函数表
- **影响文件**：新建 `soc.c`；修改 `device.h`、`board/init.c`
- **明确排除**：DMA、CLK、PWR 功能 **不在本次变更范围内**（“不实现 CLK、PWR、DMA 功能”）

### `design.md` 的关键决策
1. **寄存器访问**：对 `gpfsel0~5` 和 `pup_pdn_cntrl_reg0~3` 使用 switch/case 分发方式，按 `pin` 索引
2. **set/reset/lev 寄存器**：`gpset/clr/lev0/1` 使用直接掩码写入
3. **gpio_output**：读取 `gplev` 获取当前物理电平，计算差分后分别写入 `gpset`/`gpclr`
4. **配置结构体**：`struct rpi4bxwds_gpio_cfg { xwu32_t function; xwu32_t pud; }`
5. **自旋锁**：`struct rpi4bxwds_soc_driver_data { struct xwos_splk splk; }` 挂载于 `.dev.data`
6. **EIRQ**：最初设为空壳（`num=0`），后来在实现中补全为完整 EIRQ 驱动

### `spec.md` 摘要 —— 需求场景
- GPIO 申请/释放（单个/多个引脚、`-EBUSY`、`-EPERM`、超出范围 `-ERANGE`）
- 功能与上下拉配置（GPI/GPO/ALT、PU/PD、cfg 为 NULL 时 `-EFAULT`）
- 电平操作（set/reset/toggle/output/input）
- SMP 并发安全（自旋锁保护）
- MISRA-C:2012 合规（每个函数最多两个 return）

### `tasks.md` 摘要
实现被分解为 6 个阶段、25 个子任务：
1. 创建 `soc.c` 并定义结构体/全局变量
2. 寄存器辅助函数（`gpfsel_set`、`pupd_set`）
3. GPIO 操作函数
4. 设备生命周期函数（probe / remove / start / stop / init / fini）
5. 板级集成到 `device.h` 和 `init.c`
6. 编译验证与 MISRA-C 检查

---

## 5. 其他 RPi4B 已有外设驱动 —— `xwbd/RPi4B/board/xwac/` 目录

**路径**：`/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/board/xwac/`

```
xwbd/RPi4B/board/xwac/
├── libc/
│   ├── init.c           # libc 初始化
│   ├── mi.h             # 机器接口头
│   ├── newlib/          # newlib 移植
│   └── picolibc/        # picolibc 移植
├── xwds/                # ★ XWOS 设备栈驱动（上述详细说明）
│   ├── device.c/h       # 设备生命周期管理
│   ├── miniuart.c       # Mini UART (AUX) 驱动
│   ├── soc.c/h          # SOC 驱动 (GPIO + EIRQ — 无 DMA)
│   └── uart.c           # PL011 UART 驱动 (UART5)
└── xwos/                # XWOS 内核适配
    ├── log.c            # 日志实现
    ├── thd_stack_mempool.c  # 线程栈内存池
    └── xwskd_hook.c     # 调度器钩子
```

### 当前已实现的外设驱动汇总
| 驱动 | 文件 | 类型 | 状态 |
|------|------|------|------|
| GPIO | `soc.c` → `gpio_req/rls/cfg/set/reset/toggle/output/input` | SOC 级 | **已完成** |
| EIRQ（外部中断） | `soc.c` → `eirq_req/rls` + 3 个 bank ISR | SOC 级 | **已完成** |
| Mini UART | `miniuart.c` → `rpi4bxwds_miniuart` | UART 控制器 | **已完成** |
| UART5 (PL011) | `uart.c` → `rpi4bxwds_uart5` | UART 控制器 | **已完成** |
| DMA | — | SOC 级 | ❌ **未实现** |
| CLK | — | SOC 级 | ❌ **未实现** |
| PWR | — | SOC 级 | ❌ **未实现** |

### BCM2711 SoC 层——芯片级寄存器定义（`xwcd/soc/arm64/v8a/a72/bcm2711/`）

该目录包含 24 个文件，涵盖：
- `soc.h`：所有基地址和 IRQ 号
- `soc_dma.h`：**已有 DMA 寄存器/控制块结构（仅缺 BSP 驱动实现）**
- `soc_gpio.h`：GPIO 寄存器（GPIO 驱动已在使用）
- `soc_uart.h`：UART 寄存器（UART 驱动已在使用）
- `soc_aux.h`：AUX 寄存器（Mini UART 已在使用）
- `soc_mmu.h`、`soc_mp.h`：MMU / 多处理器
- `xwosimpl_irq.c/h`、`xwosimpl_skd.c/h`、`xwosimpl_syshwt.c/h`：XWOS 内核适配层

---

## 6. DMA 框架 API —— `xwds_dma_*` 和 DMA 结构体的完整定义

### API 函数（来自 `xwcd/ds/soc/dma.h`）

| 函数 | 用途 | 典型错误码 |
|------|------|------------|
| `xwds_dma_req(soc, ch)` | 申请 DMA 通道；递增 SOC 引用计数 | `-EFAULT`, `-ERANGE`, `-EBUSY` |
| `xwds_dma_rls(soc, ch)` | 释放 DMA 通道；递减 SOC 引用计数 | `-EFAULT`, `-ERANGE`, `-EPERM` |
| `xwds_dma_cfg(soc, ch, cfg, cb, arg)` | 配置 DMA 通道 + 注册完成回调 | `-EFAULT`, `-ERANGE`, `-EPERM` |
| `xwds_dma_enable(soc, ch)` | 使能 DMA 通道 | `-EFAULT`, `-ERANGE`, `-EPERM` |
| `xwds_dma_disable(soc, ch)` | 禁用 DMA 通道 | `-EFAULT`, `-ERANGE`, `-EPERM` |
| `xwds_dma_start(soc, ch)` | 启动 DMA 传输 | `-EFAULT`, `-ERANGE`, `-EPERM` |
| `xwds_dma_stop(soc, ch)` | 停止 DMA 传输 | `-EFAULT`, `-ERANGE`, `-EPERM` |

### 通道 ID 宏

```c
#define XWDS_DMA_CH(x)  ((xwid_t)(x))
```

### DMA 传输完成回调签名

```c
typedef void * xwds_dma_cbarg_t;

typedef void (* xwds_dma_f)(struct xwds_soc * soc,
                            xwid_t ch,         /* 生成完成的通道 */
                            xwu32_t rc,       /* 结果码 */
                            xwds_dma_cbarg_t arg);
```

### BSP 驱动函数表（`struct xwds_soc_driver` 需填充）

BSP 层必须实现 `dma_req`、`dma_rls`、`dma_cfg`、`dma_enable`、`dma_disable`、`dma_start`、`dma_stop`。其中 `dma_cfg` 接收裸的 `void * cfg`——BSP 将其转换为特定的 DMA 配置结构体（例如普通 DMA 使用 `struct soc_dma_cb`，DMA Lite 使用 `struct soc_dma_lite_cb`，DMA4 使用 `struct soc_dma4_cb`）。

### SOC 对象内部存储（`struct xwds_soc.dma`）

```c
struct {
    xwsz_t ch_num;                 /* 通道数量（例如 BCM2711 为 16） */
    void * ccfg;                   /* 控制器私有配置 */
    xwds_dma_f * chcbs;           /* 完成回调表（按通道索引） */
    xwds_dma_cbarg_t * chcbargs;  /* 回调参数表（按通道索引） */
} dma;
```

### CMA 框架实现中 `xwds_dma_cfg()` 的核心逻辑

```c
// 将回调存储到框架的 per-channel 表中
if (soc->dma.chcbs)   soc->dma.chcbs[ch] = cb;
if (soc->dma.chcbargs) soc->dma.chcbargs[ch] = arg;
// 委托给 BSP
rc = drv->dma_cfg(soc, ch, cfg);
```

### DMA 控制块结构与硬件寄存器映射

| 结构体 | 适用通道 | 关键差异 |
|--------|---------|---------|
| `struct soc_dma_cb` | 0~6, 15 | 全部 TI 功能；2D 步长；32 位寻址 |
| `struct soc_dma_lite_cb` | 7~10 | 无 TDMODE；无 STRIDE；无 SRC_IGNORE；32 位寻址 |
| `struct soc_dma4_cb` | 11~14 | 独立于源/目标的突发长度/增量/宽度/忽略/步长；40 位寻址（地址高 8 位分离） |

**值得注意的是**：DMA Lite 与普通 DMA 在 CB 上相似但不完全相同：
- `ti` 位域布局几乎相同，但 DMA Lite **没有 `tdmode` 和 `src_ignore`** 位
- DMA Lite **没有 `stride`**（CB 偏移 +0x10 保留）
- DMA Lite 的 `size` 结构体仅有 `xlength`（无 `ylength`），意味着 **仅支持 1D 传输**

DMA4 的 `ti` 完全不同：采用 `s_dreq + d_dreq` 替代 `src_dreq + dest_dreq + src_ignore`，且采用 `s_waits(8) + d_waits(8)` 替代单一 `waits(5)`。每个方向均有独立的 `burst_length`、`inc`、`size` 和 `ignore` 字段。

---

### 总结与行动要点

若要进行 DMA 驱动的实现工作：
1. **SoC 层已完成**：`soc_dma.h` 完整定义了所有寄存器结构体、控制块和通道宏。无需寄存器定义工作。
2. **框架 API 已完成**：`xwcd/ds/soc/dma.{h,c}` 提供完整的请求/释放/配置/启动/停止 API。BSP 仅需实现 `struct xwds_soc_driver` 中的 7 个 DMA 函数指针。
3. **SOC 对象当前缺少 `.dma` 子结构体**：`rpi4bxwds_soc` 必须添加上方所示的 `.dma` 字段，设置 `ch_num=16`，提供回调表，以及指向任何控制器私有配置的 `ccfg`。
4. **已有 GPIO 驱动变更可作为模板**：归档的 `2026-07-02-add-rpi4b-gpio-driver` 目录包含一个完整的设计文档、提案、任务分解和规格说明，可作为 DMA 实现的参考。
</task_result>
</task>
```

---

## Assistant (Build · DeepSeek V4 Pro · 37.3s)

_Thinking:_

Excellent! Now I have a comprehensive understanding of the codebase. Let me also look at the datasheet reference and the EIRQ driver archive for more context. Let me also understand the interrupt handling pattern since DMA needs interrupt handling.

Let me now present a thorough exploration summary to the user. The exploration mode is about thinking and visualizing, so let me organize the findings in a visual way.

Key findings:
1. DMA register definitions already exist in `soc_dma.h`
2. Framework API is complete in `xwcd/ds/soc/dma.{h,c}`
3. `soc.c` currently only has GPIO + EIRQ - needs DMA functions added
4. BCM2711 has 3 types of DMA channels (normal, lite, DMA4) across 16 channels
5. The GPIO driver archive provides a template pattern for implementation
6. req.md states: `dma_req/rls/enable/disable` should just return `XWOK` (no register ops)
7. hint.md: `dma_cfg` maps to the 3 CB struct types, `dma_start/stop` maps to `cs.active` bit

Let me present this exploration visually, raising interesting questions and challenges.

探索完成。让我把发现整理成一个清晰的图景。

---

## 现有格局

```
xwcd/ds/soc/                        xwcd/soc/arm64/v8a/a72/bcm2711/
┌──────────────────────┐            ┌──────────────────────────┐
│  dma.h / dma.c       │ ◀──调用── │  soc_dma.h               │
│  (框架层，已完成)     │            │  (寄存器定义，已完成)     │
│                      │            │                          │
│  xwds_dma_req()      │            │  struct soc_dma_cb       │
│  xwds_dma_rls()      │            │  struct soc_dma_lite_cb  │
│  xwds_dma_cfg()      │            │  struct soc_dma4_cb      │
│  xwds_dma_enable()   │            │  struct soc_dma_regs     │
│  xwds_dma_disable()  │            │  struct soc_dma_lite_regs│
│  xwds_dma_start()    │            │  struct soc_dma4_regs    │
│  xwds_dma_stop()     │            │  soc_dma0 .. soc_dma15   │
└──────┬───────────────┘            └──────────────────────────┘
       │ 需要 BSP 实现
       ▼
xwbd/RPi4B/board/xwac/xwds/soc.c
┌──────────────────────────────────┐
│  rpi4bxwds_soc_drv {             │
│    .gpio_req  = ✓                │
│    .gpio_rls  = ✓                │
│    .eirq_req  = ✓                │
│    .eirq_rls  = ✓                │
│    .dma_req   = ✗ ← 待实现       │
│    .dma_rls   = ✗                │
│    .dma_cfg   = ✗                │
│    ...                           │
│  }                               │
│                                  │
│  rpi4bxwds_soc {                 │
│    .gpio = { ... }               │
│    .eirq = { ... }               │
│    .dma  = { ✗ 缺失 }            │
│  }                               │
└──────────────────────────────────┘
```

---

## BCM2711 DMA 通道全景

BCM2711 有 **16 个 DMA 通道**，分属三种硬件类型：

```
Ch 0 ─┐
Ch 1  │
Ch 2  ├── 普通 DMA ── struct soc_dma_cb     (32bit 寻址, 2D, 全功能)
 ...   │                 struct soc_dma_regs
Ch 6  │
Ch 15 ─┘

Ch 7  ─┐
Ch 8   ├── DMA Lite  ── struct soc_dma_lite_cb  (32bit, 仅1D, 精简)
Ch 9   │                 struct soc_dma_lite_regs
Ch 10 ─┘

Ch 11 ─┐
Ch 12  ├── DMA4      ── struct soc_dma4_cb      (40bit 寻址, 独立方向控制)
Ch 13  │                 struct soc_dma4_regs
Ch 14 ─┘
```

---

## 设计上的棘手问题

我在探索中注意到几个值得深思的点：

### 1. 三种 CB 结构体，一个 `void *cfg` 参数

框架的 `dma_cfg(soc, ch, cfg)` 用裸 `void *` 传递配置。BSP 层需要根据 `ch` 判断是哪种通道类型，再分别 cast 到 `soc_dma_cb*` / `soc_dma_lite_cb*` / `soc_dma4_cb*`。三种结构体字段差异很大：

| 字段 | 普通 DMA | DMA Lite | DMA4 |
|------|:---:|:---:|:---:|
| 2D 模式 (tdmode) | ✓ | ✗ | ✓ |
| 步长 (stride) | ✓ | ✗ | ✓ (独立源/目标) |
| 突发长度 | 统一 4bit | 统一 4bit | 独立 4bit |
| DREQ 控制 | src_dreq + dest_dreq | 同左 | s_dreq + d_dreq |
| 等待周期 | 5bit 统一 | 5bit 统一 | 独立 8bit |
| 源忽略 | ✓ | ✗ | ✓ (独立) |
| 高位地址 | ✗ | ✗ | 独立 8bit |

**问题**：`dma_cfg` 是否应该对通道类型做参数校验？比如 DMA Lite 通道收到含 `tdmode=1` 的 CB 是否应该拒绝？

### 2. 中断处理

DMA 完成会产生中断。不同通道共享中断线：
- DMA0~6、15 各有独立 IRQ
- DMA7/8 共享 `SOC_VC_IRQ_DMA7_8`
- DMA9/10 共享 `SOC_VC_IRQ_DMA9_10`

**问题**：共享 IRQ 的通道（7/8 和 9/10）在 ISR 中如何区分是哪个通道完成的？需要读取每个通道的 `cs` 寄存器检查 `intr` 位？还是有全局的 `int_status` 寄存器可以一次读取？

从寄存器定义来看，全局寄存器中有 `int_status`（每通道 1 位），这可以一次性判断中断源。但共享 IRQ 仍需要 ISR 内部遍历相关通道。

### 3. 控制块（CB）的内存管理

`dma_cfg` 接收一个指向 CB 的指针。这个 CB 存在哪里？谁负责它的生命周期？

- 是调用者在 DMA 传输期间保持其有效？
- DMA 引擎会通过 `next` 字段链式读取 CB，所以 CB 必须在 DMA 活跃期间保持有效
- 如果 `dma_stop` 被调用，CB 可能处于半完成状态

**问题**：框架/驱动层是否需要管理 CB 的生命周期，还是完全交给调用者？GPIO 驱动的 EIRQ 部分有类似的中断回调管理模式可以参考吗？

### 4. req.md 的约束解读

> `xwds_dma_req()`, `xwds_dma_rls()`, `xwds_dma_enable()`, `xwds_dma_disable()` 没有具体的寄存器对应，返回 `XWOK`

这意味着 req/rls 纯粹做通道占用标记（通过 `soc->dma.chcbs` 表），enable/disable 可能只是标记使能状态。但 `dma_enable` 是否需要写全局的 `enable` 寄存器？

> `xwds_dma_start()` 与 `xwds_dma_stop()` 对应 `cs` 寄存器的 `active` 位

`cs.active` 是只读状态位还是可写的控制位？从寄存器定义看，`active` 是状态位。启动 DMA 应该是将 CB 地址写入 `cb` 寄存器然后设置 `cs` 的某些控制位。需要查 datasheet 确认。

---

## 值得进一步探索的方向

1. **查阅 datasheet** `xwbd/RPi4B/doc/bcm2711-peripherals-manual/` 确认 DMA 启动/停止的精确寄存器操作序列
2. **EIRQ 中断处理模式**作为参考——已经有共享 IRQ 的处理模板（3 个 bank 共享中断线）
3. **DMA4 的 40 位寻址**——RPi4B 上实际需要用到吗？还是可以简化处理？

你想往哪个方向深入？

---

## User

1

---

## Assistant (Build · DeepSeek V4 Pro · 3.3s)

_Thinking:_

The user just said "1" - they want to explore direction 1:查阅 datasheet 确认 DMA 启动/停止的精确寄存器操作序列.

Let me find and read the BCM2711 datasheet.

好，来深挖 datasheet。先找到它。

**Tool: glob**

**Input:**
```json
{
  "pattern": "xwbd/RPi4B/doc/bcm2711-peripherals-manual/**/*.md"
}
```

**Output:**
```
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/RP-008248-DS-1-bcm2711-peripherals.md
```

**Tool: glob**

**Input:**
```json
{
  "pattern": "xwbd/RPi4B/doc/bcm2711-peripherals-manual/**/*"
}
```

**Output:**
```
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/RP-008248-DS-1-bcm2711-peripherals.md
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-31.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-30.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-29.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-28.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-27.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-26.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-25.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-24.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-23.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-22.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-21.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-20.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-19.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-18.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-17.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-16.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-15.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-14.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-13.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-12.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-11.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-10.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-09.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-08.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-07.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-06.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-05.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-04.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-03.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-02.jpg
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/images/image-01.jpg
```

---

## Assistant (Build · DeepSeek V4 Pro · 4.0s)

_Thinking:_

Let me search for DMA-related content in the datasheet.

**Tool: grep**

**Input:**
```json
{
  "pattern": "^#.*DMA|^##.*DMA|DMA.*Channel|DMA.*Control|DMA.*Transfer|CS Register|Control Block|0x[0-9a-fA-F]+.*DMA|DMA.*start|DMA.*stop|DMA.*active|DMA.*enable|DMA.*abort|DMA.*reset",
  "path": "/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual",
  "include": "*.md"
}
```

**Output:**
```
Found 93 matches
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/doc/bcm2711-peripherals-manual/RP-008248-DS-1-bcm2711-peripherals.md:
  Line 13: <table><tr><td>Release</td><td>Date</td><td>Description</td></tr><tr><td>1</td><td>05 Feb 2020</td><td>First release.</td></tr><tr><td>2</td><td>24 Sep 2020</td><td>Corrected GPIO base address. Updated styling.</td></tr><tr><td>3</td><td>16 Oct 2020</td><td>First public release.</td></tr><tr><td>4</td><td>18 Jan 2022</td><td>Added information about switching DMA DREQ channels.Updated GPIO_PUP_PDN_CNTRL register reset values.Updated UART GPIO mapping table.</td></tr></table>

  Line 62: 4. DMA Controller 31

  Line 64: 4.2. DMA Controller Registers 31

  Line 65: 4.2.1. DMA Channel Register Address Map 32

  Line 66: 4.2.1.1. Control Block Data Structure 32

  Line 230: Software accessing RAM using the DMA engines must use legacy addresses (between 0xC000_0000 and 0xFFFF_FFFF) This accesses a 1GB window within the full 16GB SDRAM address space. If the DMA engine needs to access RAM abov the first 1GB, this window can be moved using the PAGE or PAGELITE bits - see Chapter 4 for more details. Behind the scenes, the VideoCore transparently translates these addresses to the 35-bit 0x0_nnnn_nnnn addresses.

  Line 232: Software accessing the VPU L2 cache using the DMA engines must use legacy addresses starting at 0x0000_0000 (for allocating cache) or 0x8000 0000 (for non-allocating cache) Behind the scenes, the VideoCore transparently translates these addresses to the corresponding 35-bit 0x4_nnnn_nnnn addresses. These 1GB windows can’t be moved, and are limited to the first 1GB of SDRAM.

  Line 916: # Chapter 4. DMA Controller

  Line 924: The BCM2711 DMA Controller provides a total of 16 DMA channels. Four of these are DMA Lite channels (with reduced performance and features), and four of them are DMA4 channels (with increased performance and a wider address range). Each channel operates independently from the others and is internally arbitrated onto one of the three system busses. This means that the amount of bandwidth that a DMA channel may consume can be controlled by the arbiter settings (although these are not publicly exposed).

  Line 926: Each DMA channel operates by loading a Control Block (CB) data structure from memory into internal registers. The Control Block defines the required DMA operation. Each Control Block can point to a further Control Block to be loaded and executed once the operation described in the current Control Block has completed. In this way a linked list of Contro Blocks can be constructed in order to execute a sequence of DMA operations without software intervention.

  Line 928: The DMA supports AXI read bursts to ensure efficient external SDRAM use. The DMA Control Block contains a burst parameter which indicates the required burst size of certain memory transfers. In general the DMA doesn’t do write bursts, although wide writes will be done in 2 beat bursts if possible.

  Line 940: ## 4.2. DMA Controller Registers

  Line 942: The DMA Controller is comprised of several identical DMA Channels depending upon the required configuration. Each individual DMA channel has an identical register map (although LITE channels have fewer registers and DMA4 channels have more registers).

  Line 944: DMA Channel 0 is located at the address of 0x7e007000, Channel 1 at 0x7e007100, Channel 2 at 0x7e007200 and so on. Thus adjacent DMA Channels are offset by 0x100.

  Line 946: DMA Channel 15 however, is physically removed from the other DMA Channels and so has a different address base of 0x7ee05000. DMA Channel 15 is exclusively used by the VPU.

  Line 948: <table><tr><td>Base Address</td><td>DMA Channel</td></tr><tr><td>DMA0_BASE + 0x000</td><td>DMA Channel 0 Register Set</td></tr><tr><td>DMA0_BASE + 0x100</td><td>DMA Channel 1 Register Set</td></tr><tr><td>DMA0_BASE + 0x200</td><td>DMA Channel 2 Register Set</td></tr><tr><td>DMA0_BASE + 0x300</td><td>DMA Channel 3 Register Set</td></tr><tr><td>DMA0_BASE + 0x400</td><td>DMA Channel 4 Register Set</td></tr><tr><td>DMA0_BASE + 0x500</td><td>DMA Channel 5 Register Set</td></tr><tr><td>DMA0_BASE + 0x600</td><td>DMA Channel 6 Register Set</td></tr><tr><td>DMA0_BASE + 0x700</td><td>DMA Channel 7 Register Set</td></tr><tr><td>DMA0_BASE + 0x800</td><td>DMA Channel 8 Register Set</td></tr><tr><td>DMA0_BASE + 0x900</td><td>DMA Channel 9 Register Set</td></tr><tr><td>DMA0_BASE + 0xa00</td><td>DMA Channel 10 Register Set</td></tr><tr><td>DMA0_BASE + 0xb00</td><td>DMA Channel 11 Register Set</td></tr><tr><td>DMA0_BASE + 0xc00</td><td>DMA Channel 12 Register Set</td></tr><tr><td>DMA0_BASE + 0xd00</td><td>DMA Channel 13 Register Set</td></tr><tr><td>DMA0_BASE + 0xe00</td><td>DMA Channel 14 Register Set</td></tr><tr><td>DMA15_BASE + 0x000</td><td>DMA Channel 15 Register Set</td></tr></table>

  Line 950: ## 4.2.1. DMA Channel Register Address Map

  Line 956: Only three registers in each channel’s register set are directly writeable (CS, CONBLK_AD and DEBUG). The other registers (TI, SOURCE_AD, DEST_AD, TXFR_LEN, STRIDE & NEXTCONBK) are automatically loaded from a Control Block data structure held in external memory.

  Line 958: ## 4.2.1.1. Control Block Data Structure

  Line 960: Control Blocks (CB) are 8 words (256 bits) in length and must start at a 256-bit aligned address. The format of the different CB data structures in memory, are shown below.

  Line 962: Each 32-bit word of the Control Block is automatically loaded into the corresponding 32-bit DMA Control Block register at the start of a DMA transfer. The descriptions of these registers also define the corresponding bit locations in the CB data structure in memory.

  Line 964: <table><tr><td>32-bit Word Offset</td><td>Description</td><td>Associated Read-Only Register</td></tr><tr><td>0</td><td>Transfer Information</td><td>TI</td></tr><tr><td>1</td><td>Source Address</td><td>SOURCE_AD</td></tr><tr><td>2</td><td>Destination Address</td><td>DEST_AD</td></tr><tr><td>3</td><td>Transfer Length</td><td>TXFR_LEN</td></tr><tr><td>4</td><td>2D Mode Stride</td><td>STRIDE</td></tr><tr><td>5</td><td>Next Control Block Address</td><td>NEXTCONBK</td></tr><tr><td>6-7</td><td>Reserved – set to zero.</td><td>N/A</td></tr><tr><td>4</td><td>Reserved – set to zero.</td><td>N/A</td></tr><tr><td>5</td><td>Next Control Block Address</td><td>NEXTCONBK</td></tr><tr><td>6-7</td><td>Reserved – set to zero.</td><td>N/A</td></tr></table>

  Line 966: <table><tr><td>32-bit Word Offset</td><td>Description</td><td>Associated Read-Only Register</td></tr><tr><td>0</td><td>Transfer Information</td><td>TI</td></tr><tr><td>1</td><td>Source Address</td><td>SRC</td></tr><tr><td>2</td><td>Source Information</td><td>SRCI</td></tr><tr><td>3</td><td>Destination Address</td><td>DEST</td></tr><tr><td>4</td><td>Destination Information</td><td>DESTI</td></tr><tr><td>5</td><td>Transfer Length</td><td>LEN</td></tr><tr><td>6</td><td>Next Control Block Address</td><td>NEXT_CB</td></tr><tr><td>7</td><td>Reserved – set to zero.</td><td>N/A</td></tr></table>

  Line 968: The DMA is started by writing the address of a CB structure into the CONBLK_AD register (or the CB register in the DMA4 channels) and then setting the ACTIVE bit. The DMA will fetch the CB from the address set in the SCB_ADDR field of the CONBLK_AD register (or the ADDR field of the CB register in the DMA4 channels) and it will load it into the read-only registers described below. It will then begin a DMA transfer according to the information in the CB.

  Line 970: When it has completed the current DMA transfer (length => 0) the DMA will update the CONBLK_AD register with the contents of the NEXTCONBK register (or the NEXT_CB register in the DMA4 channels), fetch a new CB from that address, and start the whole procedure once again.

  Line 972: The DMA will stop (and clear the ACTIVE bit) when it has completed a DMA transfer and the NEXTCONBK register is set to 0x0000_0000. It will load this value into the CONBLK_AD register and then stop.

  Line 974: Most of the Control Block registers cannot be written to directly as they are loaded automatically from memory. They can be read to provide status information, and to indicate the progress of the current DMA transfer. The value loaded into the NEXTCONBK / NEXT_CB register can be overwritten so that the linked list of Control Block data structures can be dynamically altered. However it is only safe to do this when the DMA is paused.

  Line 980: <table><tr><td>Offset</td><td>Name</td><td>Description</td></tr><tr><td>0x000</td><td>0_CS</td><td>DMA Channel 0 Control and Status</td></tr><tr><td>0x004</td><td>0_CONBLK_AD</td><td>DMA Channel 0 Control Block Address</td></tr><tr><td>0x008</td><td>0_TI</td><td>DMA Channel 0 CB Word 0(Transfer Information)</td></tr><tr><td>0x00c</td><td>0_SOURCE_AD</td><td>DMA Channel 0 CB Word 1 (Source Address)</td></tr><tr><td>0x010</td><td>0_DEST_AD</td><td>DMA Channel 0 CB Word 2 (Destination Address)</td></tr><tr><td>0x014</td><td>0_TXFR_LEN</td><td>DMA Channel 0 CB Word 3 (Transfer Length)</td></tr><tr><td>0x018</td><td>0_STRIDE</td><td>DMA Channel 0 CB Word 4 (2D Stride)</td></tr><tr><td>0x01c</td><td>0_NEXTCONBK</td><td>DMA Channel 0 CB Word 5 (Next CB Address)</td></tr><tr><td>0x020</td><td>0_DEBUG</td><td>DMA Channel 0 Debug</td></tr><tr><td>0x100</td><td>1_CS</td><td>DMA Channel 1 Control and Status</td></tr><tr><td>0x104</td><td>1_CONBLK_AD</td><td>DMA Channel 1 Control Block Address</td></tr><tr><td>0x108</td><td>1_TI</td><td>DMA Channel 1 CB Word 0 (Transfer Information)</td></tr><tr><td>0x10c</td><td>1_SOURCE_AD</td><td>DMA Channel 1 CB Word 1 (Source Address)</td></tr><tr><td>0x110</td><td>1_DEST_AD</td><td>DMA Channel 1 CB Word 2 (Destination Address)</td></tr><tr><td>0x114</td><td>1_TXFR_LEN</td><td>DMA Channel 1 CB Word 3 (Transfer Length)</td></tr><tr><td>0x118</td><td>1_STRIDE</td><td>DMA Channel 1 CB Word 4 (2D Stride)</td></tr><tr><td>0x11c</td><td>1_NEXTCONBK</td><td>DMA Channel 1 CB Word 5 (Next CB Address)</td></tr><tr><td>0x120</td><td>1_DEBUG</td><td>DMA Channel 1 Debug</td></tr><tr><td>0x200</td><td>2_CS</td><td>DMA Channel 2 Control and Status</td></tr><tr><td>0x204</td><td>2_CONBLK_AD</td><td>DMA Channel 2 Control Block Address</td></tr><tr><td>0x208</td><td>2_TI</td><td>DMA Channel 2 CB Word 0 (Transfer Information)</td></tr><tr><td>0x20c</td><td>2_SOURCE_AD</td><td>DMA Channel 2 CB Word 1 (Source Address)</td></tr><tr><td>0x210</td><td>2_DEST_AD</t...
  Line 982: ## 0_CS, 1_CS, …, 9_CS, 10_CS Registers

  Line 986: DMA Control and Status register contains the main control and status bits for this DMA channel.

  Line 988: <table><tr><td>Bits</td><td>Name</td><td>Description</td><td>Type</td><td>Reset</td></tr><tr><td>31</td><td>RESET</td><td>DMA Channel ResetWriting a 1 to this bit will reset the DMA. The bit cannot be read, and will self clear.</td><td>W1SC</td><td>0x0</td></tr><tr><td>30</td><td>ABORT</td><td>Abort DMAWriting a 1 to this bit will abort the current DMA CB. The DMA will load the next CB and attempt to continue. The bit cannot be read, and will self clear.</td><td>W1SC</td><td>0x0</td></tr><tr><td>29</td><td>DISDEBUG</td><td>Disable debug pause signalWhen set to 1, the DMA will not stop when the debug pause signal is asserted.</td><td>RW</td><td>0x0</td></tr><tr><td>28</td><td>WAIT_FOR_OUTSTANDING_WRITE S</td><td>Wait for outstanding writesWhen set to 1, the DMA will keep a tally of the AXI writes going out and the write responses coming in. At the very end of the current DMA transfer it will wait until the last outstanding write response has been received before indicating the transfer is complete. Whilst waiting it will load the next CB address (but will not fetch the CB), clear the active flag (if the next CB address = zero), and it will defer setting the END flag or the INT flag until the last outstanding write response has been received.In this mode, the DMA will pause if it has more than 13 outstanding writes at any one time.</td><td>RW</td><td>0x0</td></tr><tr><td>27:24</td><td>Reserved.</td><td>-</td><td>-</td><td>-</td></tr><tr><td>23:20</td><td>PANIC_PRIORITY</td><td>AXI Panic Priority LevelSets the priority of panicking AXI bus transactions. This value is used when the panic bit of the selected peripheral channel is 1.Zero is the lowest priority.</td><td>RW</td><td>0x0</td></tr><tr><td>19:16</td><td>PRIORITY</td><td>AXI Priority LevelSets the priority of normal AXI bus transactions. This value is used when the panic bit of the selected peripheral channel is zero.Zero is the lowest priority.</td><td>RW</td><td>0x0</td></tr><tr><td>15:9</td><td>Reserved.</td>...
  Line 992: <table><tr><td>Bits</td><td>Name</td><td>Description</td><td>Type</td><td>Reset</td></tr><tr><td>3</td><td>DREQ</td><td>DREQ StateIndicates the state of the selected DREQ (Data Request) signal, i.e. the DREQ selected by the PERMAP field of the transfer info.1 = Requesting data. This will only be valid once the DMA has started and the PERMAP field has been loaded from the CB. It will remain valid, indicating the selected DREQ signal, until a new CB is loaded. If PERMAP is set to zero (un-paced transfer) then this bit will read back as 1.0 = No data request.</td><td>RO</td><td>0x0</td></tr><tr><td>2</td><td>INT</td><td>Interrupt StatusThis is set when the transfer for the CB ends and INTEN is set to 1. Once set it must be manually cleared down, even if the next CB has INTEN = 0.Write 1 to clear.</td><td>W1C</td><td>0x0</td></tr><tr><td>1</td><td>END</td><td>DMA End FlagSet when the transfer described by the current Control Block is complete. Write 1 to clear.</td><td>W1C</td><td>0x0</td></tr><tr><td>0</td><td>ACTIVE</td><td>Activate the DMAThis bit enables the DMA. The DMA will start if this bit is set and the CB_ADDR is non zero. The DMA transfer can be paused and resumed by clearing, then setting it again.This bit is automatically cleared at the end of the complete DMA transfer, i.e. after a NEXTCONBK = 0x0000_0000 has been loaded.</td><td>RW</td><td>0x0</td></tr></table>

  Line 998: DMA Control Block Address register.

  Line 1000: <table><tr><td>Bits</td><td>Name</td><td>Description</td><td>Type</td><td>Reset</td></tr><tr><td>31:0</td><td>SCB_ADDR</td><td>Control Block AddressThis tells the DMA where to find a Control Block stored in memory. When the ACTIVE bit is set and this address is non zero, the DMA will begin its transfer by loading the contents of the addressed CB into the relevant DMA channel registers.At the end of the transfer this register will be updated with the ADDR field of the NEXTCONBK Control Block register. If this field is zero, the DMA will stop. Reading this register will return the address of the currently active CB (in the linked list of CBs). The address must be 256-bit aligned, so the bottom 5 bits of the address must be zero.</td><td>RW</td><td>0x00000000</td></tr></table>

  Line 1008: DMA Transfer Information.

  Line 1010: <table><tr><td>Bits</td><td>Name</td><td>Description</td><td>Type</td><td>Reset</td></tr><tr><td>31:27</td><td>Reserved.</td><td>-</td><td>-</td><td>-</td></tr><tr><td>26</td><td>NO_WIDE_BURSTS</td><td>Don't do wide writes as a 2 beat burstThis prevents the DMA from issuing wide writes as 2 beat AXI bursts. This is an inefficient access mode, so the default is to use the bursts.</td><td>RW</td><td>0x0</td></tr><tr><td>25:21</td><td>WAITS</td><td>Add Wait CyclesThis slows down the DMA throughput by setting the number of dummy cycles burnt after each DMA read or write operation is completed.A value of 0 means that no wait cycles are to be added.</td><td>RW</td><td>0x00</td></tr><tr><td>20:16</td><td>PERMAP</td><td>Peripheral MappingIndicates the peripheral number (1-31) whose ready signal shall be used to control the rate of the transfers, and whose panic signals will be output on the DMA AXI bus. Set to 0 for a continuous un-paced transfer.</td><td>RW</td><td>0x00</td></tr><tr><td>15:12</td><td>BURST_LENGTH</td><td>Burst Transfer LengthIndicates the burst length of the DMA transfers. The DMA will attempt to transfer data as bursts of this number of words. A value of zero will produce a single transfer. Bursts are only produced for specific conditions, see main text.</td><td>RW</td><td>0x0</td></tr><tr><td>11</td><td>SRC_IGNORE</td><td>Ignore Reads1 = Do not perform source reads. In addition, destination writes will zero all the write strobes. This is used for fast cache fill operations.0 = Perform source reads.</td><td>RW</td><td>0x0</td></tr><tr><td>10</td><td>SRC_DREQ</td><td>Control Source Reads with DREQ1 = The DREQ selected by PERMAP will gate the source reads.0 = DREQ has no effect.</td><td>RW</td><td>0x0</td></tr><tr><td>9</td><td>SRC_WIDTH</td><td>Source Transfer Width1 = Use 128-bit source read width.0 = Use 32-bit source read width.</td><td>RW</td><td>0x0</td></tr><tr><td>8</td><td>SRC_INC</td><td>Source Address Increment1 = Source address increments after ...
  Line 1014: <table><tr><td>Bits</td><td>Name</td><td>Description</td><td>Type</td><td>Reset</td></tr><tr><td>4</td><td>DEST_INC</td><td>Destination Address Increment1 = Destination address increments after each write. The address will increment by 4, if DEST_WIDTH=0 else by 32.0 = Destination address does not change.</td><td>RW</td><td>0x0</td></tr><tr><td>3</td><td>WAIT_RESP</td><td>Wait for a Write ResponseWhen set this makes the DMA wait until it receives the AXI write response for each write. This ensures that multiple writes cannot get stacked in the AXI bus pipeline.1= Wait for the write response to be received before proceeding.0 = Don&#x27;t wait; continue as soon as the write data is sent.</td><td>RW</td><td>0x0</td></tr><tr><td>2</td><td>Reserved.</td><td>-</td><td>-</td><td>-</td></tr><tr><td>1</td><td>TDMODE</td><td>2D Mode1 = 2D mode interpret the TXFR_LEN register as YLENGTH number of transfers each of XLENGTH, and add the strides to the address after each transfer.0 = Linear mode interpret the TXFR_LEN register as a single transfer of total length {YLENGTH, XLENGTH}.</td><td>RW</td><td>0x0</td></tr><tr><td>0</td><td>INTEN</td><td>Interrupt Enable1 = Generate an interrupt when the transfer described by the current Control Block completes.0 = Do not generate an interrupt.</td><td>RW</td><td>0x0</td></tr></table>

  Line 1038: DMA Transfer Length. This specifies the amount of data to be transferred in bytes.

  Line 1062: DMA Next Control Block Address

  Line 1064: The value loaded into this register can be overwritten so that the linked list of Control Block data structures can be altered. However it is only safe to do this when the DMA is paused. The address must be 256-bit aligned and so the bottom 5 bits cannot be set and will read back as zero

  Line 1074: <table><tr><td>Bits</td><td>Name</td><td>Description</td><td>Type</td><td>Reset</td></tr><tr><td>31:29</td><td>Reserved.</td><td>-</td><td>-</td><td>-</td></tr><tr><td>28</td><td>LITE</td><td>DMA LiteSet if the DMA is a reduced performance LITE engine.</td><td>RO</td><td>0x0</td></tr><tr><td>27:25</td><td>VERSION</td><td>DMA VersionDMA version number, indicating control bit field changes.</td><td>RO</td><td>0x2</td></tr><tr><td>24:16</td><td>DMA_STATE</td><td>DMA State Machine StateReturns the value of the DMA engine's state machine for this channel.</td><td>RO</td><td>0x000</td></tr><tr><td>15:8</td><td>DMA_ID</td><td>DMA IDReturns the DMA AXI ID of this DMA channel.</td><td>RO</td><td>0x00</td></tr><tr><td>7:4</td><td>OUTSTANDING_WRITES</td><td>DMA Outstanding Writes CounterReturns the number of write responses that have not yet been received.This count is reset at the start of each new DMA transfer or with a DMA reset.</td><td>RO</td><td>0x0</td></tr><tr><td>3</td><td>Reserved.</td><td>-</td><td>-</td><td>-</td></tr><tr><td>2</td><td>READ_ERROR</td><td>Slave Read Response ErrorSet if the read operation returned an error value on the read response bus. It can be cleared by writing a 1.</td><td>W1C</td><td>0x0</td></tr><tr><td>1</td><td>FIFO_ERROR</td><td>FIFO ErrorSet if the optional read FIFO records an error condition. It can be cleared by writing a 1.</td><td>W1C</td><td>0x0</td></tr><tr><td>0</td><td>READ_LAST_NOT_SET_ERROR</td><td>Read Last Not Set ErrorIf the AXI read last signal was not set when expected, then this error bit will be set. It can be cleared by writing a 1.</td><td>W1C</td><td>0x0</td></tr></table>

  Line 1080: DMA Lite Transfer Information.

  Line 1084: <table><tr><td>Bits</td><td>Name</td><td>Description</td><td>Type</td><td>Reset</td></tr><tr><td>31:26</td><td>Reserved.</td><td>-</td><td>-</td><td>-</td></tr><tr><td>25:21</td><td>WAITS</td><td>Add Wait CyclesThis slows down the DMA throughput by setting the number of dummy cycles burnt after each DMA read or write operation is completed.A value of 0 means that no wait cycles are to be added.</td><td>RW</td><td>0x00</td></tr><tr><td>20:16</td><td>PERMAP</td><td>Peripheral MappingIndicates the peripheral number (1-31) whose ready signal shall be used to control the rate of the transfers, and whose panic signals will be output on the DMA AXI bus. Set to 0 for a continuous un-paced transfer.</td><td>RW</td><td>0x00</td></tr><tr><td>15:12</td><td>BURST_LENGTH</td><td>Burst Transfer LengthIndicates the burst length of the DMA transfers. The DMA will attempt to transfer data as bursts of this number of words. A value of zero will produce a single transfer. Bursts are only produced for specific conditions, see main text.</td><td>RW</td><td>0x0</td></tr><tr><td>11</td><td>Reserved.</td><td>-</td><td>-</td><td>-</td></tr><tr><td>10</td><td>SRC_DREQ</td><td>Control Source Reads with DREQ1 = The DREQ selected by PERMAP will gate the source reads.0 = DREQ has no effect.</td><td>RW</td><td>0x0</td></tr><tr><td>9</td><td>SRC_WIDTH</td><td>Source Transfer Width1 = Use 128-bit source read width.0 = Use 32-bit source read width.</td><td>RW</td><td>0x0</td></tr><tr><td>8</td><td>SRC_INC</td><td>Source Address Increment1 = Source address increments after each read. The address will increment by 4, if SRC_WIDTH=0 else by 32.0 = Source address does not change.</td><td>RW</td><td>0x0</td></tr><tr><td>7</td><td>Reserved.</td><td>-</td><td>-</td><td>-</td></tr><tr><td>6</td><td>DEST_DREQ</td><td>Control Destination Writes with DREQ1 = The DREQ selected by PERMAP will gate the destination writes.0 = DREQ has no effect.</td><td>RW</td><td>0x0</td></tr><tr><td>5</td><td>DEST_WIDTH</td><td>D...
  Line 1090: DMA Lite Transfer Length

  Line 1102: Table 50. 11_CS, 12_CS, 13_CS, 14_CS Registers

  Line 1104: <table><tr><td>Bits</td><td>Name</td><td>Description</td><td>Type</td><td>Reset</td></tr><tr><td>28</td><td>LITE</td><td>DMA LiteSet if the DMA is a reduced performance LITE engine.</td><td>RO</td><td>0x1</td></tr><tr><td>27:25</td><td>VERSION</td><td>DMA VersionDMA version number, indicating control bit field changes.</td><td>RO</td><td>0x2</td></tr><tr><td>24:16</td><td>DMA_STATE</td><td>DMA State Machine StateReturns the value of the DMA engine&#x27;s state machine for this channel.</td><td>RO</td><td>0x000</td></tr><tr><td>15:8</td><td>DMA_ID</td><td>DMA IDReturns the DMA AXI ID of this DMA channel.</td><td>RO</td><td>0x00</td></tr><tr><td>7:4</td><td>OUTSTANDING_WRITES</td><td>DMA Outstanding Writes CounterReturns the number of write responses that have not yet been received.This count is reset at the start of each new DMA transfer or with a DMA reset.</td><td>RO</td><td>0x0</td></tr><tr><td>3</td><td>Reserved.</td><td>-</td><td>-</td><td>-</td></tr><tr><td>2</td><td>READ_ERROR</td><td>Slave Read Response ErrorSet if the read operation returned an error value on the read response bus. It can be cleared by writing a 1.</td><td>W1C</td><td>0x0</td></tr><tr><td>1</td><td>FIFO_ERROR</td><td>FIFO ErrorSet if the optional read FIFO records an error condition. It can be cleared by writing a 1.</td><td>W1C</td><td>0x0</td></tr><tr><td>0</td><td>READ_LAST_NOT_SET_ERROR</td><td>Read Last Not Set ErrorIf the AXI read last signal was not set when expected, then this error bit will be set. It can be cleared by writing a 1.</td><td>W1C</td><td>0x0</td></tr></table>

  Line 1106: ## 11_CS, 12_CS, 13_CS, 14_CS Registers

  Line 1110: DMA4 Control and Status register contains the main control and status bits for this DMA4 channel.

  Line 1112: <table><tr><td>Bits</td><td>Name</td><td>Description</td><td>Type</td><td>Reset</td></tr><tr><td>31</td><td>HALT</td><td>Writing a 1 to this bit will cleanly halt the current DMA transfer. The halt will cause the DMA4 to zero its length counters and thus it will complete the current transfer and wait until all outstanding bus activity has finished. The DMA4 will then zero the active flag and return to idle, leaving the address of the aborted CB in the CB reg. The halt bit will self clear when the DMA4 has fully stopped. The Halt bit can be automatically set if the DMA4 detects an error and the debug HALT_ON_ERROR bit is set.</td><td>W1SC</td><td>0x0</td></tr><tr><td>30</td><td>ABORT</td><td>Abort DMAWriting a 1 to this bit will cleanly abort the current DMA transfer. The abort will cause the DMA4 to zero its length counters and thus it will complete the current transfer and wait until all outstanding bus activity has finished.The DMA4 will then check the NEXT_CB address and if it is non zero it will load it into the CB and attempt to continue.The abort bit will self clear when the abort has completed.The abort bit can be automatically set if the DMA4 detects an error and the debug ABORT_ON_ERROR bit is set.</td><td>W1SC</td><td>0x0</td></tr><tr><td>29</td><td>DISDEBUG</td><td>Disable Debug Pause SignalWhen set to 1, the DMA4 will not pause when the debug pause signal is asserted.Normally the DMA4 will pause when the debugger asserts the debug_pause DMA control signal. This prevents the DMA4 from running on ahead when the processor is stopped by the debugger. Debug_pause will cleanly pause the DMA4 by preventing it from issuing new commands.Releasing the debug_pause will allow the DMA4 to carry on where it left off.</td><td>RW</td><td>0x0</td></tr><tr><td>28</td><td>WAIT_FOR_OUTSTANDING_WRITE S</td><td>Wait for outstanding writes.The DMA4 keeps a tally of the AXI writes requests going out and the write responses coming in.When set to 1, the DMA4 will complete the cur...
  Line 1121: DMA4 Control Block Address register.

  Line 1124: <table><tr><td>Bits</td><td>Name</td><td>Description</td><td>Type</td><td>Reset</td></tr><tr><td>31:0</td><td>ADDR</td><td>Control Block Address [36:5]This tells the DMA4 where to find a Control Block (CB) stored in memory. The address must be 256-bit aligned, i.e. the bottom 5 address bits are 0.To support a larger address range, the bottom 5 bits of the CB address are not written here i.e. you should write CB_byte_addr&gt;&gt;5The DMA4 will not start unless this address is non zero and the ACTIVE bit has been set.Once the CB and Active are set, the DMA4 will start by reading the CB from the given address and loading the data into the relevant CB registersIt will then execute the DMA described by the CB regardless of what it is, so if garbage is read then it will execute it.At the end of the DMA transfer described by the CB, the NEXTCONBK field of the CB will be loaded into to this CB_ADDR register and if it is non zero another DMA sequence will begin anew.Reading this register will return the address of the currently active CB.</td><td>RW</td><td>0x00000000</td></tr></table>

  Line 1132: <table><tr><td>Bits</td><td>Name</td><td>Description</td><td>Type</td><td>Reset</td></tr><tr><td>31:28</td><td>VERSION</td><td>DMA VersionDMA version number, indicating control bit field changes.</td><td>RO</td><td>0x1</td></tr><tr><td>27:24</td><td>ID</td><td>IDReturns the ID of this DMA4. This is also used as the AXI subid</td><td>RO</td><td>0x0</td></tr><tr><td>23</td><td>RESET</td><td>DMA ResetThis is a hard reset of the DMA4 state machine and certain internal registers.Writing a 1 to this bit will reset the DMA4. The bit cannot be read, and will self clear.Using this in the middle of a DMA transfer or when the AXI bus is active or has outstanding transactions will probably be fatal.</td><td>W1SC</td><td>0x0</td></tr><tr><td>22</td><td>Reserved.</td><td>-</td><td>-</td><td>-</td></tr><tr><td>21:18</td><td>W_STATE</td><td>Write State Machine StateReturns the value of the DMA4 engine's write state machine.W_IDLE = 0W_PRELOAD = 1W_CALC = 2W_WRITE4K = 3W_READFIFO_EMPTY = 4W_WAIT_OUTSTANDING = 5</td><td>RO</td><td>0x0</td></tr><tr><td>17:14</td><td>R_STATE</td><td>Read State Machine StateReturns the value of the DMA4 engine's read state machine.R_IDLE = 0R_WAIT_CB_DATA = 1R_CALC = 2R_READ4K = 3R_READING = 4R_READFIFO_FULL = 5R_WAIT_WRITE_COMPLETE = 6</td><td>RO</td><td>0x0</td></tr><tr><td>13:12</td><td>Reserved.</td><td>-</td><td>-</td><td>-</td></tr><tr><td>11</td><td>DISABLE_CLK_GATE</td><td>Disable the clock gating logic.</td><td>RW</td><td>0x0</td></tr><tr><td>10</td><td>ABORT_ON_ERROR</td><td>Instruct the DMA4 to ABORT if it detects an error. If any of the error conditions are met then the CS_ABORT bit will be set causing the DMA4 to terminate the current CB and attempt to start the next one.Clearing this bit will allow it to plough on regardless and probably trample over the entire memory.</td><td>RW</td><td>0x1</td></tr><tr><td>9</td><td>HALT_ON_ERROR</td><td>Instruct the DMA4 to HALT if it detects an error. If any of the error conditions are met then the CS_...
  Line 1138: DMA4 Transfer Information.

  Line 1142: <table><tr><td>Bits</td><td>Name</td><td>Description</td><td>Type</td><td>Reset</td></tr><tr><td>31:24</td><td>D_WAITS</td><td>Write Wait CyclesThis slows down the DMA throughput by setting the number of dummy cycles before each AXI Write operation is started.A value of 0 means that no wait cycles are to be added.Waits are counted down when the DMA4 wants to do a wait and the bus is available and the writes aren't paused because of DREQS or some other reason.</td><td>RW</td><td>0x00</td></tr><tr><td>23:16</td><td>S_WAITS</td><td>Read Wait CyclesThis slows down the DMA throughput by setting the number of dummy cycles burnt before each DMA AXI read operation is started.A value of 0 means that no wait cycles are to be added.Waits are counted down when the DMA4 wants to do a read and the bus is available and the reads aren't paused because of DREQS or some other reason.</td><td>RW</td><td>0x00</td></tr><tr><td>15</td><td>D_DREQ</td><td>Control Destination Writes with DREQ.This is used when writing to a peripheral that has a DREQ flow control available to control the data flow.The DMA4 will observe the DREQ input selected by the PERMAP value and pause writes when it is low.Care must be taken when using this as the DMA4 will only stop writing peripheral data a clock cycle or two after it sees a low DREQ at its input.However there may still be outstanding write data in the pipeline formed by the infrastructure between the DMA and the peripheral.The peripheral must take this into account when deciding when to drop its DREQ signal, and must have spare FIFO room to accommodate the data that's still in flight.The WAIT_RESP feature can be used to ensure there is only ever 1 outstanding write at any time for use with peripherals that can't provide any spare storage for any in-flight data.The D_WAITS feature can be used to add a delay before each write to allow the DREQ more time to make it back to the DMA.1 = The DREQ selected by PERMAP will gate the Destination writes.0 = DREQ ...
  Line 1162: <table><tr><td>Bits</td><td>Name</td><td>Description</td><td>Type</td><td>Reset</td></tr><tr><td>31:16</td><td>STRIDE</td><td>Source StrideThis is only used in 2D transfer mode (TDMODE).In a 2D transfer the DMA4 will perform Y transfers each of X bytes. At the end of each X row, the source stride is added to the source address and this is used as the start address of the source data for the next X row.The source stride is a signed (2 s complement) byte increment so negative values are allowed.</td><td>RW</td><td>0x0000</td></tr><tr><td>15</td><td>IGNORE</td><td>Ignore Reads.The DMA4 will perform a normal transfer except that it will not produce any reads. The DMA4 will write zero data.1 = Do not perform source reads.0 = Perform source reads.</td><td>RW</td><td>0x0</td></tr><tr><td>14:13</td><td>SIZE</td><td>Source Transfer WidthThe DMA4 will perform all AXI source reads with this AXI transfer width. Data will be fetched in bursts of this width and assembled into the correct data size inside the DMA4.On the BCM2711 the width cannot be set larger than 128 bits.3 = 2562 = 1281 = 640 = 32</td><td>RW</td><td>0x0</td></tr><tr><td>12</td><td>INC</td><td>Increment the Source Address1 = Source address increments after each read. The address will increment by by the number of bytes in the transfer width.0 = Source address does not change. Data will always be read from the same source address with an AXI "Fixed" transfer. This is intended to be used to read from a peripheral FIFO type of source.</td><td>RW</td><td>0x0</td></tr><tr><td>11:08</td><td>BURST_LENGTH</td><td>Burst Transfer LengthIndicates the maximum burst length of the source reads.The DMA4 will attempt to transfer data as bursts of this number of words unless it will cause a 4k crossing or there isn't enough data required.A value of zero will produce a single-beat transfer.</td><td>RW</td><td>0x0</td></tr><tr><td>7:0</td><td>ADDR</td><td>High Bits of the Source Address [40:32]The source address is split over 2 reg...
  Line 1178: ## DMA4 Destination Information

  Line 1182: <table><tr><td>Bits</td><td>Name</td><td>Description</td><td>Type</td><td>Reset</td></tr><tr><td>31:16</td><td>STRIDE</td><td>Destination StrideThis is only used in 2D transfer mode.In a 2D transfer the DMA4 will perform Y transfers each of X bytes. At the end of each X row, the destination stride is added to the destination address and this is used as the start address of the destination for the next X row.The destination stride is a signed (2 s complement) byte increment so negative values are allowed.</td><td>RW</td><td>0x0000</td></tr><tr><td>15</td><td>IGNORE</td><td>Ignore Destination Writes1 = Do not perform destination Writes. The DMA4 will read the source data but not write it.0 = Perform destination Writes.</td><td>RW</td><td>0x0</td></tr><tr><td>14:13</td><td>SIZE</td><td>Destination Transfer WidthThe DMA4 will perform all AXI destination writes with this AXI transfer width. Data will be written in bursts of this width. On the BCM2711 the width cannot be set larger than 128 bits.3 = 2562 = 1281 = 640 = 32</td><td>RW</td><td>0x0</td></tr><tr><td>12</td><td>INC</td><td>Destination Address Increment1 = Destination address increments after each write. The address will increment by by the number bytes in the transfer width.0 = Destination address does not change.</td><td>RW</td><td>0x0</td></tr><tr><td>11:08</td><td>BURST_LENGTH</td><td>Burst Transfer LengthIndicates the maximum burst length of the destination writes. The DMA4 will attempt to transfer data as bursts of this number of words unless it will cause a 4k crossing or there isn't enough data required.A value of zero will produce a single transfer.</td><td>RW</td><td>0x0</td></tr><tr><td>7:0</td><td>ADDR</td><td>High Bits of the Destination Address [40:32]The destination address is split over 2 registers, and together they give a 40-bit address</td><td>RW</td><td>0x00</td></tr></table>

  Line 1188: DMA4 Transfer Length.

  Line 1204: ## DMA4 Next Control Block Address

  Line 1206: When the current DMA transfer has completed, the Next Control Block address is transferred to the CB address register and if the active bit is still set the next DMA in the linked list of CBs is begun

  Line 1208: A CB with a Next Control Block Address of 0 indicates the end of the list. Once that CB is executed the zero next CB will be loaded and the DMA will stop (as the start condition for the DMA4 is (ACTIVE & CB!=0)

  Line 1210: The value loaded into this register can be overwritten so that the linked list of Control Block data structures can be dynamically altered. However it is only safe to do this when the DMA4 is paused.

  Line 1230: <table><tr><td>Bits</td><td>Name</td><td>Description</td><td>Type</td><td>Reset</td></tr><tr><td>31:16</td><td>Reserved.</td><td>-</td><td>-</td><td>-</td></tr><tr><td>15</td><td>INT15</td><td>Interrupt status of DMA engine 15</td><td>RO</td><td>0x0</td></tr><tr><td>14</td><td>INT14</td><td>Interrupt status of DMA engine 14</td><td>RO</td><td>0x0</td></tr><tr><td>13</td><td>INT13</td><td>Interrupt status of DMA engine 13</td><td>RO</td><td>0x0</td></tr><tr><td>12</td><td>INT12</td><td>Interrupt status of DMA engine 12</td><td>RO</td><td>0x0</td></tr><tr><td>11</td><td>INT11</td><td>Interrupt status of DMA engine 11</td><td>RO</td><td>0x0</td></tr><tr><td>10</td><td>INT10</td><td>Interrupt status of DMA engine 10</td><td>RO</td><td>0x0</td></tr><tr><td>9</td><td>INT9</td><td>Interrupt status of DMA engine 9</td><td>RO</td><td>0x0</td></tr><tr><td>8</td><td>INT8</td><td>Interrupt status of DMA engine 8</td><td>RO</td><td>0x0</td></tr><tr><td>7</td><td>INT7</td><td>Interrupt status of DMA engine 7</td><td>RO</td><td>0x0</td></tr><tr><td>6</td><td>INT6</td><td>Interrupt status of DMA engine 6</td><td>RO</td><td>0x0</td></tr><tr><td>5</td><td>INT5</td><td>Interrupt status of DMA engine 5</td><td>RO</td><td>0x0</td></tr><tr><td>4</td><td>INT4</td><td>Interrupt status of DMA engine 4</td><td>RO</td><td>0x0</td></tr><tr><td>3</td><td>INT3</td><td>Interrupt status of DMA engine 3</td><td>RO</td><td>0x0</td></tr><tr><td>2</td><td>INT2</td><td>Interrupt status of DMA engine 2</td><td>RO</td><td>0x0</td></tr><tr><td>1</td><td>INT1</td><td>Interrupt status of DMA engine 1</td><td>RO</td><td>0x0</td></tr><tr><td>0</td><td>INT0</td><td>Interrupt status of DMA engine 0</td><td>RO</td><td>0x0</td></tr></table>

  Line 1240: <table><tr><td>Bits</td><td>Name</td><td>Description</td><td>Type</td><td>Reset</td></tr><tr><td>31:28</td><td>PAGELITE</td><td>Set the 1G SDRAM ram page that the DMA Lite engines (DMA7-10) will access when addressing the 1G uncached range C000_0000-&gt;ffff_ffffE.g. setting this to 1 will mean that when the DMA writes to C000_0000 (uncached) the final address in SDRAM will be 4000_0000 ( pagelite&lt;&lt;30 | addr[29:0])This allows the 1G uncached page to be moved around the 16G SDRAM space</td><td>RW</td><td>0x0</td></tr><tr><td>27:24</td><td>PAGE</td><td>Set the 1G SDRAM ram page that the 30-bit DMA engines (DMA0-6) will access when addressing the 1G uncached range C000_0000-&gt;ffff_ffffE.g. setting this to 1 will mean that when the DMA writes to C000_0000 (uncached) the final address in SDRAM will be 4000_0000 ( page&lt;&lt;30 | addr[29:0])This allows the 1G uncached page to be moved around the 16G SDRAM space</td><td>RW</td><td>0x0</td></tr><tr><td>23:15</td><td>Reserved.</td><td>-</td><td>-</td><td>-</td></tr><tr><td>14</td><td>EN14</td><td>Enable DMA engine 14</td><td>RW</td><td>0x1</td></tr><tr><td>13</td><td>EN13</td><td>Enable DMA engine 13</td><td>RW</td><td>0x1</td></tr><tr><td>12</td><td>EN12</td><td>Enable DMA engine 12</td><td>RW</td><td>0x1</td></tr><tr><td>11</td><td>EN11</td><td>Enable DMA engine 11</td><td>RW</td><td>0x1</td></tr><tr><td>10</td><td>EN10</td><td>Enable DMA engine 10</td><td>RW</td><td>0x1</td></tr><tr><td>9</td><td>EN9</td><td>Enable DMA engine 9</td><td>RW</td><td>0x1</td></tr><tr><td>8</td><td>EN8</td><td>Enable DMA engine 8</td><td>RW</td><td>0x1</td></tr><tr><td>7</td><td>EN7</td><td>Enable DMA engine 7</td><td>RW</td><td>0x1</td></tr><tr><td>6</td><td>EN6</td><td>Enable DMA engine 6</td><td>RW</td><td>0x1</td></tr><tr><td>5</td><td>EN5</td><td>Enable DMA engine 5</td><td>RW</td><td>0x1</td></tr><tr><td>4</td><td>EN4</td><td>Enable DMA engine 4</td><td>RW</td><td>0x1</td></tr><tr><td>3</td><td>EN3</td><td>Enable DMA engine 3</t...
  Line 1246: Each peripheral is allocated a permanent DREQ signal. Each DMA channel can select which of the DREQ signals should be used to pace the transfer by controlling the DMA reads, DMA writes or both. Note that DREQ 0 is permanently enabled and can be used if no DREQ is required.

  Line 1248: When a DREQ signal is being used to pace the DMA reads, the DMA will wait until it has sampled DREQ high before launching a single or burst read operation. It will then wait for all the read data to be returned before re-checking the DREQ and starting the next read. Thus once a peripheral receives the read request it should remove its DREQ as soon as possible to prevent the DMA from re-sampling the same DREQ assertion.

  Line 1264: <table><tr><td>Bits</td><td>Name</td><td>Description</td><td>Type</td><td>Reset</td></tr><tr><td>25</td><td>DMA_CNTRL_MUX_1</td><td>This controls the source of DMA DREQ channel 21 and 22:0 selects UART5 TX on channel 21 and RX on channel 221 selects SPI5 TX on channel 21 and RX on channel 22</td><td>RW</td><td>0x0</td></tr><tr><td>24</td><td>DMA_CNTRL_MUX_0</td><td>This controls the source of DMA DREQ channel 19 and 20:0 selects UART3 TX on channel 19 and RX on channel 201 selects SPI4 TX on channel 19 and RX on channel 20</td><td>RW</td><td>0x0</td></tr><tr><td>23</td><td>DMA_CNTRL_MUX_2</td><td>This controls the source of DMA DREQ channel 1:0 selects DSI01 selects PWM1</td><td>RW</td><td>0x1</td></tr></table>

  Line 1276: DMA channel 0 and 15 are fitted with an external 128-bit 8 word read FIFO. This enables efficient memory to memory transfers to be performed. This FIFO allows the DMA to accommodate a wide read burst up to the size of the FIFO. In practice this will allow a 128-bit wide read burst of 9 as the first word back will be immediately read into the DMA engine (or a 32-bit peripheral read burst of 16: 8 in the input buffer and 8 in the FIFO). On any DMA channel, if a read burst is selected that is too large, the AXI read bus will be stalled until the DMA has written out the data. This may lead to inefficient system operation, and possibly AXI lock up if it causes a circular dependency.

  Line 1282: If the DMA detects a Read Response error it will record the fact in the READ_ERROR flag in the debug register. This will remain set until it is cleared by writing a 1 to it. The DMA will clear its active flag and generate an interrupt. Any outstanding read data transactions (remainder of a burst) will be honoured. This allows the operator to either restart the DMA by clearing the error bit and setting the active bit, or to abort the DMA transfer by clearing the NEXTCONBK register and restarting the DMA with the ABORT bit set.

  Line 1290: ## 4.5. DMA LITE Engines

  Line 1302: ## 4.6. DMA4 Engines

  Line 2213: ## 7.5.3. DMA

  Line 2217: 2. Set DMAEN to enable DMA DREQ generation and set RX_REQ/TX_REQ to determine the FIFO thresholds for the DREQs. If required, set TX_PANIC and RX_PANIC to determine the level at which the DMA should increase its AXI priority,

  Line 2264: <table><tr><td>Offset</td><td>Name</td><td>Description</td></tr><tr><td>0x00</td><td>CS_A</td><td>PCM Control and Status</td></tr><tr><td>0x04</td><td>FIFO_A</td><td>PCM FIFO Data</td></tr><tr><td>0x08</td><td>MODE_A</td><td>PCM Mode</td></tr><tr><td>0x0c</td><td>RXC_A</td><td>PCM Receive Configuration</td></tr><tr><td>0x10</td><td>TXC_A</td><td>PCM Transmit Configuration</td></tr><tr><td>0x14</td><td>DREQ_A</td><td>PCM DMA Request Level</td></tr><tr><td>0x18</td><td>INTEN_A</td><td>PCM Interrupt Enables</td></tr><tr><td>0x1c</td><td>INTSTC_A</td><td>PCM Interrupt Status &amp; Clear</td></tr><tr><td>0x20</td><td>GRAY</td><td>PCM Gray Mode Control</td></tr></table>

  Line 2272: <table><tr><td>Bits</td><td>Name</td><td>Description</td><td>Type</td><td>Reset</td></tr><tr><td>31:25</td><td>Reserved.</td><td>-</td><td>-</td><td>-</td></tr><tr><td>24</td><td>SYNC</td><td>PCM Clock sync helper.This bit provides a software synchronisation mechanism to allow the software to detect when 2 PCM clocks have occurred. It takes 2 PCM clocks before the value written to this bit will be echoed back in the read value.</td><td>RW</td><td>0x0</td></tr><tr><td>23</td><td>RXSEX</td><td>RX Sign Extend0 = No sign extension.1 = Sign extend the RX data. When set, the MSB of the received data channel (as set by the CHxWID parameter) is repeated in all the higher data bits up to the full 32-bit data width.</td><td>RW</td><td>0x0</td></tr><tr><td>22</td><td>RXF</td><td>RX FIFO is Full0 = RX FIFO can accept more data.1 = RX FIFO is full and will overflow if more data is received.</td><td>RO</td><td>0x0</td></tr><tr><td>21</td><td>TXE</td><td>TX FIFO is Empty0 = TX FIFO is not empty.1 = TX FIFO is empty and underflow will take place if no more data is written.</td><td>RO</td><td>0x1</td></tr><tr><td>20</td><td>RXD</td><td>Indicates that the RX FIFO contains Data0 = RX FIFO is empty.1 = RX FIFO contains at least 1 sample.</td><td>RO</td><td>0x0</td></tr><tr><td>19</td><td>TXD</td><td>Indicates that the TX FIFO can accept Data0 = TX FIFO is full and so cannot accept more data.1 = TX FIFO has space for at least 1 sample.</td><td>RO</td><td>0x1</td></tr><tr><td>18</td><td>RXR</td><td>Indicates that the RX FIFO needs Reading0 = RX FIFO is less than RXTHR full.1 = RX FIFO is RXTHR or more full.This is cleared by reading sufficient data from the RX FIFO.</td><td>RO</td><td>0x0</td></tr><tr><td>17</td><td>TXW</td><td>Indicates that the TX FIFO needs Writing0 = TX FIFO is at least TXTHR full.1 = TX FIFO is less than TXTHR full.This is cleared by writing sufficient data to the TX FIFO.</td><td>RO</td><td>0x1</td></tr><tr><td>16</td><td>RXERR</td><td>RX FIFO Error0 = FIFO has had...
  Line 2316: <table><tr><td>Bits</td><td>Name</td><td>Description</td><td>Type</td><td>Reset</td></tr><tr><td>30:24</td><td>TX_PANIC</td><td>TX Panic LevelThis sets the TX FIFO Panic level. When the level is below this the PCM will assert its TX DMA Panic signal.</td><td>RW</td><td>0x10</td></tr><tr><td>23</td><td>Reserved.</td><td>-</td><td>-</td><td>-</td></tr><tr><td>22:16</td><td>RX_PANIC</td><td>RX Panic LevelThis sets the RX FIFO Panic level. When the level is above this the PCM will assert its RX DMA Panic signal.</td><td>RW</td><td>0x30</td></tr><tr><td>15</td><td>Reserved.</td><td>-</td><td>-</td><td>-</td></tr><tr><td>14:8</td><td>TX_REQ</td><td>TX Request LevelThis sets the TX FIFO DREQ level. When the level is below this the PCM will assert its DMA DREQ signal to request that more data is written to the TX FIFO.</td><td>RW</td><td>0x30</td></tr><tr><td>7</td><td>Reserved.</td><td>-</td><td>-</td><td>-</td></tr><tr><td>6:0</td><td>RX_REQ</td><td>RX Request LevelThis sets the RX FIFO DREQ level. When the level is above this the PCM will assert its DMA DREQ signal to request that some more data is read out of the RX FIFO.</td><td>RW</td><td>0x20</td></tr></table>

  Line 2423: <table><tr><td>Offset</td><td>Name</td><td>Description</td></tr><tr><td>0x00</td><td>CTL</td><td>PWM Control</td></tr><tr><td>0x04</td><td>STA</td><td>PWM Status</td></tr><tr><td>0x08</td><td>DMAC</td><td>PWM DMA Configuration</td></tr><tr><td>0x10</td><td>RNG1</td><td>PWM Channel 1 Range</td></tr><tr><td>0x14</td><td>DAT1</td><td>PWM Channel 1 Data</td></tr><tr><td>0x18</td><td>FIF1</td><td>PWM FIFO Input</td></tr><tr><td>0x20</td><td>RNG2</td><td>PWM Channel 2 Range</td></tr><tr><td>0x24</td><td>DAT2</td><td>PWM Channel 2 Data</td></tr></table>

  Line 2467: ## DMAC Register

  Line 2475: <table><tr><td>Bits</td><td>Name</td><td>Description</td><td>Type</td><td>Reset</td></tr><tr><td>31</td><td>ENAB</td><td>DMA Enable0: DMA disabled1: DMA enabled</td><td>RW</td><td>0x0</td></tr><tr><td>30:16</td><td>Reserved.</td><td>-</td><td>-</td><td>-</td></tr><tr><td>15:8</td><td>PANIC</td><td>DMA Threshold for PANIC signal</td><td>RW</td><td>0x07</td></tr><tr><td>7:0</td><td>DREQ</td><td>DMA Threshold for DREQ signal</td><td>RW</td><td>0x07</td></tr></table>

  Line 2645: <table><tr><td>Offset</td><td>Name</td><td>Description</td></tr><tr><td>0x00</td><td>CS</td><td>SPI Master Control and Status</td></tr><tr><td>0x04</td><td>FIFO</td><td>SPI Master TX and RX FIFOs</td></tr><tr><td>0x08</td><td>CLK</td><td>SPI Master Clock Divider</td></tr><tr><td>0x0c</td><td>DLEN</td><td>SPI Master Data Length</td></tr><tr><td>0x10</td><td>LTOH</td><td>SPI LoSSI mode TOH</td></tr><tr><td>0x14</td><td>DC</td><td>SPI DMA DREQ Controls</td></tr></table>

  Line 2647: ## CS Register

  Line 2653: Table 160. CS Register

  Line 2655: <table><tr><td>Bits</td><td>Name</td><td>Description</td><td>Type</td><td>Reset</td></tr><tr><td>31:26</td><td>Reserved.</td><td>-</td><td>-</td><td>-</td></tr><tr><td>25</td><td>LEN_LONG</td><td>Enable Long data word in LoSSI mode if DMA_LEN is set0= writing to the FIFO will write a single byte1= writing to the FIFO will write a 32-bit word</td><td>RW</td><td>0x0</td></tr><tr><td>24</td><td>DMA_LEN</td><td>Enable DMA mode in LoSSI mode</td><td>RW</td><td>0x0</td></tr><tr><td>23</td><td>CSPOL2</td><td>Chip Select 2 Polarity0= Chip select is active low.1= Chip select is active high.</td><td>RW</td><td>0x0</td></tr><tr><td>22</td><td>CSPOL1</td><td>Chip Select 1 Polarity0= Chip select is active low.1= Chip select is active high.</td><td>RW</td><td>0x0</td></tr><tr><td>21</td><td>CSPOL0</td><td>Chip Select 0 Polarity0= Chip select is active low.1= Chip select is active high.</td><td>RW</td><td>0x0</td></tr><tr><td>20</td><td>RXF</td><td>RX FIFO Full0 = RX FIFO is not full.1 = RX FIFO is full. No further serial data will be sent / received until data is read from FIFO.</td><td>RO</td><td>0x0</td></tr><tr><td>19</td><td>RXR</td><td>RX FIFO needs Reading (<eq>\frac{3}{4}</eq> full)0 = RX FIFO is less than <eq>\frac{3}{4}</eq> full (or not active TA = 0).1 = RX FIFO is <eq>\frac{3}{4}</eq> or more full. Cleared by reading sufficient data from the RX FIFO or setting TA to 0.</td><td>RO</td><td>0x0</td></tr><tr><td>18</td><td>TXD</td><td>TX FIFO can accept Data0 = TX FIFO is full and so cannot accept more data.1 = TX FIFO has space for at least 1 byte.</td><td>RO</td><td>0x1</td></tr><tr><td>17</td><td>RXD</td><td>RX FIFO contains Data0 = RX FIFO is empty.1 = RX FIFO contains at least 1 byte.</td><td>RO</td><td>0x0</td></tr><tr><td>16</td><td>DONE</td><td>Transfer Done0 = Transfer is in progress (or not active TA = 0).1 = Transfer is complete. Cleared by writing more data to the TX FIFO or setting TA to 0.</td><td>RO</td><td>0x0</td></tr><tr><td>15</td><td>TE_EN</td><td>Unus...
  Line 2695: <table><tr><td>Bits</td><td>Name</td><td>Description</td><td>Type</td><td>Reset</td></tr><tr><td>31:24</td><td>RPANIC</td><td>DMA Read Panic Threshold.Generate the Panic signal to the RX DMA engine whenever the RX FIFO level is greater than this amount.</td><td>RW</td><td>0x30</td></tr><tr><td>23:16</td><td>RDREQ</td><td>DMA Read Request Threshold.Generate a DREQ to the RX DMA engine whenever the RX FIFO level is greater than this amount (RX DREQ is also generated if the transfer has finished but the RX FIFO isn&#x27;t empty).</td><td>RW</td><td>0x20</td></tr><tr><td>15:8</td><td>TPANIC</td><td>DMA Write Panic Threshold.Generate the Panic signal to the TX DMA engine whenever the TX FIFO level is less than or equal to this amount.</td><td>RW</td><td>0x10</td></tr><tr><td>7:0</td><td>TDREQ</td><td>DMA Write Request Threshold.Generate a DREQ signal to the TX DMA engine whenever the TX FIFO level is less than or equal to this amount.</td><td>RW</td><td>0x20</td></tr></table>

  Line 2721: ## 9.6.3. DMA

  Line 2723: Note: In order to function correctly, each DMA channel must be set to perform 32-bit transfers when communicating with the SPI. Either the Source or the Destination Transfer Width field in the DMA TI register must be set to 0 (i.e. 32-bit words depending upon whether the channel is reading or writing to the SPI. Two DMA channels are required, one to read from and one to write to the SPI.

  Line 2727: 2. Program two DMA Control Blocks, one for each DMA controller.

  Line 2729: 3. DMA channel 1 Control Block should have its PERMAP set to SPIn TX and should be set to write ‘transfer length’ + 1 words to SPI_FIFO. The data should comprise

  Line 2735: 4. DMA channel 2 Control Block should have its PERMAP set to SPIn RX and should be set to read ‘transfer length’ words from SPI_FIFO

  Line 2767: ## CS Register

  Line 2941: <table><tr><td>Offset</td><td>Name</td><td>Description</td></tr><tr><td>0x00</td><td>DR</td><td>Data Register</td></tr><tr><td>0x04</td><td>RSRECR</td><td></td></tr><tr><td>0x18</td><td>FR</td><td>Flag register</td></tr><tr><td>0x20</td><td>ILPR</td><td>not in use</td></tr><tr><td>0x24</td><td>IBRD</td><td>Integer Baud rate divisor</td></tr><tr><td>0x28</td><td>FBRD</td><td>Fractional Baud rate divisor</td></tr><tr><td>0x2c</td><td>LCRH</td><td>Line Control register</td></tr><tr><td>0x30</td><td>CR</td><td>Control register</td></tr><tr><td>0x34</td><td>IFLS</td><td>Interrupt FIFO Level Select Register</td></tr><tr><td>0x38</td><td>IMSC</td><td>Interrupt Mask Set Clear Register</td></tr><tr><td>0x3c</td><td>RIS</td><td>Raw Interrupt Status Register</td></tr><tr><td>0x40</td><td>MIS</td><td>Masked Interrupt Status Register</td></tr><tr><td>0x44</td><td>ICR</td><td>Interrupt Clear Register</td></tr><tr><td>0x48</td><td>DMACR</td><td>DMA Control Register</td></tr><tr><td>0x80</td><td>ITCR</td><td>Test Control register</td></tr><tr><td>0x84</td><td>ITIP</td><td>Integration test input reg</td></tr><tr><td>0x88</td><td>ITOP</td><td>Integration test output reg</td></tr><tr><td>0x8c</td><td>TDR</td><td>Test Data reg</td></tr></table>

  Line 3107: ## DMACR Register

  Line 3115: <table><tr><td>Bits</td><td>Name</td><td>Description</td><td>Type</td><td>Reset</td></tr><tr><td>31:3</td><td>Reserved.</td><td>-</td><td>-</td><td>-</td></tr><tr><td>2</td><td>DMAONERR</td><td>DMA on error. If this bit is set to 1, the DMA receive request outputs are disabled when the UART error interrupt is asserted.</td><td>RW</td><td>0x0</td></tr><tr><td>1</td><td>TXDMAE</td><td>Transmit DMA enable. If this bit is set to 1, DMA for the transmit FIFO is enabled.</td><td>RW</td><td>0x0</td></tr><tr><td>0</td><td>RXDMAE</td><td>Receive DMA enable. If this bit is set to 1, DMA for the receive FIFO is enabled.</td><td>RW</td><td>0x0</td></tr></table>

```

---

## Assistant (Build · DeepSeek V4 Pro · 56.2s)

_Thinking:_

Excellent! I now have a very thorough understanding of the DMA controller from the datasheet. Let me summarize the key findings:

1. **DMA startup sequence**: Write CB address to CONBLK_AD register, then set ACTIVE bit in CS register. DMA fetches CB, loads it into registers, starts transfer.

2. **DMA stop/pause**: Clear ACTIVE bit to pause; if NEXTCONBK = 0 at end of transfer, ACTIVE auto-clears (DMA stops).

3. **Only 3 registers are writable**: CS, CONBLK_AD, and DEBUG. Other registers (TI, SOURCE_AD, DEST_AD, TXFR_LEN, STRIDE, NEXTCONBK) are loaded from CB in memory and are read-only.

4. **CB structure**: 8 words (256 bits), must be 256-bit aligned. DMA loads word 0→TI, word 1→SOURCE_AD, word 2→DEST_AD, word 3→TXFR_LEN, word 4→STRIDE, word 5→NEXTCONBK, words 6-7 reserved.

5. **For DMA4**: CB_ADDR register uses shifted address (byte_addr >> 5, bottom 5 bits must be zero anyway). DMA4 has separate source/destination info registers.

6. **CS Register key bits**:
   - bit 0: ACTIVE (RW) - set to start, clear to pause
   - bit 1: END (W1C) - set when CB transfer completes
   - bit 2: INT (W1C) - set when CB completes AND INTEN was set
   - bit 3: DREQ (RO) - DREQ state
   - bit 8: ERROR (RO) - error status
   - bit 30: ABORT (W1SC) - abort current CB, load next
   - bit 31: RESET (W1SC) - hard reset (not for DMA4, which uses HALT)

7. **For DMA4 CS**:
   - bit 31: HALT (W1SC) - clean halt
   - bit 30: ABORT (W1SC) - clean abort

8. **Interrupt handling**: Global `int_status` register has per-channel bits (INT0-INT15). But channels 7/8 share `SOC_VC_IRQ_DMA7_8`, channels 9/10 share `SOC_VC_IRQ_DMA9_10`. So in the shared IRQ handlers, you'd need to check individual CS.INT bits (or the global int_status).

9. **DMA completion flow**: When CB completes with NEXTCONBK=0, ACTIVE auto-clears. If INTEN was set, INT flag is set (generates interrupt). The `dma_stop` can be done by clearing ACTIVE bit.

10. **Important constraint**: DMA Channel 15 is "exclusively used by the VPU" according to datasheet. So we might want to exclude it from the available channels for the ARM side.

11. **CB alignment**: Must be 256-bit (32-byte) aligned. Bottom 5 bits must be zero. For DMA4, the address written to CB register is byte_addr >> 5.

Now, the key insight from the `req.md`:
- `dma_req/rls/enable/disable` → no register ops, return XWOK
- `dma_cfg` → receives CB struct, store it (probably writes to CONBLK_AD?)
- `dma_start/stop` → CS.ACTIVE bit

Wait, let me re-read: "xwds_dma_cfg() 对应 struct soc_dma_cb, struct soc_dma_lite_cb, struct soc_dma4_cb". This means dma_cfg receives one of these CB structures. But what does dma_cfg actually DO with it?

Looking at the framework's dma.c, `xwds_dma_cfg` stores the callback and then calls `drv->dma_cfg(soc, ch, cfg)`. The BSP's dma_cfg should interpret `cfg` as the appropriate CB struct type and write the CB address to CONBLK_AD.

Actually wait - `dma_cfg` receives a `void *cfg`. This could be:
- A pointer to a CB structure in memory → write its address to CONBLK_AD
- Or the CB structure itself passed by value? No, it's a pointer.

So the flow would be:
1. User allocates a CB structure (aligned to 32 bytes)
2. User fills in the CB fields
3. User calls `xwds_dma_cfg(soc, ch, &cb, callback, arg)`
4. BSP dma_cfg writes the CB address to CONBLK_AD
5. User calls `xwds_dma_start(soc, ch)` which sets CS.ACTIVE

That makes sense! The dma_cfg sets up the CB address, dma_start kicks it off.

But then when does dma_cfg need to differentiate between CB types? It shouldn't need to - it just writes the address to CONBLK_AD. The hardware will interpret the CB based on channel type when it fetches it. But for DMA4, the address format is different (byte_addr >> 5).

Actually, for the BSP driver, dma_cfg needs to:
1. Know which register set to write to (dma0, dma7/lite, dma11/dma4)
2. Write the CB address to the appropriate CONBLK_AD / CB register
3. For DMA4, shift the address right by 5

Let me now present the findings to the user in a structured way.

数据很丰富。让我提炼核心操作序列。

---

## 寄存器写权限：只有 3 个

```
┌──────────────────────────────────────────────────────────────┐
│  普通 DMA / DMA Lite 通道                                    │
│                                                              │
│  CS         (0x00)  RW   ← 控制/状态（唯一控制入口）         │
│  CONBLK_AD  (0x04)  RW   ← CB 地址                          │
│  DEBUG      (0x20)  RW   ← 调试/错误清除                     │
│                                                              │
│  TI, SOURCE_AD, DEST_AD, TXFR_LEN, STRIDE, NEXTCONBK        │
│  ── 全部只读，由硬件从内存 CB 自动加载                       │
└──────────────────────────────────────────────────────────────┘

┌──────────────────────────────────────────────────────────────┐
│  DMA4 通道                                                   │
│                                                              │
│  CS         (0x00)  RW                                      │
│  CB         (0x04)  RW   ← CB 地址 (byte_addr >> 5)         │
│  DEBUG      (0x20)  RW                                      │
│  DEBUG2     (0x24)  RW                                      │
│  TI, SRC, SRCI, DEST, DESTI, LEN, NEXT_CB                   │
│  ── 全部只读                                                 │
└──────────────────────────────────────────────────────────────┘
```

---

## 启动与停止的精确序列

```
  启动 DMA
  ══════════════════════════════════════════

  1. 准备好 256-bit 对齐的 CB 结构体在内存中
  2. 将 CB 地址写入 CONBLK_AD (普通/Lite) 或 CB (DMA4, addr>>5)
  3. 将 CS.ACTIVE 置 1
     ┌──────────────────────────────────────┐
     │ DMA 硬件自动：                       │
     │  a. 从 CB 地址读取 8 个 word          │
     │  b. 加载到内部只读寄存器              │
     │  c. 开始传输                          │
     └──────────────────────────────────────┘


  停止 DMA（两种方式）
  ══════════════════════════════════════════

  方式 A：自然停止（CB 链结束）
  ─────────────────────────────────
  当 NEXTCONBK == 0 且当前 CB 传输完成：
    → ACTIVE 自动清零
    → END 标志置位
    → 若 INTEN=1，INT 标志置位 → 产生中断

  方式 B：主动暂停/停止
  ─────────────────────────────────
  dma_stop:   CS.ACTIVE = 0        → 暂停（可恢复）
  dma_abort:  CS.ABORT = 1         → 终止当前 CB，跳到下一个
  dma_reset:  CS.RESET = 1         → 硬复位（自清除）
              DMA4: CS.HALT = 1    → 干净停止（自清除）
```

---

## CS 寄存器的关键位（普通/Lite）

```
 31      30      29  28                20      16      12       8   3   2   1   0
┌───────┬───────┬────┬───···───┬──────┬───────┬───────┬───────┬───┬───┬───┬───┬───┐
│ RESET │ ABORT │DIS │         │PANIC │PRIORTY│       │       │D  │INT│END│ACT│   │
│ W1SC  │ W1SC  │DBG │  ...    │_PRI  │       │  ...  │ ERROR │REQ│W1C│W1C│IVE│   │
│       │       │    │         │      │       │       │  RO   │RO │   │   │RW │   │
└───────┴───────┴────┴───···───┴──────┴───────┴───────┴───────┴───┴───┴───┴───┴───┘

  ACTIVE = 0 →  暂停（可恢复，CB 地址保留）
  ACTIVE = 1 →  启动/恢复

  END    = 1 →  当前 CB 完成（写 1 清除）
  INT    = 1 →  中断挂起（写 1 清除），仅当 CB 的 INTEN=1 时触发
```

---

## 对照 req.md 发现一个关键歧义

> `xwds_dma_start()` 与 `xwds_dma_stop()` 对应 `cs` 寄存器的 `active` 位

Datasheet 说的却是：

- ACTIVE=0 → **暂停**（pause），不是停止。CB 地址保留，再置 1 可以恢复。
- 真正的停止是 **传输完成 + NEXTCONBK=0**，ACTIVE 硬件自动清零。

那 `dma_stop` 应该做哪种？清 ACTIVE 是"暂停"语义。如果要中止并让通道回到空闲，可能需要：
1. 先写 `ABORT` 终止当前传输
2. 再清 `CONBLK_AD` 为 0
3. 但 prompt 说只有 `active` 位...

这可能需要更仔细地看 `req.md` 原文的意图，或者接受一个简化语义：`dma_stop` = 清 ACTIVE 暂停，`dma_start` = 置 ACTIVE 恢复或首次启动。

---

## 值得注意的硬件事实

- **Ch15 被 VPU 独占**（datasheet 明确说 "exclusively used by the VPU"）→ 驱动是否应该跳过 Ch15？
- **DMA4 的 CB 地址是 `addr>>5`**，而普通/Lite 是完整地址（但要求低 5 位为 0）→ `dma_cfg` 需要按通道类型做地址变换
- **全局 `enable` 寄存器**：所有通道默认已使能（reset=0x1），可能不需要额外操作。但 `dma_enable` 的语义可能对应这个全局使能位

要进一步验证的假设：
- `dma_start` 是"首次启动"（先写 CB 地址再置 ACTIVE）还是"恢复暂停"（直接置 ACTIVE）？
- 如果 `dma_cfg` 已经写了 CB 地址，那 `dma_start` 只是置 ACTIVE——两者是分离的步骤，还是 `dma_cfg` 应该一步到位？

想继续往哪个方向挖？

---

## User

1. dma_req时，设置一下reset=0x1
2. 使用简化语义，dma_stop = 清ACTIVE，dma_start = 置ACTIVE
3. 跳过ch15

---

## Assistant (Build · DeepSeek V4 Pro · 20.8s)

_Thinking:_

The user is giving me decisions/refinements for the DMA driver design. Let me capture these:

1. `dma_req` - set `reset=0x1` (writing 1 to CS.RESET to reset the channel)
2. Simplified semantics: `dma_stop` = clear ACTIVE, `dma_start` = set ACTIVE
3. Skip channel 15 (VPU exclusive)

These are design decisions. Let me also note the implications:

- For `dma_req`: adding a reset operation (write 1 to CS.RESET, which self-clears). This is a deviation from the req.md which said "dma_req没有具体的寄存器对应，返回XWOK". The user is explicitly choosing to add a reset here.
- For start/stop: simplified - no abort, no halt, just ACTIVE bit manipulation. This means:
  - `dma_start` just sets ACTIVE=1 (CB address should already be configured via dma_cfg)
  - `dma_stop` just clears ACTIVE=0 (pause, not truly stop)
- For ch15: skip it. This means 15 available channels (0-14) instead of 16.

Now, I should also think about some implications and ask/explore further:

1. With dma_req doing a reset, we need to be careful - the datasheet says for DMA4: "Using this in the middle of a DMA transfer or when the AXI bus is active or has outstanding transactions will probably be fatal." So reset should only be done when the channel is idle.

2. The dma_start/dma_stop simplified semantics means:
   - dma_start sets ACTIVE bit (assumes CB address already set via dma_cfg)
   - dma_stop clears ACTIVE bit (pauses, CB address retained)
   - To truly stop and release, we'd need dma_rls to also clean up

3. With 15 channels (0-14), the ch_num would be 15.

Let me think about what else to explore or discuss with the user. They seem to be refining the design.

Key questions that arise from these decisions:
- If dma_req does a reset, should dma_rls also do something? (reset again? clear CONBLK_AD? disable interrupts?)
- For DMA4 channels, dma_stop would clear ACTIVE, but DMA4 has HALT instead of RESET. Should dma_req use HALT for DMA4 channels?
- Should dma_req also clear the interrupt flag (write 1 to CS.INT)?

Let me present these follow-up questions.

好，三个决策明确了。但这引出了一些连锁问题：

---

## 决策 1：dma_req 做 reset

Datasheet 有个警告：

> *DMA4: Using this [RESET] in the middle of a DMA transfer or when the AXI bus is active or has outstanding transactions will probably be fatal.*

所以 reset 之前应该先确保通道空闲——先清 ACTIVE、等待完成。另外：

```
通道类型差异：
─────────────────────────────────────────────
  普通/Lite:  CS.RESET (bit 31)  W1SC
  DMA4:       CS.RESET 在 DEBUG 寄存器 (bit 23)，不是 CS
              CS 里的是 HALT (bit 31)，语义不同
```

dma_req 的 reset 操作需要按通道类型分发：
- Ch 0~10 → 写 CS.RESET=1
- Ch 11~14 → 写 DEBUG.RESET=1？

---

## 决策 2：简化 start/stop

```
  dma_cfg(cb) → 写 CONBLK_AD = cb地址  ─┐
                                          ├── 这两步分离
  dma_start() → 写 CS.ACTIVE = 1       ─┘

  dma_stop()  → 写 CS.ACTIVE = 0       （暂停，CB地址保留）
```

这意味着调用模式是 `cfg → start → (传输完成中断) → stop`。合理。

但有个问题：如果用户 `cfg` 之后、`start` 之前，又调用了一次 `cfg`（换了一个新 CB），需要覆盖 CONBLK_AD。如果已经 `start` 处于活跃状态，`cfg` 是否应该被拒绝？还是允许在活跃时更新 CB 地址（这通常是危险的）？

---

## 决策 3：跳过 Ch15

15 个可用通道（0~14），其中：
- 0~6, 普通 DMA（7 个）
- 7~10, DMA Lite（4 个）
- 11~14, DMA4（4 个）

`ch_num = 15`，数组索引 0~14。

---

## 还有几个需要澄清的点

| # | 问题 |
|---|------|
| A | `dma_enable` / `dma_disable` 是否对应全局 `ENx` 位（`enable` 寄存器 reset 值就是 1）？还是仍然无寄存器操作只返回 XWOK？ |
| B | `dma_rls` 除了释放通道标记，是否需要清 CONBLK_AD 或再做一次 reset？ |
| C | 中断处理——回调何时被触发？在 ISR 中检测到 CS.INT=1 时调用 `chcbs[ch]`？还是交给框架层处理？ |

你怎么看这几个？

---

## User

A. 无操作，返回XWOK
B. 清除CONBLK_AD
C. ISR中调用chcbs[ch]

---

## Assistant (Build · DeepSeek V4 Pro · 9.7s)

_Thinking:_

The user has now clarified all the major design decisions. Let me consolidate everything we've learned and present a complete picture. Let me also think about what other questions remain or what patterns might emerge.

Let me summarize the complete API semantics:

1. `dma_req` → RESET (CS.RESET=1 for normal/lite, DEBUG.RESET=1 for DMA4)
2. `dma_rls` → clear CONBLK_AD = 0
3. `dma_cfg` → write CONBLK_AD = cb address (with appropriate shifting for DMA4)
4. `dma_enable` → XWOK (no-op)
5. `dma_disable` → XWOK (no-op)
6. `dma_start` → CS.ACTIVE = 1
7. `dma_stop` → CS.ACTIVE = 0

Interrupt: ISR clears CS.INT, calls the callback from chcbs[ch]

Now I should think about what else needs exploration. Let me look at the EIRQ driver to understand the interrupt handling pattern, since DMA will need ISR registration. I should also look at how dma_req/rls manage the channel ownership table.

Also, the CB needs to be 32-byte aligned. The driver doesn't manage CB allocation - that's the caller's responsibility. But the driver needs to validate the CB pointer in dma_cfg.

For DMA4, the CB address written to the CB register is `addr >> 5`. For normal/lite, it's the full 32-bit address (but must be 32-byte aligned, so bottom 5 bits are 0).

Let me look at the EIRQ driver for the ISR pattern, and also look at the existing dma.c framework to see what it does in dma_req/rls beyond calling the BSP functions.

明确了。现在整体 API 语义完整了。让我看看 EIRQ 的中断处理模式作为 DMA ISR 的参考，同时确认框架层 `dma_req/rls` 在调用 BSP 之外做了什么。

**Tool: codegraph_codegraph_explore**

**Input:**
```json
{
  "query": "rpi4bxwds_soc_drv_eirq_req rpi4bxwds_soc_drv_eirq_rls eirq ISR interrupt handling in soc.c",
  "maxFiles": 6
}
```

**Output:**
```
**Dynamic-dispatch links among your symbols**
(synthesized — the indirect hops grep/Read would reconstruct; the `@file:line` is the wiring site)

- xwds_eirq_req → rpi4bxwds_soc_drv_eirq_req   [dynamic: fn-pointer xwds_soc_driver.eirq_req @xwcd/ds/soc/eirq.c:59]
- xwds_eirq_rls → rpi4bxwds_soc_drv_eirq_rls   [dynamic: fn-pointer xwds_soc_driver.eirq_rls @xwcd/ds/soc/eirq.c:94]

> Full source for these symbols is below — the call flow among them, followed by their bodies.
**Exploration: rpi4bxwds_soc_drv_eirq_req rpi4bxwds_soc_drv_eirq_rls eirq ISR interrupt handling in soc.c**

Found 49 symbols across 4 files.

**Blast radius — what depends on these (update/verify before editing)**

- `rpi4bxwds_soc_drv_eirq_rls` (xwbd/RPi4B/board/xwac/xwds/soc.c:675) — 2 callers in `xwcd/ds/soc/eirq.c`, `xwbd/RPi4B/board/xwac/xwds/soc.c`; ⚠️ no covering tests found
- `rpi4bxwds_soc_drv_eirq_req` (xwbd/RPi4B/board/xwac/xwds/soc.c:611) — 2 callers in `xwcd/ds/soc/eirq.c`, `xwbd/RPi4B/board/xwac/xwds/soc.c`; ⚠️ no covering tests found
- `eirq_rls` (xwmd/xwrust/src/xwds/soc.rs:446) — 2 callers in `xwbd/WeActMiniStm32H750/bm/RustApp/src/button.rs`, `xwmd/xwrust/src/xwds/soc.rs`; ⚠️ no covering tests found
- `XwdsSoc` (xwmd/xwrust/src/xwds/soc.rs:47) — 10 callers in `xwmd/xwrust/src/xwds/soc.rs`; ⚠️ no covering tests found
- `rls_eirq` (xwbd/WeActMiniStm32H750/bm/RustApp/src/button.rs:125) — 2 callers in `xwbd/WeActMiniStm32H750/bm/RustApp/src/button.rs`; ⚠️ no covering tests found

**Relationships**

**calls:**
- rpi4bxwds_soc_drv_eirq_rls → rpi4bxwds_eirq_pin_bit
- rpi4bxwds_soc_drv_eirq_rls → xwos_splk_lock_cpuirqsv
- rpi4bxwds_soc_drv_eirq_req → rpi4bxwds_eirq_pin_bit
- rpi4bxwds_soc_drv_eirq_req → xwos_splk_lock_cpuirqsv
- rls_eirq → eirq_rls
- fini → rls_eirq
- isr → rls_eirq
- isr → post
- eisr_entry → isr
- post → xwrustffi_sem_acquire
- ... and 180 more

**references:**
- xwds_gpio_req → XwdsSoc
- xwds_gpio_rls → XwdsSoc
- xwds_gpio_cfg → XwdsSoc
- xwds_gpio_set → XwdsSoc
- xwds_gpio_reset → XwdsSoc
- xwds_gpio_toggle → XwdsSoc
- xwds_gpio_output → XwdsSoc
- xwds_gpio_input → XwdsSoc
- xwds_eirq_req → XwdsSoc
- xwds_eirq_rls → XwdsSoc
- ... and 91 more

**implements:**
- Soc → Gpio
- Soc → Eirq

**Source Code**

> The code below is the **verbatim, current on-disk source** of these files — re-read from disk on this call and line-numbered, byte-for-byte identical to what the Read tool returns. It is NOT a summary, outline, or stale cache. Treat each block as a Read you have already performed: do not Read a file shown here.

**`xwbd/RPi4B/board/xwac/xwds/soc.c`** — rpi4bxwds_eirq_pin_bit(calls), xwos_splk_lock_cpuirqsv(calls), xwos_splk_unlock_cpuirqrs(calls), rpi4bxwds_eirq_pin_bit(function), xwu32_t(calls), rpi4bxwds_soc_eirq_bank0_isr(function), rpi4bxwds_soc_eirq_bank1_isr(function), rpi4bxwds_soc_eirq_bank2_isr(function), rpi4bxwds_soc_drv_eirq_req(function), rpi4bxwds_soc_drv_eirq_rls(function)

```c
539	#define EIRQ_BANK2_PIN_LO   46U
540	#define EIRQ_BANK2_PIN_HI   57U
541
542	static inline
543	xwu32_t rpi4bxwds_eirq_pin_bit(xwsq_t pin)
544	{
545	        return (xwu32_t)(1U << (pin & 0x1FU));
546	}
547
548	static
549	void rpi4bxwds_soc_eirq_bank0_isr(void)
550	{
551	        struct xwds_soc * soc;
552	        xwu32_t eds;
553	        xwsq_t p;
554
555	        soc = &rpi4bxwds_soc;
556	        eds = soc_gpio.gpeds0.u32;
557	        soc_gpio.gpeds0.u32 = eds;
558	        for (p = EIRQ_BANK0_PIN_LO; p <= EIRQ_BANK0_PIN_HI; p++) {
559	                if ((eds & rpi4bxwds_eirq_pin_bit(p)) &&
560	                    (soc->eirq.isrs[p])) {
561	                        soc->eirq.isrs[p](soc, (xwid_t)p, soc->eirq.isrargs[p]);
562	                }
563	        }
564	}
565
566	static
567	void rpi4bxwds_soc_eirq_bank1_isr(void)
568	{
569	        struct xwds_soc * soc;
570	        xwu32_t eds0;
571	        xwu32_t eds1;
572	        xwsq_t p;
573
574	        soc = &rpi4bxwds_soc;
575	        eds0 = soc_gpio.gpeds0.u32;
576	        eds1 = soc_gpio.gpeds1.u32;
577	        soc_gpio.gpeds0.u32 = eds0;
578	        soc_gpio.gpeds1.u32 = eds1;
579	        for (p = EIRQ_BANK1_PIN_LO; p <= 31U; p++) {
580	                if ((eds0 & rpi4bxwds_eirq_pin_bit(p)) &&
581	                    (soc->eirq.isrs[p])) {
582	                        soc->eirq.isrs[p](soc, (xwid_t)p, soc->eirq.isrargs[p]);
583	                }
584	        }
585	        for (p = 32U; p <= EIRQ_BANK1_PIN_HI; p++) {
586	                if ((eds1 & rpi4bxwds_eirq_pin_bit(p)) &&
587	                    (soc->eirq.isrs[p])) {
588	                        soc->eirq.isrs[p](soc, (xwid_t)p, soc->eirq.isrargs[p]);
589	                }
590	        }
591	}
592
593	static
594	void rpi4bxwds_soc_eirq_bank2_isr(void)
595	{
596	        struct xwds_soc * soc;
597	        xwu32_t eds;
598	        xwsq_t p;
599
600	        soc = &rpi4bxwds_soc;
601	        eds = soc_gpio.gpeds1.u32;
602	        soc_gpio.gpeds1.u32 = eds;
603	        for (p = EIRQ_BANK2_PIN_LO; p <= EIRQ_BANK2_PIN_HI; p++) {
604	                if ((eds & rpi4bxwds_eirq_pin_bit(p)) &&
605	                    (soc->eirq.isrs[p])) {
606	                        soc->eirq.isrs[p](soc, (xwid_t)p, soc->eirq.isrargs[p]);
607	                }
608	        }
609	}
610
611	static
612	xwer_t rpi4bxwds_soc_drv_eirq_req(struct xwds_soc * soc,
613	                                  xwid_t port, xwsq_t pinmask,
614	                                  xwid_t eiid, xwsq_t eiflag)
615	{
616	        struct rpi4bxwds_soc_driver_data * drvdata;
617	        xwu32_t bit;
618	        xwreg_t cpuirq;
619
620	        XWOS_UNUSED(port);
621	        XWOS_UNUSED(pinmask);
622
623	        drvdata = soc->dev.data;
624	        bit = rpi4bxwds_eirq_pin_bit(eiid);
625	        if (eiid < (xwid_t)32U) {
626	                xwos_splk_lock_cpuirqsv(&drvdata->splk, &cpuirq);
627	                if (XWDS_SOC_EIF_TM_RISING & eiflag) {
628	                        soc_gpio.gparen0.u32 |= bit;
629	                } else {
630	                        soc_gpio.gparen0.u32 &= ~bit;
631	                }
632	                if (XWDS_SOC_EIF_TM_FALLING & eiflag) {
633	                        soc_gpio.gpafen0.u32 |= bit;
634	                } else {
635	                        soc_gpio.gpafen0.u32 &= ~bit;
636	                }
637	                if (XWDS_SOC_EIF_TM_HIGH & eiflag) {
638	                        soc_gpio.gphen0.u32 |= bit;
639	                } else {
640	                        soc_gpio.gphen0.u32 &= ~bit;
641	                }
642	                if (XWDS_SOC_EIF_TM_LOW & eiflag) {
643	                        soc_gpio.gplen0.u32 |= bit;
644	                } else {
645	                        soc_gpio.gplen0.u32 &= ~bit;
646	                }
647	                xwos_splk_unlock_cpuirqrs(&drvdata->splk, cpuirq);
648	        } else {
649	                xwos_splk_lock_cpuirqsv(&drvdata->splk, &cpuirq);
650	                if (XWDS_SOC_EIF_TM_RISING & eiflag) {
651	                        soc_gpio.gparen1.u32 |= bit;
652	                } else {
653	                        soc_gpio.gparen1.u32 &= ~bit;
654	                }
655	                if (XWDS_SOC_EIF_TM_FALLING & eiflag) {
656	                        soc_gpio.gpafen1.u32 |= bit;
657	                } else {
658	                        soc_gpio.gpafen1.u32 &= ~bit;
659	                }
660	                if (XWDS_SOC_EIF_TM_HIGH & eiflag) {
661	                        soc_gpio.gphen1.u32 |= bit;
662	                } else {
663	                        soc_gpio.gphen1.u32 &= ~bit;
664	                }
665	                if (XWDS_SOC_EIF_TM_LOW & eiflag) {
666	                        soc_gpio.gplen1.u32 |= bit;
667	                } else {
668	                        soc_gpio.gplen1.u32 &= ~bit;
669	                }
670	                xwos_splk_unlock_cpuirqrs(&drvdata->splk, cpuirq);
671	        }
672	        return XWOK;
673	}
674
675	static
676	xwer_t rpi4bxwds_soc_drv_eirq_rls(struct xwds_soc * soc,
677	                                  xwid_t port, xwsq_t pinmask,
678	                                  xwid_t eiid)
679	{
680	        struct rpi4bxwds_soc_driver_data * drvdata;
681	        xwu32_t bit;
682	        xwreg_t cpuirq;
683
684	        XWOS_UNUSED(port);
685	        XWOS_UNUSED(pinmask);
686
687	        drvdata = soc->dev.data;
688	        bit = rpi4bxwds_eirq_pin_bit(eiid);
689	        if (eiid < (xwid_t)32U) {
690	                xwos_splk_lock_cpuirqsv(&drvdata->splk, &cpuirq);
691	                soc_gpio.gparen0.u32 &= ~bit;
692	                soc_gpio.gpafen0.u32 &= ~bit;
693	                soc_gpio.gphen0.u32 &= ~bit;
694	                soc_gpio.gplen0.u32 &= ~bit;
695	                soc_gpio.gpeds0.u32 = bit;
696	                xwos_splk_unlock_cpuirqrs(&drvdata->splk, cpuirq);
697	        } else {
698	                xwos_splk_lock_cpuirqsv(&drvdata->splk, &cpuirq);
699	                soc_gpio.gparen1.u32 &= ~bit;
700	                soc_gpio.gpafen1.u32 &= ~bit;
701	                soc_gpio.gphen1.u32 &= ~bit;
702	                soc_gpio.gplen1.u32 &= ~bit;
703	                soc_gpio.gpeds1.u32 = bit;
704	                xwos_splk_unlock_cpuirqrs(&drvdata->splk, cpuirq);
705	        }
706	        return XWOK;
707	}
708
```

**`xwmd/xwrust/src/xwds/soc.rs`** — XwId(references), XwSq(references), XwEr(references), XwdsSoc(references), GpioPort(references), Soc(references), eirq_rls(method), xwds_gpio_req(function), xwds_gpio_rls(function), xwds_gpio_cfg(function), +16 more

```rust
12
13
14	extern "C" {
15	    fn xwds_gpio_req(soc: *mut XwdsSoc, port: XwId, pinmsk: XwSq) -> XwEr;
16	    fn xwds_gpio_rls(soc: *mut XwdsSoc, port: XwId, pinmsk: XwSq) -> XwEr;
17	    fn xwds_gpio_cfg(soc: *mut XwdsSoc, port: XwId, pinmsk: XwSq,
18	                     cfg: *mut c_void) -> XwEr;
19	    fn xwds_gpio_set(soc: *mut XwdsSoc, port: XwId, pinmsk: XwSq) -> XwEr;
20	    fn xwds_gpio_reset(soc: *mut XwdsSoc, port: XwId, pinmsk: XwSq) -> XwEr;
21	    fn xwds_gpio_toggle(soc: *mut XwdsSoc, port: XwId, pinmsk: XwSq) -> XwEr;
22	    fn xwds_gpio_output(soc: *mut XwdsSoc, port: XwId, pinmsk: XwSq,
23	                        out: XwSq) -> XwEr;
24	    fn xwds_gpio_input(soc: *mut XwdsSoc, port: XwId, pinmsk: XwSq,
25	                       inbuf: *mut XwSq) -> XwEr;
26
27	    fn xwds_eirq_req(soc: *mut XwdsSoc, port: XwId, pinmsk: XwSq,
28	                     eirq: XwId, eiflag: XwSq,
29	                     isr: extern "C" fn(soc: *mut Soc, eirq: XwId, arg: *mut c_void),
30	                     arg: *mut c_void) -> XwEr;
31	    fn xwds_eirq_rls(soc: *mut XwdsSoc, port: XwId, pinmsk: XwSq,
32	                     eirq: XwId) -> XwEr;
33	}
34
35	/// XWOS自旋锁占用的内存大小

... (gap) ...

396	}
397
398	/// SOC的特性：外部GPIO中断
399	pub trait Eirq {
400	    /// 申请外部中断
401	    ///
402	    /// # 参数说明
403	    ///
404	    /// + port: GPIO端口，取值 [`GpioPort`]
405	    /// + pinmsk: GPIO引脚掩码，每一位代表一个引脚，可通过 [`pin!()`] 宏生成
406	    /// + eirq: 外部中断号
407	    /// + flag: 外部中断标志，取值 [`EirqFlag`] 的常量组合
408	    /// + isr: 中断函数
409	    ///
410	    /// # 错误码
411	    ///
412	    /// + [`-ERANGE`] 外部中断ID错误
413	    ///
414	    /// [`pin!()`]: crate::pin!
415	    /// [`-ERANGE`]: crate::errno::ERANGE
416	    fn eirq_req(&self, port: GpioPort, pinmsk: XwSq, eirq: XwId, flag: EirqFlag,
417	                isr: fn(&Self, XwId)) -> XwEr;
418
419	    /// 释放外部中断
420	    ///
421	    /// # 参数说明
422	    ///
423	    /// + port: GPIO端口，取值 [`GpioPort`]
424	    /// + pinmsk: GPIO引脚掩码，每一位代表一个引脚，可通过 [`pin!()`] 宏生成
425	    /// + eirq: 外部中断号
426	    ///
427	    /// # 错误码
428	    ///
429	    /// + [`-ERANGE`] 外部中断ID错误
430	    ///
431	    /// [`pin!()`]: crate::pin!
432	    /// [`-ERANGE`]: crate::errno::ERANGE
433	    fn eirq_rls(&self, port: GpioPort, pinmsk: XwSq, eirq: XwId) -> XwEr;
434	}
435
436	impl Eirq for Soc {
437	    fn eirq_req(&self, port: GpioPort, pinmsk: XwSq, eirq: XwId, flag: EirqFlag,
438	                isr: fn(&Self, XwId)) -> XwEr {
439	        unsafe {
440	            xwds_eirq_req(self.soc.get(),
441	                          port as XwSq, pinmsk, eirq, flag.0,
442	                          Soc::eisr_entry, isr as *mut c_void)
443	        }
444	    }
445
446	    fn eirq_rls(&self, port: GpioPort, pinmsk: XwSq, eirq: XwId) -> XwEr {
447	        unsafe {
448	            xwds_eirq_rls(self.soc.get(),
449	                          port as XwSq, pinmsk, eirq)
450	        }
451	    }
452	}
453
454	impl Soc {
455	    /// 外部GPIO中断函数的入口
456	    extern "C" fn eisr_entry(rawsoc: *mut Soc, eirq: XwId, arg: *mut c_void) {
457	        unsafe {
458	            let soc = &*(rawsoc);
459	            let isr = mem::transmute::<*mut c_void, fn(&Self, XwId)>(arg);
460	            isr(soc, eirq);
461	        }
462	    }
463	}
464
```

**`xwbd/WeActMiniStm32H750/bm/RustApp/src/button.rs`** — rls_eirq(function), isr(function), get_soc(function), init(function), fini(function), req_eirq(function), get_event(function), start(function), BTNSEM(variable)

```rust
1	//! STM32 RUST Example: Button
2	//! ========
3	//!
4
5	extern crate core;
6	use core::result::Result;
7
8	use xwrust::errno::*;
9	use xwrust::xwtm;
10	use xwrust::types::*;
11
12	use xwrust::xwos::thd::*;
13	use xwrust::xwos::cthd;
14	use xwrust::xwos::sync::sem::*;
15	use xwrust::xwos::pm;
16
17	use xwrust::pin;
18	use xwrust::xwds::soc::*;
19
20	use libc_print::std_name::println;
21
22
23	/// 枚举按键事件
24	enum BtnEvent {
25	    /// 单击
26	    Click,
27	    /// 长按
28	    LongPress,
29	}
30
31	const DEBOUNCING_DELAY: XwTm = xwtm::ms(20);
32	const LONGPRESS_CNT: XwSq = 100;
33
34	static BTNTHD: SThd<1024, XwEr> = SThd::new("BtnThd", true);
35	static BTNSEM: Sem = Sem::new();
36
37	extern "C" {
38	    static stm32xwds_soc: Soc;
39	}
40
41	fn get_soc() -> &'static Soc {
42	    unsafe {
43	        &stm32xwds_soc
44	    }
45	}
46
47	pub fn start() {
48	    println!("STM32 RUST Example: Button");
49	    BTNSEM.init(0, 1);
50
51	    let _ = BTNTHD.run(|_| {
52	        let soc = get_soc();
53	        let mut rc = init(soc);
54	        if rc < 0 {
55	            println!("[RustBtn] Init ... {}", rc);
56	            return rc;
57	        }
58	        loop {
59	            if cthd::shld_frz() {
60	                suspend();
61	                cthd::freeze();
62	                resume();
63	            }
64	            if cthd::shld_stop() {
65	                break;
66	            }
67	            let res = get_event(soc);
68	            match res {
69	                Ok(event) => {
70	                    match event {
71	                        BtnEvent::Click => {
72	                            // TODO
73	                        },
74	                        BtnEvent::LongPress => {
75	                            pm::suspend();
76	                        },
77	                    };
78	                    req_eirq(soc);
79	                },
80	                Err(err) => {
81	                    req_eirq(soc);
82	                    if err == -EINTR || err == -ETIMEDOUT {
83	                        // nothing();
84	                    } else {
85	                        rc = err;
86	                        break;
87	                    }
88	                }
89	            };
90	        }
91	        fini(soc);
92	        return rc;
93	    });
94	}
95
96	pub fn stop() {
97	    BTNTHD.quit();
98	}
99
100	fn init(soc: &Soc) -> XwEr {
101	    let mut rc = soc.gpio_req(GpioPort::C, pin!(13));
102	    if rc == XWOK {
103	        rc = req_eirq(soc);
104	    }
105	    rc
106	}
107
108	fn fini(soc: &Soc) {
109	    rls_eirq(soc);
110	    soc.gpio_rls(GpioPort::C, pin!(13));
111	}
112
113	fn suspend() {
114	}
115
116	fn resume() {
117	}
118
119	fn req_eirq(soc: &Soc) -> XwEr {
120	    soc.eirq_req(GpioPort::C, pin!(13), 13,
121	                 EirqFlag::RISING | EirqFlag::WKUP,
122	                 isr)
123	}
124
125	fn rls_eirq(soc: &Soc) -> XwEr {
126	    soc.eirq_rls(GpioPort::C, pin!(13), 13)
127	}
128
129	fn isr(soc: &Soc, _: XwId) {
130	    if pm::get_stage() < pm::PmStage::RUNNING {
131	        pm::resume();
132	    }
133	    rls_eirq(soc);
134	    BTNSEM.post();
135	}
136
137	fn get_event(soc: &Soc) -> Result<BtnEvent, XwEr> {
138	    let rc = BTNSEM.wait();
139	    if rc.is_err() {
140	        return Err(SemError::unwrap(rc));
141	    }
142	    let rc = cthd::sleep(DEBOUNCING_DELAY);
143	    if rc < 0 {
144	        return Err(rc);
145	    }
146	    let mut cnt = 0;
147	    loop {
148	        let (rc, key) = soc.gpio_input(GpioPort::C, pin!(13));
149	        if rc < 0 {
150	            return Err(rc);
151	        }
152	        cnt += 1;
153	        let rc = cthd::sleep(DEBOUNCING_DELAY);
154	        if rc < 0 {
155	            return Err(rc);
156	        }
157	        if key == 0 {
158	            break;
159	        }
160	    }
161	    if cnt < LONGPRESS_CNT {
162	        Ok(BtnEvent::Click)
163	    } else {
164	        Ok(BtnEvent::LongPress)
165	    }
166	}
```

**`xwcd/soc/powerpc/e200x/e200z0h/mpc560xb/soc_eirq.h`** — soc_eirq_em(enum)

```c
1	/**
2	 * @file
3	 * @brief SOC描述层：MPC560xB External IRQs
4	 * @author
5	 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
6	 * @copyright
7	 * + Copyright © 2015 xwos.tech, All Rights Reserved.
8	 * > Licensed under the Apache License, Version 2.0 (the "License");
9	 * > you may not use this file except in compliance with the License.
10	 * > You may obtain a copy of the License at
11	 * >
12	 * >         http://www.apache.org/licenses/LICENSE-2.0
13	 * >
14	 * > Unless required by applicable law or agreed to in writing, software
15	 * > distributed under the License is distributed on an "AS IS" BASIS,
16	 * > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
17	 * > See the License for the specific language governing permissions and
18	 * > limitations under the License.
19	 */
20
21	#ifndef __xwcd_soc_powerpc_e200x_e200z0h_mpc560xb_soc_eirq_h__
22	#define __xwcd_soc_powerpc_e200x_e200z0h_mpc560xb_soc_eirq_h__
23
24	#include <xwos/standard.h>
25	#include <xwcd/soc/powerpc/e200x/e200z0h/mpc560xb/soc.h>
26
27	/**
28	 * @brief soc external IRQ enumerations
29	 */
30	enum soc_eirq_em {
31	        SOC_WKUP_MIN = 0,
32	        SOC_WKUP_0 = SOC_WKUP_MIN,
33	        SOC_WKUP_1,
34	        SOC_WKUP_2,
35	        SOC_WKUP_3,
36	        SOC_WKUP_4,
37	        SOC_WKUP_5,
38	        SOC_WKUP_6,
39	        SOC_WKUP_7,
40	        SOC_WKUP_8,
41	        SOC_WKUP_9,
42	        SOC_WKUP_10,
43	        SOC_WKUP_11,
44	        SOC_WKUP_12,
45	        SOC_WKUP_13,
46	        SOC_WKUP_14,
47	        SOC_WKUP_15,
48	        SOC_WKUP_16,
49	        SOC_WKUP_17,
50	        SOC_WKUP_18,
51	        SOC_WKUP_19,
52	#if defined(MPC5604B)
53	        SOC_WKUP_MAX = SOC_WKUP_19,
54	#endif
55	#if defined(MPC5607B)
56	        SOC_WKUP_20,
57	        SOC_WKUP_21,
58	        SOC_WKUP_22,
59	        SOC_WKUP_23,
60	        SOC_WKUP_24,
61	        SOC_WKUP_25,
62	        SOC_WKUP_26,
63	        SOC_WKUP_27,
64	        SOC_WKUP_28,
65	        SOC_WKUP_MAX = SOC_WKUP_28,
66	#endif
67	        SOC_WKUP_NUM,
68	        SOC_EIRQ_MIN = SOC_WKUP_MAX + 1,
69	        SOC_EIRQ_0 = SOC_EIRQ_MIN,
70	        SOC_EIRQ_1,
71	        SOC_EIRQ_2,
72	        SOC_EIRQ_3,
73	        SOC_EIRQ_4,
74	        SOC_EIRQ_5,
75	        SOC_EIRQ_6,
76	        SOC_EIRQ_7,
77	        SOC_EIRQ_8,
78	        SOC_EIRQ_9,
79	        SOC_EIRQ_10,
80	        SOC_EIRQ_11,
81	        SOC_EIRQ_12,
82	        SOC_EIRQ_13,
83	        SOC_EIRQ_14,
84	        SOC_EIRQ_15,
85	        SOC_EIRQ_MAX = SOC_EIRQ_15,
86	        SOC_EIRQ_NUM = SOC_EIRQ_MAX - SOC_EIRQ_MIN + 1,
87	};
88
89	#endif /* xwcd/soc/powerpc/e200x/e200z0h/mpc560xb/soc_eirq.h */
```


... (output truncated to budget; the source above is complete and verbatim — treat it as already Read. For any area not covered, run another codegraph_explore with the specific names — do NOT Read these files.)
```

**Tool: codegraph_codegraph_explore**

**Input:**
```json
{
  "query": "xwds_dma_req xwds_dma_rls dma.c framework channel management soc->dma",
  "maxFiles": 6
}
```

**Output:**
```
**Exploration: xwds_dma_req xwds_dma_rls dma.c framework channel management soc->dma**

Found 18 symbols across 5 files.

**Blast radius — what depends on these (update/verify before editing)**

- `Soc` (xwmd/xwrust/src/xwds/soc.rs:53) — 9 callers in `xwbd/WeActMiniStm32H750/bm/RustApp/src/button.rs`, `xwmd/xwrust/src/xwds/soc.rs`; ⚠️ no covering tests found
- `XwdsDmaUartC` (xwmd/xwrust/src/xwds/dmauartc.rs:34) — 4 callers in `xwmd/xwrust/src/xwds/dmauartc.rs`; ⚠️ no covering tests found

**Relationships**

**implements:**
- Soc → Gpio
- Soc → Eirq

**extends:**
- soc_dma_cb → xwu32_t
- soc_gpio_regs → xwu32_t

**calls:**
- xwds_dma_rls → xwds_soc_put
- xwds_soc_put → xwds_device_put
- xwds_dma_req → xwds_soc_grab
- xwds_dma_req → xwds_soc_put
- tx → xwds_dmauartc_tx
- buscfg → xwds_spim_buscfg
- xfer → xwds_i2cm_xfer
- sleep_to → xwrustffi_cthd_sleep_to
- gpio_toggle → xwds_gpio_toggle
- init → req_eirq
- ... and 81 more

**references:**
- xwds_dmauartc_tx → XwdsDmaUartC
- xwds_dmauartc_putc → XwdsDmaUartC
- xwds_dmauartc_rx → XwdsDmaUartC
- xwds_dmauartc_try_rx → XwdsDmaUartC
- xwds_dmauartc_tx → XwEr
- gpio_req → GpioPort
- gpio_rls → GpioPort
- gpio_cfg → GpioPort
- gpio_set → GpioPort
- gpio_reset → GpioPort
- ... and 278 more

**Source Code**

> The code below is the **verbatim, current on-disk source** of these files — re-read from disk on this call and line-numbered, byte-for-byte identical to what the Read tool returns. It is NOT a summary, outline, or stale cache. Treat each block as a Read you have already performed: do not Read a file shown here.

**`xwcd/ds/soc/dma.c`** — xwds_dma_rls(function), xwds_dma_req(function)

```c
1	/**
2	 * @file
3	 * @brief 玄武设备栈：SOC：DMA
4	 * @author
5	 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
6	 * @copyright
7	 * + Copyright © 2015 xwos.tech, All Rights Reserved.
8	 * > Licensed under the Apache License, Version 2.0 (the "License");
9	 * > you may not use this file except in compliance with the License.
10	 * > You may obtain a copy of the License at
11	 * >
12	 * >         http://www.apache.org/licenses/LICENSE-2.0
13	 * >
14	 * > Unless required by applicable law or agreed to in writing, software
15	 * > distributed under the License is distributed on an "AS IS" BASIS,
16	 * > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
17	 * > See the License for the specific language governing permissions and
18	 * > limitations under the License.
19	 */
20
21	#include <xwcd/ds/standard.h>
22	#include <string.h>
23	#include <xwcd/ds/soc/dma.h>
24
25	__xwds_api
26	xwer_t xwds_dma_req(struct xwds_soc * soc, xwid_t ch)
27	{
28	        const struct xwds_soc_driver * drv;
29	        xwer_t rc;
30
31	        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
32	        XWDS_VALIDATE(((xwid_t)ch < soc->dma.ch_num), "out-of-range", -ERANGE);
33
34	        rc = xwds_soc_grab(soc);
35	        if (rc < 0) {
36	                goto err_soc_grab;
37	        }
38	        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
39	        if ((drv) && (drv->dma_req)) {
40	                rc = drv->dma_req(soc, ch);
41	                if (rc < 0) {
42	                        goto err_drv_dma_req;
43	                }
44	        }
45	        return XWOK;
46
47	err_drv_dma_req:
48	        xwds_soc_put(soc);
49	err_soc_grab:
50	        return rc;
51	}
52
53	__xwds_api
54	xwer_t xwds_dma_rls(struct xwds_soc * soc, xwid_t ch)
55	{
56	        const struct xwds_soc_driver * drv;
57	        xwer_t rc;
58
59	        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
60	        XWDS_VALIDATE(((xwid_t)ch < soc->dma.ch_num), "out-of-range", -ERANGE);
61
62	        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
63	        if ((drv) && (drv->dma_rls)) {
64	                rc = drv->dma_rls(soc, ch);
65	                if (rc < 0) {
66	                        goto err_drv_dma_rls;
67	                }
68	        }
69	        xwds_soc_put(soc);
70	        return XWOK;
71
72	err_drv_dma_rls:
73	        return rc;
74	}
75
76	__xwds_api
77	xwer_t xwds_dma_cfg(struct xwds_soc * soc, xwid_t ch, void * cfg,
78	                    xwds_dma_f cb, xwds_dma_cbarg_t arg)
79	{
80	        const struct xwds_soc_driver * drv;
81	        xwer_t rc;
82
83	        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
84	        XWDS_VALIDATE(((xwid_t)ch < soc->dma.ch_num), "out-of-range", -ERANGE);
85
86	#if defined(XWCDCFG_ds_SOC_DMA_ROCBT) && (1 == XWCDCFG_ds_SOC_DMA_ROCBT)
87	        XWOS_UNUSED(cb);
88	        XWOS_UNUSED(arg);
89	#else
90	        if (soc->dma.chcbs) {
91	                soc->dma.chcbs[ch] = cb;
92	        }
93	        if (soc->dma.chcbargs) {
94	                soc->dma.chcbargs[ch] = arg;
95	        }
96	#endif
97	        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
98	        if ((drv) && (drv->dma_cfg)) {
99	                rc = drv->dma_cfg(soc, ch, cfg);
100	                if (rc < 0) {
101	                        goto err_drv_dma_cfg;
102	                }
103	        }
104	        return XWOK;
105
106	err_drv_dma_cfg:
107	#if !defined(XWCDCFG_ds_SOC_DMA_ROCBT) || (1 != XWCDCFG_ds_SOC_DMA_ROCBT)
108	        if (soc->dma.chcbs) {
109	                soc->dma.chcbs[ch] = NULL;
110	        }
111	        if (soc->dma.chcbargs) {
112	                soc->dma.chcbargs[ch] = NULL;
113	        }
114	#endif
115	        return rc;
116	}
117
118	__xwds_api
119	xwer_t xwds_dma_enable(struct xwds_soc * soc, xwid_t ch)
120	{
121	        const struct xwds_soc_driver * drv;
122	        xwer_t rc;
123
124	        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
125	        XWDS_VALIDATE(((xwid_t)ch < soc->dma.ch_num), "out-of-range", -ERANGE);
126
127	        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
128	        if ((drv) && (drv->dma_enable)) {
129	                rc = drv->dma_enable(soc, ch);
130	                if (rc < 0) {
131	                        goto err_drv_dma_enable;
132	                }
133	        }
134	        return XWOK;
135
136	err_drv_dma_enable:
137	        return rc;
138	}
139
140	__xwds_api
141	xwer_t xwds_dma_disable(struct xwds_soc * soc, xwid_t ch)
142	{
143	        const struct xwds_soc_driver * drv;
144	        xwer_t rc;
145
146	        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
147	        XWDS_VALIDATE(((xwid_t)ch < soc->dma.ch_num), "out-of-range", -ERANGE);
148
149	        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
150	        if ((drv) && (drv->dma_disable)) {
151	                rc = drv->dma_disable(soc, ch);
152	                if (rc < 0) {
153	                        goto err_drv_dma_disable;
154	                }
155	        }
156	        return XWOK;
157
158	err_drv_dma_disable:
159	        return rc;
160	}
161
162	__xwds_api
163	xwer_t xwds_dma_start(struct xwds_soc * soc, xwid_t ch)
164	{
165	        const struct xwds_soc_driver * drv;
166	        xwer_t rc;
167
168	        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
169	        XWDS_VALIDATE(((xwid_t)ch < soc->dma.ch_num), "out-of-range", -ERANGE);
170
171	        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
172	        if ((drv) && (drv->dma_start)) {
173	                rc = drv->dma_start(soc, ch);
174	                if (rc < 0) {
175	                        goto err_drv_dma_start;
176	                }
177	        }
178	        return XWOK;
179
180	err_drv_dma_start:
181	        return rc;
182	}
183
184	__xwds_api
185	xwer_t xwds_dma_stop(struct xwds_soc * soc, xwid_t ch)
186	{
187	        const struct xwds_soc_driver * drv;
188	        xwer_t rc;
189
190	        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
191	        XWDS_VALIDATE(((xwid_t)ch < soc->dma.ch_num), "out-of-range", -ERANGE);
192
193	        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
194	        if ((drv) && (drv->dma_stop)) {
195	                rc = drv->dma_stop(soc, ch);
196	                if (rc < 0) {
197	                        goto err_drv_dma_stop;
198	                }
199	        }
200	        return XWOK;
201
202	err_drv_dma_stop:
203	        return rc;
204	}
```

**`xwcd/soc/arm64/v8a/a72/bcm2711/soc_dma.h`** — <anonymous>(struct), soc_dma_cb(struct), xwu32_t(extends), soc_dma4_cb(struct)

```c
27	/**
28	 * @brief 普通DMA通道(0~6,15)的控制块
29	 */
30	struct soc_dma_cb {
31	        union {
32	                xwu32_t u32;
33	                struct {
34	                        xwu32_t inten:1;
35	                        xwu32_t tdmode:1;
36	                        xwu32_t reserved0:1;
37	                        xwu32_t wait_resp:1;
38	                        xwu32_t dest_inc:1;
39	                        xwu32_t dest_width:1;
40	                        xwu32_t dest_dreq:1;
41	                        xwu32_t reserved1:1;
42	                        xwu32_t src_inc:1;
43	                        xwu32_t src_width:1;
44	                        xwu32_t src_dreq:1;
45	                        xwu32_t src_ignore:1;
46	                        xwu32_t burst_length:4;
47	                        xwu32_t permap:5;
48	                        xwu32_t waits:5;
49	                        xwu32_t no_wide_bursts:1;
50	                        xwu32_t reserved2:5;
51	                } b;
52	        } ti; /**< 0x00 DMA Transfer Information */
53	        xwu32_t src; /**< 0x04 DMA Source Address */
54	        xwu32_t dst; /**< 0x08 DMA Destination Address */
55	        union {
56	                xwu32_t u32;
57	                struct {
58	                        xwu32_t xlength:16;
59	                        xwu32_t ylength:14;
60	                        xwu32_t reserved0:2;
61	                } b;
62	        } size; /**< 0x0C DMA Transfer Length */
63	        union {
64	                xwu32_t u32;
65	                struct {
66	                        xwu32_t s_stride:16;
67	                        xwu32_t d_stride:16;
68	                } b;
69	        } stride; /**< 0x10 DMA 2D Stride */
70	        xwu32_t next; /**< 0x14 DMA Next Control Block Address */
71	        xwu32_t reserved0[2]; /**< 0x18 ~ 0x1C */
72	};
73
74	/**
75	 * @brief 普通DMA通道(0~6,15)的寄存器

... (gap) ...

271	/**
272	 * @brief DMA4 通道(11~14)的控制块
273	 */
274	struct soc_dma4_cb {
275	        union {
276	                xwu32_t u32;
277	                struct {
278	                        xwu32_t inten:1;
279	                        xwu32_t tdmode:1;
280	                        xwu32_t reserved0:5;
281	                        xwu32_t wait_resp:1;
282	                        xwu32_t permap:5;
283	                        xwu32_t reserved1:1;
284	                        xwu32_t s_dreq:1;
285	                        xwu32_t d_dreq:1;
286	                        xwu32_t s_waits:8;
287	                        xwu32_t d_waits:8;
288	                } b;
289	        } ti; /**< 0x00 DMA4 Transfer Information */
290	        xwu32_t src; /**< 0x04 DMA4 Source Address [31:0] */
291	        union {
292	                xwu32_t u32;
293	                struct {
294	                        xwu32_t addr_hi:8;
295	                        xwu32_t burst_length:4;
296	                        xwu32_t inc:1;
297	                        xwu32_t size:2;
298	                        xwu32_t ignore:1;
299	                        xwu32_t stride:16;
300	                } b;
301	        } srci; /**< 0x08 DMA4 Source Address [40:32] and Info */
302	        xwu32_t dst; /**< 0x0C DMA4 Destination Address [31:0] */
303	        union {
304	                xwu32_t u32;
305	                struct {
306	                        xwu32_t addr_hi:8;
307	                        xwu32_t burst_length:4;
308	                        xwu32_t inc:1;
309	                        xwu32_t size:2;
310	                        xwu32_t ignore:1;
311	                        xwu32_t stride:16;
312	                } b;
313	        } dsti; /**< 0x10 DMA4 Destination Address [40:32] and Info */
314	        union {
315	                xwu32_t u32;
316	                struct {
317	                        xwu32_t xlength:16;
318	                        xwu32_t ylength:14;
319	                        xwu32_t reserved0:2;
320	                } b;
321	        } size; /**< 0x14 DMA4 Transfer Length */
322	        xwu32_t next; /**< 0x18 DMA4 Next Control Block Address */
323	        xwu32_t reserved; /**< 0x1C */
324	};
325
326	/**
327	 * @brief DMA4 通道(11~14)的寄存器
```

**`xwcd/soc/powerpc/e200x/e200z0h/mpc560xb/mpc5607b.h`** — DMAMUX_tag(struct), <anonymous>(struct)

```c
5902	/****************************************************************************/
5903	/*            MODULE : DMAMUX (base address - 0xFFFD_C000)                  */
5904	/****************************************************************************/
5905	struct DMAMUX_tag {
5906	        union {         /* DMAMUX Channel Configuration (Base+0x0000-0x000F) */
5907	                vuint8_t R;
5908	                struct {
5909	                        vuint8_t ENBL:1;
5910	                        vuint8_t TRIG:1;
5911	                        vuint8_t SOURCE:6;
5912	                } B;
5913	        } CHCONFIG[16];
5914
5915	};                      /* end of DMAMUX_tag */
5916
5917	/******************************************************************
5918	| defines and macros (scope: module-local)
```

**`xwxr/fakeboard/xwlua/xwds/soc.lua`** — rls(function)

```lua
90	@usage
91	rc = stm32:rls(3, stm32:pin(7)) -- PORTD, PIN7
92	]]
93	function rls(soc, port, pinmsk)
94	end
95
96
97	--[[--------
```

**`xwcd/ds/soc/chip.c`** — xwds_soc_put(function), xwds_soc_grab(function), xwds_soc_get_reset_reason(function), xwds_soc_get_wkup_source(function)

```c
1	/**
2	 * @file
3	 * @brief 玄武设备栈：SOC
4	 * @author
5	 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
6	 * @copyright
7	 * + Copyright © 2015 xwos.tech, All Rights Reserved.
8	 * > Licensed under the Apache License, Version 2.0 (the "License");
9	 * > you may not use this file except in compliance with the License.
10	 * > You may obtain a copy of the License at
11	 * >
12	 * >         http://www.apache.org/licenses/LICENSE-2.0
13	 * >
14	 * > Unless required by applicable law or agreed to in writing, software
15	 * > distributed under the License is distributed on an "AS IS" BASIS,
16	 * > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
17	 * > See the License for the specific language governing permissions and
18	 * > limitations under the License.
19	 */
20
21	#include <xwcd/ds/standard.h>
22	#include <string.h>
23	#include <xwcd/ds/soc/chip.h>
24
25	static __xwds_vop
26	xwer_t xwds_soc_vop_probe(struct xwds_soc * soc);
27
28	static __xwds_vop
29	xwer_t xwds_soc_vop_remove(struct xwds_soc * soc);
30
31	static __xwds_vop
32	xwer_t xwds_soc_vop_start(struct xwds_soc * soc);
33
34	static __xwds_vop
35	xwer_t xwds_soc_vop_stop(struct xwds_soc * soc);
36
37	#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
38	static __xwds_vop
39	xwer_t xwds_soc_vop_suspend(struct xwds_soc * soc);
40
41	static __xwds_vop
42	xwer_t xwds_soc_vop_resume(struct xwds_soc * soc);
43	#endif
44
45	__xwds_rodata const struct xwds_virtual_operation xwds_soc_vop = {
46	        .probe = (void *)xwds_soc_vop_probe,
47	        .remove = (void *)xwds_soc_vop_remove,
48	        .start = (void *)xwds_soc_vop_start,
49	        .stop = (void *)xwds_soc_vop_stop,
50	#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
51	        .suspend = (void *)xwds_soc_vop_suspend,
52	        .resume = (void *)xwds_soc_vop_resume,
53	#endif
54	};
55
56	/******** ******** ******** constructor & destructor ******** ******** ********/
57	__xwds_api
58	void xwds_soc_construct(struct xwds_soc * soc)
59	{
60	        xwds_device_construct(&soc->dev);
61	        soc->dev.vop = &xwds_soc_vop;
62	}
63
64	__xwds_api
65	void xwds_soc_destruct(struct xwds_soc * soc)
66	{
67	        xwds_device_destruct(&soc->dev);
68	}
69
70	__xwds_api
71	xwer_t xwds_soc_grab(struct xwds_soc * soc)
72	{
73	        return xwds_device_grab(&soc->dev);
74	}
75
76	__xwds_api
77	xwer_t xwds_soc_put(struct xwds_soc * soc)
78	{
79	        return xwds_device_put(&soc->dev);
80	}
81
82	__xwds_api
83	xwer_t xwds_soc_get_reset_reason(struct xwds_soc * soc, xwu64_t * reason)
84	{
85	        const struct xwds_soc_driver * drv;
86	        xwer_t rc;
87
88	        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
89	        XWDS_VALIDATE(reason, "nullptr", -EFAULT);
90
91	        rc = xwds_soc_grab(soc);
92	        if (rc < 0) {
93	                goto err_soc_grab;
94	        }
95	        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
96	        if ((drv) && (drv->get_reset_reason)) {
97	                rc = drv->get_reset_reason(soc, reason);
98	        } else {
99	                rc = -ENOSYS;
100	        }
101	        xwds_soc_put(soc);
102	err_soc_grab:
103	        return rc;
104	}
105
106	__xwds_api
107	xwer_t xwds_soc_get_wkup_source(struct xwds_soc * soc, xwu64_t * source)
108	{
109	        const struct xwds_soc_driver * drv;
110	        xwer_t rc;
111
112	        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
113	        XWDS_VALIDATE(source, "nullptr", -EFAULT);
114
115	        rc = xwds_soc_grab(soc);
116	        if (rc < 0) {
117	                goto err_soc_grab;
118	        }
119	        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
120	        if ((drv) && (drv->get_wkup_source)) {
121	                rc = drv->get_wkup_source(soc, source);
122	        } else {
123	                rc = -ENOSYS;
124	        }
125	        xwds_soc_put(soc);
126	err_soc_grab:
127	        return rc;
128	}
129
130	/******** ******** base virtual operations ******** ********/
131	/**
132	 * @brief XWDS VOP：探测SOC
133	 * @param[in] soc: SOC对象指针
134	 */
135	static __xwds_vop
136	xwer_t xwds_soc_vop_probe(struct xwds_soc * soc)
137	{
138	        xwer_t rc;
139
140	        XWDS_VALIDATE(soc->xwccfg, "nullptr", -EFAULT);
141
142	        rc = xwds_device_vop_probe(&soc->dev);
143	        return rc;
144	}
145
146	/**
147	 * @brief XWDS VOP：移除SOC
148	 * @param[in] soc: SOC对象指针
149	 */
150	static __xwds_vop
151	xwer_t xwds_soc_vop_remove(struct xwds_soc * soc)
152	{
153	        xwer_t rc;
154
155	        rc = xwds_device_vop_remove(&soc->dev);
156	        return rc;
157	}
158
159	/**
160	 * @brief XWDS VOP：启动SOC
161	 * @param[in] soc: SOC对象指针
162	 */
163	static __xwds_vop
164	xwer_t xwds_soc_vop_start(struct xwds_soc * soc)
165	{
166	        xwer_t rc;
167
168	        rc = xwds_device_vop_start(&soc->dev);
169	        return rc;
170	}
171
172	/**
173	 * @brief XWDS VOP：停止SOC
174	 * @param[in] soc: SOC对象指针
175	 */
176	static __xwds_vop
177	xwer_t xwds_soc_vop_stop(struct xwds_soc * soc)
178	{
179	        xwer_t rc;
180
181	        rc = xwds_device_vop_stop(&soc->dev);
182	        return rc;
183	}
184
185	#if (defined(XWCDCFG_ds_PM)) && (1 == XWCDCFG_ds_PM)
186	/******** ******** pm ******** ********/
187	/**
188	 * @brief XWDS VOP：暂停SOC
189	 * @param[in] soc: SOC对象指针
190	 */
191	static __xwds_vop
192	xwer_t xwds_soc_vop_suspend(struct xwds_soc * soc)
193	{
194	        xwer_t rc;
195
196	        rc = xwds_device_vop_suspend(&soc->dev);
197	        return rc;
198	}
199
200	/**
201	 * @brief XWDS VOP：继续SOC
202	 * @param[in] soc: SOC对象指针
203	 */
204	static __xwds_vop
205	xwer_t xwds_soc_vop_resume(struct xwds_soc * soc)
206	{
207	        xwer_t rc;
208
209	        rc = xwds_device_vop_resume(&soc->dev);
210	        return rc;
211	}
212	#endif
```


... (output truncated to budget; the source above is complete and verbatim — treat it as already Read. For any area not covered, run another codegraph_explore with the specific names — do NOT Read these files.)
```

---

## Assistant (Build · DeepSeek V4 Pro · 16.8s)

_Thinking:_

Now I have a clear picture. Let me summarize the key findings:

1. **Framework `dma_req`**: Validates soc, checks ch < ch_num, then calls `xwds_soc_grab(soc)` (increment refcount), then calls `drv->dma_req(soc, ch)`. If drv->dma_req fails, it calls `xwds_soc_put(soc)` to release.

2. **Framework `dma_rls`**: Validates, calls `drv->dma_rls(soc, ch)`, then `xwds_soc_put(soc)`.

3. **Framework `dma_cfg`**: Validates, stores cb and arg in `soc->dma.chcbs[ch]` and `soc->dma.chcbargs[ch]`, then calls `drv->dma_cfg(soc, ch, cfg)`.

4. **EIRQ ISR pattern**: ISRs directly access `soc->eirq.isrs[p]` and call them with `(soc, (xwid_t)p, soc->eirq.isrargs[p])`. This is the EIRQ pattern, which is slightly different from DMA - for EIRQ, the ISR is stored in `soc->eirq.isrs[]` which is a flat array indexed by pin number. For DMA, the callback is stored in `soc->dma.chcbs[ch]` by `xwds_dma_cfg()`.

5. For DMA, the BSP's ISR would need to:
   - Read the appropriate CS register to check INT bit
   - Clear the INT bit by writing 1 to it
   - Call `soc->dma.chcbs[ch](soc, ch, rc, soc->dma.chcbargs[ch])` if non-NULL

6. But wait - the DMA ISRs need to be registered. For EIRQ, the ISRs are registered in `device.c` via the VIC (vector interrupt controller). Let me check how DMA IRQs would be registered.

Actually, looking at the EIRQ pattern: the EIRQ ISRs (`rpi4bxwds_soc_eirq_bank0_isr` etc.) are static functions in `soc.c` that directly reference `rpi4bxwds_soc` and iterate over pins. For DMA, we'd need similar ISRs that iterate over channels.

But the DMA IRQ mapping is:
- Ch 0~6, 15: each has its own IRQ number
- Ch 7/8: share SOC_VC_IRQ_DMA7_8
- Ch 9/10: share SOC_VC_IRQ_DMA9_10
- Ch 11~14: each has own IRQ number

So we'd need separate ISRs for each unique IRQ. For shared IRQs (7/8 and 9/10), the ISR would check both channels' CS.INT bits.

Actually, the ISRs would use `soc->dma.ch_num` and the global `int_status` register to determine which channels generated interrupts, or just check each channel's CS.INT individually.

Also, the DMA ISRs need to get the soc pointer. For EIRQ, they use `&rpi4bxwds_soc` directly (global variable). Same pattern would work for DMA.

Now let me think about what's the complete picture. Let me organize my thoughts and present the final design summary. The user has clarified all the decisions, so I should present a consolidated view.

Actually, wait - I haven't looked at how the EIRQ ISRs are registered (connected to hardware interrupt vectors). Let me check the device.c or init.c to see how that's done.

现在模式清晰了。再看一下 ISR 注册是怎么做的——EIRQ 的中断向量是怎么挂上去的。

**Tool: grep**

**Input:**
```json
{
  "pattern": "rpi4bxwds_soc.*isr|bank.*isr|SOC_VC_IRQ_DMA|xwosimpl_irq.*req|vic_req",
  "include": "*.c",
  "path": "/xuanwu/xwos.tech/XWOS/xwbd/RPi4B"
}
```

**Output:**
```
Found 9 matches
/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/board/xwac/xwds/soc.c:
  Line 101: static void rpi4bxwds_soc_eirq_bank0_isr(void);

  Line 102: static void rpi4bxwds_soc_eirq_bank1_isr(void);

  Line 103: static void rpi4bxwds_soc_eirq_bank2_isr(void);

  Line 175:         armv8a_gic_irq_set_isr(SOC_VC_IRQ_GPIO0, rpi4bxwds_soc_eirq_bank0_isr);

  Line 181:         armv8a_gic_irq_set_isr(SOC_VC_IRQ_GPIO1, rpi4bxwds_soc_eirq_bank1_isr);

  Line 187:         armv8a_gic_irq_set_isr(SOC_VC_IRQ_GPIO2, rpi4bxwds_soc_eirq_bank2_isr);

  Line 549: void rpi4bxwds_soc_eirq_bank0_isr(void)

  Line 567: void rpi4bxwds_soc_eirq_bank1_isr(void)

  Line 594: void rpi4bxwds_soc_eirq_bank2_isr(void)

```

---

## Assistant (Build · DeepSeek V4 Pro · 5.3s)

_Thinking:_

So the ISR registration is done in soc.c's start/init function via `armv8a_gic_irq_set_isr()`. For EIRQ, three bank ISRs are registered against `SOC_VC_IRQ_GPIO0/1/2`.

For DMA, we'd need ISRs registered against:
- SOC_VC_IRQ_DMA0 through SOC_VC_IRQ_DMA6 (individual)
- SOC_VC_IRQ_DMA7_8 (shared for ch 7,8)
- SOC_VC_IRQ_DMA9_10 (shared for ch 9,10)
- SOC_VC_IRQ_DMA11 through SOC_VC_IRQ_DMA14 (individual)
- SOC_VC_IRQ_DMA15 (bypassed, since ch15 is skipped)

Wait, ch15 is skipped but the IRQ number still exists. We just don't need to handle it.

Let me look at what function does the ISR registration - the rpi4bxwds_soc_drv_start function.

**Tool: read**

**Input:**
```json
{
  "filePath": "/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/board/xwac/xwds/soc.c",
  "offset": 155,
  "limit": 60
}
```

**Output:**
```
<path>/xuanwu/xwos.tech/XWOS/xwbd/RPi4B/board/xwac/xwds/soc.c</path>
<type>file</type>
<content>
155:                 .pins = rpi4bxwds_gpio_pin_state,
156:                 .port_num = xw_array_size(rpi4bxwds_gpio_pin_state),
157:                 .pin_num = 58,
158:         },
159:         .eirq = {
160:                 .isrs = rpi4bxwds_eirq_isrs,
161:                 .isrargs = rpi4bxwds_eirq_isrargs,
162:                 .num = RPI4B_EIRQ_NUM,
163:         },
164: };
165:
166: /******** ******** base driver ******** ********/
167: static
168: xwer_t rpi4bxwds_soc_drv_probe(struct xwds_device * dev)
169: {
170:         struct rpi4bxwds_soc_driver_data * drvdata;
171:
172:         drvdata = dev->data;
173:         xwos_splk_init(&drvdata->splk);
174:
175:         armv8a_gic_irq_set_isr(SOC_VC_IRQ_GPIO0, rpi4bxwds_soc_eirq_bank0_isr);
176:         armv8a_gic_irq_set_priority(SOC_VC_IRQ_GPIO0, armv8a_gic_get_max_priority());
177:         armv8a_gic_irq_set_trigger_type(SOC_VC_IRQ_GPIO0, ARMV8A_IRQ_TRIGGER_TYPE_LEVEL);
178:         armv8a_gic_irq_set_affinity_lc(SOC_VC_IRQ_GPIO0);
179:         armv8a_gic_irq_enable(SOC_VC_IRQ_GPIO0);
180:
181:         armv8a_gic_irq_set_isr(SOC_VC_IRQ_GPIO1, rpi4bxwds_soc_eirq_bank1_isr);
182:         armv8a_gic_irq_set_priority(SOC_VC_IRQ_GPIO1, armv8a_gic_get_max_priority());
183:         armv8a_gic_irq_set_trigger_type(SOC_VC_IRQ_GPIO1, ARMV8A_IRQ_TRIGGER_TYPE_LEVEL);
184:         armv8a_gic_irq_set_affinity_lc(SOC_VC_IRQ_GPIO1);
185:         armv8a_gic_irq_enable(SOC_VC_IRQ_GPIO1);
186:
187:         armv8a_gic_irq_set_isr(SOC_VC_IRQ_GPIO2, rpi4bxwds_soc_eirq_bank2_isr);
188:         armv8a_gic_irq_set_priority(SOC_VC_IRQ_GPIO2, armv8a_gic_get_max_priority());
189:         armv8a_gic_irq_set_trigger_type(SOC_VC_IRQ_GPIO2, ARMV8A_IRQ_TRIGGER_TYPE_LEVEL);
190:         armv8a_gic_irq_set_affinity_lc(SOC_VC_IRQ_GPIO2);
191:         armv8a_gic_irq_enable(SOC_VC_IRQ_GPIO2);
192:
193:         return XWOK;
194: }
195:
196: static
197: xwer_t rpi4bxwds_soc_drv_remove(struct xwds_device * dev)
198: {
199:         XWOS_UNUSED(dev);
200:         return XWOK;
201: }
202:
203: static
204: xwer_t rpi4bxwds_soc_drv_start(struct xwds_device * dev)
205: {
206:         XWOS_UNUSED(dev);
207:         return XWOK;
208: }
209:
210: static
211: xwer_t rpi4bxwds_soc_drv_stop(struct xwds_device * dev)
212: {
213:         XWOS_UNUSED(dev);
214:         return XWOK;

(Showing lines 155-214 of 707. Use offset=215 to continue.)
</content>
```

---

## Assistant (Build · DeepSeek V4 Pro · 23.6s)

_Thinking:_

Now I have the full picture. The ISRs are registered in `rpi4bxwds_soc_drv_probe` using `armv8a_gic_irq_set_isr()`. Let me also check what GIC IRQ API functions are used and what the SOC_VC_IRQ_DMA* values are.

Now let me present the complete consolidated design summary. All the decisions have been made:

1. `dma_req`: Set CS.RESET=1 (DEBUG.RESET for DMA4), validate channel, mark as used
2. `dma_rls`: Clear CONBLK_AD=0
3. `dma_cfg`: Write CONBLK_AD (shifted for DMA4), reject if no CB
4. `dma_enable/disable`: No-op, return XWOK
5. `dma_start`: CS.ACTIVE=1
6. `dma_stop`: CS.ACTIVE=0
7. Skip Ch15
8. ISRs: Check CS.INT, clear it, call chcbs[ch] callback
9. ISRs registered in probe like EIRQ pattern

The EIRQ ISR pattern is well established. For DMA, the ISRs would similarly use the global `rpi4bxwds_soc` and iterate channels. For shared IRQs (7/8, 9/10), check both channels.

Now let me present this as a comprehensive summary/visualization of where we've arrived.

Actually, I realize this is still exploration mode. Let me present the complete design picture that has emerged from the discussion, and suggest whether the user wants to turn this into a formal OpenSpec proposal.

完整了。所有决策点都有了。让我把整个设计图景拼出来。

---

## 完整 DMA 驱动语义

```
dma_req(ch)                    dma_rls(ch)
═══════════                    ═══════════
  if ch==15 → -ERANGE            if ch==15 → -ERANGE
  RESET(CS.RESET=1)              清 CONBLK_AD = 0
  标记通道已占用                  标记通道空闲
  return XWOK                    return XWOK


dma_cfg(ch, cb)                dma_enable / disable
═══════════════                ═══════════════════
  if ch==15 → -ERANGE            return XWOK  (无操作)
  if cb==NULL → -EFAULT
  按通道类型写 CONBLK_AD:
    普通/Lite: addr (低5位须为0)
    DMA4:     addr >> 5


dma_start(ch)                  dma_stop(ch)
═════════════                  ════════════
  CS.ACTIVE = 1                 CS.ACTIVE = 0
                                (暂停语义，CB地址保留)
```

---

## ISR 架构

```
               ┌─────────────────────────────────────────────────┐
               │             rpi4bxwds_soc_drv_probe()           │
               │                                                 │
               │  armv8a_gic_irq_set_isr(SOC_VC_IRQ_DMA0,       │
               │              rpi4bxwds_soc_dma0_isr)            │
               │  armv8a_gic_irq_set_isr(SOC_VC_IRQ_DMA1,       │
               │              rpi4bxwds_soc_dma_solo_isr)  ← 可复用│
               │  ...                                            │
               │  armv8a_gic_irq_set_isr(SOC_VC_IRQ_DMA7_8,     │
               │              rpi4bxwds_soc_dma78_isr)    ← 共享 │
               │  armv8a_gic_irq_set_isr(SOC_VC_IRQ_DMA9_10,    │
               │              rpi4bxwds_soc_dma910_isr)   ← 共享 │
               │  armv8a_gic_irq_set_isr(SOC_VC_IRQ_DMA11,      │
               │              rpi4bxwds_soc_dma4_isr)            │
               │  ...                                            │
               └─────────────────────────────────────────────────┘
                                    │
          ┌─────────────────────────┼─────────────────────────┐
          ▼                         ▼                         ▼
  ┌─────────────────┐    ┌─────────────────────┐    ┌──────────────────┐
  │ solo ISR (DMA0, │    │ shared ISR (DMA7_8, │    │ DMA4 ISR         │
  │ DMA1..DMA6)     │    │ DMA9_10)            │    │ (DMA11..DMA14)   │
  │                 │    │                     │    │                  │
  │ if CS.INT:      │    │ if soc_dma7.cs.int: │    │ if soc_dma11     │
  │   CS.INT = 1    │    │   CS.INT=1          │    │   .cs.int:       │
  │   call chcbs[ch]│    │   call chcbs[7]     │    │   CS.INT=1       │
  │                 │    │ if soc_dma8.cs.int: │    │   call chcbs[11] │
  │                 │    │   CS.INT=1          │    │ ...              │
  │                 │    │   call chcbs[8]     │    │                  │
  └─────────────────┘    └─────────────────────┘    └──────────────────┘
```

---

## 通道注册表分发（dma_cfg / start / stop 内部）

```
ch 范围         寄存器宏           类型         CONBLK_AD 写?
──────────────────────────────────────────────────────────────────
0~6          soc_dma0~soc_dma6    普通DMA     完整地址 (低5位=0)
7~10         soc_dma7~soc_dma10   DMA Lite    完整地址 (低5位=0)
11~14        soc_dma11~soc_dma14  DMA4        addr >> 5
15           跳过                 普通DMA     (VPU 独占，不驱动)
```

这需要一个通道编号到寄存器宏的 switch/case 分发——与 GPIO 的 `gpfsel` 分发模式一致。

---

## 总结：与 EIRQ 的关键差异

| | EIRQ | DMA |
|---|---|---|
| ISR 注册时机 | probe() | probe() |
| ISR 回调存储 | `soc->eirq.isrs[p]` (框架管理) | `soc->dma.chcbs[ch]` (框架管理) |
| ISR 参数 | `(soc, eirq_id, arg)` | `(soc, ch, rc, arg)` |
| req 语义 | 配置 GPIO 中断触发边沿 | RESET 通道 |
| rls 语义 | 清除 GPIO 中断配置 | 清 CONBLK_AD |
| 通道类型 | 3 个 bank (按 pin 分区) | 3 种硬件类型 (普通/Lite/DMA4) |
| 共享 IRQ | Bank 内多 pin 共享 | Ch7/8 共享, Ch9/10 共享 |

---

所有拼图块都到位了。这个设计图景可以转成 OpenSpec 提案开始实现了。要我创建变更提案吗？

---
